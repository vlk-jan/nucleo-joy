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
#include "stm32wlxx_hal.h"

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
#define RTC_N_PREDIV_S 10
#define RTC_PREDIV_S ((1<<RTC_N_PREDIV_S)-1)
#define RTC_PREDIV_A ((1<<(15-RTC_N_PREDIV_S))-1)
#define LED1_Pin GPIO_PIN_15
#define LED1_GPIO_Port GPIOB
#define LED2_Pin GPIO_PIN_9
#define LED2_GPIO_Port GPIOB
#define BUT1_Pin GPIO_PIN_0
#define BUT1_GPIO_Port GPIOA
#define PROB1_Pin GPIO_PIN_12
#define PROB1_GPIO_Port GPIOB
#define BUT3_Pin GPIO_PIN_6
#define BUT3_GPIO_Port GPIOC
#define BUT2_Pin GPIO_PIN_1
#define BUT2_GPIO_Port GPIOA
#define LED3_Pin GPIO_PIN_11
#define LED3_GPIO_Port GPIOB
#define USARTx_RX_Pin GPIO_PIN_3
#define USARTx_RX_GPIO_Port GPIOA
#define USARTx_TX_Pin GPIO_PIN_2
#define USARTx_TX_GPIO_Port GPIOA

/* USER CODE BEGIN Private defines */
#define L_BUTT_Port GPIOA
#define L1_BUTT_Pin GPIO_PIN_5
#define L2_BUTT_Pin GPIO_PIN_6
#define L3_BUTT_Pin GPIO_PIN_7

#define R_BUTT_Port GPIOA
#define R1_BUTT_Pin GPIO_PIN_4
#define R2_BUTT_Pin GPIO_PIN_9
#define R3_BUTT_Pin GPIO_PIN_12

#define LH_JOY_OFFSET 72
#define LV_JOY_OFFSET 48
#define RH_JOY_OFFSET 131
#define RV_JOY_OFFSET 131

#define DEAD_ZONE 100
#define LOWER_JOY_BOUND -1948
#define UPPER_JOY_BOUND 2047
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
