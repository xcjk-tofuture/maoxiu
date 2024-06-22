#include "lcd_proc.h"
#include "lcd_init.h"
#include "oled_init.h"


u8 LcdDisp[40];
u8 LcdPage = 1;
osThreadId LCDTaskHandle;


void LCD_Task_Proc(void const * argument)
{
  /* USER CODE BEGIN LCD_Task_Proc */
  /* Infinite loop */
//	OLED_Init();                           //OLED初始
//  OLED_Clear();                         //清屏
	LCD_Init();//LCD初始化
	u8 oledDisp[40];
  for(;;)
  {
		
		LCD_Chinese16ForFile(10,0,0, LIGHTBLUE, BLACK); //标题  星
		LCD_Chinese16ForFile(10 + 18,0,1, LIGHTBLUE, BLACK); //标题  枢
		LCD_ShowString(10 + 36,0,(char *)"V1.0 RTOS",YELLOW,BLACK,16,0);
		sprintf((char *)LcdDisp,"TIME:%d", uwTick/1000);
		LCD_ShowString(0,160 - 16, LcdDisp,WHITE,BLACK,16,0);
		sprintf((char *)LcdDisp,"PAGE:%d", LcdPage);
		LCD_ShowString(80,160 - 16, LcdDisp,WHITE,BLACK,16,0);
//OLED_ShowString(0,0,"TEST",strlen((char *)"TEST"), 4); 
//		OLED_ShowString(0,20,"TEST",strlen((char *)"TEST"), 4); 
//		OLED_ShowString(0,40,"TEST",strlen((char *)"TEST"), 4); 
    osDelay(50);
  }
  /* USER CODE END LCD_Task_Proc */
}



