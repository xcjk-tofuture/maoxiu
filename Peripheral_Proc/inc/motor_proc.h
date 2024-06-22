#ifndef __MOTOR_PROC_H
#define __MOTOR_PROC_H


#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"
#include "tim.h"

typedef struct motor
{
	u8 Num;       //编号
	u8 Direction; //方向
	s32 Trig;     //脉冲数
	float TargetSpeed;  //当前速度
	float Speed;  //当前速度
	float LastSpeed;  //当前速度
	s16 SetSpeedRange; //设定的速度档位   负号代表反方向
	s8 SetDirection; //   负号代表反方向
	s16 Pwm;
	
	s16 VelocityKP;
	s16 VelocityKI;
	s16 VelocityKD;
	
	
	float Bias;
	float LastBias;
}MOTOR;

typedef struct car_para
{
 u16 hall;           //减速比
 u16 photoelectric;  //线数
 float mecanum; //轮子直径
 u16 encoderMultiples;  //倍频数
 float wheelPerimeter;   //轮子直径
 float encoderPrecision;  //转一圈所的脉冲
 float wheelSpacing;    //
 float axleSpacing;
	
	
}CAR_PARA;


typedef struct car_state
{
	
  u8 state;

  float vx;
	float vy;
	float vz;
	
	float targetvx;
	float targetvy;
	float targetvz;
	
	float x;
	float y;
	float z;
	
}CAR_STATE;


void Car_Para_Init(void);
void Motor_Encoder_Init(void);
void Motor_Get_Value(MOTOR  *motor);
void Motor_Init(void);
void Motor_Pwm_Set(MOTOR *motor, s16 speed);
void Motor_Speed_Set_Extra(MOTOR *motor, s16 speed);
void Incremental_PI(MOTOR *motor, float Encoder,float Target);
void Car_Vel_TO_PWM(float Vx, float Vy,  float Vz);
void Car_PWM_TO_VEL(CAR_STATE * car);
#endif