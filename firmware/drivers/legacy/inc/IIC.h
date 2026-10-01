#ifndef _IIC_H
#define _IIC_H


/*
硬件I2C模式
需要：
1.I2C
    I2C
    (默认设置)
    标准模式
    时钟频率100kHz
    地址长度7bit
    不用填写设备地址
取消下方注释
*/

// extern I2C_HandleTypeDef hi2c2;
// #define MPU6050_I2C_Handle hi2c2
// #define MPU6050_Hardware_I2C

/*
软件I2C模式
需要：
1.GPIO 2个
    均为开漏输出（上不上拉取决于外部电路）
    最高等级
取消下方注释,按照自己的管脚更改即可
*/

#define MPU_ADDR  0x68
void I2CInit(void);
#define MPU6050_Software_I2C

#ifdef MPU6050_Software_I2C
#define I2C_Group_SCL GPIOB // I2C的时钟GPIO组号
#define I2C_SCL GPIO_PIN_10  // I2C时钟的GPIO端口号

#define I2C_Group_SDA GPIOB // I2C的数据GPIO组号
#define I2C_SDA GPIO_PIN_11  // I2C数据的GPIO端口号

#define I2C_Write_SCL(x) HAL_GPIO_WritePin(I2C_Group_SCL, I2C_SCL, x)
#define I2C_Write_SDA(x) HAL_GPIO_WritePin(I2C_Group_SDA, I2C_SDA, x)

#define I2C_Read_SCL() HAL_GPIO_ReadPin(I2C_Group_SCL, I2C_SCL)
#define I2C_Read_SDA() HAL_GPIO_ReadPin(I2C_Group_SDA, I2C_SDA)
#endif


#ifdef MPU6050_Hardware_I2C

#endif


#endif