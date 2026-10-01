#include <stdint.h>
void SystemInit(void) {
    /* Enable CP10/CP11 before any hard-float C code; clock is set by existing BSP. */
    *(volatile uint32_t *)0xE000ED88 |= (0xFu << 20);
    __asm volatile("dsb\nisb" ::: "memory");
}
