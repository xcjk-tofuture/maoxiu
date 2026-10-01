#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "cmsis_os.h"
#include "motor_device.h"
#include "chassis_port.h"
#include "platform_time.h"
#include "maoxiu_board.h"
#include "parameter_flash.h"
#include <string.h>
#include <math.h>
osThreadId MotorTaskHandle;
osThreadId EncoderTaskHandle;
static QueueHandle_t commands;
static chassis_service_t service;
static chassis_snapshot_t published;
static chassis_parameters_t parameters, published_parameters;
static uint32_t parameter_revision, published_revision, overruns;
static uint8_t maintenance, maintenance_ready, config_pending, feedback_stopped;
static QueueHandle_t config_updates;
typedef struct {
    chassis_velocity_t target;
    uint32_t submitted_ms;
} command_t;

#include "voltage_device.h"
extern void Error_Handler(void);
int chassis_port_init(void) {
    pj_value_t stored;
    cp_defaults(&parameters);
    if (pj_load(parameter_flash_io(), 1, 1, &stored) == PJ_OK && stored.length == CP_BYTES)
        (void)cp_decode(&parameters, stored.payload);
    published_parameters = parameters;
    chassis_init(&service, &parameters.control);
    config_updates = xQueueCreate(1, sizeof(chassis_parameters_t));
    commands = xQueueCreate(4, sizeof(command_t));
    return commands && config_updates ? 0 : -1;
}
int chassis_port_submit(chassis_velocity_t v) {
    command_t command;
    chassis_snapshot_t state;
    chassis_parameters_t config;
    if (!isfinite(v.vx_mps) || !isfinite(v.vy_mps) || !isfinite(v.wz_radps) ||
        fabsf(v.vx_mps) > MAOXIU_MAX_LINEAR_MPS || fabsf(v.vy_mps) > MAOXIU_MAX_LINEAR_MPS ||
        fabsf(v.wz_radps) > MAOXIU_MAX_ANGULAR_RADPS)
        return -2;
    chassis_port_snapshot(&state);
    chassis_port_parameters(&config, NULL);
    if (!(state.voltage > config.control.min_voltage))
        return -3;
    command.target = v;
    command.submitted_ms = platform_millis();
    taskENTER_CRITICAL();
    int result = maintenance || config_pending
                     ? -3
                     : (commands && xQueueSend(commands, &command, 0) == pdPASS ? 0 : -1);
    taskEXIT_CRITICAL();
    return result;
}
void chassis_port_snapshot(chassis_snapshot_t *s) {
    taskENTER_CRITICAL();
    *s = published;
    taskEXIT_CRITICAL();
}
void Encoder_Task_Proc(void const *argument) {
    TickType_t wake = xTaskGetTickCount();
    command_t command;
    float speed[4], pwm[4];
    chassis_parameters_t next;
    unsigned i, divider = 0;
    (void)argument;
    if (!commands) {
        vTaskDelete(NULL);
        return;
    }
    if (maoxiu_voltage_init() != 0)
        Error_Handler();
    maoxiu_motor_init();
    for (;;) {
        vTaskDelayUntil(&wake, pdMS_TO_TICKS(MAOXIU_CONTROL_MS));
        taskENTER_CRITICAL();
        uint8_t stopped = maintenance;
        taskEXIT_CRITICAL();
        if (stopped) {
            service.has_command = 0;
            for (i = 0; i < 4; i++)
                maoxiu_motor_write(i, 0);
            taskENTER_CRITICAL();
            maintenance_ready = 1;
            taskEXIT_CRITICAL();
        }
        if (xQueueReceive(config_updates, &next, 0) == pdPASS) {
            parameters = next;
            chassis_init(&service, &parameters.control);
            parameter_revision++;
            taskENTER_CRITICAL();
            published_parameters = parameters;
            published_revision = parameter_revision;
            config_pending = 0;
            taskEXIT_CRITICAL();
        }
        while (xQueueReceive(commands, &command, 0) == pdPASS)
            if (!stopped)
                chassis_submit(&service, command.target, command.submitted_ms);
        maoxiu_motor_read(speed);
        uint8_t wheels_stopped = 1;
        for (i = 0; i < 4; i++) {
            speed[i] *= parameters.speed_scale;
            if (!isfinite(speed[i]) || fabsf(speed[i]) > 0.02f)
                wheels_stopped = 0;
        }
        if ((TickType_t)(xTaskGetTickCount() - wake) > pdMS_TO_TICKS(MAOXIU_CONTROL_MS)) {
            wake = xTaskGetTickCount();
            service.has_command = 0;
            overruns++;
        }
        chassis_step(&service, speed, maoxiu_voltage_read(), platform_millis(), pwm);
        /* One owner, preserving the prior 20 ms output cadence. Faults stop immediately. */
        if (++divider >= MAOXIU_OUTPUT_DIVIDER || service.snapshot.state != CHASSIS_ACTIVE) {
            divider = 0;
            for (i = 0; i < 4; i++)
                maoxiu_motor_write(i, (int16_t)pwm[i]);
        }
        taskENTER_CRITICAL();
        published = service.snapshot;
        feedback_stopped = wheels_stopped;
        taskEXIT_CRITICAL();
    }
}

void chassis_port_parameters(chassis_parameters_t *p, uint32_t *revision) {
    taskENTER_CRITICAL();
    *p = published_parameters;
    if (revision)
        *revision = published_revision;
    taskEXIT_CRITICAL();
}
static int stopped_feedback(void) {
    return feedback_stopped && isfinite(published.velocity.vx_mps) &&
           isfinite(published.velocity.vy_mps) && isfinite(published.velocity.wz_radps) &&
           fabsf(published.velocity.vx_mps) <= 0.02f && fabsf(published.velocity.vy_mps) <= 0.02f &&
           fabsf(published.velocity.wz_radps) <= 0.1f;
}
int chassis_port_parameter_set(uint16_t id, float value) {
    chassis_parameters_t next;
    uint32_t revision;
    chassis_port_parameters(&next, &revision);
    (void)revision;
    int r = cp_set(&next, id, value);
    if (r)
        return r == -2 ? -4 : -2;
    taskENTER_CRITICAL();
    if (maintenance || config_pending || published.state == CHASSIS_ACTIVE || !stopped_feedback() ||
        uxQueueMessagesWaiting(commands))
        r = -3;
    else {
        r = xQueueSend(config_updates, &next, 0) == pdPASS ? 0 : -1;
        if (!r)
            config_pending = 1;
    }
    taskEXIT_CRITICAL();
    return r;
}
int chassis_port_maintenance_begin(void) {
    int r = 0;
    taskENTER_CRITICAL();
    if (maintenance || config_pending || published.state == CHASSIS_ACTIVE || !stopped_feedback() ||
        !(published.voltage > published_parameters.control.min_voltage) ||
        uxQueueMessagesWaiting(commands))
        r = -1;
    else {
        maintenance = 1;
        maintenance_ready = 0;
    }
    taskEXIT_CRITICAL();
    return r;
}
int chassis_port_maintenance_ready(void) {
    int r;
    taskENTER_CRITICAL();
    r = maintenance_ready;
    taskEXIT_CRITICAL();
    return r;
}
void chassis_port_maintenance_end(void) {
    taskENTER_CRITICAL();
    xQueueReset(commands);
    maintenance = 0;
    maintenance_ready = 0;
    taskEXIT_CRITICAL();
}
uint32_t chassis_port_overruns(void) {
    taskENTER_CRITICAL();
    uint32_t result = overruns;
    taskEXIT_CRITICAL();
    return result;
}
