#ifndef STAR_PC_SERVICE_H
#define STAR_PC_SERVICE_H
#include <stdint.h>
/* Startup before scheduler: 0 success, -1 queue creation failure. */
int PC_Init(void);
/* UART RX interrupt only; copies 1..configured DMA-capacity bytes. */
void PC_Data_Rx_Proc(uint16_t size);
void PC_Task_Proc(void const *argument);
#endif
