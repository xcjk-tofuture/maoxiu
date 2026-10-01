#include "include.h"
#include "maoxiu_motor.h"
void tm4c_motor_port_write(unsigned motor,int value) {
    uint32_t period,pulse,base;
    if(motor>1) return;
    if(value>9999)value=9999;if(value< -9999)value= -9999;
    if(value==0){PWMOutputState(PWM0_BASE,3u<<(motor*2u),false);return;}
    PWMOutputState(PWM0_BASE,3u<<(motor*2u),true);
    period=PWMGenPeriodGet(PWM0_BASE,motor?PWM_GEN_1:PWM_GEN_0);
    base=motor?PWM_OUT_2:PWM_OUT_0;
    /* Avoid the legacy abs(0)-1 unsigned underflow. */
    pulse=(uint32_t)(((uint64_t)period*(unsigned)(value<0?-value:value))/10000u);
    if(pulse<1)pulse=1;if(period>1 && pulse>=period)pulse=period-1;
    PWMPulseWidthSet(PWM0_BASE,base,value>0?pulse:1);
    PWMPulseWidthSet(PWM0_BASE,base+1,value<0?pulse:1);
}

void tm4c_motor_port_read(int32_t counts[2]) {
    counts[0]=(int32_t)QEIVelocityGet(QEI0_BASE)*QEIDirectionGet(QEI0_BASE);
    counts[1]=(int32_t)QEIVelocityGet(QEI1_BASE)*QEIDirectionGet(QEI1_BASE);
}
