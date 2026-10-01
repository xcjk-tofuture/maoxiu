#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "driverlib/adc.h"
#include "driverlib/pwm.h"
#include "board_devices.h"
#include "driverlib/gpio.h"
#include "main.h"
extern void Buzzer_Init(void);
int tm4c_voltage_raw(uint16_t *out) {
    uint32_t value = 0, remaining = 10000;
    ADCIntClear(ADC0_BASE, 0);
    ADCProcessorTrigger(ADC0_BASE, 0);
    while (!ADCIntStatus(ADC0_BASE, 0, false) && --remaining) {
    }
    if (!remaining)
        return -1;
    ADCSequenceDataGet(ADC0_BASE, 0, &value);
    ADCIntClear(ADC0_BASE, 0);
    *out = (uint16_t)value;
    return 0;
}
void tm4c_buzzer_init(void) { Buzzer_Init(); }
void tm4c_buzzer_tone(uint16_t hz, int enabled) {
    if (hz) {
        PWMGenPeriodSet(PWM0_BASE, PWM_GEN_3, 1250000u / hz);
        PWMPulseWidthSet(PWM0_BASE, PWM_OUT_6, PWMGenPeriodGet(PWM0_BASE, PWM_GEN_3) / 2u - 1u);
    }
    PWMOutputState(PWM0_BASE, PWM_OUT_6_BIT, enabled != 0);
}

void tm4c_led_write(uint8_t bits) {
    GPIOPinWrite(RGB_R_GPIO_Port, RGB_R_Pin, (bits & 4) ? RGB_R_Pin : 0);
    GPIOPinWrite(RGB_B_GPIO_Port, RGB_B_Pin, (bits & 2) ? RGB_B_Pin : 0);
    GPIOPinWrite(RGB_G_GPIO_Port, RGB_G_Pin, (bits & 1) ? RGB_G_Pin : 0);
}
