/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
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
#define Window_4_Arm_Pin GPIO_PIN_0
#define Window_4_Arm_GPIO_Port GPIOC
#define Window_4_Disarm_Pin GPIO_PIN_1
#define Window_4_Disarm_GPIO_Port GPIOC
#define System_Alarm_Pin GPIO_PIN_2
#define System_Alarm_GPIO_Port GPIOC
#define Door_1_Pin GPIO_PIN_3
#define Door_1_GPIO_Port GPIOC
#define Door_1_Disarm_Pin GPIO_PIN_0
#define Door_1_Disarm_GPIO_Port GPIOA
#define Door_1_Arm_Pin GPIO_PIN_1
#define Door_1_Arm_GPIO_Port GPIOA
#define USART_TX_Pin GPIO_PIN_2
#define USART_TX_GPIO_Port GPIOA
#define USART_RX_Pin GPIO_PIN_3
#define USART_RX_GPIO_Port GPIOA
#define Door_2_Pin GPIO_PIN_4
#define Door_2_GPIO_Port GPIOA
#define LD2_Pin GPIO_PIN_5
#define LD2_GPIO_Port GPIOA
#define Door_2_Disarm_Pin GPIO_PIN_6
#define Door_2_Disarm_GPIO_Port GPIOA
#define Door_2_Arm_Pin GPIO_PIN_7
#define Door_2_Arm_GPIO_Port GPIOA
#define Front_Door_Light_Pin GPIO_PIN_4
#define Front_Door_Light_GPIO_Port GPIOC
#define Back_Door_Light_Pin GPIO_PIN_5
#define Back_Door_Light_GPIO_Port GPIOC
#define Buzzer_Pin GPIO_PIN_0
#define Buzzer_GPIO_Port GPIOB
#define Window_4_Pin GPIO_PIN_1
#define Window_4_GPIO_Port GPIOB
#define Window_3_Pin GPIO_PIN_2
#define Window_3_GPIO_Port GPIOB
#define Keyboard_Input_Pin GPIO_PIN_12
#define Keyboard_Input_GPIO_Port GPIOB
#define Keyboard_Input_EXTI_IRQn EXTI15_10_IRQn
#define Keyboard_InputB13_Pin GPIO_PIN_13
#define Keyboard_InputB13_GPIO_Port GPIOB
#define Keyboard_InputB13_EXTI_IRQn EXTI15_10_IRQn
#define Keyboard_InputB14_Pin GPIO_PIN_14
#define Keyboard_InputB14_GPIO_Port GPIOB
#define Keyboard_InputB14_EXTI_IRQn EXTI15_10_IRQn
#define Keyboard_InputB15_Pin GPIO_PIN_15
#define Keyboard_InputB15_GPIO_Port GPIOB
#define Keyboard_InputB15_EXTI_IRQn EXTI15_10_IRQn
#define LCD_TX_Pin GPIO_PIN_6
#define LCD_TX_GPIO_Port GPIOC
#define Comm_Terminal_RX_Pin GPIO_PIN_7
#define Comm_Terminal_RX_GPIO_Port GPIOC
#define Window_1_Pin GPIO_PIN_8
#define Window_1_GPIO_Port GPIOC
#define Window_2_Pin GPIO_PIN_9
#define Window_2_GPIO_Port GPIOC
#define Keyboard_Output_Pin GPIO_PIN_10
#define Keyboard_Output_GPIO_Port GPIOA
#define Keyboard_OutputA11_Pin GPIO_PIN_11
#define Keyboard_OutputA11_GPIO_Port GPIOA
#define Keyboard_OutputA12_Pin GPIO_PIN_12
#define Keyboard_OutputA12_GPIO_Port GPIOA
#define TMS_Pin GPIO_PIN_13
#define TMS_GPIO_Port GPIOA
#define TCK_Pin GPIO_PIN_14
#define TCK_GPIO_Port GPIOA
#define Window_1_Arm_Pin GPIO_PIN_11
#define Window_1_Arm_GPIO_Port GPIOC
#define Window_1_Disarm_Pin GPIO_PIN_12
#define Window_1_Disarm_GPIO_Port GPIOC
#define SWO_Pin GPIO_PIN_3
#define SWO_GPIO_Port GPIOB
#define Window_2_Arm_Pin GPIO_PIN_4
#define Window_2_Arm_GPIO_Port GPIOB
#define Window_2_Disarm_Pin GPIO_PIN_5
#define Window_2_Disarm_GPIO_Port GPIOB
#define Window_3_Arm_Pin GPIO_PIN_8
#define Window_3_Arm_GPIO_Port GPIOB
#define Window_3_Disarm_Pin GPIO_PIN_9
#define Window_3_Disarm_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
