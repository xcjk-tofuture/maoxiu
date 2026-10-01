#ifndef MAOXIU_TM4C_MOTOR_H
#define MAOXIU_TM4C_MOTOR_H
void maoxiu_tm4c_motor_write(unsigned motor, int pwm);
void maoxiu_tm4c_motor_read(float speed[4]);
#endif
