#include "FreeRTOS.h"
#include "task.h"
extern void app_fatal(void);
void vApplicationMallocFailedHook(void) {app_fatal();}
void vApplicationStackOverflowHook(TaskHandle_t task,char *name) {(void)task;(void)name;app_fatal();}
