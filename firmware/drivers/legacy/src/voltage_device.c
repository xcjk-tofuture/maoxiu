#include "voltage_device.h"
#include "board_io.h"
#include "maoxiu_board.h"
int maoxiu_voltage_init(void) { return maoxiu_adc_start(); }
float maoxiu_voltage_read(void) { return (float)maoxiu_adc_sample() * MAOXIU_ADC_VOLTS_PER_COUNT; }
