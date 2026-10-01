#ifndef MAOXIU_BOARD_IO_H
#define MAOXIU_BOARD_IO_H
#include <stdint.h>
int maoxiu_adc_start(void);
uint32_t maoxiu_adc_sample(void);
uint8_t maoxiu_board_key_scan(void);
void maoxiu_board_rgb_write(uint8_t rgb_mask);
#endif
