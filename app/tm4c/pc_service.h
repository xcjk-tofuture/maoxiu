#ifndef MAOXIU_TM4C_PC_SERVICE_H
#define MAOXIU_TM4C_PC_SERVICE_H
#include <stdint.h>
int maoxiu_pc_init(void);
void maoxiu_pc_rx_isr(uint8_t byte);
void maoxiu_pc_task(void *argument);
#endif
