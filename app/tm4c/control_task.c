#include <stdint.h>
#include "FreeRTOS.h"
#include "task.h"
#include "maoxiu_motor.h"
#include "board_devices.h"
extern void Motor_Init(void);
extern void Motor_Encoder_Init(void);
#include "queue.h"
#include "chassis_port.h"
#include "maoxiu_board.h"
#include <math.h>
static QueueHandle_t commands;
static chassis_service_t service;
static chassis_snapshot_t published;
typedef struct {
    chassis_velocity_t velocity;
    uint32_t time_ms;
} command_t;
static uint32_t now_ms(void) { return (uint32_t)xTaskGetTickCount() * portTICK_PERIOD_MS; }
int chassis_port_init(void) {
    const chassis_config_t cfg = {2,
                                  MAOXIU_TM4C_CALIBRATED ? MAOXIU_TM4C_TRACK_M : 1.0f,
                                  MAOXIU_TM4C_KP,
                                  MAOXIU_TM4C_KI,
                                  9999,
                                  MAOXIU_TM4C_MIN_VOLTAGE,
                                  500};
    chassis_init(&service, &cfg);
    commands = xQueueCreate(4, sizeof(command_t));
    return commands ? 0 : -1;
}
int chassis_port_submit(chassis_velocity_t velocity) {
    command_t command;
    if (!isfinite(velocity.vx_mps) || !isfinite(velocity.vy_mps) || !isfinite(velocity.wz_radps) ||
        fabsf(velocity.vx_mps) > 2 || fabsf(velocity.wz_radps) > 6 || velocity.vy_mps != 0)
        return -2;
    if (!MAOXIU_TM4C_CALIBRATED)
        return -3;
    command.velocity = velocity;
    command.time_ms = now_ms();
    return commands && xQueueSend(commands, &command, 0) == pdPASS ? 0 : -1;
}
void chassis_port_snapshot(chassis_snapshot_t *s) {
    taskENTER_CRITICAL();
    *s = published;
    taskEXIT_CRITICAL();
}
void maoxiu_control_task(void *argument) {
    TickType_t wake = xTaskGetTickCount();
    command_t command;
    float measured[4] = {0}, output[4];
    (void)argument;
    Motor_Init();
    maoxiu_tm4c_motor_write(0, 0);
    maoxiu_tm4c_motor_write(1, 0);
    Motor_Encoder_Init();
    for (;;) {
        vTaskDelayUntil(&wake, pdMS_TO_TICKS(MAOXIU_TM4C_CONTROL_MS));
        while (xQueueReceive(commands, &command, 0) == pdPASS)
            chassis_submit(&service, command.velocity, command.time_ms);
        maoxiu_tm4c_motor_read(measured);
        uint16_t raw_voltage = 0;
        float voltage = tm4c_voltage_raw(&raw_voltage) == 0
                            ? raw_voltage * MAOXIU_TM4C_ADC_VOLTS_PER_COUNT
                            : 0.0f;
        chassis_step(&service, measured, voltage, now_ms(), output);
        if (!MAOXIU_TM4C_CALIBRATED) {
            service.snapshot.state = CHASSIS_UNCALIBRATED;
            for (unsigned i = 0; i < 4; i++)
                output[i] = 0;
        }
        maoxiu_tm4c_motor_write(0, (int)output[0]);
        maoxiu_tm4c_motor_write(1, (int)output[1]);
        taskENTER_CRITICAL();
        published = service.snapshot;
        taskEXIT_CRITICAL();
    }
}
