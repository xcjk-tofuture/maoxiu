#ifndef TM4C_KEY_EVENTS_H
#define TM4C_KEY_EVENTS_H
#include <stdint.h>
int tm4c_key_events_init(void);
void tm4c_key_event_isr(uint8_t bits);
int tm4c_key_event_wait(uint8_t *bits, uint32_t timeout_ms);
#endif
