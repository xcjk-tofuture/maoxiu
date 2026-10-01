#include "cmsis_os.h"
#include "log_service.h"
#include "log_port.h"
osThreadId BleUart3TaskHandle;
void Ble_Uart3_Task_Proc(void const *argument) {
    uint8_t bytes[64];(void)argument;
    for(;;){size_t count=maoxiu_log_receive(bytes,sizeof(bytes),1000);if(count)maoxiu_log_port_send(bytes,count);}
}
