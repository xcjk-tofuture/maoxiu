#include "FreeRTOS.h"
#include "task.h"
#include "board_devices.h"
#include "chassis_port.h"
static void tone(uint16_t hz) {
    tm4c_buzzer_tone(hz, 1);
    vTaskDelay(pdMS_TO_TICKS(200));
    tm4c_buzzer_tone(hz, 0);
}
void Buzzer_Task_Proc(void const *argument) {
    static const uint16_t startup[] = {523, 587, 659, 523, 587, 659, 784, 698};
    static const uint16_t alarm[] = {659, 587, 523, 659, 587, 523};
    (void)argument;
    tm4c_buzzer_init();
    for (unsigned i = 0; i < 8; i++)
        tone(startup[i]);
    for (;;) {
        chassis_snapshot_t state;
        chassis_port_snapshot(&state);
        if (state.voltage > 4.5f && state.voltage < 10.6f)
            for (unsigned i = 0; i < 6; i++)
                tone(alarm[i]);
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}
