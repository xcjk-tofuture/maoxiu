#ifndef __MPU6050_PROC_H
#define __MPU6050_PROC_H




#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

#include "mpu6050.h"

typedef struct  
{
 float pitch;
 float roll;
 float yaw;
	
	short gx;
	short gy;
	short gz;
	
	short gyrox;
	short gyroy;
	short gyroz;
	
	float Mechanical_Angle;
}My_Pos;


#endif