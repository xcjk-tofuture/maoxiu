#include "log_port.h"
#include "usart.h"
int maoxiu_log_port_send(const uint8_t *bytes, size_t length) {
    if (!bytes || length > 64)
        return -1;
    return HAL_UART_Transmit(&huart3, (uint8_t *)bytes, (uint16_t)length, 20) == HAL_OK ? 0 : -1;
}
