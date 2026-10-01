#include "FreeRTOS.h"
#include "task.h"
extern void tm4c_emergency_stop(void);
void tm4c_fatal(void) {
    __asm volatile("cpsid i" ::: "memory");
    tm4c_emergency_stop();
    for (;;) {
    }
}
void vApplicationMallocFailedHook(void) { tm4c_fatal(); }
void vApplicationStackOverflowHook(TaskHandle_t task, char *name) {
    (void)task;
    (void)name;
    tm4c_fatal();
}
