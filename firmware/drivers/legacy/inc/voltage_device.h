#ifndef MAOXIU_VOLTAGE_DEVICE_H
#define MAOXIU_VOLTAGE_DEVICE_H
/* Called once by control owner before reading volts. Init returns -1 on DMA failure.
 * Read is a nonblocking task-context load; raw ADC scaling comes from the board. */
int maoxiu_voltage_init(void);
float maoxiu_voltage_read(void);
#endif
