#include "log_service.h"
#include "FreeRTOS.h"
#include "queue.h"
static QueueHandle_t bytes;
int maoxiu_log_init(void) {bytes=xQueueCreate(256,sizeof(uint8_t));return bytes?0:-1;}
void maoxiu_log_byte(uint8_t byte) {if(bytes)xQueueSend(bytes,&byte,0);}
size_t maoxiu_log_receive(uint8_t *out,size_t capacity,uint32_t timeout_ms) {
    if(!out || !capacity || !bytes)return 0;
    if(xQueueReceive(bytes,out,pdMS_TO_TICKS(timeout_ms))!=pdPASS)return 0;
    size_t count=1;
    while(count<capacity && xQueueReceive(bytes,out+count,0)==pdPASS)count++;
    return count;
}
