/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2022 STMicroelectronics.
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
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
/*Variables for Stepper-------------------------------------------------------*/
int x_3 = 0;
int y_3 = 0;
int x_max = 160;
int y_max = 120;
int x_origin = 15;
int y_origin = 60;
int x_dir = 0;
int y_dir = 0;
int x_dif = 0;
int y_dif = 0;
int x_steps = 0;
int y_steps = 0;
int x_delay = 1000;
int y_delay = 650;
int steps_per_rev = 200;
int steps_max = 2000;
double pix_per_rev_x = 15.24;
double pix_per_rev_y = 11.42;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
/*Define for Stepper----------------------------------------------------------*/
#define DIR_PORT_x GPIOC
#define DIR_PIN_x GPIO_PIN_0
#define STEP_PORT_x GPIOC
#define STEP_PIN_x GPIO_PIN_1
#define DIR_PORT_y GPIOC
#define DIR_PIN_y GPIO_PIN_2
#define STEP_PORT_y GPIOC
#define STEP_PIN_y GPIO_PIN_3
/*UART_Cordinates--------------------------------------------------------------*/
uint8_t rec_data[4] = {0};
/*Functions for Stepper--------------------------------------------------------*/
void delay_us (uint16_t delay)
{
  __HAL_TIM_SET_COUNTER(&htim1, 0);
  while (__HAL_TIM_GET_COUNTER(&htim1) < delay);
}

void step_x (int steps, uint8_t direction, uint16_t delay)
{
  int i;
  if (direction == 0)
    HAL_GPIO_WritePin(DIR_PORT_x, DIR_PIN_x, GPIO_PIN_SET);
  else
    HAL_GPIO_WritePin(DIR_PORT_x, DIR_PIN_x, GPIO_PIN_RESET);
  for(i=0; i<steps; i++)
  {
    HAL_GPIO_WritePin(STEP_PORT_x, STEP_PIN_x, GPIO_PIN_SET);
    delay_us(delay);
    HAL_GPIO_WritePin(STEP_PORT_x, STEP_PIN_x, GPIO_PIN_RESET);
    delay_us(delay);
  }
}

void step_y (int steps, uint8_t direction, uint16_t delay)
{
  int i;
  if (direction == 0)
    HAL_GPIO_WritePin(DIR_PORT_y, DIR_PIN_y, GPIO_PIN_SET);
  else
    HAL_GPIO_WritePin(DIR_PORT_y, DIR_PIN_y, GPIO_PIN_RESET);
  for(i=0; i<steps; i++)
  {
    HAL_GPIO_WritePin(STEP_PORT_y, STEP_PIN_y, GPIO_PIN_SET);
    delay_us(delay);
    HAL_GPIO_WritePin(STEP_PORT_y, STEP_PIN_y, GPIO_PIN_RESET);
    delay_us(delay);
  }
}
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
  MX_USART3_UART_Init();
  MX_TIM1_Init();
  /* USER CODE BEGIN 2 */
  //HAL_UART_Receive_IT(&huart3,rec_data,4);
  HAL_TIM_Base_Start(&htim1);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  HAL_UART_Receive_IT(&huart3,rec_data,4);
	  HAL_Delay(20);
	  /*store the coordinate*/
	  while(rec_data[0] == 0xA5 && rec_data[3] == 0X5B){
	  	  x_3 = rec_data[1];
	  	  y_3 = rec_data[2];
	  	  /*calculate the difference in pixels from the origin to desired coordinate*/
	  	  x_dif = x_3 - x_origin;
	  	  y_dif = y_3 - y_origin;
	  	  /*choose direction and make differences positive*/
	  	  if(x_dif < 0){
	  		  x_dif = -1 * x_dif;
	  		  x_dir = 0;
	  	  }
	  	  else{
	  		  x_dir = 1;
	  	  }
	  	  if(y_dif < 0){
	  		  y_dif = -1 * y_dif;
	  		  y_dir = 1;
	  	  }
	  	  else{
	  		  y_dir = 0;
	  	  }
	  	  /*convert pixel differences to steps and make sure steps don't go OB*/
	  	  x_steps = (x_dif / pix_per_rev_x) * steps_per_rev;
	  	  if(x_steps > steps_max){
	  		  x_steps = steps_max;
	  	  }
	  	  y_steps = (y_dif / pix_per_rev_y) * steps_per_rev;
	  	  if(y_steps > steps_max){
	  	  		  y_steps = steps_max;
	  	  }
	  	  /*move striker to desired location*/
	  	  step_y(y_steps, y_dir, y_delay);
	  	  step_x(x_steps, x_dir, x_delay);
	  	  HAL_Delay(200);
	  	  /*move striker back to origin*/
	  	  if(x_dir == 0){
	  		  x_dir = 1;
	  	  }
	  	  else{
	  		  x_dir = 0;
	  	  }
	  	  if(y_dir == 0){
	  		  y_dir = 1;
	  	  }
	  	  else{
	  		  y_dir = 0;
	  	  }
	  	  step_y(y_steps, y_dir, y_delay);
	  	  step_x(x_steps, x_dir, x_delay);
	  	  HAL_Delay(200);
	  	  /*reset variables*/
	  	  x_3 = 0;
	  	  y_3 = 0;
	  	  x_dif = 0;
	  	  y_dif = 0;
	  	  x_steps = 0;
	  	  y_steps = 0;
	  	  rec_data[0] = 0;
	  	  rec_data[1] = 0;
	  	  rec_data[2] = 0;
	  	  rec_data[3] = 0;
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
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
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 16;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
//printf�?构
#ifdef __GNUC__
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch,FILE *f)
#endif

PUTCHAR_PROTOTYPE
{
    HAL_UART_Transmit(&huart3 , (uint8_t *)&ch, 1, HAL_MAX_DELAY);
    return ch;
}
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart->Instance == USART3) {
        if(rec_data[0] == 0xA5 && rec_data[3] == 0X5B)
        {
        	printf("x3 == %d\r\n",rec_data[1]);
        	printf("y3 == %d\r\n",rec_data[2]);
        }

       //__HAL_UART_CLEAR_FLAG(&huart3,UART_FLAG_RXNE);
       HAL_UART_Receive_IT(&huart3,rec_data,4);
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
