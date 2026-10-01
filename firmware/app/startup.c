#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os.h"
#include "main.h"
#include "chassis_port.h"
#include "pc_proc.h"
#include "log_service.h"
extern osThreadId KeyTaskHandle;
extern void Key_Task_Proc(void const *argument);
extern osThreadId RGBTaskHandle;
extern void RGB_Task_Proc(void const *argument);
extern osThreadId PCTaskHandle;
extern void PC_Task_Proc(void const *argument);
extern osThreadId LCDTaskHandle;
extern void LCD_Task_Proc(void const *argument);
extern osThreadId EncoderTaskHandle;
extern void Encoder_Task_Proc(void const *argument);
extern osThreadId BleUart3TaskHandle;
extern void Ble_Uart3_Task_Proc(void const *argument);
extern osThreadId IMUTaskHandle;
extern void IMU_Task_Proc(void const *argument);
void app_tasks_init(void) {
    if (chassis_port_init()!=0 || PC_Init()!=0 || maoxiu_log_init()!=0) Error_Handler();
    osThreadDef(Ble,Ble_Uart3_Task_Proc,osPriorityIdle,0,256);
    BleUart3TaskHandle=osThreadCreate(osThread(Ble),NULL);
    if(!BleUart3TaskHandle) Error_Handler();
    osThreadDef(Key,Key_Task_Proc,osPriorityIdle,0,128);
    KeyTaskHandle=osThreadCreate(osThread(Key),NULL);
    if(!KeyTaskHandle) Error_Handler();
    osThreadDef(RGB,RGB_Task_Proc,osPriorityIdle,0,128);
    RGBTaskHandle=osThreadCreate(osThread(RGB),NULL);
    if(!RGBTaskHandle) Error_Handler();
    osThreadDef(PC,PC_Task_Proc,osPriorityNormal,0,768);
    PCTaskHandle=osThreadCreate(osThread(PC),NULL);
    if(!PCTaskHandle) Error_Handler();
    osThreadDef(LCD,LCD_Task_Proc,osPriorityIdle,0,512);
    LCDTaskHandle=osThreadCreate(osThread(LCD),NULL);
    if(!LCDTaskHandle) Error_Handler();
    osThreadDef(Control,Encoder_Task_Proc,osPriorityHigh,0,512);
    EncoderTaskHandle=osThreadCreate(osThread(Control),NULL);
    if(!EncoderTaskHandle) Error_Handler();
    osThreadDef(IMU,IMU_Task_Proc,osPriorityLow,0,256);
    IMUTaskHandle=osThreadCreate(osThread(IMU),NULL);
    if(!IMUTaskHandle) Error_Handler();
}
