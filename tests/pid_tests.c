#include "pid.h"
#include <assert.h>
#include <math.h>
#include <stdio.h>
int main(void) {
    PID a={0},b={0};PID_DATA data={0};
    data.Kp=2;data.Ki=1;data.ErrorMax=100;data.IntegrateMax=5;data.DifferentialMax=100;
    assert(fabsf(PID_Control(&a,&data,0.1f,0,2,1,0)-2.1f)<0.0001f);
    for(unsigned i=0;i<100;i++)PID_Control(&a,&data,0.1f,0,2,1,0);
    assert(fabsf(a.Integrate-5)<0.0001f);
    assert(b.Integrate==0);PID_Reset_I(&a);assert(a.Integrate==0);
    assert(PID_Control(&a,&data,0,0,1,0,0)==0);
    assert(PID_Control(&a,&data,NAN,0,1,0,0)==0);
    assert(PID_Control(&a,&data,0.1f,0,NAN,0,0)==0);
    assert(PID_Control(&a,&data,0.1f,0,1,0,NAN)==0);
    puts("PASS: PID seconds, integral saturation/reset, finite inputs and independent instances");
    return 0;
}
