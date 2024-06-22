#include "key_proc.h"

extern u8 LcdPage;

osThreadId KeyTaskHandle;


u8 KeyVal, KeyOld, KeyUp, KeyDown;
u16 volValue;

float volReal;




void Key_Task_Proc(void const * argument)
{
	HAL_ADC_Start_DMA(&hadc1,(u32 *)&volValue, 1);
	
	for(;;)
	{
		
	KeyVal = Key_Scan();
	KeyDown = KeyVal & (KeyVal ^ KeyOld);
	KeyUp = ~KeyVal & (KeyVal ^ KeyOld);
	KeyOld = KeyVal;
//	printf("vol:%f\r\n", volReal);
	
	volReal = volValue * 33.0f / 4096.0f;
	if(KeyDown == 1)
	{
//		printf("key1 down\r\n");
	}
	else if(KeyDown == 2)
	{
//		printf("key2 down\r\n");
	}
	osDelay(50);
	}

}


u8 Key_Scan()
{
		if(HAL_GPIO_ReadPin(GPIOE,GPIO_PIN_0) == GPIO_PIN_RESET)
			return 2;
		else if(HAL_GPIO_ReadPin(GPIOE,GPIO_PIN_1) == GPIO_PIN_RESET)
			return 1;
		else
			return 0;
}