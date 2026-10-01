#include "include.h"

#include "lcd.h"
#include "lcd_proc.h"
#include "lcd_init.h"
#include "imu_snapshot.h"
#include "chassis_port.h"
extern unsigned char startup[40968];
extern u32 uwTick;


u8 LcdDisp[40];

u8 LcdPage = 1;

void LCD_Task_Proc(void const * argument)
{
  /* USER CODE BEGIN LCD_Task_Proc */
  /* Infinite loop */
	LCD_Init();//LCD³õÊ¼»¯
//	LCD_Image(0, 0, 128,160, startup);
	vTaskDelay(200);
	LCD_Clear(BLACK);
  for(;;)
  {
    chassis_snapshot_t chassis;tm4c_imu_snapshot_t attitude;chassis_port_snapshot(&chassis);tm4c_imu_snapshot(&attitude);
	LCD_Chinese16ForFile(10,0,0, LIGHTBLUE, BLACK); //±êÌâ  ÐÇ
	LCD_Chinese16ForFile(10 + 18,0,1, LIGHTBLUE, BLACK); //±êÌâ  Êà
	LCD_ShowString(10 + 36,0,"V2.0 RTOS",YELLOW,BLACK,16,0);
	snprintf((char *)LcdDisp,sizeof(LcdDisp),"BAT: %.2fV", chassis.voltage);
	LCD_ShowString(0,160 - 32, LcdDisp,WHITE,BLACK,16,0);	
	snprintf((char *)LcdDisp,sizeof(LcdDisp),"TIME:%d", uwTick/1000);
	LCD_ShowString(0,160 - 16, LcdDisp,WHITE,BLACK,16,0);
	snprintf((char *)LcdDisp,sizeof(LcdDisp),"PAGE:%d", LcdPage);
	LCD_ShowString(80,160 - 16, LcdDisp,WHITE,BLACK,16,0);
	
	snprintf((char *)LcdDisp,sizeof(LcdDisp),"pitch:%0.2f        ",attitude.pitch);  //¸©Ñö½Ç
	LCD_ShowString(0,16 + 16 + 3 * 12, LcdDisp,WHITE,BLACK,12,0);


	snprintf((char *)LcdDisp,sizeof(LcdDisp)," roll:%0.2f        ",attitude.roll);  //·­¹ö½Ç
	LCD_ShowString(0,16 + 16 + 4 * 12, LcdDisp,WHITE,BLACK,12,0);	  

	snprintf((char *)LcdDisp,sizeof(LcdDisp),"  yaw:%0.2f       ", attitude.yaw);    //Æ«º½½Ç
	LCD_ShowString(0,16 + 16 + 5 * 12, LcdDisp,WHITE,BLACK,12,0);		
	
		
	vTaskDelay(100);
  }
  /* USER CODE END LCD_Task_Proc */
}