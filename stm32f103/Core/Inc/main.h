/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
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
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

//#include "ihm_mvs.h"
	
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

extern CAN_HandleTypeDef hcan;

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define BUILTIN_LED_Pin GPIO_PIN_13
#define BUILTIN_LED_GPIO_Port GPIOC
#define LCD_RST_Pin GPIO_PIN_2
#define LCD_RST_GPIO_Port GPIOA
#define LCD_EN_Pin GPIO_PIN_3
#define LCD_EN_GPIO_Port GPIOA
#define LCD_D4_Pin GPIO_PIN_4
#define LCD_D4_GPIO_Port GPIOA
#define LCD_D5_Pin GPIO_PIN_5
#define LCD_D5_GPIO_Port GPIOA
#define LCD_D6_Pin GPIO_PIN_6
#define LCD_D6_GPIO_Port GPIOA
#define LCD_D7_Pin GPIO_PIN_7
#define LCD_D7_GPIO_Port GPIOA
#define BTN_OK_Pin GPIO_PIN_12
#define BTN_OK_GPIO_Port GPIOB
#define BTN_VOLTAR_Pin GPIO_PIN_13
#define BTN_VOLTAR_GPIO_Port GPIOB
#define BTN_SALVAR_Pin GPIO_PIN_14
#define BTN_SALVAR_GPIO_Port GPIOB
#define BTN_TRIGGER_Pin GPIO_PIN_15
#define BTN_TRIGGER_GPIO_Port GPIOB
#define ENCODER_A_EXTI_Pin GPIO_PIN_3
#define ENCODER_A_EXTI_GPIO_Port GPIOB
#define ENCODER_A_EXTI_EXTI_IRQn EXTI3_IRQn
#define ENCODER_B_Pin GPIO_PIN_4
#define ENCODER_B_GPIO_Port GPIOB
#define ENCODER_BTN_Pin GPIO_PIN_5
#define ENCODER_BTN_GPIO_Port GPIOB
#define ENCODER_BTN_EXTI_IRQn EXTI9_5_IRQn
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
