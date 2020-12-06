/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
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
#include "adc.h"
#include "dma.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "OLED_Animation.h"
#include "OLED_Driver.h"
#include "OLED_GFX.h"
#include "OLED_FFT.h"
#include "OLED_UI.h"
#include "string.h"
#include "math.h"
#include "stdio.h"
#include "stm32_dsp.h"
#include "stmflash.h"
#include "table_fft.h"
#include "ds3231.h"
#include "i2c.h"
#include "flash.h"
#include "vfd.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
uint8_t Flag_Refrash = False;
uint8_t Flag_Blink = False;
uint8_t Flag_Sleep = False;
uint8_t Flag_Continue = False;
uint8_t Flag_Reception = True;
uint8_t OfflineCount = 6;
uint8_t SystemActive = False;
u8 datatemp[256] = {};
u16 RandomX=30,RandomY=30;

u16 Display_Mode = MODE_OFFLINE;
u16 Current_Mode = MODE_OFFLINE;

OLED_GFX oled;
OLED_FFT fft;
OLED_Animation motion;
OLED_UI ui;

void MainSysRun()
{
	static int runcount=0;
	if(Display_Mode==Current_Mode)
	{
		switch(Current_Mode)
		{
			case MODE_GAME:ui.SUIDataPrss();break;
			case MODE_NORMAL:ui.NUIDataPrss();break;
			case MODE_MUSIC:ui.MUIDataPrss();break;
			case MODE_DATE:ui.TUIDataPrss();break;
			case MODE_SHOW:ui.HUIDataPrss();break;
			case MODE_OFFLINE:break;
			default:ui.SUIDataPrss();break;
		}
		
		return;
	}
	else
	{
		runcount++;
		if(runcount==1)
		{
			switch(Current_Mode)
			{
				case MODE_GAME:ui.SUI_Out();;break;
				case MODE_NORMAL:ui.NUI_Out();break;
				case MODE_MUSIC:ui.MUI_Out();break;
				case MODE_DATE:ui.TUI_Out();break;
				case MODE_SHOW:ui.HUI_Out();break;
				case MODE_OFFLINE:break;
				default:ui.SUI_Out();break;
			}
		}
		else if(runcount>60)
		{
			runcount=0;
			Current_Mode=Display_Mode;
			switch(Current_Mode)
			{
				case MODE_GAME:ui.SUI_In();;break;
				case MODE_NORMAL:ui.NUI_In();break;
				case MODE_MUSIC:ui.MUI_In();break;
				case MODE_DATE:ui.TUI_In();break;
				case MODE_SHOW:ui.HUI_In();break;
				case MODE_OFFLINE:break;
				default:ui.SUI_In();break;
			}
		}
	}
}

#define Radius 60
#define RadiusC 56
#define RadiusB 53
#define HourRadius 30
#define MinRadius 40
#define SecRadius 50
u16 ColorPointer[3];

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
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
  MX_DMA_Init();
  MX_SPI1_Init();
  MX_USART1_UART_Init();
  MX_ADC1_Init();
  MX_TIM3_Init();
  MX_TIM4_Init();
  MX_TIM5_Init();
  MX_TIM9_Init();
  MX_SPI3_Init();
  MX_SPI2_Init();
  /* USER CODE BEGIN 2 */
	__HAL_UART_ENABLE_IT(&huart1, UART_IT_IDLE);
  HAL_UART_Receive_DMA(&huart1,Uart_Recv1_Buf,Uart_Max_Length);
	HAL_TIM_Base_Start_IT(&htim4);
	HAL_TIM_Base_Start_IT(&htim5);
	HAL_TIM_Base_Start_IT(&htim9);
	printf("Sys OK!\r\n");
  oled.Device_Init();
	motion.OLED_AllMotion_Init();
	InitData();
	SPI_Flash_Init();
	ui.SUI_In();
	VFD_Init();
	UsartCommand(&huart1,0xA002,3);
		
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
//		MUSIC_Mode();
		oled.Clear_Screen();
		motion.OLED_AllMotion(Device_Cmd.commandmotion,Device_Cmd.commandspeed);
		switch(Current_Mode)
		{
			case MODE_GAME:ui.SUIMainShow();break;
			case MODE_NORMAL:ui.NUIMainShow();break;
			case MODE_MUSIC:ui.MUIMainShow();break;
			case MODE_DATE:ui.TUIMainShow();break;
			case MODE_SHOW:ui.HUIMainShow();break;
			case MODE_OFFLINE:break;
			default:ui.SUIMainShow();break;
		}
		oled.Refrash_Screen();
	VFD_Clear();
	VFD_Display();
	VFD_Refresh_Vram();
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
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 4;
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
u16 offlinecount = 0;
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	static u16 TimeCount = 0;
	 
	if (htim->Instance == htim4.Instance)
	{
		DampAutoPos(0);
		MainSysRun();
	}
	if (htim->Instance == htim5.Instance)
	{
		oled.Calc_Color();
		Flag_Refrash = True;
	}
	if (htim->Instance == htim9.Instance)
	{
		switch(Device_Cmd.commandrgbmode)
		{
			case 1:oled.Set_Wheelf(TimeCount%96);break;
			case 2:oled.Set_Wheel(TimeCount%96);break;
			case 3:oled.Set_Wheelf(Device_Cmd.commandrgbcolor*96/256);break;
			default:oled.Set_Wheelf(Device_Cmd.commandrgbcolor*96/256);break;
		}
		TimeCount++;
		oled.Clear_FpsCount();
		if(Display_Mode != MODE_OFFLINE)
			if(offlinecount++>5)
				Display_Mode = MODE_OFFLINE;
	}
//	if (htim->Instance == htim6.Instance)
//	{
//		if(ContinueCount++>20)
//		{
//			ContinueCount = 0;
//			HAL_TIM_Base_Stop_IT(&htim6);
//		}
//	}
//	if (htim->Instance == htim7.Instance)
//	{
//		oled.Set_Wheel(TimeRun%96);
////		if(TimeRun++>20)
////		{
////			TimeRun = 0;
////		}
//		
//		if(SleepCount++>SLEEPTIME*2)
//		{
//				Flag_Sleep = True;
//				SleepCount = 0;
//		}
//	}
}
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	switch(GPIO_Pin)
	{
		case DS_INT_Pin:
		if(HAL_GPIO_ReadPin(DS_INT_GPIO_Port,DS_INT_Pin)==GPIO_PIN_RESET)  //LED1翻转
		{
			Time_Handle();
//			printf("Time:%s\r\n",ds3231.Time); 
		}
		break;
	}
}
//void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
//{
////	HAL_ADC_Stop_DMA(&hadc1);	
//}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

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
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
