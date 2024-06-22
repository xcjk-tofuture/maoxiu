#include "motor_proc.h"
#include "simplepid.h"
#include "niming.h"

//*****************************
//注意1软件pwm生成 用到了 定时器7  10us一次中断 分为1000份 生成不同占空比的1khz的波形来驱动电机
//*****************************
//*****************************
//注意2电机由于是镜像放置 所以有方向正 方向反  现采用强制定义 定义A C为正方向电机   B D为反方向电机 带有强制取反操作  故安装电机时 例如 前轮为C D一组 后轮 为 A B 一组的安装
//*****************************
#define   PI 3.1415926
#define   HALL_30F    30
#define		Photoelectric_500 500
#define		Mecanum_60  0.060f
#define   EncoderMultiples 4
#define   MECWheelSpacing        0.250f / 2.f
#define   MECAxlespacing            0.220f / 2.f
#define   Wheel_Perimeter Mecanum_60 * PI
#define   Encoder_Precision  HALL_30F * Photoelectric_500 * EncoderMultiples

#define ENCODER_FRQ 100

osThreadId MotorTaskHandle;
osThreadId EncoderTaskHandle;
int motor1;
int motor2;
extern int change_output;
extern int change_x;
extern tPid speed_left;
extern tPid speed_right;
u32 TIM7Count;
extern int angle;
extern int change_output;
extern int change_x;
extern int x;
extern float volReal;
MOTOR MotorA, MotorB, MotorC, MotorD;  //编号对应 0 1 
CAR_PARA MEC_CAR;
CAR_STATE REAL_CAR;


void ditongfilter(float* motor,float target)
{
	*motor = *motor * 0.8 + target * 0.2;
}

uint8_t data_buffer[3] = {0xE1, 0x36, 0x17}; // 16 进制数据缓冲区

void Motor_Task_Proc(void const * argument)   //电机驱动进程
{
	Car_Para_Init();
	Motor_Init();
	static portTickType s_SystickCount_MOTOR;
	s_SystickCount_MOTOR = xTaskGetTickCount();
	for(;;)
	{
		vTaskDelayUntil(&s_SystickCount_MOTOR,20);
		Motor_Pwm_Set(&MotorA, MotorA.Pwm);
		Motor_Pwm_Set(&MotorB, MotorB.Pwm);
		Motor_Pwm_Set(&MotorC, MotorC.Pwm);
		Motor_Pwm_Set(&MotorD, MotorD.Pwm);
	
//		printf("%f  %f %f %f\r\n", MotorA.Speed, MotorB.Speed, MotorC.Speed, MotorD.Speed);
//		printf("%d  %d %d %d\r\n", MotorA.Trig, MotorB.Trig, MotorC.Trig, MotorD.Trig);


	}
}

void Encoder_Task_Proc(void const * argument)		//编码器读取进程
{
		static portTickType s_SystickCount;
    s_SystickCount = xTaskGetTickCount();
		Motor_Encoder_Init();//编码器初始化
		for(;;)
	{
		vTaskDelayUntil(&s_SystickCount,10);
		Motor_Get_Value(&MotorA);
		Motor_Get_Value(&MotorB);
		Motor_Get_Value(&MotorC);
		Motor_Get_Value(&MotorD);
		
		Car_PWM_TO_VEL(&REAL_CAR);
		if(volReal > 6.0)
		{
			  Car_Vel_TO_PWM(REAL_CAR.targetvx, REAL_CAR.targetvy,REAL_CAR.targetvz);
				Incremental_PI(&MotorA, MotorA.Speed ,MotorA.TargetSpeed);
				Incremental_PI(&MotorB, MotorB.Speed ,MotorB.TargetSpeed);
				Incremental_PI(&MotorC, MotorC.Speed ,MotorC.TargetSpeed);
				Incremental_PI(&MotorD, MotorD.Speed ,MotorD.TargetSpeed);
				
				
				ditongfilter(&(MotorA.Speed) ,(MotorA.LastSpeed));
				MotorA.LastSpeed = MotorA.Speed;
						
				ditongfilter(&(MotorB.Speed) ,(MotorB.LastSpeed));
				MotorB.LastSpeed = MotorB.Speed;
						
				ditongfilter(&(MotorC.Speed) ,(MotorC.LastSpeed));
				MotorC.LastSpeed = MotorC.Speed;
						
				ditongfilter(&(MotorD.Speed) ,(MotorD.LastSpeed));
				MotorD.LastSpeed = MotorD.Speed;
		
		}
		else
		{
			Motor_Pwm_Set(&MotorA, 0);
			Motor_Pwm_Set(&MotorB, 0);
			Motor_Pwm_Set(&MotorC, 0);
			Motor_Pwm_Set(&MotorD, 0);
			MotorA.TargetSpeed = 0;
			MotorB.TargetSpeed = 0;
			MotorC.TargetSpeed = 0;
			MotorD.TargetSpeed = 0;
			MotorA.LastBias = 0;
			MotorA.Bias = 0;
			MotorB.LastBias = 0;
			MotorB.Bias = 0;
			MotorC.LastBias = 0;
			MotorC.Bias = 0;
			MotorD.LastBias = 0;
			MotorD.Bias = 0;
		}

    //ANO_DT_Send_F2(MotorA.Speed * 1000, MotorB.Speed  * 1000, MotorC.Speed  * 1000, MotorD.Speed  * 1000);
		
	
	}
}

//电机参数初始化
/******************************************************************************
      函数说明：电机参数初始化
      入口数据：无    
      返回值：  无
******************************************************************************/
void Car_Para_Init()
{

	MotorA.Num = 0;
	MotorB.Num = 1;
	MotorC.Num = 2;
	MotorD.Num = 3;
	
	MotorA.VelocityKP = 1800;
	MotorB.VelocityKP = 1800;
	MotorC.VelocityKP = 1800;
	MotorD.VelocityKP = 1800;

	MotorA.VelocityKI = 180;
	MotorB.VelocityKI = 180;
	MotorC.VelocityKI = 180;
	MotorD.VelocityKI = 180;

	MotorA.VelocityKD = 0;
	MotorB.VelocityKD = 0;
	MotorC.VelocityKD = 0;
	MotorD.VelocityKD = 0;
	
	MEC_CAR.hall = HALL_30F;
	MEC_CAR.mecanum = Mecanum_60;
	MEC_CAR.photoelectric = Photoelectric_500;
	MEC_CAR.encoderMultiples = EncoderMultiples;
	MEC_CAR.encoderPrecision = MEC_CAR.encoderMultiples * MEC_CAR.hall * MEC_CAR.photoelectric;
	MEC_CAR.wheelPerimeter = MEC_CAR.mecanum * PI;
	MEC_CAR.axleSpacing = MECAxlespacing;
	MEC_CAR.wheelSpacing = MECWheelSpacing;
	
	
}



//电机编码器初始化
/******************************************************************************
      函数说明：电机编码器初始化（封存编码器模式读脉冲）
      入口数据：无    
      返回值：  无
******************************************************************************/
void Motor_Encoder_Init()
{

	HAL_TIM_Encoder_Start(&htim2, TIM_CHANNEL_1); 
	HAL_TIM_Encoder_Start(&htim2, TIM_CHANNEL_2);
																									//开启编码器模式
  HAL_TIM_Base_Start_IT(&htim2);                  //开启编码器的中断
	HAL_TIM_Encoder_Start(&htim3, TIM_CHANNEL_1); //开启编码器模式
  HAL_TIM_Encoder_Start(&htim3, TIM_CHANNEL_2);
	HAL_TIM_Base_Start_IT(&htim3);                  //开启编码器的中断
                 //开启编码器的中断
	HAL_TIM_Encoder_Start(&htim4, TIM_CHANNEL_1); //开启编码器模式
  HAL_TIM_Encoder_Start(&htim4, TIM_CHANNEL_2);
	HAL_TIM_Base_Start_IT(&htim4);
	
	HAL_TIM_Encoder_Start(&htim5, TIM_CHANNEL_1); //开启编码器模式
  HAL_TIM_Encoder_Start(&htim5, TIM_CHANNEL_2);
	HAL_TIM_Base_Start_IT(&htim5);

}

//获取电机的脉冲值 方向  并转换为速度 电机 A B C D 对应 0 1 2 3
/******************************************************************************
      函数说明：四倍频外部中断法 M测速法 获取电机的脉冲值 方向  并转换为速度 电机 A B C D 对应 0 1 2 3
      入口数据：电机结构体指针    
      返回值：  无
******************************************************************************/
void Motor_Get_Value(MOTOR  *motor)
{
	switch(motor->Num)
	{
		case 0: motor->Trig = (short)__HAL_TIM_GetCounter(&htim2);//获取计数值  
						__HAL_TIM_SetCounter(&htim2, 0);//清空计数值
						motor->Direction = __HAL_TIM_IS_TIM_COUNTING_DOWN(&htim2);//获取方向		
						motor->Trig = -motor->Trig;						//强制取反 
						motor->Speed = -motor->Trig * ENCODER_FRQ *  MEC_CAR.wheelPerimeter / MEC_CAR.encoderPrecision ;
						break;
		case 1: motor->Trig = (short)__HAL_TIM_GetCounter(&htim3);//获取计数值  
						__HAL_TIM_SetCounter(&htim3, 0);//清空计数值
						motor->Direction = __HAL_TIM_IS_TIM_COUNTING_DOWN(&htim3);//获取方向		
						motor->Trig = -motor->Trig;						//强制取反
						motor->Speed = -motor->Trig * ENCODER_FRQ *  MEC_CAR.wheelPerimeter / MEC_CAR.encoderPrecision ;
						break;
		case 2: motor->Trig = (short)__HAL_TIM_GetCounter(&htim4);//获取计数值  
						__HAL_TIM_SetCounter(&htim4, 0);//清空计数值
						motor->Direction = __HAL_TIM_IS_TIM_COUNTING_DOWN(&htim4);//获取方向		
						motor->Direction = motor->Direction;  //强制取反
						motor->Trig = -motor->Trig;						//强制取反
						motor->Speed = motor->Trig * ENCODER_FRQ *   MEC_CAR.wheelPerimeter / MEC_CAR.encoderPrecision ;
						break;
		case 3: motor->Trig = (short)__HAL_TIM_GetCounter(&htim5);//获取计数值  
						__HAL_TIM_SetCounter(&htim5, 0);//清空计数值
						motor->Direction = __HAL_TIM_IS_TIM_COUNTING_DOWN(&htim5);//获取方向		
						motor->Direction = motor->Direction;  //强制取反
						motor->Trig = -motor->Trig;						//强制取反
						motor->Speed = motor->Trig * ENCODER_FRQ *   MEC_CAR.wheelPerimeter / MEC_CAR.encoderPrecision ;
						break;
	}
					
}

/******************************************************************************
      函数说明：电机输出pwm初始化
      入口数据：无    
      返回值：  无
******************************************************************************/
void Motor_Init()
{
	HAL_TIM_PWM_Start(&htim10, TIM_CHANNEL_1);  //MOTORA_PWM
	HAL_TIM_PWM_Start(&htim11, TIM_CHANNEL_1);	 //MOTORA_PWM
	
	HAL_TIM_PWM_Start(&htim9, TIM_CHANNEL_1);  //MOTORB_PWM
	HAL_TIM_PWM_Start(&htim9, TIM_CHANNEL_2);	 //MOTORB_PWM
	
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);  //MOTORC_PWM
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);	 //MOTORC_PWM
	
	
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3);  //MOTORD_PWM
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_4);	 //MOTORD_PWM
	
	TIM10->CCR1 = 0;
	TIM11->CCR1 = 0;
	TIM9->CCR1 = 0;
	TIM9->CCR2 = 0;
	
	TIM1->CCR1 = 0;
	TIM1->CCR2 = 0;
  TIM1->CCR3 = 0;	
	TIM1->CCR4 = 0;
}


/******************************************************************************
      函数说明：绝对值函数
      入口数据：数值    
      返回值：  无
******************************************************************************/
int ABS(int a)
{
	a = a>0?a:(-a);
	return a;

}

/******************************************************************************
      函数说明：电机速度设置函数 设置电机速度  范围 -9999 - +9999
      入口数据：电机结构体指针   速度    
      返回值：  无
******************************************************************************/
void Motor_Pwm_Set(MOTOR *motor, s16 speed)
{

	if(speed >= 16799 ) speed = 16799;
	if(speed <= -16799 ) speed = -16799;   //限幅
	switch(motor->Num)
	{
		case 0: motor->SetSpeedRange = speed;
						motor->SetDirection =  motor->SetSpeedRange >= 0 ? 0 : 1;  //判断方向   0为正 1为负
						if(!motor->SetDirection) //正向
						{
								TIM10->CCR1 =16799 - ABS(speed);
								//if(REAL_CAR.vx >= 0.01 || REAL_CAR.vy >= 0.01 || REAL_CAR.vz >= 0.01)

								TIM11->CCR1 = 16799;
						}
					else if(motor->SetDirection) //反向
					{			
								TIM10->CCR1 = 16799;
								TIM11->CCR1 =16799 -  ABS(speed);								
						
						}
						break;
		case 1: motor->SetSpeedRange = speed;
						motor->SetDirection =  motor->SetSpeedRange >= 0 ? 0 : 1;  //判断方向
						if(!motor->SetDirection) //正向
						{
								TIM9->CCR1 = 16799 - ABS(speed);

	
								TIM9->CCR2 = 16799;
						}
					else if(motor->SetDirection) //反向
					{
								TIM9->CCR1 = 16799;
								TIM9->CCR2 = 16799 -  ABS(speed);

		
					}
						break;
		 case 2: motor->SetSpeedRange = speed;
						 motor->SetDirection =  motor->SetSpeedRange >= 0 ? 0 : 1;  //判断方向
						if(!motor->SetDirection) //正向
						{
								TIM1->CCR1 = 16799;
								TIM1->CCR2 =16799 - ABS(speed);


						}
					else if(motor->SetDirection) //反向
					{
							  TIM1->CCR1 = 16799 - ABS(speed);
								TIM1->CCR2 = 16799;
	
					}
						break;
			case 3: motor->SetSpeedRange = speed;
						motor->SetDirection =  motor->SetSpeedRange >= 0 ? 0 : 1;  //判断方向
						if(!motor->SetDirection) //正向
						{
								TIM1->CCR3 = 16799;
								TIM1->CCR4 = 16799 - ABS(speed);

	

						}
					else if(motor->SetDirection) //反向
					{
							  TIM1->CCR3 = 16799 -  ABS(speed);


								TIM1->CCR4 = 16799;
					}
						break;
	}
	
					

}



void Car_Vel_TO_PWM(float Vx, float Vy,  float Vz)
{

		MotorA.TargetSpeed   = +Vy+Vx-Vz*(MEC_CAR.axleSpacing+MEC_CAR.wheelSpacing);
		MotorB.TargetSpeed   = -Vy+Vx-Vz*(MEC_CAR.axleSpacing+MEC_CAR.wheelSpacing);
		MotorC.TargetSpeed   = -Vy+Vx+Vz*(MEC_CAR.axleSpacing+MEC_CAR.wheelSpacing);
		MotorD.TargetSpeed   = +Vy+Vx+Vz*(MEC_CAR.axleSpacing+MEC_CAR.wheelSpacing);

}



void Car_PWM_TO_VEL(CAR_STATE * car)
{
	car->vx = (MotorA.Speed + MotorC.Speed) / 2.f;
	car->vy = (MotorA.Speed - MotorB.Speed) / 2.f;
	car->vz = (MotorD.Speed - MotorA.Speed) / (2.f * (MEC_CAR.axleSpacing+MEC_CAR.wheelSpacing));
}

void Incremental_PI(MOTOR *motor, float Encoder,float Target)
{ 	
	 motor->Bias=Target-Encoder; //Calculate the deviation //计算偏差
	 //printf("%f\r\n", Bias);
	 motor->Pwm+=motor->VelocityKP*(motor->Bias - motor->LastBias)+motor->VelocityKI*motor->Bias; 
	 if(motor->Pwm > 16700)motor->Pwm=16700;
	 if(motor->Pwm < -16700)motor->Pwm=-16700;
	 motor->LastBias=motor->Bias; //Save the last deviation //保存上一次偏差 
  
}
