/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "ssd1306.h"
#include "test.h"
#include "fonts.h"
#include <stdio.h>

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define V25 1.53 //V25=Vsense at 25 degrees , 25 derecedeki voltaj degeri(typical value: 1.32 & 1.43 & 1.50).

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;
DMA_HandleTypeDef hdma_adc1;

I2C_HandleTypeDef hi2c1;

/* USER CODE BEGIN PV */


float temp_voltage=0;
float temperature=0;

uint16_t volatile adcData[3];
uint16_t volatile adcDataBuffer[3];
uint16_t volatile adcConvComplt = 0; //Flag for Conversion completion
char message[50];
char adc_char[20];

char harf = 'C';

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_ADC1_Init(void);
static void MX_I2C1_Init(void);
/* USER CODE BEGIN PFP */

void display_adcData(uint32_t adcValue);

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
  MX_DMA_Init();
  MX_ADC1_Init();
  MX_I2C1_Init();
  /* USER CODE BEGIN 2 */

  SSD1306_Init();

  SSD1306_GotoXY (0, 0);
  SSD1306_Puts ("Initializing...", &Font_7x10, 1);
  SSD1306_UpdateScreen ();  // update display
  HAL_Delay(500);


  HAL_ADCEx_Calibration_Start(&hadc1);

  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_10, GPIO_PIN_SET);
  HAL_Delay(500);
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_10, GPIO_PIN_RESET);
  HAL_Delay(200);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	while (1) {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

		HAL_ADC_Start_DMA(&hadc1,(uint32_t *)adcDataBuffer, 3);
		while(adcConvComplt == 0)
		{

		}
		adcConvComplt = 0;

		uint16_t okupot=adcData[0];//adcData[0] indicates pot ADC Value
		uint16_t okuldr=adcData[1];//adcData[1] indicates ldr ADC Value
		uint16_t okutemp=adcData[2];//adcData[2] indicates temperature ADC Value

		temp_voltage = (float) okutemp * (3.3 / 4095);
		temperature = (V25 - temp_voltage) / 0.0043 + 25;   // V25 =1.50
		//Calculate the current temperature value -->  temp_voltage = Vsense
		//Temperature={(V25-Vsense)/Avg_Slope}+25 ; Avg_Slope=4.0 & 4.3 & 4.6 mV

		//HAL_Delay(100);
		//sprintf(message,"POT = %u || LDR = %u || Temperature = %0.1f\n",okupot, okuldr, temperature);


		if (okupot < 2300 ) {

			HAL_GPIO_WritePin(GPIOC, LD4_Pin,GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOC, LD3_Pin | GPIO_PIN_10, GPIO_PIN_RESET);

		} else if (okupot >= 2300  && okupot < 3700 ) {

			HAL_GPIO_WritePin(GPIOC, LD3_Pin,GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOC, LD4_Pin | GPIO_PIN_10, GPIO_PIN_RESET);

		} else if (okupot >= 3700 ) {
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_10,GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOC, LD3_Pin | LD4_Pin, GPIO_PIN_RESET);

		} else{
			HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_10 | LD3_Pin |LD4_Pin);
			HAL_Delay(30);
		}
		///////////////////////////////////////////////////
/*
		SSD1306_Clear();

		SSD1306_GotoXY(0, 2);
		SSD1306_Puts("POT:", &Font_11x18, 1);

		SSD1306_GotoXY(30, 2);
		SSD1306_Puts("LDR:", &Font_11x18, 1);

		SSD1306_GotoXY(0, 45);
		SSD1306_Puts("TEMP:", &Font_11x18, 1);

		SSD1306_UpdateScreen();

		HAL_Delay(1000);
*/
		SSD1306_Clear();

		SSD1306_GotoXY(10, 5);
		SSD1306_Puts("POT: ", &Font_7x10, 1);
		SSD1306_GotoXY(48, 1);
		display_adcData(okupot);

		SSD1306_GotoXY(10, 30);
		SSD1306_Puts("LDR: ", &Font_7x10, 1);
		SSD1306_GotoXY(48, 24);
		display_adcData(okuldr);

		SSD1306_GotoXY(10, 52);
		SSD1306_Puts("TEMP: ", &Font_7x10, 1);
		SSD1306_GotoXY(48, 45);
		sprintf(adc_char,"%0.2f", temperature);
		SSD1306_Puts(adc_char, &Font_11x18, 1);

		//SSD1306_ScrollRight(5, 50); //Nasıl işlediğini tam anlamadım

		SSD1306_UpdateScreen();

		HAL_Delay(500);

		/////////////////////////////////////////////////
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
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

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
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
  PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV2;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */

  /** Common config
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ScanConvMode = ADC_SCAN_ENABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 3;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_1;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_55CYCLES_5;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_2;
  sConfig.Rank = ADC_REGULAR_RANK_2;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_TEMPSENSOR;
  sConfig.Rank = ADC_REGULAR_RANK_3;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 400000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Channel1_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn);

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
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, LD4_Pin|LD3_Pin|K_rm_z__pin_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : LD4_Pin LD3_Pin K_rm_z__pin_Pin */
  GPIO_InitStruct.Pin = LD4_Pin|LD3_Pin|K_rm_z__pin_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */


void display_adcData(uint32_t adcValue)
{
	sprintf(adc_char,"%lu", adcValue);
	SSD1306_Puts(adc_char, &Font_11x18, 1);
	//SSD1306_UpdateScreen();
}


void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
	if(hadc->Instance==ADC1){
		for(uint8_t i=0; i<3; i++){
			adcData[i]=adcDataBuffer[i];
			//hadc->Instance->DR
			//hadc->Instance==ADC1

		}

	}
	adcConvComplt = 1;
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
