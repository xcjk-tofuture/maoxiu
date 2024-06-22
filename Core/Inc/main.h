/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

typedef int32_t  s32;
typedef int16_t s16;
typedef int8_t  s8;
typedef __IO uint32_t  vu32;
typedef __IO uint16_t vu16;
typedef __IO uint8_t  vu8;
typedef uint32_t  u32;
typedef uint16_t u16;
typedef uint8_t  u8;
typedef const uint32_t uc32;  /*!< Read Only */
typedef const uint16_t uc16;  /*!< Read Only */
typedef const uint8_t uc8;   /*!< Read Only */
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define M1PWM0_Pin GPIO_PIN_5
#define M1PWM0_GPIO_Port GPIOE
#define M1PWM1_Pin GPIO_PIN_6
#define M1PWM1_GPIO_Port GPIOE
#define LCD_RES_Pin GPIO_PIN_2
#define LCD_RES_GPIO_Port GPIOA
#define LCD_DC_Pin GPIO_PIN_3
#define LCD_DC_GPIO_Port GPIOA
#define LCD_CS_Pin GPIO_PIN_4
#define LCD_CS_GPIO_Port GPIOA
#define M2PWM0_Pin GPIO_PIN_9
#define M2PWM0_GPIO_Port GPIOE
#define M2PWM1_Pin GPIO_PIN_11
#define M2PWM1_GPIO_Port GPIOE
#define M3PWM0_Pin GPIO_PIN_13
#define M3PWM0_GPIO_Port GPIOE
#define M3PWM1_Pin GPIO_PIN_14
#define M3PWM1_GPIO_Port GPIOE
#define RGB_green_Pin GPIO_PIN_12
#define RGB_green_GPIO_Port GPIOD
#define RGB_red_Pin GPIO_PIN_13
#define RGB_red_GPIO_Port GPIOD
#define RGB_blue_Pin GPIO_PIN_14
#define RGB_blue_GPIO_Port GPIOD
#define BUZZER_Pin GPIO_PIN_8
#define BUZZER_GPIO_Port GPIOA
#define M0PWM0_Pin GPIO_PIN_8
#define M0PWM0_GPIO_Port GPIOB
#define M0PWM1_Pin GPIO_PIN_9
#define M0PWM1_GPIO_Port GPIOB
#define key2_Pin GPIO_PIN_0
#define key2_GPIO_Port GPIOE
#define key1_Pin GPIO_PIN_1
#define key1_GPIO_Port GPIOE

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
