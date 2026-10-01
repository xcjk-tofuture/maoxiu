#include <stdint.h>
#include "maoxiu_motor.h"
#include "maoxiu_board.h"
extern void tm4c_motor_port_write(unsigned motor, int pwm);
extern void tm4c_motor_port_read(int32_t counts[2]);
void maoxiu_tm4c_motor_write(unsigned motor, int pwm) { tm4c_motor_port_write(motor, pwm); }
void maoxiu_tm4c_motor_read(float speed[4]) {
    int32_t counts[2];
    tm4c_motor_port_read(counts);
    speed[0] = counts[0] * MAOXIU_TM4C_METRES_PER_ENCODER_COUNT * 100.0f;
    speed[1] = counts[1] * MAOXIU_TM4C_METRES_PER_ENCODER_COUNT * 100.0f;
    speed[2] = speed[3] = 0;
}
