#include "mpu6050_proc.h"
#include "IIC.h"
#include "mpu6050.h"


osThreadId IMUTaskHandle;


Vector3f_t CarAcc;
Vector3f_t CarGryo;
short Temp;
My_Pos CarPos;



Vector3i_t accOri;
Vector3i_t accOriCal;
Vector3i_t gryoOri;
Vector3i_t gryoOriCal;
s32 zeroDriftCount = 1;
s32 zeroDriftAccAll[3];
s32 zeroDriftAccAvg[3];
s32 zeroDriftGyroAll[3];
s32 zeroDriftGyroAvg[3];


u8 zeroDriftFlag = 0;
void IMU_Task_Proc()
{


	MPU_Init();
	//data = i2cReadData(MPU_ADRESS,WHO_AM_I,&IMU_ADDRESS,1);
	//data = IIC_CheckDevice(0x68);	
//	
		osDelay(500);
		static portTickType s_SystickCount_IMU;
    s_SystickCount_IMU = xTaskGetTickCount();
	for(;;)
	{
		vTaskDelayUntil(&s_SystickCount_IMU,10);
		Temp = MPU_Get_Temperature();
		if(!zeroDriftFlag )
		{
				MPU_Get_Accelerometer(&(accOri.x), &(accOri.y),&(accOri.z));
				MPU_Get_Gyroscope(&(gryoOri.x), &(gryoOri.y),&(gryoOri.z));
				zeroDriftAccAll[0] += accOri.x;
				zeroDriftAccAll[1] += accOri.y;
				zeroDriftAccAll[2] += accOri.z;
				zeroDriftGyroAll[0] += gryoOri.x;
				zeroDriftGyroAll[1] += gryoOri.y;
				zeroDriftGyroAll[2] += gryoOri.z;
				zeroDriftCount++;
				if(zeroDriftCount == 500)
				{
					zeroDriftAccAvg[0] = zeroDriftAccAll[0] / zeroDriftCount;
					zeroDriftAccAvg[1] = zeroDriftAccAll[1] / zeroDriftCount;
					zeroDriftAccAvg[2] = zeroDriftAccAll[2] / zeroDriftCount;
					zeroDriftGyroAvg[0] = zeroDriftGyroAll[0] / zeroDriftCount;
					zeroDriftGyroAvg[1] = zeroDriftGyroAll[1] / zeroDriftCount;
					zeroDriftGyroAvg[2] = zeroDriftGyroAll[2] / zeroDriftCount;
					zeroDriftFlag = 1;
				}
		}
		else
		{
				MPU_Get_Accelerometer(&(accOri.x), &(accOri.y),&(accOri.z));
				MPU_Get_Gyroscope(&(gryoOri.x), &(gryoOri.y),&(gryoOri.z));
				taskENTER_CRITICAL();
                accOriCal.x = accOri.x - zeroDriftAccAvg[0];
				accOriCal.y = accOri.y - zeroDriftAccAvg[1];
				accOriCal.z = accOri.z - zeroDriftAccAvg[2]  + 16384;
				gryoOriCal.x = gryoOri.x - zeroDriftGyroAvg[0];
				gryoOriCal.y = gryoOri.y - zeroDriftGyroAvg[1];
				gryoOriCal.z = gryoOri.z - zeroDriftGyroAvg[2];
                taskEXIT_CRITICAL();
			
				accOriCal.x = accOriCal.x;
			  accOriCal.y = accOriCal.y;
				
				gryoOriCal.x = gryoOriCal.x;
				gryoOriCal.y = gryoOriCal.y;
		}
		
		
		
		
//		printf("3÷·Õ”¬›“«:%f\t%f\t%f\r\n",(CarAcc.x),(CarAcc.y),(CarAcc.z));
//		printf("%d\t%d\t%d\r\n",(accOri.x),(int16_t)(accOri.y),(int16_t)(accOri.z));
//		printf("%d\t%d\t%d\r\n",(gryoOri.x),(int16_t)(gryoOri.y),(int16_t)(gryoOri.z));
//		printf("IMUŒ¬∂»°Ê:%f\r\n",Temp);
//		printf("temp: %f %f\r\n",Mpu6050_Data.Temp);
		
	}
}

