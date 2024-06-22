/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
//#define portCONFIGURE_TIMER_FOR_RUN_TIME_STATS() ConfigureTimeForRunTimeStats()
//#define portGET_RUN_TIME_COUNTER_VALUE() FreeRTOSRunTimeTicks
#include "stdio.h"
#define XC_FREERTOS_MAKE_FLAG //编译标志位 彻底解决cube改代码需要删除东西


/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
#include "tim.h"
//#include "trcConfig.h"
//#include "trcKernelPort.h"
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
extern	osThreadId RGBTaskHandle;
extern	osThreadId BuzzerTaskHandle;
extern  osThreadId LCDTaskHandle;
extern osThreadId KeyTaskHandle;
extern osThreadId PCTaskHandle;
extern osThreadId ServoTaskHandle;
extern osThreadId MotorTaskHandle;
extern osThreadId EncoderTaskHandle;
extern osThreadId IMUTaskHandle;
extern osThreadId BleTaskHandle;
#ifndef XC_FREERTOS_MAKE_FLAG
/* USER CODE END Variables */
osThreadId KeyTaskHandle;
osThreadId RGBTaskHandle;
osThreadId PCTaskHandle;
osThreadId LCDTaskHandle;
osThreadId BuzzerTaskHandle;
osThreadId MotorTaskHandle;
osThreadId EncoderTaskHandle;
osThreadId BleTaskHandle;
osThreadId IMUTaskHandle;
osThreadId ServoTaskHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
#endif
extern void RGB_Task_Proc(void const * argument);
extern void Buzzer_Task_Proc(void const * argument);
extern void LCD_Task_Proc(void const * argument);
extern void Key_Task_Proc(void const * argument);
extern void PC_Task_Proc(void const * argument);
extern void Servo_Task_Proc(void const * argument);
extern void Motor_Task_Proc(void const * argument);
extern void Encoder_Task_Proc(void const * argument);
extern void IMU_Task_Proc(void const * argument);
extern void Ble_Task_Proc(void const * argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* GetIdleTaskMemory prototype (linked to static allocation support) */
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize );

/* GetTimerTaskMemory prototype (linked to static allocation support) */
void vApplicationGetTimerTaskMemory( StaticTask_t **ppxTimerTaskTCBBuffer, StackType_t **ppxTimerTaskStackBuffer, uint32_t *pulTimerTaskStackSize );

/* Hook prototypes */


#ifndef XC_FREERTOS_MAKE_FLAG
/* USER CODE END FunctionPrototypes */

void Key_Task_Proc(void const * argument);
void RGB_Task_Proc(void const * argument);
void PC_Task_Proc(void const * argument);
void LCD_Task_Proc(void const * argument);
void Buzzer_Task_Proc(void const * argument);
void Motor_Task_Proc(void const * argument);
void Encoder_Task_Proc(void const * argument);
void Ble_Task_Proc(void const * argument);
void IMU_Task_Proc(void const * argument);
void Servo_Task_Proc(void const * argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* GetIdleTaskMemory prototype (linked to static allocation support) */
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize );

/* GetTimerTaskMemory prototype (linked to static allocation support) */
void vApplicationGetTimerTaskMemory( StaticTask_t **ppxTimerTaskTCBBuffer, StackType_t **ppxTimerTaskStackBuffer, uint32_t *pulTimerTaskStackSize );

/* Hook prototypes */
void configureTimerForRunTimeStats(void);
unsigned long getRunTimeCounterValue(void);

/* USER CODE BEGIN 1 */
#endif
/* Functions needed when configGENERATE_RUN_TIME_STATS is on */

//void TaskFunction(void *pvParameters)
//{
//    vTaskSetApplicationTaskTag(NULL, __LINE__); // 设置任务标签
//    // 任务的其他代码
//}


__weak void configureTimerForRunTimeStats(void)
{
//	HAL_TIM_Base_Start_IT(&htim13);
//	TIM12Count = 0;
}

__weak unsigned long getRunTimeCounterValue(void)
{
	return 0;
}
/* USER CODE END 1 */

/* USER CODE BEGIN GET_IDLE_TASK_MEMORY */
static StaticTask_t xIdleTaskTCBBuffer;
static StackType_t xIdleStack[configMINIMAL_STACK_SIZE];

void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize )
{
  *ppxIdleTaskTCBBuffer = &xIdleTaskTCBBuffer;
  *ppxIdleTaskStackBuffer = &xIdleStack[0];
  *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
  /* place for user code */
}
/* USER CODE END GET_IDLE_TASK_MEMORY */

/* USER CODE BEGIN GET_TIMER_TASK_MEMORY */
static StaticTask_t xTimerTaskTCBBuffer;
static StackType_t xTimerStack[configTIMER_TASK_STACK_DEPTH];

void vApplicationGetTimerTaskMemory( StaticTask_t **ppxTimerTaskTCBBuffer, StackType_t **ppxTimerTaskStackBuffer, uint32_t *pulTimerTaskStackSize )
{
  *ppxTimerTaskTCBBuffer = &xTimerTaskTCBBuffer;
  *ppxTimerTaskStackBuffer = &xTimerStack[0];
  *pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;
  /* place for user code */
}
/* USER CODE END GET_TIMER_TASK_MEMORY */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
	
  printf("rtos loading...........\r\n");
	printf("\\   |   /\r\n");
	printf("-  xcjk  -\r\n");
	printf("/    |   \\\r\n");
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of KeyTask */
  osThreadDef(KeyTask, Key_Task_Proc, osPriorityIdle, 0, 128);
  KeyTaskHandle = osThreadCreate(osThread(KeyTask), NULL);

  /* definition and creation of RGBTask */
  osThreadDef(RGBTask, RGB_Task_Proc, osPriorityIdle, 0, 128);
  RGBTaskHandle = osThreadCreate(osThread(RGBTask), NULL);

  /* definition and creation of PCTask */
  osThreadDef(PCTask, PC_Task_Proc, osPriorityIdle, 0, 128);
  PCTaskHandle = osThreadCreate(osThread(PCTask), NULL);

  /* definition and creation of LCDTask */
  osThreadDef(LCDTask, LCD_Task_Proc, osPriorityIdle, 0, 256);
  LCDTaskHandle = osThreadCreate(osThread(LCDTask), NULL);

  /* definition and creation of BuzzerTask */
  osThreadDef(BuzzerTask, Buzzer_Task_Proc, osPriorityIdle, 0, 128);
  BuzzerTaskHandle = osThreadCreate(osThread(BuzzerTask), NULL);

  /* definition and creation of MotorTask */
  osThreadDef(MotorTask, Motor_Task_Proc, osPriorityIdle, 0, 128);
  MotorTaskHandle = osThreadCreate(osThread(MotorTask), NULL);

  /* definition and creation of EncoderTask */
  osThreadDef(EncoderTask, Encoder_Task_Proc, osPriorityIdle, 0, 128);
  EncoderTaskHandle = osThreadCreate(osThread(EncoderTask), NULL);

  /* definition and creation of BleTask */
  osThreadDef(BleTask, Ble_Task_Proc, osPriorityIdle, 0, 128);
  BleTaskHandle = osThreadCreate(osThread(BleTask), NULL);

  /* definition and creation of IMUTask */
  osThreadDef(IMUTask, IMU_Task_Proc, osPriorityIdle, 0, 256);
  IMUTaskHandle = osThreadCreate(osThread(IMUTask), NULL);

  /* definition and creation of ServoTask */
  osThreadDef(ServoTask, Servo_Task_Proc, osPriorityIdle, 0, 128);
  ServoTaskHandle = osThreadCreate(osThread(ServoTask), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
	
		if(RGBTaskHandle != NULL)
	{ printf("rgb_task_creat_sucess!\r\n"); }

	if(BuzzerTaskHandle != NULL)
	{ printf("buzzer_task_creat_sucess!\r\n"); }


	if(LCDTaskHandle != NULL)
	{ printf("lcd_task_creat_sucess!\r\n"); }

	if(KeyTaskHandle != NULL)
	{ printf("key_task_creat_sucess!\r\n"); }

	if(PCTaskHandle != NULL)
	{ printf("pc_task_creat_sucess!\r\n"); }



	if(ServoTaskHandle != NULL)
	{ printf("servo_task_creat_sucess!\r\n"); }

	if(MotorTaskHandle != NULL)
	{ printf("motor_task_creat_sucess!\r\n"); }    //指示打印是否正常
	
	if(EncoderTaskHandle != NULL)
	{ printf("Encoder_task_creat_sucess!\r\n"); }    //指示打印是否正常
	
	if(IMUTaskHandle != NULL)
	{ printf("MPU6050_task_creat_sucess!\r\n"); }    //指示打印是否正常
	
    printf("  /\\_/\\  \r\n");
    printf(" ( o.o ) \r\n");
    printf("  > ^ <  \r\n");
	printf(" 星橙祝您使用一路顺风！(ノ￣▽￣)   \r\n");
  /* USER CODE END RTOS_THREADS */

}

/* USER CODE BEGIN Header_Key_Task_Proc */
#ifndef XC_FREERTOS_MAKE_FLAG
/**
* @brief Function implementing the KeyTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_Key_Task_Proc */
void Key_Task_Proc(void const * argument)
{
  /* USER CODE BEGIN Key_Task_Proc */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END Key_Task_Proc */
}

/* USER CODE BEGIN Header_RGB_Task_Proc */

/**
  * @brief  Function implementing the RGBTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_RGB_Task_Proc */
void RGB_Task_Proc(void const * argument)
{
  /* USER CODE BEGIN RGB_Task_Proc */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END RGB_Task_Proc */
}

/* USER CODE BEGIN Header_PC_Task_Proc */
/**
* @brief Function implementing the PCTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_PC_Task_Proc */
void PC_Task_Proc(void const * argument)
{
  /* USER CODE BEGIN PC_Task_Proc */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END PC_Task_Proc */
}

/* USER CODE BEGIN Header_LCD_Task_Proc */
/**
* @brief Function implementing the LCDTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_LCD_Task_Proc */
void LCD_Task_Proc(void const * argument)
{
  /* USER CODE BEGIN LCD_Task_Proc */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END LCD_Task_Proc */
}

/* USER CODE BEGIN Header_Buzzer_Task_Proc */
/**
* @brief Function implementing the BuzzerTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_Buzzer_Task_Proc */
void Buzzer_Task_Proc(void const * argument)
{
  /* USER CODE BEGIN Buzzer_Task_Proc */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END Buzzer_Task_Proc */
}

/* USER CODE BEGIN Header_Motor_Task_Proc */
/**
* @brief Function implementing the MotorTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_Motor_Task_Proc */
void Motor_Task_Proc(void const * argument)
{
  /* USER CODE BEGIN Motor_Task_Proc */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END Motor_Task_Proc */
}

/* USER CODE BEGIN Header_Encoder_Task_Proc */
/**
* @brief Function implementing the EncoderTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_Encoder_Task_Proc */
void Encoder_Task_Proc(void const * argument)
{
  /* USER CODE BEGIN Encoder_Task_Proc */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END Encoder_Task_Proc */
}

/* USER CODE BEGIN Header_Ble_Task_Proc */
/**
* @brief Function implementing the BleTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_Ble_Task_Proc */
void Ble_Task_Proc(void const * argument)
{
  /* USER CODE BEGIN Ble_Task_Proc */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END Ble_Task_Proc */
}

/* USER CODE BEGIN Header_IMU_Task_Proc */
/**
* @brief Function implementing the IMUTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_IMU_Task_Proc */
void IMU_Task_Proc(void const * argument)
{
  /* USER CODE BEGIN IMU_Task_Proc */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END IMU_Task_Proc */
}

/* USER CODE BEGIN Header_Servo_Task_Proc */
/**
* @brief Function implementing the ServoTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_Servo_Task_Proc */
void Servo_Task_Proc(void const * argument)
{
  /* USER CODE BEGIN Servo_Task_Proc */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END Servo_Task_Proc */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
#endif
/* USER CODE END Application */
