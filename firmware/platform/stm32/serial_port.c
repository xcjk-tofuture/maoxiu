#include "serial_port.h"
#include "usart.h"
int serial_port_send(const uint8_t *bytes, size_t length) {
    if (!bytes || length > 140)
        return -1;
    return HAL_UART_Transmit(&huart1, (uint8_t *)bytes, (uint16_t)length, 20) == HAL_OK ? 0 : -1;
}
