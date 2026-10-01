#include "include.h"
#include "queue.h"
#include "pc_service.h"
#include "chassis_port.h"
#include "star_dispatch.h"
static QueueHandle_t bytes;
static volatile uint8_t lost;
static star_parser_t parser;
static star_dispatch_t dispatcher;
static uint16_t sequence;
static uint32_t now_ms(void) {return (uint32_t)xTaskGetTickCount()*portTICK_PERIOD_MS;}
static uint8_t business(void *context,const star_frame_t *q,star_frame_t *r) {
    chassis_snapshot_t s;chassis_velocity_t v;int status;(void)context;
    if(q->command==STAR_CMD_STATUS) {
        if(q->length)return STAR_BAD_LENGTH;
        chassis_port_snapshot(&s);r->payload[1]=s.state;
        star_write_f32(r->payload+2,s.velocity.vx_mps);star_write_f32(r->payload+6,s.velocity.vy_mps);
        star_write_f32(r->payload+10,s.velocity.wz_radps);r->payload[14]=2;
        star_write_f32(r->payload+15,s.voltage);memset(r->payload+19,0,12);r->length=31;return STAR_OK;
    }
    if(q->command!=STAR_CMD_CHASSIS_VELOCITY)return STAR_UNSUPPORTED;
    if(q->length!=12)return STAR_BAD_LENGTH;
    v.vx_mps=star_read_f32(q->payload);v.vy_mps=star_read_f32(q->payload+4);v.wz_radps=star_read_f32(q->payload+8);
    status=chassis_port_submit(v);return status==0?STAR_OK:status==-2?STAR_RANGE:status==-3?STAR_STATE:STAR_BUSY;
}
int maoxiu_pc_init(void) {
    bytes=xQueueCreate(256,sizeof(uint8_t));star_parser_init(&parser);
    dispatcher.device_id=0x4d540001u;dispatcher.capabilities=STAR_CAP_STATUS|STAR_CAP_TELEMETRY_PERIOD;
    dispatcher.minimum_period_ms=50;dispatcher.telemetry_period_ms=100;dispatcher.business=business;
    return bytes?0:-1;
}
void maoxiu_pc_rx_isr(uint8_t byte) {
    BaseType_t woken=pdFALSE;
    if(!bytes)return;
    if(xQueueSendFromISR(bytes,&byte,&woken)!=pdPASS)lost=1;
    portYIELD_FROM_ISR(woken);
}
static void transmit(const star_frame_t *f) {
    uint8_t data[STAR_FRAME_MAX];size_t length=star_encode(f,data,sizeof(data)),i;
    uint32_t start=now_ms();
    for(i=0;i<length;i++) {
        while(!UARTCharPutNonBlocking(UART0_BASE,data[i])) {
            if((uint32_t)(now_ms()-start)>=20)return;vTaskDelay(1);
        }
    }
}
static void received(void *context,const star_frame_t *q) {
    star_frame_t r;(void)context;if(q->flags!=STAR_REQUEST)return;
    star_dispatch(&dispatcher,q,&r);transmit(&r);
}
void maoxiu_pc_task(void *argument) {
    uint8_t byte;uint32_t last=now_ms(),now;star_frame_t q={0},event;(void)argument;
    for(;;) {
        if(lost) {taskENTER_CRITICAL();xQueueReset(bytes);lost=0;taskEXIT_CRITICAL();star_parser_init(&parser);}
        if(xQueueReceive(bytes,&byte,pdMS_TO_TICKS(5))==pdPASS)star_parser_feed(&parser,&byte,1,now_ms(),received,NULL);
        now=now_ms();star_parser_expire(&parser,now);
        if((uint32_t)(now-last)>=dispatcher.telemetry_period_ms) {
            q.command=STAR_CMD_STATUS;q.sequence=sequence++;
            star_dispatch(&dispatcher,&q,&event);event.flags=STAR_EVENT;transmit(&event);last=now;
        }
    }
}
