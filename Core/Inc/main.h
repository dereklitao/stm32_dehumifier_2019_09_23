/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
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
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

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
#define STEP4_Pin GPIO_PIN_2
#define STEP4_GPIO_Port GPIOE
#define STEP3_Pin GPIO_PIN_3
#define STEP3_GPIO_Port GPIOE
#define STEP2_Pin GPIO_PIN_4
#define STEP2_GPIO_Port GPIOE
#define STEP1_Pin GPIO_PIN_5
#define STEP1_GPIO_Port GPIOE
#define TH2H_Pin GPIO_PIN_2
#define TH2H_GPIO_Port GPIOC
#define TH2T_Pin GPIO_PIN_3
#define TH2T_GPIO_Port GPIOC
#define NTC4_Pin GPIO_PIN_0
#define NTC4_GPIO_Port GPIOA
#define NTC3_Pin GPIO_PIN_1
#define NTC3_GPIO_Port GPIOA
#define NTC2_Pin GPIO_PIN_2
#define NTC2_GPIO_Port GPIOA
#define NTC1_Pin GPIO_PIN_3
#define NTC1_GPIO_Port GPIOA
#define DA34_Pin GPIO_PIN_4
#define DA34_GPIO_Port GPIOA
#define DA12_Pin GPIO_PIN_5
#define DA12_GPIO_Port GPIOA
#define TH1H_Pin GPIO_PIN_6
#define TH1H_GPIO_Port GPIOA
#define TH1T_Pin GPIO_PIN_7
#define TH1T_GPIO_Port GPIOA
#define SW6_Pin GPIO_PIN_4
#define SW6_GPIO_Port GPIOC
#define SW5_Pin GPIO_PIN_5
#define SW5_GPIO_Port GPIOC
#define SW4_Pin GPIO_PIN_0
#define SW4_GPIO_Port GPIOB
#define SW3_Pin GPIO_PIN_1
#define SW3_GPIO_Port GPIOB
#define SW2_Pin GPIO_PIN_2
#define SW2_GPIO_Port GPIOB
#define SW1_Pin GPIO_PIN_7
#define SW1_GPIO_Port GPIOE
#define PWM2_Pin GPIO_PIN_10
#define PWM2_GPIO_Port GPIOB
#define PWM1_Pin GPIO_PIN_11
#define PWM1_GPIO_Port GPIOB
#define UTx3_Pin GPIO_PIN_8
#define UTx3_GPIO_Port GPIOD
#define URx3_Pin GPIO_PIN_9
#define URx3_GPIO_Port GPIOD
#define UEn3_Pin GPIO_PIN_10
#define UEn3_GPIO_Port GPIOD
#define SR1_Pin GPIO_PIN_13
#define SR1_GPIO_Port GPIOD
#define SR2_Pin GPIO_PIN_14
#define SR2_GPIO_Port GPIOD
#define SR3_Pin GPIO_PIN_15
#define SR3_GPIO_Port GPIOD
#define SR4_Pin GPIO_PIN_6
#define SR4_GPIO_Port GPIOC
#define DR1_Pin GPIO_PIN_7
#define DR1_GPIO_Port GPIOC
#define DR2_Pin GPIO_PIN_8
#define DR2_GPIO_Port GPIOC
#define DR3_Pin GPIO_PIN_9
#define DR3_GPIO_Port GPIOC
#define DR4_Pin GPIO_PIN_8
#define DR4_GPIO_Port GPIOA
#define UTx4_Pin GPIO_PIN_9
#define UTx4_GPIO_Port GPIOA
#define URx4_Pin GPIO_PIN_10
#define URx4_GPIO_Port GPIOA
#define UEn4_Pin GPIO_PIN_11
#define UEn4_GPIO_Port GPIOA
#define SWDIO_Pin GPIO_PIN_13
#define SWDIO_GPIO_Port GPIOA
#define SWCLK_Pin GPIO_PIN_14
#define SWCLK_GPIO_Port GPIOA
#define PWM4_Pin GPIO_PIN_15
#define PWM4_GPIO_Port GPIOA
#define UTx2_Pin GPIO_PIN_10
#define UTx2_GPIO_Port GPIOC
#define URx2_Pin GPIO_PIN_11
#define URx2_GPIO_Port GPIOC
#define UEn2_Pin GPIO_PIN_12
#define UEn2_GPIO_Port GPIOC
#define UTx1_Pin GPIO_PIN_5
#define UTx1_GPIO_Port GPIOD
#define URx1_Pin GPIO_PIN_6
#define URx1_GPIO_Port GPIOD
#define UEn1_Pin GPIO_PIN_7
#define UEn1_GPIO_Port GPIOD
#define PWM3_Pin GPIO_PIN_3
#define PWM3_GPIO_Port GPIOB
#define LED_Pin GPIO_PIN_7
#define LED_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
