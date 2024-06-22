#include "pc_proc.h"

#define FRAME_HEADER  0X7B
#define FRAME_TAIL 0X7D

//串口对于的是usart1  对应的缓冲数组为 Uart1_Rx, Uart1_Tx



osThreadId PCTaskHandle;
//extern u8 TaskListFlag;
//extern u8 RunTimeFlag;
//extern char InfoBuffer[1000];
extern u8 PCdatabufRx[200];
extern u8 PCdatabufTx[200];
//extern u8 Uart1Rx[MAX_UART_RX_NUM];
//extern u8 Uart1Tx[MAX_UART_TX_NUM];

extern CAR_STATE REAL_CAR;
extern float volReal;
extern Vector3i_t accOriCal;
extern Vector3i_t gryoOriCal;
extern u8 zeroDriftFlag;
u8 ROSControl = 1;

void PC_Task_Proc(void const * argument)
{
	for(; ;)
	{
		//printf("%f %f %f\r\n",REAL_CAR.vx, REAL_CAR.vy, REAL_CAR.vz);
			if(ROSControl && zeroDriftFlag)
			{
				PCdatabufTx[0] = FRAME_HEADER;
				PCdatabufTx[2] = (int)(REAL_CAR.vx * 1000) >> 8;
				PCdatabufTx[3] = (int)(REAL_CAR.vx * 1000);
				PCdatabufTx[4] = (int)(REAL_CAR.vy * 1000) >> 8;
				PCdatabufTx[5] = (int)(REAL_CAR.vy * 1000);
				PCdatabufTx[6] = (int)(REAL_CAR.vz * 1000) >> 8;
				PCdatabufTx[7] = (int)(REAL_CAR.vz * 1000);
				
				PCdatabufTx[8] = (int)(accOriCal.x) >> 8;
				PCdatabufTx[9] = (int)(accOriCal.x);
				PCdatabufTx[10] = (int)(accOriCal.y) >> 8;
				PCdatabufTx[11] = (int)(accOriCal.y);
				PCdatabufTx[12] = (int)(accOriCal.z) >> 8;
				PCdatabufTx[13] = (int)(accOriCal.z);
				
				PCdatabufTx[14] = (int)(gryoOriCal.x) >> 8;
				PCdatabufTx[15] = (int)(gryoOriCal.x);
				PCdatabufTx[16] = (int)(gryoOriCal.y) >> 8;
				PCdatabufTx[17] = (int)(gryoOriCal.y);
				PCdatabufTx[18] = (int)(gryoOriCal.z) >> 8;
				PCdatabufTx[19] = (int)(gryoOriCal.z);
				
				PCdatabufTx[20] = (int)(volReal * 1000);
				PCdatabufTx[21] = (int)(volReal * 1000) >> 8;

				PCdatabufTx[22]=  Check_Sum(22,1); 
				PCdatabufTx[23] = FRAME_TAIL;
				
				HAL_UART_Transmit_DMA(&huart1, PCdatabufTx, 24);
			}
// if(TaskListFlag)
// {
//	 
//	TaskListFlag = 0;
//	memset(InfoBuffer,0,1000);	 
//	vTaskList(InfoBuffer);
//	printf("\r\n任务名称 运行状态 优先级 剩余堆栈 任务序号\r\n");
//	printf("%s\r\n", InfoBuffer);
//	printf("B : 阻塞, R : 就绪, D : 删除, S : 暂停\r\n"); 
// }
// if(RunTimeFlag)
// {
//	Rprintf("%d\r\n", size);unTimeFlag = 0; 
//	memset(InfoBuffer,0,1000);				//信息缓冲区清零
//	vTaskGetRunTimeStats(InfoBuffer);		//获取任务运行时间信息
//	printf("任务名\t\t\t运行时间\t运行所占百分比\r\n");
//	printf("%s\r\n",InfoBuffer);

// }
	osDelay(100);
	}
}



void PC_Data_Rx_Proc(u16 size)
{
		printf("%d\r\n", size);
		float vxTemp = 0;
		float vyTemp = 0;
		float vzTemp = 0;
		if(PCdatabufRx[0] == FRAME_HEADER && PCdatabufRx[size - 1] ==  FRAME_TAIL)
		{
			
			REAL_CAR.targetvx = XYZ_Target_Speed_transition(PCdatabufRx[3], PCdatabufRx[4]);
			REAL_CAR.targetvy = XYZ_Target_Speed_transition(PCdatabufRx[5], PCdatabufRx[6]); 
			REAL_CAR.targetvz = XYZ_Target_Speed_transition(PCdatabufRx[7], PCdatabufRx[8]);
			printf("%f %f %f \r\n", REAL_CAR.targetvx, REAL_CAR.targetvy ,REAL_CAR.targetvz);
		}

}


/**************************************************************************
Function: After the top 8 and low 8 figures are integrated into a short type data, the unit reduction is converted
Input   : 8 bits high, 8 bits low
Output  : The target velocity of the robot on the X/Y/Z axis
函数功能：将上位机发过来的高8位和低8位数据整合成一个short型数据后，再做单位还原换算
入口参数：高8位，低8位
返回  值：机器人X/Y/Z轴的目标速度
**************************************************************************/
float XYZ_Target_Speed_transition(u8 High,u8 Low)
{
	//Data conversion intermediate variable
	//数据转换的中间变量
	short transition; 
	
	//将高8位和低8位整合成一个16位的short型数据
	//The high 8 and low 8 bits are integrated into a 16-bit short data
	transition=((High<<8)+Low); 
	return 
		transition/1000+(transition%1000)*0.001; //Unit conversion, mm/s->m/s //单位转换, mm/s->m/s						
}


u8 Check_Sum(unsigned char Count_Number,unsigned char Mode)
{
	unsigned char check_sum=0,k;
	
	//Validate the data to be sent
	//对要发送的数据进行校验
	if(Mode==1)
	for(k=0;k<Count_Number;k++)
	{
	check_sum=check_sum^PCdatabufTx[k];
	}
	
	//Verify the data received
	//对接收到的数据进行校验
	if(Mode==0)
	for(k=0;k<Count_Number;k++)
	{
	check_sum=check_sum^PCdatabufTx[k];
	}
	return check_sum;
}

//	Send_Data.buffer[0]=Send_Data.Sensor_Str.Frame_Header; //Frame_heade //帧头
//  Send_Data.buffer[1]=Flag_Stop; //Car software loss marker //小车软件失能标志位
//	
//	//The three-axis speed of / / car is split into two eight digit Numbers
//	//小车三轴速度,各轴都拆分为两个8位数据再发送
//	Send_Data.buffer[2]=Send_Data.Sensor_Str.X_speed >>8; 
//	Send_Data.buffer[3]=Send_Data.Sensor_Str.X_speed ;    
//	Send_Data.buffer[4]=Send_Data.Sensor_Str.Y_speed>>8;  
//	Send_Data.buffer[5]=Send_Data.Sensor_Str.Y_speed;     
//	Send_Data.buffer[6]=Send_Data.Sensor_Str.Z_speed >>8; 
//	Send_Data.buffer[7]=Send_Data.Sensor_Str.Z_speed ;    
//	
//	//The acceleration of the triaxial axis of / / imu accelerometer is divided into two eight digit reams
//	//IMU加速度计三轴加速度,各轴都拆分为两个8位数据再发送
//	Send_Data.buffer[8]=Send_Data.Sensor_Str.Accelerometer.X_data>>8; 
//	Send_Data.buffer[9]=Send_Data.Sensor_Str.Accelerometer.X_data;   
//	Send_Data.buffer[10]=Send_Data.Sensor_Str.Accelerometer.Y_data>>8;
//	Send_Data.buffer[11]=Send_Data.Sensor_Str.Accelerometer.Y_data;
//	Send_Data.buffer[12]=Send_Data.Sensor_Str.Accelerometer.Z_data>>8;
//	Send_Data.buffer[13]=Send_Data.Sensor_Str.Accelerometer.Z_data;
//	
//	//The axis of the triaxial velocity of the / /imu is divided into two eight digits
//	//IMU角速度计三轴角速度,各轴都拆分为两个8位数据再发送
//	Send_Data.buffer[14]=Send_Data.Sensor_Str.Gyroscope.X_data>>8;
//	Send_Data.buffer[15]=Send_Data.Sensor_Str.Gyroscope.X_data;
//	Send_Data.buffer[16]=Send_Data.Sensor_Str.Gyroscope.Y_data>>8;
//	Send_Data.buffer[17]=Send_Data.Sensor_Str.Gyroscope.Y_data;
//	Send_Data.buffer[18]=Send_Data.Sensor_Str.Gyroscope.Z_data>>8;
//	Send_Data.buffer[19]=Send_Data.Sensor_Str.Gyroscope.Z_data;
//	
//	//Battery voltage, split into two 8 digit Numbers
//	//电池电压,拆分为两个8位数据发送
//	Send_Data.buffer[20]=Send_Data.Sensor_Str.Power_Voltage >>8; 
//	Send_Data.buffer[21]=Send_Data.Sensor_Str.Power_Voltage; 

//  //Data check digit calculation, Pattern 1 is a data check
//  //数据校验位计算，模式1是发送数据校验
//	Send_Data.buffer[22]=Check_Sum(22,1); 
//	
//	Send_Data.buffer[23]=Send_Data.Sensor_Str.Frame_Tail; //Frame_tail //帧尾