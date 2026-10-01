#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "cmsis_os.h"
#include "motor_device.h"
#include "chassis_port.h"
#include "platform_time.h"
#include "maoxiu_board.h"
#include <math.h>
osThreadId MotorTaskHandle;
osThreadId EncoderTaskHandle;
static QueueHandle_t commands;
static chassis_service_t service;
static chassis_snapshot_t published;
typedef struct {chassis_velocity_t target;uint32_t submitted_ms;} command_t;

#include "voltage_device.h"
extern void Error_Handler(void);
int chassis_port_init(void) {
    const chassis_config_t config={4,MAOXIU_HALF_SPAN_M,MAOXIU_KP,MAOXIU_KI,
        MAOXIU_PWM_LIMIT,MAOXIU_MIN_VOLTAGE,MAOXIU_COMMAND_TIMEOUT_MS};
    chassis_init(&service,&config);
    commands=xQueueCreate(4,sizeof(command_t));
    return commands?0:-1;
}
int chassis_port_submit(chassis_velocity_t v) {
    command_t command;chassis_snapshot_t state;
    if(!isfinite(v.vx_mps)||!isfinite(v.vy_mps)||!isfinite(v.wz_radps) ||
        fabsf(v.vx_mps)>MAOXIU_MAX_LINEAR_MPS || fabsf(v.vy_mps)>MAOXIU_MAX_LINEAR_MPS ||
        fabsf(v.wz_radps)>MAOXIU_MAX_ANGULAR_RADPS) return -2;
    chassis_port_snapshot(&state);
    if(!(state.voltage>MAOXIU_MIN_VOLTAGE)) return -3;
    command.target=v;command.submitted_ms=platform_millis();
    return commands && xQueueSend(commands,&command,0)==pdPASS?0:-1;
}
void chassis_port_snapshot(chassis_snapshot_t *s) {
    taskENTER_CRITICAL();*s=published;taskEXIT_CRITICAL();
}
void Encoder_Task_Proc(void const *argument) {
    TickType_t wake=xTaskGetTickCount();command_t command;float speed[4],pwm[4];unsigned i,divider=0;
    (void)argument;
    if(!commands) {vTaskDelete(NULL);return;}
    if(maoxiu_voltage_init()!=0) Error_Handler();
    maoxiu_motor_init();
    for(;;) {
        vTaskDelayUntil(&wake,pdMS_TO_TICKS(MAOXIU_CONTROL_MS));
        while(xQueueReceive(commands,&command,0)==pdPASS)
            chassis_submit(&service,command.target,command.submitted_ms);
        maoxiu_motor_read(speed);
        chassis_step(&service,speed,maoxiu_voltage_read(),platform_millis(),pwm);
        /* One owner, preserving the prior 20 ms output cadence. Faults stop immediately. */
        if(++divider>=MAOXIU_OUTPUT_DIVIDER || service.snapshot.state!=CHASSIS_ACTIVE) {
            divider=0;for(i=0;i<4;i++) maoxiu_motor_write(i,(int16_t)pwm[i]);
        }
        taskENTER_CRITICAL();published=service.snapshot;taskEXIT_CRITICAL();
    }
}
