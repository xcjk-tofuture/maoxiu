#ifndef MAOXIU_MOTOR_DEVICE_H
#define MAOXIU_MOTOR_DEVICE_H
#include <stdint.h>
/* Sole owner: fixed-period control task. Init/read/write do not allocate or wait.
 * Speeds are m/s. Index 0..3; PWM is signed, clamped by board period. */
void maoxiu_motor_init(void);
void maoxiu_motor_read(float speeds_mps[4]);
void maoxiu_motor_write(unsigned index, int16_t pwm);
#endif
