#ifndef TM4C_BOARD_DEVICES_H
#define TM4C_BOARD_DEVICES_H
#include <stdint.h>
int tm4c_voltage_raw(uint16_t *out);
void tm4c_buzzer_init(void);
void tm4c_buzzer_tone(uint16_t hz, int enabled);
void tm4c_led_write(uint8_t bits);
#endif
