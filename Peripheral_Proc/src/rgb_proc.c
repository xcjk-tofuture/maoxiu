#include "rgb_proc.h"






osThreadId RGBTaskHandle;

u8 ucLed;


extern u8 zeroDriftFlag;


void RGB_Task_Proc(void const * argument)           //RGB进程主程序
{
  /* USER CODE BEGIN RGB_Task_Proc */
  /* Infinite loop */
	ucLed = 0x01;
  for(;;)
  {
		if(!zeroDriftFlag)
		{
			ucLed ^= 0x01;
			osDelay(200);
		}
		else
		{
			if(ucLed == 0)
			{
				ucLed = 0x01;
			}
			ucLed *= 2;
			if(ucLed > 0x04) ucLed = 0x01;
			osDelay(1000);
		}	
		//printf("23333\r\n");
		RGB_Show_Proc(ucLed);
		
  }
  /* USER CODE END RGB_Task_Proc */
}






//下面都是驱动



void RGB_Show_Proc(u8 ucled)                      //0b000 后三位赋1 灯亮
{
	//
	HAL_GPIO_WritePin(GPIOD, RGB_red_Pin, (ucled & 0x04) == 0 ? GPIO_PIN_SET : GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOD, RGB_green_Pin, (ucled & 0x02) == 0 ? GPIO_PIN_SET : GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOD, RGB_blue_Pin, (ucled & 0x01) == 0 ? GPIO_PIN_SET : GPIO_PIN_RESET);
	

};