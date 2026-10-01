#ifndef MAOXIU_MOTOR_HAL_H
#define MAOXIU_MOTOR_HAL_H
#include <stdint.h>
/* Private platform sample used only by the motor driver, control task context. */
typedef struct {uint8_t Num,Direction;int32_t Trig;float Speed;int16_t SetSpeedRange;int8_t SetDirection;} maoxiu_motor_hal_sample_t;
void maoxiu_motor_hal_init(void);
void maoxiu_motor_hal_encoder_init(void);
void maoxiu_motor_hal_read(maoxiu_motor_hal_sample_t *sample);
void maoxiu_motor_hal_write(maoxiu_motor_hal_sample_t *sample,int16_t pwm);
#endif
