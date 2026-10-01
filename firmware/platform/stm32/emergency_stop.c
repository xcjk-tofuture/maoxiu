#include "main.h"
void platform_emergency_stop(void) {
    __disable_irq();
    if (RCC->APB2ENR & RCC_APB2ENR_TIM1EN) {
        TIM1->CCR1 = 0;
        TIM1->CCR2 = 0;
        TIM1->CCR3 = 0;
        TIM1->CCR4 = 0;
    }
    if (RCC->APB2ENR & RCC_APB2ENR_TIM9EN) {
        TIM9->CCR1 = 0;
        TIM9->CCR2 = 0;
    }
    if (RCC->APB2ENR & RCC_APB2ENR_TIM10EN)
        TIM10->CCR1 = 0;
    if (RCC->APB2ENR & RCC_APB2ENR_TIM11EN)
        TIM11->CCR1 = 0;
}
