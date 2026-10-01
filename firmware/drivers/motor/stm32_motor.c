#include "motor_device.h"
#include "motor_hal.h"
static maoxiu_motor_hal_sample_t motors[4];
void maoxiu_motor_init(void) {
    for(unsigned i=0;i<4;i++)motors[i].Num=(uint8_t)i;
    maoxiu_motor_hal_init();maoxiu_motor_hal_encoder_init();
}
void maoxiu_motor_read(float speeds[4]) {
    if(!speeds)return;
    for(unsigned i=0;i<4;i++){maoxiu_motor_hal_read(&motors[i]);speeds[i]=motors[i].Speed;}
}
void maoxiu_motor_write(unsigned index,int16_t pwm) {if(index<4)maoxiu_motor_hal_write(&motors[index],pwm);}
