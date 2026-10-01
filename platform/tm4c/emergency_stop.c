#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/pwm.h"
void tm4c_emergency_stop(void) {
    if (SysCtlPeripheralReady(SYSCTL_PERIPH_PWM0))
        PWMOutputState(PWM0_BASE, 0x3fu, 0);
}
