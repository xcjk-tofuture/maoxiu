#ifndef __SERVO_PROC_H
#define __SERVO_PROC_H



#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

#include "tim.h"
#include "pid.h"
#include "mathTool.h"
#include "niming.h"

typedef struct servo
{
	u8 Num;       //编号
	float TargetPos;  //当前速度
	s16 Pos;  //当前速度
	s16 SetPos;
	s16 LastPos;  //当前速度
}SERVO;


typedef struct arm
{
	s16 x;
	s16 y;
	s16 z;
	s16 targetx;
	s16 targety;
	s16 targetz;
	
	float baselinkAngle;
	float bigArmAngle;
	float smallArmAngle;
}ARM;


void Servo_Diver(u8 servo_num, u16 range);
void Servo_Disable(u8 Servo_Num);
void Servo_Enable_All();
void Arm_Link_Dirver(SERVO *servo, s16 angle);
void Arm_Pos_To_Angle(s16 x, s16 y , s16 z);
#endif