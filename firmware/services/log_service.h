#ifndef MAOXIU_LOG_SERVICE_H
#define MAOXIU_LOG_SERVICE_H
#include <stdint.h>
#include <stddef.h>
/* Task-context API. Init before task creation. Emit never blocks and drops on full.
 * One low-priority consumer owns UART3; startup logs before init are discarded. */
int maoxiu_log_init(void);
void maoxiu_log_byte(uint8_t byte);
size_t maoxiu_log_receive(uint8_t *bytes, size_t capacity, uint32_t timeout_ms);
#endif
