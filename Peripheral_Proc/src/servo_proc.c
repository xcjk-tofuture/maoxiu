#include "servo_proc.h"


#define RAD_TO_ANG 57.32484
#define BIG_ARM_LENGTH 153
#define SMALL_ARM_LENGTH 110
#define BASE_LINK_LENGTH 33
osThreadId ServoTaskHandle;



//并行舵机对4应的定时器为
//TIM1 TIM_CHANNEL 1-4 
//TIM8 TIM_CHANNEL 3 4 
//TIM10 TIM_CHANNEL 1
//TIM11 TIM_CHANNEL 1

//时钟默认分配为1MHZ  arr默认为19999  则对应ARR 5% - 25%  CCR为 999 - 4999
ARM realArm;
SERVO servo1, servo2, servo3, servo4;
PID servo1PID, servo2PID, servo3PID ,servo4PID;
PID_DATA servo1PidData,servo2PidData,servo3PidData, servo4PidData; 
//舵机进程
u16 count;
void Servo_Task_Proc(void const * argument)
{
  /* USER CODE BEGIN Servo_Task_Proc */
  /* Infinite loop */
	
	Servo_Enable_All();
	//Servo_Diver(8, 80);		if(++Count  > 10)
	Arm_Link_Dirver(&servo1, 0);
	Arm_Link_Dirver(&servo2, 0);
	Arm_Link_Dirver(&servo3, 0);
	osDelay(3000);
	//		u8 Count;
  for(;;)
  {
	count++;
	if(count % 5 == 0)
	{
		Arm_Pos_To_Angle(-200 + count , 0 + count, 150);
	
	}

		
	//	Arm_Pos_To_Angle(-150 , 150, 200);
//	
	
	//Arm_Pos_To_Angle(-150 , 150, 189);
	PID_Control(&servo1PID, &servo1PidData, 0.05, 0, servo1.TargetPos, servo1.Pos, 1000);
	PID_Control(&servo2PID, &servo2PidData, 0.05, 0, servo2.TargetPos, servo2.Pos, 1000);
	PID_Control(&servo3PID, &servo3PidData, 0.05, 0, servo3.TargetPos, servo3.Pos, 1000);
	PID_Control(&servo4PID, &servo4PidData, 0.05, 0, servo4.TargetPos, servo4.Pos, 1000);
	printf("%f , %f   %f  %f\r\n", servo1.TargetPos,servo2.TargetPos, servo3.TargetPos, servo4.TargetPos);	
	servo1.SetPos = servo1PID.Out;
	servo2.SetPos = servo2PID.Out;
  servo3.SetPos = servo3PID.Out;
	servo4.SetPos = servo4PID.Out;
		
	servo1.Pos = servo1.SetPos;
	servo2.Pos = servo2.SetPos;
	servo3.Pos = servo3.SetPos;
	servo4.Pos = servo4.SetPos;
	Arm_Link_Dirver(&servo1, servo1.SetPos);
	Arm_Link_Dirver(&servo2, servo2.SetPos);
	Arm_Link_Dirver(&servo3, servo3.SetPos);
	Arm_Link_Dirver(&servo4, servo4.SetPos);
  //printf("%d , %d\r\n", servo1.SetPos, servo1.SetPos);
		
	//ANO_DT_Send_F2(servo1.SetPos,800,0, 0);
//	Servo_Diver(4, 50);
//	Servo_Diver(5, 50);
//	Servo_Diver(6, 50);
//	Servo_Diver(7, 70);

		osDelay(50);
  }
  /* USER CODE END Servo_Task_Proc */
}


/******************************************************************************
      函数说明：使能全部舵机 舵机全部回中
      入口数据：无    
      返回值：  无
******************************************************************************/
void Servo_Enable_All()
{

	HAL_TIM_PWM_Start(&htim8, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim8, TIM_CHANNEL_2);
	HAL_TIM_PWM_Start(&htim8, TIM_CHANNEL_3);
	HAL_TIM_PWM_Start(&htim8, TIM_CHANNEL_4);
	HAL_TIM_PWM_Start(&htim12, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim12, TIM_CHANNEL_2);

	
//	TIM1->CCR1 = 1499;
//	TIM1->CCR2 = 1499;
	TIM8->CCR3 = 1499;
	TIM8->CCR4 = 1499;
	TIM8->CCR3 = 1499;
	TIM8->CCR4 = 1499;
	TIM12->CCR1 = 1499;
	TIM12->CCR2 = 1499;
	
	servo1.Num = 1;
	servo2.Num = 2;
	servo3.Num = 3;
	servo4.Num = 4;
	
	servo1PidData.Kp = 0.00005;
	servo2PidData.Kp = 0.00003;
	servo3PidData.Kp = 0.00005;
	servo4PidData.Kp = 0.00005;
	
	servo1PidData.Ki = 2;
	servo2PidData.Ki = 1;
	servo3PidData.Ki = 2;
	servo4PidData.Ki = 2;
	
	servo1PidData.Kd = 0.00001;
	servo2PidData.Kd = 0.00001;
	servo3PidData.Kd = 0.00001;
	servo4PidData.Kd = 0.00001;
	
	servo1PidData.DifferentialMax = 30000;
	servo2PidData.DifferentialMax = 30000;
	servo3PidData.DifferentialMax = 30000;
	servo4PidData.DifferentialMax = 30000;
	
	servo1PidData.IntegrateMax = 30000;
	servo2PidData.IntegrateMax = 30000;
	servo3PidData.IntegrateMax = 30000;
	servo4PidData.IntegrateMax = 30000;
	
	
	servo1PidData.ErrorMax = 1800;
	servo2PidData.ErrorMax = 1800;
	servo3PidData.ErrorMax = 1800;
	servo4PidData.ErrorMax = 1800;

	//TIM1->CCR1 = 3399;
}

/******************************************************************************
      函数说明：失能舵机
      入口数据：servo_num   舵机id 对应板子上的1到8            
      返回值：  无
******************************************************************************/
void Servo_Disable(u8 servo_num)
{
	if(servo_num == 1)
		HAL_TIM_PWM_Stop(&htim8, TIM_CHANNEL_1);
	if(servo_num == 2)
		HAL_TIM_PWM_Stop(&htim8, TIM_CHANNEL_2);
	if(servo_num == 3)
		HAL_TIM_PWM_Stop(&htim8, TIM_CHANNEL_3);
	if(servo_num == 4)
		HAL_TIM_PWM_Stop(&htim8, TIM_CHANNEL_4);
	if(servo_num == 5)
		HAL_TIM_PWM_Stop(&htim12, TIM_CHANNEL_1);
	if(servo_num == 6)
		HAL_TIM_PWM_Stop(&htim12, TIM_CHANNEL_2);
	
}



void Arm_Link_Dirver(SERVO *servo, s16 angle)
{
		angle = angle > 850 ? 850 : angle ;
		angle = angle < -850 ? -850 : angle ;
	if(servo->Num == 1)
	{
		Servo_Diver(1, 900 + angle);
	
	}
	else if(servo->Num == 2)
	{
		Servo_Diver(2, 900 + angle);
	
	}
	else if(servo->Num == 3)
	{
	
		Servo_Diver(3, 900 + angle);
	}
		else if(servo->Num == 4)
	{
	
		Servo_Diver(5,  900 + angle);
	}




}
/******************************************************************************
      函数说明：指定舵机转相应范围  对应占空比2.5%-12.5%
      入口数据：servo_num   舵机id 对应板子上的1到8
							 range			 范围 默认0-100
      返回值：  无
******************************************************************************/
void Servo_Diver(u8 servo_num, u16 range)
{
		if(servo_num == 1)
		{
				TIM8->CCR1 =  (TIM8->ARR + 1 ) * 0.025 + (TIM8->ARR + 1 ) * (range / 18.0f) * 0.001  - 1;
				//printf("%d\r\n",TIM1->CCR1);
		}
		else if(servo_num == 2)
		{
				TIM8->CCR2 =  (TIM8->ARR + 1 ) * 0.025 + (TIM8->ARR+ 1 ) * (range / 18.0f)  * 0.001  - 1;
				//printf("%d\r\n",TIM1->CCR2);
		}
		else if(servo_num == 3)
		{
				TIM8->CCR3 =  (TIM8->ARR + 1 ) * 0.025 + (TIM8->ARR + 1 ) * (range / 18.0f)  * 0.001  - 1;
				//printf("%d\r\n",TIM1->CCR3);
		}
		else if(servo_num == 4)
		{
				TIM8->CCR4 =  (TIM8->ARR + 1 ) * 0.025 + (TIM8->ARR + 1 ) * (range / 18.0f)  * 0.001  - 1;
				//printf("%d\r\n",TIM1->CCR4);
		}
				else if(servo_num == 5)
		{
				TIM12->CCR1 =  (TIM12->ARR + 1 ) * 0.025 + (TIM12->ARR + 1 ) * (range / 18.0f)  * 0.001  - 1;
				//printf("%d\r\n",TIM8->CCR3);
		}
		else if(servo_num == 6)
		{
				TIM12->CCR2 =  (TIM12->ARR + 1 ) * 0.025 + (TIM12->ARR + 1 ) * (range / 18.0f)  * 0.001  - 1;
				//printf("%d\r\n",TIM8->CCR4);
		}
}

void Arm_Pos_To_Angle(s16 x, s16 y , s16 z)
{
	
	servo1.TargetPos = atan((float)x / y) * RAD_TO_ANG;
	servo1.TargetPos *= 10;
	if( y < 0)
	{
		servo1.TargetPos = -servo1.TargetPos;
	}
	if(z < BASE_LINK_LENGTH)
	{	 
			float verticalLineLength = sqrt(x*x + y*y); // 注意C语言中使用*表示乘法，而非^
			float shortLineLength = BASE_LINK_LENGTH - z;
			float obliLineLength = sqrt(shortLineLength*shortLineLength + verticalLineLength*verticalLineLength);
			realArm.bigArmAngle = atan(shortLineLength / verticalLineLength) * RAD_TO_ANG + acos((obliLineLength * obliLineLength + BIG_ARM_LENGTH * BIG_ARM_LENGTH - SMALL_ARM_LENGTH * SMALL_ARM_LENGTH) / (2 * BIG_ARM_LENGTH * obliLineLength)) * RAD_TO_ANG;
			realArm.smallArmAngle = acos((BIG_ARM_LENGTH * BIG_ARM_LENGTH  +  SMALL_ARM_LENGTH * SMALL_ARM_LENGTH  - obliLineLength * obliLineLength) /(2 * BIG_ARM_LENGTH * SMALL_ARM_LENGTH)) * RAD_TO_ANG;
			float servo4Angle = 360 - realArm.bigArmAngle  - realArm.smallArmAngle;	
			servo4.TargetPos = servo4Angle + 90;
			servo4.TargetPos *= 10;
			realArm.smallArmAngle *= 10;															//		 ____
			realArm.bigArmAngle *= 10;																	//	/		 \
			servo2.TargetPos =1800 - realArm.bigArmAngle;							//	 /	
			servo3.TargetPos =1800 - realArm.smallArmAngle;           //  |
	}
	else if(z == BASE_LINK_LENGTH)
	{
			float verticalLineLength = sqrt(x*x + y*y); // 注意C语言中使用*表示乘法，而非^
			realArm.bigArmAngle = acos((verticalLineLength * verticalLineLength + BIG_ARM_LENGTH * BIG_ARM_LENGTH - SMALL_ARM_LENGTH * SMALL_ARM_LENGTH) / (2 * BIG_ARM_LENGTH * verticalLineLength)) * RAD_TO_ANG;
			realArm.smallArmAngle = acos((BIG_ARM_LENGTH * BIG_ARM_LENGTH  +  SMALL_ARM_LENGTH * SMALL_ARM_LENGTH  - verticalLineLength * verticalLineLength) /(2 * BIG_ARM_LENGTH * SMALL_ARM_LENGTH)) * RAD_TO_ANG;
			float servo4Angle = 360 - realArm.bigArmAngle  - realArm.smallArmAngle;	
			servo4.TargetPos = servo4Angle + 90;
			realArm.bigArmAngle += 90;
			servo4.TargetPos *= 10;
			realArm.smallArmAngle *= 10;
			realArm.bigArmAngle *= 10;
			servo2.TargetPos =1800 - realArm.bigArmAngle;
			servo3.TargetPos =1800 - realArm.smallArmAngle;
		
	}
	else
	{
			float shortLineLength = -BASE_LINK_LENGTH + z;
			float verticalLineLength = sqrt(x*x + y*y); // 注意C语言中使用*表示乘法，而非^
			float obliLineLength = sqrt(shortLineLength*shortLineLength + verticalLineLength*verticalLineLength);
			realArm.bigArmAngle =atan(shortLineLength / verticalLineLength) * RAD_TO_ANG + acos((BIG_ARM_LENGTH*BIG_ARM_LENGTH  -  SMALL_ARM_LENGTH*SMALL_ARM_LENGTH  + obliLineLength*obliLineLength)/ (2.0f *(float) BIG_ARM_LENGTH * (float)obliLineLength) ) * RAD_TO_ANG;
			realArm.smallArmAngle = acos((BIG_ARM_LENGTH*BIG_ARM_LENGTH  +  SMALL_ARM_LENGTH*SMALL_ARM_LENGTH  - obliLineLength*obliLineLength) /(2 * BIG_ARM_LENGTH * SMALL_ARM_LENGTH)) * RAD_TO_ANG;
			if((BIG_ARM_LENGTH*BIG_ARM_LENGTH  +  SMALL_ARM_LENGTH*SMALL_ARM_LENGTH  - obliLineLength*obliLineLength) /(2 * BIG_ARM_LENGTH * SMALL_ARM_LENGTH) < -1)
			{
				realArm.smallArmAngle = 180.0f;
			}
			
			float servo4Angle = 360 - realArm.bigArmAngle  - realArm.smallArmAngle;	
			servo4.TargetPos = servo4Angle;
			servo4.TargetPos *= 10;
			realArm.bigArmAngle *= 10;
			realArm.bigArmAngle += 900; 
			realArm.smallArmAngle *= 10;
			servo2.TargetPos = 1800 - realArm.bigArmAngle;
			servo3.TargetPos = 1800 - realArm.smallArmAngle;
//	  printf("smallArmAngle before adjustment: %f\n", realArm.smallArmAngle);
//		printf("bigArmAngle after adjustment: %f\n", realArm.bigArmAngle);
//		printf("smallArmAngle after adjustment: %f\n", realArm.smallArmAngle);
//		printf("TargetPos for servo2: %f\n", servo2.TargetPos);
//		printf("TargetPos for servo2: %f\n", servo2.TargetPos);
//		printf("TargetPos for servo3: %f\n", servo3.TargetPos);
//		printf("TargetPos for servo1: %f\n", servo1.TargetPos);
	}	
}