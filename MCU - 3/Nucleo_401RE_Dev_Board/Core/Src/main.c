/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <string.h>
#include <stdbool.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

	/* Bytes for comm terminal interrupt. */
	uint8_t CT_command[3];

	/* Bytes for keypresses. */
	uint8_t keypress[4];
	uint8_t keyCount = 0;

	/* Count for activation delay. */
	uint8_t delayCount = 0;

	/* LCD Commands. */
	uint8_t LCD_Instcode = 254;	/* Needs to be sent before each command. */
	uint8_t LCD_CLS = 1;		/* Clear screen. */
	uint8_t LCD_1strow = 128;	/* Sends cursor to start of 1st row. */
	uint8_t LCD_2ndrow = 192;	/* Sends cursor to start of 2nd row. */
	uint8_t LCD_3rdrow = 148;	/* Sends cursor to start of 3rd row. */
	uint8_t LCD_4throw = 212;	/* Sends cursor to start of 4th row. */

	/* LCD Line Buffers. */
	uint8_t LCD_Text1[20] = "00:00:00  Setup";		/* Stores line1 text. */
	uint8_t LCD_Text2[20] = "W1:D W2:D W3:D W4:D";	/* Stores line2 text. */
	uint8_t LCD_Text3[20] = "D1:D D2:D";			/* Stores line3 text. */
	uint8_t LCD_Text4[20];							/* Stores line4 text. */

	/* Runtime Parameters. */
	uint8_t password[4] = {'1', '1', '1', '1'};
	uint8_t activationDelay = 0;
	bool w1Armed = false;
	bool w2Armed = false;
	bool w3Armed = false;
	bool w4Armed = false;
	bool d1Armed = false;
	bool d2Armed = false;
	bool runMode = false;
	bool frontDSArmed = false;
	bool backDSArmed = false;
	uint8_t frontDSDistance = 4;
	uint8_t backDSDistance = 4;
	uint8_t timeofday[6] = {'0', '0', '0', '0', '0', '0'};

	/* Global States. */
	bool ARMED = false;
	bool ALARM = false;
	bool DISARMED = true;

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
TIM_HandleTypeDef htim10;

UART_HandleTypeDef huart2;
UART_HandleTypeDef huart6;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_USART6_UART_Init(void);
static void MX_TIM10_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART2_UART_Init();
  MX_USART6_UART_Init();
  MX_TIM10_Init();
  /* USER CODE BEGIN 2 */

  /* Trigger interrupt on huart6 if 3 bytes are received. Store in CT_byte. */
  /* On interrupt, go to HAL_UART_RxCpltCallback(). */
  HAL_UART_Receive_IT(&huart6, CT_command, 3);

  /* Initialize LCD. */
  HAL_Delay(100);
  HAL_UART_Transmit(&huart6,&LCD_Instcode,1,1000);
  HAL_UART_Transmit(&huart6,&LCD_1strow,1,1000);
  HAL_Delay(100);
  HAL_UART_Transmit(&huart6,LCD_Text1,sizeof(LCD_Text1),1000);
  HAL_UART_Transmit(&huart6,&LCD_Instcode,1,1000);
  HAL_UART_Transmit(&huart6,&LCD_2ndrow,1,1000);
  HAL_Delay(100);
  HAL_UART_Transmit(&huart6,LCD_Text2,sizeof(LCD_Text2),1000);
  HAL_UART_Transmit(&huart6,&LCD_Instcode,1,1000);
  HAL_UART_Transmit(&huart6,&LCD_3rdrow,1,1000);
  HAL_Delay(100);
  HAL_UART_Transmit(&huart6,LCD_Text3,sizeof(LCD_Text3),1000);

  /* Start TIM10 timer and enable Timer10 interrupt. */
  HAL_TIM_Base_Start_IT(&htim10);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  if (!HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_8) && w1Armed) {
		  LCD_Text2[3] = '!';
		  HAL_UART_Transmit(&huart6,&LCD_Instcode,1,1000);
		  HAL_UART_Transmit(&huart6,&LCD_2ndrow,1,1000);
		  HAL_UART_Transmit(&huart6,LCD_Text2,sizeof(LCD_Text2),1000);
		  ARMED = false;
		  ALARM = true;
		  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_11, GPIO_PIN_RESET);
		  while (ALARM) {
			  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_10, GPIO_PIN_SET);
			  HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_2);
			  if (!HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_9) && w2Armed) {
				  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_RESET);
				  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_SET);
				  LCD_Text2[8] = '!';
			  }
			  if (!HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_2) && w3Armed) {
				  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_RESET);
				  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_SET);
				  LCD_Text2[13] = '!';
			  }
			  if (!HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_1) && w4Armed) {
				  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0, GPIO_PIN_RESET);
				  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_2, GPIO_PIN_SET);
				  LCD_Text2[18] = '!';
			  }
			  if (!HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_3) && d1Armed) {
				  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_RESET);
				  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_SET);
				  LCD_Text3[3] = '!';
			  }
			  if (!HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_4) && d2Armed) {
				  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_RESET);
				  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET);
				  LCD_Text3[8] = '!';
			  }
			  HAL_UART_Transmit(&huart6,&LCD_Instcode,1,1000);
			  HAL_UART_Transmit(&huart6,&LCD_2ndrow,1,1000);
			  HAL_UART_Transmit(&huart6,LCD_Text2,sizeof(LCD_Text2),1000);
			  HAL_Delay(100);
			  HAL_UART_Transmit(&huart6,&LCD_Instcode,1,1000);
			  HAL_UART_Transmit(&huart6,&LCD_3rdrow,1,1000);
			  HAL_UART_Transmit(&huart6,LCD_Text3,sizeof(LCD_Text3),1000);
			  HAL_Delay(100);

			  if (delayCount >= activationDelay && ALARM) {
				  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
			  }
		  }
		  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_10, GPIO_PIN_RESET);
		  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_RESET);
		  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_RESET);
		  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_2, GPIO_PIN_RESET);
		  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_RESET);
		  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_RESET);
		  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_2, GPIO_PIN_RESET);
	  }
	  else if (!HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_9) && w2Armed) {
		  LCD_Text2[8] = '!';
		  HAL_UART_Transmit(&huart6,&LCD_Instcode,1,1000);
		  HAL_UART_Transmit(&huart6,&LCD_2ndrow,1,1000);
		  HAL_UART_Transmit(&huart6,LCD_Text2,sizeof(LCD_Text2),1000);
		  ARMED = false;
		  ALARM = true;
		  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_RESET);
		  while (ALARM) {
			  HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_2);
			  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_SET);
			  if (!HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_8) && w1Armed) {
				  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_11, GPIO_PIN_RESET);
				  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_10, GPIO_PIN_SET);
			  }
			  if (!HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_2) && w3Armed) {
				  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_RESET);
				  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_SET);
			  }
			  if (!HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_1) && w4Armed) {
				  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0, GPIO_PIN_RESET);
				  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_2, GPIO_PIN_SET);
			  }
			  if (!HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_3) && d1Armed) {
				  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_RESET);
				  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_SET);
			  }
			  if (!HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_4) && d2Armed) {
				  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_RESET);
				  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET);
			  }
			  HAL_UART_Transmit(&huart6,&LCD_Instcode,1,1000);
			  HAL_UART_Transmit(&huart6,&LCD_2ndrow,1,1000);
			  HAL_UART_Transmit(&huart6,LCD_Text2,sizeof(LCD_Text2),1000);
			  HAL_Delay(100);
			  HAL_UART_Transmit(&huart6,&LCD_Instcode,1,1000);
			  HAL_UART_Transmit(&huart6,&LCD_3rdrow,1,1000);
			  HAL_UART_Transmit(&huart6,LCD_Text3,sizeof(LCD_Text3),1000);
			  HAL_Delay(100);

			  if (delayCount >= activationDelay && ALARM) {
				  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
			  }
		  }
		  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_10, GPIO_PIN_RESET);
		  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_RESET);
		  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_RESET);
		  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_2, GPIO_PIN_RESET);
		  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_RESET);
		  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_RESET);
		  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_2, GPIO_PIN_RESET);
	  }
	  else if (!HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_2) && w3Armed) {
		  LCD_Text2[13] = '!';
		  HAL_UART_Transmit(&huart6,&LCD_Instcode,1,1000);
		  HAL_UART_Transmit(&huart6,&LCD_2ndrow,1,1000);
		  HAL_UART_Transmit(&huart6,LCD_Text2,sizeof(LCD_Text2),1000);
		  ARMED = false;
		  ALARM = true;
		  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_RESET);
		  while (ALARM) {
			  HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_2);
			  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_SET);
			  if (!HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_9) && w2Armed) {
				  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_RESET);
				  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_SET);
				  LCD_Text2[8] = '!';
			  }
			  if (!HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_8) && w1Armed) {
				  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_11, GPIO_PIN_RESET);
				  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_10, GPIO_PIN_SET);
				  LCD_Text2[3] = '!';
			  }
			  if (!HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_1) && w4Armed) {
				  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0, GPIO_PIN_RESET);
				  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_2, GPIO_PIN_SET);
				  LCD_Text2[18] = '!';
			  }
			  if (!HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_3) && d1Armed) {
				  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_RESET);
				  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_SET);
				  LCD_Text3[3] = '!';
			  }
			  if (!HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_4) && d2Armed) {
				  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_RESET);
				  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET);
				  LCD_Text3[8] = '!';
			  }
			  HAL_UART_Transmit(&huart6,&LCD_Instcode,1,1000);
			  HAL_UART_Transmit(&huart6,&LCD_2ndrow,1,1000);
			  HAL_UART_Transmit(&huart6,LCD_Text2,sizeof(LCD_Text2),1000);
			  HAL_Delay(100);
			  HAL_UART_Transmit(&huart6,&LCD_Instcode,1,1000);
			  HAL_UART_Transmit(&huart6,&LCD_3rdrow,1,1000);
			  HAL_UART_Transmit(&huart6,LCD_Text3,sizeof(LCD_Text3),1000);
			  HAL_Delay(100);

			  if (delayCount >= activationDelay && ALARM) {
				  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
			  }
		  }
		  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_10, GPIO_PIN_RESET);
		  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_RESET);
		  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_RESET);
		  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_2, GPIO_PIN_RESET);
		  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_RESET);
		  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_RESET);
		  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_2, GPIO_PIN_RESET);
	  }
	  else if (!HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_1) && w4Armed) {
		  LCD_Text2[18] = '!';
		  HAL_UART_Transmit(&huart6,&LCD_Instcode,1,1000);
		  HAL_UART_Transmit(&huart6,&LCD_2ndrow,1,1000);
		  HAL_UART_Transmit(&huart6,LCD_Text2,sizeof(LCD_Text2),1000);
		  ARMED = false;
		  ALARM = true;
		  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0, GPIO_PIN_RESET);
		  while (ALARM) {
			  HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_2);
			  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_2, GPIO_PIN_SET);
			  if (!HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_9) && w2Armed) {
				  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_RESET);
				  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_SET);
				  LCD_Text2[8] = '!';
			  }
			  if (!HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_2) && w3Armed) {
				  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_RESET);
				  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_SET);
				  LCD_Text2[13] = '!';
			  }
			  if (!HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_8) && w1Armed) {
				  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_11, GPIO_PIN_RESET);
				  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_10, GPIO_PIN_SET);
				  LCD_Text2[3] = '!';
			  }
			  if (!HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_3) && d1Armed) {
				  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_RESET);
				  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_SET);
				  LCD_Text3[3] = '!';
			  }
			  if (!HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_4) && d2Armed) {
				  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_RESET);
				  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET);
				  LCD_Text3[8] = '!';
			  }
			  HAL_UART_Transmit(&huart6,&LCD_Instcode,1,1000);
			  HAL_UART_Transmit(&huart6,&LCD_2ndrow,1,1000);
			  HAL_UART_Transmit(&huart6,LCD_Text2,sizeof(LCD_Text2),1000);
			  HAL_Delay(100);
			  HAL_UART_Transmit(&huart6,&LCD_Instcode,1,1000);
			  HAL_UART_Transmit(&huart6,&LCD_3rdrow,1,1000);
			  HAL_UART_Transmit(&huart6,LCD_Text3,sizeof(LCD_Text3),1000);
			  HAL_Delay(100);

			  if (delayCount >= activationDelay && ALARM) {
				  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
			  }
		  }
		  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_10, GPIO_PIN_RESET);
		  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_RESET);
		  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_RESET);
		  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_2, GPIO_PIN_RESET);
		  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_RESET);
		  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_RESET);
		  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_2, GPIO_PIN_RESET);
	  }
	  else if (!HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_3) && d1Armed) {
		  LCD_Text3[3] = '!';
		  HAL_UART_Transmit(&huart6,&LCD_Instcode,1,1000);
		  HAL_UART_Transmit(&huart6,&LCD_3rdrow,1,1000);
		  HAL_UART_Transmit(&huart6,LCD_Text3,sizeof(LCD_Text3),1000);
		  ARMED = false;
		  ALARM = true;
		  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_RESET);
		  while (ALARM) {
			  HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_2);
			  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_SET);
			  if (!HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_9) && w2Armed) {
				  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_RESET);
				  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_SET);
				  LCD_Text2[8] = '!';
			  }
			  if (!HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_2) && w3Armed) {
				  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_RESET);
				  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_SET);
				  LCD_Text2[13] = '!';
			  }
			  if (!HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_1) && w4Armed) {
				  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0, GPIO_PIN_RESET);
				  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_2, GPIO_PIN_SET);
				  LCD_Text2[18] = '!';
			  }
			  if (!HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_8) && w1Armed) {
				  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_11, GPIO_PIN_RESET);
				  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_10, GPIO_PIN_SET);
				  LCD_Text2[3] = '!';
			  }
			  if (!HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_4) && d2Armed) {
				  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_RESET);
				  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET);
				  LCD_Text3[8] = '!';
			  }
			  HAL_UART_Transmit(&huart6,&LCD_Instcode,1,1000);
			  HAL_UART_Transmit(&huart6,&LCD_2ndrow,1,1000);
			  HAL_UART_Transmit(&huart6,LCD_Text2,sizeof(LCD_Text2),1000);
			  HAL_Delay(100);
			  HAL_UART_Transmit(&huart6,&LCD_Instcode,1,1000);
			  HAL_UART_Transmit(&huart6,&LCD_3rdrow,1,1000);
			  HAL_UART_Transmit(&huart6,LCD_Text3,sizeof(LCD_Text3),1000);
			  HAL_Delay(100);

			  if (delayCount >= activationDelay && ALARM) {
				  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
			  }
		  }
		  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_10, GPIO_PIN_RESET);
		  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_RESET);
		  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_RESET);
		  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_2, GPIO_PIN_RESET);
		  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_RESET);
		  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_RESET);
		  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_2, GPIO_PIN_RESET);
	  }
	  else if (!HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_4) && d2Armed) {
		  LCD_Text3[8] = '!';
		  HAL_UART_Transmit(&huart6,&LCD_Instcode,1,1000);
		  HAL_UART_Transmit(&huart6,&LCD_3rdrow,1,1000);
		  HAL_UART_Transmit(&huart6,LCD_Text3,sizeof(LCD_Text3),1000);
		  ARMED = false;
		  ALARM = true;
		  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_RESET);
		  while (ALARM) {
			  HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_2);
			  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET);
			  if (!HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_9) && w2Armed) {
				  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_RESET);
				  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_SET);
				  LCD_Text2[8] = '!';
			  }
			  if (!HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_2) && w3Armed) {
				  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_RESET);
				  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_SET);
				  LCD_Text2[13] = '!';
			  }
			  if (!HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_1) && w4Armed) {
				  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0, GPIO_PIN_RESET);
				  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_2, GPIO_PIN_SET);
				  LCD_Text2[18] = '!';
			  }
			  if (!HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_3) && d1Armed) {
				  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_RESET);
				  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_SET);
				  LCD_Text3[3] = '!';
			  }
			  if (!HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_8) && w1Armed) {
				  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_11, GPIO_PIN_RESET);
				  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_10, GPIO_PIN_SET);
				  LCD_Text2[3] = '!';
			  }
			  HAL_UART_Transmit(&huart6,&LCD_Instcode,1,1000);
			  HAL_UART_Transmit(&huart6,&LCD_2ndrow,1,1000);
			  HAL_UART_Transmit(&huart6,LCD_Text2,sizeof(LCD_Text2),1000);
			  HAL_Delay(100);
			  HAL_UART_Transmit(&huart6,&LCD_Instcode,1,1000);
			  HAL_UART_Transmit(&huart6,&LCD_3rdrow,1,1000);
			  HAL_UART_Transmit(&huart6,LCD_Text3,sizeof(LCD_Text3),1000);
			  HAL_Delay(100);

			  if (delayCount >= activationDelay && ALARM) {
				  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
			  }
		  }
		  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_10, GPIO_PIN_RESET);
		  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_RESET);
		  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_RESET);
		  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_2, GPIO_PIN_RESET);
		  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_RESET);
		  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_RESET);
		  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_2, GPIO_PIN_RESET);
	  }

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief TIM10 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM10_Init(void)
{

  /* USER CODE BEGIN TIM10_Init 0 */

  /* USER CODE END TIM10_Init 0 */

  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM10_Init 1 */

  /* USER CODE END TIM10_Init 1 */
  htim10.Instance = TIM10;
  htim10.Init.Prescaler = 16000-1;
  htim10.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim10.Init.Period = 1000-1;
  htim10.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim10.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_Base_Init(&htim10) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_OC_Init(&htim10) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_TIMING;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_OC_ConfigChannel(&htim10, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM10_Init 2 */

  /* USER CODE END TIM10_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief USART6 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART6_UART_Init(void)
{

  /* USER CODE BEGIN USART6_Init 0 */

  /* USER CODE END USART6_Init 0 */

  /* USER CODE BEGIN USART6_Init 1 */

  /* USER CODE END USART6_Init 1 */
  huart6.Instance = USART6;
  huart6.Init.BaudRate = 9600;
  huart6.Init.WordLength = UART_WORDLENGTH_8B;
  huart6.Init.StopBits = UART_STOPBITS_1;
  huart6.Init.Parity = UART_PARITY_NONE;
  huart6.Init.Mode = UART_MODE_TX_RX;
  huart6.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart6.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart6) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART6_Init 2 */

  /* USER CODE END USART6_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, Window_4_Arm_Pin|System_Alarm_Pin|Front_Door_Light_Pin|Back_Door_Light_Pin
                          |Window_1_Arm_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, Window_4_Disarm_Pin|Window_1_Disarm_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, Door_1_Disarm_Pin|Door_2_Disarm_Pin|Keyboard_Output_Pin|Keyboard_OutputA11_Pin
                          |Keyboard_OutputA12_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, Door_1_Arm_Pin|LD2_Pin|Door_2_Arm_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, Buzzer_Pin|Window_2_Arm_Pin|Window_3_Arm_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, Window_2_Disarm_Pin|Window_3_Disarm_Pin, GPIO_PIN_SET);

  /*Configure GPIO pins : Window_4_Arm_Pin Window_4_Disarm_Pin System_Alarm_Pin Front_Door_Light_Pin
                           Back_Door_Light_Pin Window_1_Arm_Pin Window_1_Disarm_Pin */
  GPIO_InitStruct.Pin = Window_4_Arm_Pin|Window_4_Disarm_Pin|System_Alarm_Pin|Front_Door_Light_Pin
                          |Back_Door_Light_Pin|Window_1_Arm_Pin|Window_1_Disarm_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : Door_1_Pin Window_1_Pin Window_2_Pin */
  GPIO_InitStruct.Pin = Door_1_Pin|Window_1_Pin|Window_2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : Door_1_Disarm_Pin Door_1_Arm_Pin LD2_Pin Door_2_Disarm_Pin
                           Door_2_Arm_Pin Keyboard_Output_Pin Keyboard_OutputA11_Pin Keyboard_OutputA12_Pin */
  GPIO_InitStruct.Pin = Door_1_Disarm_Pin|Door_1_Arm_Pin|LD2_Pin|Door_2_Disarm_Pin
                          |Door_2_Arm_Pin|Keyboard_Output_Pin|Keyboard_OutputA11_Pin|Keyboard_OutputA12_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : Door_2_Pin */
  GPIO_InitStruct.Pin = Door_2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(Door_2_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : Buzzer_Pin Window_2_Arm_Pin Window_2_Disarm_Pin Window_3_Arm_Pin
                           Window_3_Disarm_Pin */
  GPIO_InitStruct.Pin = Buzzer_Pin|Window_2_Arm_Pin|Window_2_Disarm_Pin|Window_3_Arm_Pin
                          |Window_3_Disarm_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : Window_4_Pin Window_3_Pin */
  GPIO_InitStruct.Pin = Window_4_Pin|Window_3_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : Keyboard_Input_Pin Keyboard_InputB13_Pin Keyboard_InputB14_Pin Keyboard_InputB15_Pin */
  GPIO_InitStruct.Pin = Keyboard_Input_Pin|Keyboard_InputB13_Pin|Keyboard_InputB14_Pin|Keyboard_InputB15_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : PA8 */
  GPIO_InitStruct.Pin = GPIO_PIN_8;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF1_TIM1;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

}

/* USER CODE BEGIN 4 */

/* This callback is called by the HAL_UART_IRQHandler when the given number of bytes are received. (1) */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {

	if (huart->Instance == USART6 && !ALARM) {
		HAL_TIM_Base_Stop_IT(&htim10);

		/* Set password. */
		if (strcmp((char*)CT_command, "-p ") == 0) {
			HAL_UART_Receive_IT(&huart6, password, 4);
			CT_command[0] = 0;
			return;
		}

		/* Set activation delay. */
		else if (strcmp((char*)CT_command, "-a ") == 0) {
			HAL_UART_Receive_IT(&huart6, &activationDelay, 1);
			CT_command[0] = 0;
			return;
		}

		/* Set time of day. */
		else if (strcmp((char*)CT_command, "-t ") == 0) {
			HAL_UART_Receive_IT(&huart6, timeofday, 6);
			CT_command[0] = 0;
			return;
		}

		/* Set front and back distance sensors. */
		else if (strcmp((char*)CT_command, "-f ") == 0) {
			HAL_UART_Receive_IT(&huart6, &frontDSDistance, 1);
			CT_command[0] = 0;
			return;
		}
		else if (strcmp((char*)CT_command, "-b ") == 0) {
			HAL_UART_Receive_IT(&huart6, &backDSDistance, 1);
			CT_command[0] = 0;
			return;
		}

		/* Toggle runmode. */
		else if (strcmp((char*)CT_command, "-mt") == 0) {
			if (runMode) {
				runMode = false;
				LCD_Text1[10] = 'S';
				LCD_Text1[11] = 'e';
				LCD_Text1[12] = 't';
				LCD_Text1[13] = 'u';
				LCD_Text1[14] = 'p';
			}
			else {
				runMode = true;
				LCD_Text1[10] = 'R';
				LCD_Text1[11] = 'u';
				LCD_Text1[12] = 'n';
				LCD_Text1[13] = ' ';
				LCD_Text1[14] = ' ';
			}
		    HAL_UART_Transmit(&huart6,&LCD_Instcode,1,1000);
		    HAL_UART_Transmit(&huart6,&LCD_1strow,1,1000);
		    HAL_UART_Transmit(&huart6,LCD_Text1,sizeof(LCD_Text1),1000);
		}

		/* Toggle windows/door alarms. */
		else if (strcmp((char*)CT_command, "-w1") == 0) {
			if (w1Armed) {
				w1Armed = false;
				HAL_GPIO_WritePin(GPIOC, GPIO_PIN_11, GPIO_PIN_RESET);
				HAL_GPIO_WritePin(GPIOC, GPIO_PIN_12, GPIO_PIN_SET);
				LCD_Text2[3] = 'D';
			}
			else {
				w1Armed = true;
				HAL_GPIO_WritePin(GPIOC, GPIO_PIN_12, GPIO_PIN_RESET);
				HAL_GPIO_WritePin(GPIOC, GPIO_PIN_11, GPIO_PIN_SET);
				LCD_Text2[3] = 'A';
			}
		    HAL_UART_Transmit(&huart6,&LCD_Instcode,1,1000);
		    HAL_UART_Transmit(&huart6,&LCD_2ndrow,1,1000);
		    HAL_UART_Transmit(&huart6,LCD_Text2,sizeof(LCD_Text2),1000);
		}
		else if (strcmp((char*)CT_command, "-w2") == 0) {
			if (w2Armed) {
				w2Armed = false;
				LCD_Text2[8] = 'D';
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_RESET);
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_SET);
			}
			else {
				w2Armed = true;
				LCD_Text2[8] = 'A';
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_RESET);
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_SET);
			}
		    HAL_UART_Transmit(&huart6,&LCD_Instcode,1,1000);
		    HAL_UART_Transmit(&huart6,&LCD_2ndrow,1,1000);
		    HAL_UART_Transmit(&huart6,LCD_Text2,sizeof(LCD_Text2),1000);
		}
		else if (strcmp((char*)CT_command, "-w3") == 0) {
			if (w3Armed) {
				w3Armed = false;
				LCD_Text2[13] = 'D';
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_RESET);
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, GPIO_PIN_SET);
			}
			else {
				w3Armed = true;
				LCD_Text2[13] = 'A';
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, GPIO_PIN_RESET);
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_SET);
			}
		    HAL_UART_Transmit(&huart6,&LCD_Instcode,1,1000);
		    HAL_UART_Transmit(&huart6,&LCD_2ndrow,1,1000);
		    HAL_UART_Transmit(&huart6,LCD_Text2,sizeof(LCD_Text2),1000);
		}
		else if (strcmp((char*)CT_command, "-w4") == 0) {
			if (w4Armed) {
				w4Armed = false;
				LCD_Text2[18] = 'D';
				HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0, GPIO_PIN_RESET);
				HAL_GPIO_WritePin(GPIOC, GPIO_PIN_1, GPIO_PIN_SET);
			}
			else {
				w4Armed = true;
				LCD_Text2[18] = 'A';
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
			}
		    HAL_UART_Transmit(&huart6,&LCD_Instcode,1,1000);
		    HAL_UART_Transmit(&huart6,&LCD_2ndrow,1,1000);
		    HAL_UART_Transmit(&huart6,LCD_Text2,sizeof(LCD_Text2),1000);
		}
		else if (strcmp((char*)CT_command, "-d1") == 0) {
			if (d1Armed) {
				d1Armed = false;
				LCD_Text3[3] = 'D';
				HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_RESET);
				HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_SET);
			}
			else {
				d1Armed = true;
				LCD_Text3[3] = 'A';
				HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_RESET);
				HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_SET);
			}
		    HAL_UART_Transmit(&huart6,&LCD_Instcode,1,1000);
		    HAL_UART_Transmit(&huart6,&LCD_3rdrow,1,1000);
		    HAL_UART_Transmit(&huart6,LCD_Text3,sizeof(LCD_Text3),1000);
		}
		else if (strcmp((char*)CT_command, "-d2") == 0) {
			if (d2Armed) {
				d2Armed = false;
				LCD_Text3[8] = 'D';
				HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_RESET);
				HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_SET);
			}
			else {
				d2Armed = true;
				LCD_Text3[8] = 'A';
				HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_RESET);
				HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_SET);
			}
		    HAL_UART_Transmit(&huart6,&LCD_Instcode,1,1000);
		    HAL_UART_Transmit(&huart6,&LCD_3rdrow,1,1000);
		    HAL_UART_Transmit(&huart6,LCD_Text3,sizeof(LCD_Text3),1000);
		}

		/* Set up interrupt again. */
		HAL_UART_Receive_IT(&huart6, CT_command, 3);
		HAL_TIM_Base_Start_IT(&htim10);
	}
	HAL_UART_Receive_IT(&huart6, CT_command, 3);

}

/* Callback function for the keyboard. */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {

	HAL_NVIC_DisableIRQ(EXTI15_10_IRQn);

    if(GPIO_Pin == GPIO_PIN_12) {
	  	  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_RESET);
	  	  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, GPIO_PIN_RESET);
	  	  if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_12)) {
	  		  keypress[keyCount] = '1';
	  		  ++keyCount;
	  	  }

	  	  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_RESET);
	  	  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_SET);
	  	  if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_12)) {
	  		  keypress[keyCount] = '2';
	  		  ++keyCount;
	  	  }

	  	  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_RESET);
	  	  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, GPIO_PIN_SET);
	  	  if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_12)) {
	  		  keypress[keyCount] = '3';
	  		  ++keyCount;
	  	  }
    }
    else if(GPIO_Pin == GPIO_PIN_13) {
	  	  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_RESET);
	  	  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, GPIO_PIN_RESET);
	  	  if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_13)) {
	  		  keypress[keyCount] = '4';
	  		  ++keyCount;
	  	  }

	  	  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_RESET);
	  	  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_SET);
	  	  if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_13)) {
	  		  keypress[keyCount] = '5';
	  		  ++keyCount;
	  	  }

	  	  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_RESET);
	  	  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, GPIO_PIN_SET);
	  	  if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_13)) {
	  		  keypress[keyCount] = '6';
	  		  ++keyCount;
	  	  }
    }
    else if(GPIO_Pin == GPIO_PIN_14) {
	  	  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_RESET);
	  	  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, GPIO_PIN_RESET);
	  	  if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_14)) {
	  		  keypress[keyCount] = '7';
	  		  ++keyCount;
	  	  }

	  	  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_RESET);
	  	  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_SET);
	  	  if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_14)) {
	  		  keypress[keyCount] = '8';
	  		  ++keyCount;
	  	  }

	  	  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_RESET);
	  	  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, GPIO_PIN_SET);
	  	  if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_14)) {
	  		  keypress[keyCount] = '9';
	  		  ++keyCount;
	  	  }
    }
    else if(GPIO_Pin == GPIO_PIN_15) {
	  	  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_RESET);
	  	  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, GPIO_PIN_RESET);
	  	  if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_15)) {
	  		  keypress[keyCount] = '*';
	  		  ++keyCount;
	  	  }

	  	  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_RESET);
	  	  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_SET);
	  	  if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_15)) {
	  		  keypress[keyCount] = '0';
	  		  ++keyCount;
	  	  }

	  	  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_RESET);
	  	  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, GPIO_PIN_SET);
	  	  if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_15)) {
	  		  keypress[keyCount] = '#';
	  		  ++keyCount;
	  	  }
    }
    /* Set back to initial state. */
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, GPIO_PIN_SET);

    if (keyCount == 4) {
    	  /* Check if password correct and arm/disarm. */
    	  if ((password[0] == keypress[0]) && (password[1] == keypress[1]) && (password[2] == keypress[2]) && (password[3] == keypress[3])) {
    	  	  if (ARMED || ALARM) {
    	  	  	  ARMED = false;
    	  	  	  ALARM = false;
    	  	  	  DISARMED = true;
    	  	  	  LCD_Text2[3] = 'D'; LCD_Text2[8] = 'D'; LCD_Text2[13] = 'D'; LCD_Text2[18] = 'D';
    	  	  	  LCD_Text3[3] = 'D'; LCD_Text3[8] = 'D';
    	  	  	  w1Armed = false;
    	  	  	  w2Armed = false;
    	  	  	  w3Armed = false;
    	  	  	  w4Armed = false;
    	  	  	  d1Armed = false;
    	  	  	  d2Armed = false;
    	  	  	  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_12, GPIO_PIN_SET);
    	  	  	  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_SET);
    	  	  	  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, GPIO_PIN_SET);
    	  	  	  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_1, GPIO_PIN_SET);
    	  	  	  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_SET);
    	  	  	  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_SET);
    	  	  	  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_11, GPIO_PIN_RESET);
    	  	  	  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_RESET);
    	  	  	  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_RESET);
    	  	  	  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0, GPIO_PIN_RESET);
    	  	  	  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_RESET);
    	  	  	  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_RESET);
    	  	  	  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    			  HAL_UART_Transmit(&huart6,&LCD_Instcode,1,1000);
    			  HAL_UART_Transmit(&huart6,&LCD_2ndrow,1,1000);
    			  HAL_UART_Transmit(&huart6,LCD_Text2,sizeof(LCD_Text2),1000);
    			  HAL_UART_Transmit(&huart6,&LCD_Instcode,1,1000);
    			  HAL_UART_Transmit(&huart6,&LCD_3rdrow,1,1000);
    			  HAL_UART_Transmit(&huart6,LCD_Text3,sizeof(LCD_Text3),1000);
    	  	  }
    	  	  else {
    	  	  	  ARMED = true;
    	  	  	  DISARMED = false;
    	  	  	  LCD_Text2[3] = 'A'; LCD_Text2[8] = 'A'; LCD_Text2[13] = 'A'; LCD_Text2[18] = 'A';
    	  	  	  LCD_Text3[3] = 'A'; LCD_Text3[8] = 'A';
    	  	  	  w1Armed = true;
    	  	  	  w2Armed = true;
    	  	  	  w3Armed = true;
    	  	  	  w4Armed = true;
    	  	  	  d1Armed = true;
    	  	  	  d2Armed = true;
    	  	  	  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_11, GPIO_PIN_SET);
    	  	  	  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_SET);
    	  	  	  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_SET);
    	  	  	  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0, GPIO_PIN_SET);
    	  	  	  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_SET);
    	  	  	  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_SET);
    	  	  	  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_12, GPIO_PIN_RESET);
    	  	  	  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_RESET);
    	  	  	  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, GPIO_PIN_RESET);
    	  	  	  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_1, GPIO_PIN_RESET);
    	  	  	  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_RESET);
    	  	  	  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_RESET);
    			  HAL_UART_Transmit(&huart6,&LCD_Instcode,1,1000);
    			  HAL_UART_Transmit(&huart6,&LCD_2ndrow,1,1000);
    			  HAL_UART_Transmit(&huart6,LCD_Text2,sizeof(LCD_Text2),1000);
    			  HAL_UART_Transmit(&huart6,&LCD_Instcode,1,1000);
    			  HAL_UART_Transmit(&huart6,&LCD_3rdrow,1,1000);
    			  HAL_UART_Transmit(&huart6,LCD_Text3,sizeof(LCD_Text3),1000);
    	  	  }
    	  }
    	  keyCount = 0;
    }

    /* Wait for button to be released. */
    while(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_12) || HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_13) || HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_14) || HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_15)) {}
    HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
}

/* Interrupt routine for Timer. */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {

	if (htim->Instance == TIM10) {
		if (timeofday[5] < '9') {
			++timeofday[5];
		}
		else if (timeofday[4] < '5') {
			++timeofday[4];
			timeofday[5] = '0';
		}
		else if (timeofday[3] < '9') {
			++timeofday[3];
			timeofday[4] = '0';
		}
		else if (timeofday[2] < '5') {
			++timeofday[2];
			timeofday[3] = '0';
		}
		else if (timeofday[1] == '3' && timeofday[0] == '2') {
			timeofday[0] = '0';
			timeofday[1] = '0';
			timeofday[2] = '0';
			timeofday[3] = '0';
			timeofday[4] = '0';
			timeofday[5] = '0';
		}
		else if (timeofday[1] < '9') {
			++timeofday[1];
			timeofday[2] = '0';
		}
		else if (timeofday[0] < '2') {
			++timeofday[0];
			timeofday[1] = '0';
		}
		LCD_Text1[0] = timeofday[0];
		LCD_Text1[1] = timeofday[1];
		LCD_Text1[3] = timeofday[2];
		LCD_Text1[4] = timeofday[3];
		LCD_Text1[6] = timeofday[4];
		LCD_Text1[7] = timeofday[5];
		HAL_UART_Transmit(&huart6,&LCD_Instcode,1,1000);
		HAL_UART_Transmit(&huart6,&LCD_1strow,1,1000);
		HAL_UART_Transmit(&huart6,LCD_Text1,sizeof(LCD_Text1),1000);

		if (ALARM) {
			++delayCount;
		}
	}
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
