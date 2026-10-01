#include "pc_proc.h"
#include "platform_time.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "cmsis_os.h"
#include "serial_port.h"
#include "star_dispatch.h"
#include <string.h>
#include "chassis_port.h"
#include "platform_time.h"
#include "mathTool.h"
extern Vector3i_t accOriCal, gryoOriCal;
osThreadId PCTaskHandle;
typedef struct {
    uint16_t length;
    uint8_t data[200];
} rx_packet_t;
static QueueHandle_t receive_queue;
static volatile uint8_t receive_lost;
static star_parser_t parser;
static star_dispatch_t dispatcher;
static uint16_t event_sequence;
extern uint8_t PCdatabufRx[200];
static uint8_t business(void *context, const star_frame_t *q, star_frame_t *r) {
    (void)context;

    chassis_snapshot_t s;
    chassis_velocity_t v;
    int result;
    Vector3i_t acc, gyro;
    if (q->command == STAR_CMD_STATUS) {
        if (q->length)
            return STAR_BAD_LENGTH;
        chassis_port_snapshot(&s);
        taskENTER_CRITICAL();
        acc = accOriCal;
        gyro = gryoOriCal;
        taskEXIT_CRITICAL();
        r->payload[1] = s.state;
        star_write_f32(r->payload + 2, s.velocity.vx_mps);
        star_write_f32(r->payload + 6, s.velocity.vy_mps);
        star_write_f32(r->payload + 10, s.velocity.wz_radps);
        r->payload[14] = s.wheels;
        star_write_f32(r->payload + 15, s.voltage);
        star_write_u16(r->payload + 19, (uint16_t)acc.x);
        star_write_u16(r->payload + 21, (uint16_t)acc.y);
        star_write_u16(r->payload + 23, (uint16_t)acc.z);
        star_write_u16(r->payload + 25, (uint16_t)gyro.x);
        star_write_u16(r->payload + 27, (uint16_t)gyro.y);
        star_write_u16(r->payload + 29, (uint16_t)gyro.z);
        r->length = 31;
        return STAR_OK;
    }
    if (q->command != STAR_CMD_CHASSIS_VELOCITY)
        return STAR_UNSUPPORTED;
    if (q->length != 12)
        return STAR_BAD_LENGTH;
    v.vx_mps = star_read_f32(q->payload);
    v.vy_mps = star_read_f32(q->payload + 4);
    v.wz_radps = star_read_f32(q->payload + 8);
    result = chassis_port_submit(v);
    return result == 0    ? STAR_OK
           : result == -2 ? STAR_RANGE
           : result == -3 ? STAR_STATE
                          : STAR_BUSY;
}
int PC_Init(void) {
    receive_queue = xQueueCreate(4, sizeof(rx_packet_t));
    star_parser_init(&parser);
    dispatcher.device_id = 0x4d530001u;
    dispatcher.capabilities = STAR_CAP_STATUS | STAR_CAP_TELEMETRY_PERIOD | STAR_CAP_CHASSIS;
    dispatcher.telemetry_period_ms = 100;
    dispatcher.minimum_period_ms = 50;
    dispatcher.business = business;
    dispatcher.context = NULL;
    return receive_queue ? 0 : -1;
}
/* UART RX ISR: copy bounded bytes before DMA reuses PCdatabufRx. No decoding,
 * printf, transmission, or control writes in this interrupt. Priority must
 * be numerically >= configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY. */
void PC_Data_Rx_Proc(uint16_t size) {
    rx_packet_t packet;
    BaseType_t woken = pdFALSE;
    if (!receive_queue || !size || size > 200)
        return;
    packet.length = size;
    memcpy(packet.data, PCdatabufRx, size);
    if (xQueueSendFromISR(receive_queue, &packet, &woken) != pdPASS)
        receive_lost = 1;
    portYIELD_FROM_ISR(woken);
}
static void send_frame(const star_frame_t *frame) {
    uint8_t bytes[STAR_FRAME_MAX];
    size_t length = star_encode(frame, bytes, sizeof(bytes));
    /* Only this task owns USART1 TX. Blocking transfer bounds buffer lifetime. */
    if (length && serial_port_send(bytes, length) != 0) {
        /* Dropped telemetry/response is recoverable; host retries with a new sequence. */
    }
}
static void on_frame(void *context, const star_frame_t *request) {
    star_frame_t response;
    (void)context;
    if (request->flags != STAR_REQUEST)
        return;
    star_dispatch(&dispatcher, request, &response);
    send_frame(&response);
}
void PC_Task_Proc(void const *argument) {
    rx_packet_t packet;
    star_frame_t request = {0}, event;
    uint32_t last_telemetry = platform_millis(), now;
    (void)argument;
    if (!receive_queue) {
        vTaskDelete(NULL);
        return;
    }
    for (;;) {
        if (receive_lost) {
            taskENTER_CRITICAL();
            xQueueReset(receive_queue);
            receive_lost = 0;
            taskEXIT_CRITICAL();
            star_parser_init(&parser);
        }
        if (xQueueReceive(receive_queue, &packet, pdMS_TO_TICKS(5)) == pdPASS)
            star_parser_feed(&parser, packet.data, packet.length, platform_millis(), on_frame,
                             NULL);
        now = platform_millis();
        star_parser_expire(&parser, now);
        if ((uint32_t)(now - last_telemetry) >= dispatcher.telemetry_period_ms) {
            request.command = STAR_CMD_STATUS;
            request.flags = STAR_REQUEST;
            request.sequence = event_sequence++;
            star_dispatch(&dispatcher, &request, &event);
            event.flags = STAR_EVENT;
            send_frame(&event);
            last_telemetry = now;
        }
    }
}
