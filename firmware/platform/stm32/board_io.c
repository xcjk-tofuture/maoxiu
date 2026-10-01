#include "board_io.h"
#include "adc.h"
#include "gpio.h"
/* ADC DMA owns this aligned word. Readers load one coherent volatile sample. */
static volatile uint32_t voltage_sample;
int maoxiu_adc_start(void) {
    return HAL_ADC_Start_DMA(&hadc1, (uint32_t *)&voltage_sample, 1) == HAL_OK ? 0 : -1;
}
uint32_t maoxiu_adc_sample(void) { return voltage_sample; }
uint8_t maoxiu_board_key_scan(void) {
    if (HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_0) == GPIO_PIN_RESET)
        return 2;
    if (HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_1) == GPIO_PIN_RESET)
        return 1;
    return 0;
}
void maoxiu_board_rgb_write(uint8_t mask) {
    HAL_GPIO_WritePin(GPIOD, RGB_red_Pin, (mask & 4u) ? GPIO_PIN_RESET : GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOD, RGB_green_Pin, (mask & 2u) ? GPIO_PIN_RESET : GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOD, RGB_blue_Pin, (mask & 1u) ? GPIO_PIN_RESET : GPIO_PIN_SET);
}
