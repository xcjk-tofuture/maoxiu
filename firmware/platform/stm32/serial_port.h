#ifndef STAR_SERIAL_PORT_H
#define STAR_SERIAL_PORT_H
#include <stdint.h>
#include <stddef.h>
/* One communication task owns UART TX. Returns 0 success; blocking <=20ms. */
int serial_port_send(const uint8_t *bytes, size_t length);
#endif
