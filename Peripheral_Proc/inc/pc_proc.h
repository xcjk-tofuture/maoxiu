#ifndef __PC_PROC_H
#define __PC_PROC_H


#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"
#include "usart.h"
#include "motor_proc.h"
#include "mathTool.h"

void PC_Data_Rx_Proc(u16 size);
float XYZ_Target_Speed_transition(u8 High,u8 Low);
u8 Check_Sum(unsigned char Count_Number,unsigned char Mode);

#endif