#include "rgb_proc.h"
#include "board_io.h"






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



void RGB_Show_Proc(u8 color) {maoxiu_board_rgb_write(color);}
