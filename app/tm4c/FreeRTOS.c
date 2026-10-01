#include "include.h"
#include "pc_service.h"
#include "chassis_port.h"
#include "key_events.h"
extern void RGB_Task_Proc(void const *);
extern void Key_Task_Proc(void const *);
extern void LCD_Task_Proc(void const *);
extern void Buzzer_Task_Proc(void const *);
extern void MPU6050_Task_Proc(void const *);
extern void tm4c_fatal(void);
static void failed(void) { tm4c_fatal(); }
void FREERTOS_Init(void) {
    if (chassis_port_init() != 0 || maoxiu_pc_init() != 0 || tm4c_key_events_init() != 0)
        failed();
    if (xTaskCreate(maoxiu_control_task, "Control", 512, NULL, 3, NULL) != pdPASS)
        failed();
    if (xTaskCreate(maoxiu_pc_task, "PC", 768, NULL, 2, NULL) != pdPASS)
        failed();
    if (xTaskCreate((TaskFunction_t)Buzzer_Task_Proc, "Buzzer", 128, NULL, 1, NULL) != pdPASS)
        failed();
    /* Retain existing display/key/IMU functions; their hardware regression is pending. */
    if (xTaskCreate((TaskFunction_t)LCD_Task_Proc, "LCD", 512, NULL, 1, NULL) != pdPASS)
        failed();
    if (xTaskCreate((TaskFunction_t)RGB_Task_Proc, "RGB", 128, NULL, 1, NULL) != pdPASS)
        failed();
    if (xTaskCreate((TaskFunction_t)MPU6050_Task_Proc, "IMU", 256, NULL, 1, NULL) != pdPASS)
        failed();
}
