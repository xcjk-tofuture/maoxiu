#include "FreeRTOS.h"
#include "task.h"
#include "key_events.h"
#include "board_devices.h"
void RGB_Task_Proc(void const *argument) {
    uint8_t current = 1, event;
    (void)argument;
    for (;;) {
        if (tm4c_key_event_wait(&event, 1000) == 0)
            current ^= event;
        else {
            current = (uint8_t)(current * 2);
            if (current > 4 || current == 0)
                current = 1;
        }
        tm4c_led_write(current);
    }
}
void RGB_Show_Proc(uint8_t bits) { tm4c_led_write(bits); }
