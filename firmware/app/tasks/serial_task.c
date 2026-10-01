#include "pc_proc.h"
#include "platform_time.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "cmsis_os.h"
#include "serial_port.h"
#include "star_dispatch.h"
#include <string.h>
#include "parameter_flash.h"
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
static volatile uint32_t rx_lost_count;
static uint32_t tx_failed_count, saved_sequence;
static uint8_t boot_parameters_loaded;
extern uint8_t PCdatabufRx[200];
static uint8_t business(void *context, const star_frame_t *q, star_frame_t *r) {
    (void)context;

    chassis_snapshot_t s;
    chassis_velocity_t v;
    int result;
    Vector3i_t acc, gyro;

    if (q->command == STAR_CMD_PARAM_READ) {
        if (q->length != 2)
            return STAR_BAD_LENGTH;
        chassis_parameters_t p;
        float value;
        chassis_port_parameters(&p, NULL);
        if (cp_get(&p, star_read_u16(q->payload), &value))
            return STAR_UNSUPPORTED;
        star_write_f32(r->payload + 1, value);
        r->length = 5;
        return STAR_OK;
    }
    if (q->command == STAR_CMD_PARAM_WRITE) {
        if (q->length != 6)
            return STAR_BAD_LENGTH;
        result =
            chassis_port_parameter_set(star_read_u16(q->payload), star_read_f32(q->payload + 2));
        return result == 0    ? STAR_OK
               : result == -2 ? STAR_RANGE
               : result == -3 ? STAR_STATE
               : result == -4 ? STAR_UNSUPPORTED
                              : STAR_BUSY;
    }
    if (q->command ==
        STAR_CMD_CHASSIS_SAVE) { /* Explicit durable save; never writes during motion. */
        if (q->length)
            return STAR_BAD_LENGTH;
        if (chassis_port_maintenance_begin())
            return STAR_STATE;
        uint32_t began = platform_millis();
        while (!chassis_port_maintenance_ready() && (uint32_t)(platform_millis() - began) < 100)
            osDelay(1);
        if (!chassis_port_maintenance_ready()) {
            chassis_port_maintenance_end();
            return STAR_BUSY;
        }
        chassis_parameters_t p;
        uint8_t bytes[CP_BYTES];
        chassis_port_parameters(&p, NULL);
        p.telemetry_period_ms = dispatcher.telemetry_period_ms;
        cp_encode(&p, bytes);
        result = pj_save(parameter_flash_io(), 1, 1, bytes, sizeof(bytes), &saved_sequence);
        chassis_port_maintenance_end();
        return result == PJ_OK ? STAR_OK : STAR_INTERNAL;
    }
    if (q->command == STAR_CMD_CHASSIS_DIAGNOSTICS) {
        if (q->length)
            return STAR_BAD_LENGTH;
        chassis_parameters_t p;
        uint32_t revision;
        chassis_port_parameters(&p, &revision);
        star_write_u32(r->payload + 1, revision);
        star_write_u32(r->payload + 5, saved_sequence);
        star_write_u32(r->payload + 9, rx_lost_count);
        star_write_u32(r->payload + 13, parser.rejected);
        star_write_u32(r->payload + 17, parser.timed_out);
        star_write_u32(r->payload + 21, tx_failed_count);
        star_write_u32(r->payload + 25, chassis_port_overruns());
        r->payload[29] = boot_parameters_loaded;
        r->length = 30;
        return STAR_OK;
    }
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
    dispatcher.capabilities = STAR_CAP_STATUS | STAR_CAP_TELEMETRY_PERIOD | STAR_CAP_CHASSIS |
                              STAR_CAP_PARAMETER_STORE | STAR_CAP_DIAGNOSTICS;
    pj_value_t stored;
    chassis_parameters_t parameters;
    if (pj_load(parameter_flash_io(), 1, 1, &stored) == PJ_OK && stored.length == CP_BYTES &&
        cp_decode(&parameters, stored.payload) == 0) {
        saved_sequence = stored.sequence;
        boot_parameters_loaded = 1;
    }
    dispatcher.telemetry_period_ms = 100;
    chassis_port_parameters(&parameters, NULL);
    dispatcher.telemetry_period_ms = parameters.telemetry_period_ms;
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
    if (xQueueSendFromISR(receive_queue, &packet, &woken) != pdPASS) {
        receive_lost = 1;
        rx_lost_count++;
    }
    portYIELD_FROM_ISR(woken);
}
static void send_frame(const star_frame_t *frame) {
    uint8_t bytes[STAR_FRAME_MAX];
    size_t length = star_encode(frame, bytes, sizeof(bytes));
    /* Only this task owns USART1 TX. Blocking transfer bounds buffer lifetime. */
    if (length && serial_port_send(bytes, length) != 0) {
        tx_failed_count++; /* Host retries with a new sequence. */
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
            uint32_t rejected = parser.rejected, timed_out = parser.timed_out;
            star_parser_init(&parser);
            parser.rejected = rejected;
            parser.timed_out = timed_out;
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
