#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "driverlib/gpio.h"
#include "key_events.h"
void KEY_EXIT_Handler(void) {
    uint32_t state = GPIOIntStatus(GPIO_PORTF_BASE, true);
    GPIOIntClear(GPIO_PORTF_BASE, state);
    tm4c_key_event_isr((uint8_t)(((state & 1u) ? 4u : 0u) | ((state & 16u) ? 1u : 0u)));
}
