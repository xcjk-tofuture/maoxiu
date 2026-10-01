#include "key_events.h"
#include "FreeRTOS.h"
#include "queue.h"
static QueueHandle_t events;
int tm4c_key_events_init(void) {
    events = xQueueCreate(4, 1);
    return events ? 0 : -1;
}
void tm4c_key_event_isr(uint8_t bits) {
    if (!events)
        return;
    BaseType_t wake = pdFALSE;
    xQueueSendFromISR(events, &bits, &wake);
    portYIELD_FROM_ISR(wake);
}
int tm4c_key_event_wait(uint8_t *bits, uint32_t ms) {
    return xQueueReceive(events, bits, pdMS_TO_TICKS(ms)) == pdPASS ? 0 : -1;
}
