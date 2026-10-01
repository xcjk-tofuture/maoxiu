#include "platform_time.h"
#include "main.h"
uint32_t platform_millis(void) {return HAL_GetTick();}
