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

u16 DATA_THEME = MODE_DATE;

#define Radius 60
#define RadiusC 56
#define RadiusB 53
#define HourRadius 30
#define MinRadius 40
#define SecRadius 50
u16 ColorPointer[3];
	
uint8_t adc_dma_ok = 0;					//adc的DMA传输完成标志
uint32_t adc_buf[NPT]={0};			//用于存储ADC转换结果的数组	

static long lBufInArray[NPT];					//传入给FFT的数组
//long lBufOutArray[NPT/2];				//FFT输出 因为输出结果是对称的 所以我们只取了前面的一半
//long lBufMagArray[NPT/2];				//每个频率对用的幅值
static long lBufOutArray[NPT];				//FFT输出 
long lBufMagArray[NPT];				//每个频率对用的幅值

void FFT_Start(void)
{
	/*启动ADC的DMA传输，配合定时器触发ADC转换*/
	HAL_ADC_Start_DMA(&hadc1, adc_buf, NPT);
	/*开启定时器，用溢出时间来触发ADC*/
	HAL_TIM_Base_Start(&htim3);
}

void FFT_Stop(void)
{
	/*停止ADC的DMA传输*/
	HAL_ADC_Stop_DMA(&hadc1);
	/*停止定时器*/
	HAL_TIM_Base_Stop(&htim3);
}

void GetPowerMag(void)
{
    signed short lX,lY;
    float X,Y,Mag;
    unsigned short i;
	
    for(i=0; i<NPT; i++)
    {
        lX  = (lBufOutArray[i] << 16) >> 16;
        lY  = (lBufOutArray[i] >> 16);
			
				//除以32768再乘65536是为了符合浮点数计算规律
        X = NPT * ((float)lX) / 32768;
        Y = NPT * ((float)lY) / 32768;
        Mag = sqrt(X * X + Y * Y)*1.0/ NPT;
        if(i == 0)	
            lBufMagArray[i] = (unsigned long)(Mag * 32768);
        else
            lBufMagArray[i] = (unsigned long)(Mag * 65536);
    }
}

/* 函数名称：void FFT_Pro(void)
 * 功能描述：FFT处理函数
 * 参数：无
 * 返回值：无
 */
void FFT_Pro(void)
{
	uint16_t i = 0;
	//填充数组
	for(i=0;i<NPT;i++)
	{
		//这里因为单片机的ADC只能测正的电压 所以需要前级加直流偏执
		//加入直流偏执后 1.25V 对应AD值为3103
		lBufInArray[i] = ((signed short)(adc_buf[i])-1551) << 18;		
	}
	//256点FFT变换
	cr4_fft_256_stm32(lBufOutArray, lBufInArray, NPT);
	//计算出对应频率的模 即每个频率对应的幅值
	GetPowerMag();	
}


/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
int Count=0;

u8 Dataleng;
char DataDis[20];
char DataDisf[3];

u8 showfpsflag = 0;
//void CopyString(char *ch1,char *ch2)
//{
//	int i;
//	for(i=0;i<cont_str(ch2);i++)
//		ch1[i]=ch2[i];
//}
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

OLED_GFX oled = OLED_GFX();
OLED_FFT fft = OLED_FFT();
OLED_Animation motion = OLED_Animation();
OLED_UI ui;

typedef struct
{
	uint8_t theme;
	uint8_t animotion;
} SAVE;

SAVE save={1,1};


void savedata(void)
{
	u32 msg[2];
	msg[0] = save.theme;
	msg[1] = save.animotion;
	
	STMFLASH_Write(FLASH_SAVE_ADDR,(u32*)msg,2);
}
void getdata(void)
{
	u32 msg[2];
	STMFLASH_Read(FLASH_SAVE_ADDR,(u32*)msg,2);
	save.theme = msg[0];
	save.animotion = msg[1];
}


u8 key1UpFlag = False;
u8 key1_fall_flag = False;
u8 short_key1_flag = False;
u8 key1_long_down = False;
u8 long_key1_flag = False;
int key1_holdon_ms = 0;
int key1upCnt = 0;

u8 key2UpFlag = False;
u8 key2_fall_flag = False;
u8 short_key2_flag = False;
u8 key2_long_down = False;
u8 long_key2_flag = False;
int key2_holdon_ms = 0;
int key2upCnt = 0;

void KeyProcess(void)
{
	if(short_key1_flag)
	{
		short_key1_flag=0;
		save.theme++;			
		if(save.theme>6)
			save.theme=1;
		
		oled.Display_hbmp(64-81/2,48-26/2,81,26,themetop[save.theme-1],0xFFFF);
		oled.Refrash_Screen();
		savedata();
		oled.Clear_Screen();
	}
	else if(key1_long_down)
	{
		showfpsflag = 1-showfpsflag;
		key1_long_down=0;
	}
	if(short_key2_flag)
	{
		short_key2_flag=0;
		save.animotion++;			
		if(save.animotion>6)
			save.animotion=0;
		
		oled.Display_hbmp(64-81/2,48-26/2,81,26,animationtop[save.animotion],0xFFFF);
		oled.Refrash_Screen();
		savedata();
		oled.Clear_Screen();
	}
	else if(key2_long_down)
	{
		key2_long_down=0;
		if(save.animotion==7)
			save.animotion=0;
		else
			save.animotion=7;
		savedata();
	}
}

u16 fps = 0;
char fpschar[20];
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
	getdata();
	__HAL_UART_ENABLE_IT(&huart1, UART_IT_IDLE);
  HAL_UART_Receive_DMA(&huart1,Uart_Recv1_Buf,Uart_Max_Length);
	HAL_TIM_Base_Start_IT(&htim4);
	HAL_TIM_Base_Start_IT(&htim5);
	HAL_TIM_Base_Start_IT(&htim9);
	printf("Sys OK!\r\n");
  oled.Device_Init();
	motion.OLED_AllMotion_Init();
	FFT_Start();
	InitData();
//	DS3231_Time_Init(DS3231_Init_Buf);
	SPI_Flash_Init();
//	HAL_RTC_MspInit(&hrtc);
		ui.SUI_In();
//  RTC_Set_WakeUp(RTC_WAKEUPCLOCK_CK_SPRE_16BITS,0); //配置WAKE UP中断,1秒钟中断一次

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
//		MUSIC_Mode();
		oled.Clear_Screen();
		ui.SUIDataPrss();
		ui.SUIMainShow();
//		oled.OLED_SHFAny(0,0,fpschar,19,0xffff);
//		fps++;
		oled.Refrash_Screen();
//		HAL_Delay(10);
//		
//    HAL_GPIO_WritePin(DS_SCL_GPIO_Port, DS_SCL_Pin, GPIO_PIN_RESET);//拉低时钟开始数据传输
//    HAL_GPIO_WritePin(DS_SCL_GPIO_Port, DS_SDA_Pin, GPIO_PIN_RESET);//拉低时钟开始数据传输
//		IIC_SCL=0;
//		IIC_SDA=0;
//		HAL_Delay(100);
//		HAL_ADC_Start(&hadc1);
//		HAL_ADC_PollForConversion(&hadc1, 50);
//		printf("ADC:%X\r\n",HAL_ADC_GetValue(&hadc1));
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
uint8_t prt = 1;	
int ip=0;
void MUSIC_Mode(void)
{
	int i;
  static uint8_t OTheme = 0;
	if(Flag_Refrash)
	{
		Flag_Refrash = False;
		oled.Clear_Screen();
		motion.OLED_CustormMotion(6);
		if(adc_dma_ok)
		{
			adc_dma_ok = 0;
			FFT_Pro();	
			for(i=0;i<200;i++)
			{
				if(lBufMagArray[i]/prt>20)
					Device_Msg.fft[i] = lBufMagArray[i]/prt;
				else
					Device_Msg.fft[i] = lBufMagArray[i]/prt/4;
			}
			Device_Msg.leftvol=Device_Msg.fft[1];
			FFT_Start();
		}
		KeyProcess();
		if(OTheme!=save.theme)
		{
			OTheme=save.theme;
			
			for(i=0;i<sizeof(fall_pot);i++)
			{
				fall_pot[i] = 128;
				flow_pot[i] = 128;
			}
		}
		
		
//		switch(save.theme)
		switch(2)
		{
			case 1:fft.Display_Style1();break;
			case 2:fft.Display_Style2();break;
			case 3:fft.Display_Style3();break;
			case 4:fft.Display_Style4();break;
			case 5:fft.Display_Style5();break;
			case 6:fft.Display_Style6();break;
			default:fft.Display_Style1();break;
		}
		
		fps++;
		if(1)
			oled.OLED_SHFAny(0,0,fpschar,19,0xffff);
		HAL_GPIO_TogglePin(SYSLED_GPIO_Port, SYSLED_Pin);
		ui.SUIMainShow();
		oled.Refrash_Screen();
	}
	HAL_Delay(1);
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	static u16 TimeRun = 0;
	if (htim->Instance == htim4.Instance)
	{
//		if(key1_fall_flag==1)//发生按键按下事件
//		{
//			if(!HAL_GPIO_ReadPin(KEY_1_GPIO_Port,KEY_1_Pin))//按键持续按下
//			{         
//				if(key1_holdon_ms <= 200)  
//				{
//					key1_holdon_ms++; 
//				} 
//				else //按键按下到2000ms就判断长按时间成立，生成长按标志 
//				{ 
//					key1_holdon_ms = 0; 
//					short_key1_flag = 0;//清短按键标志
//					key1_long_down = 1;//长按键标志置位
//					key1_fall_flag = 0;//清按键按下标志 
//				} 
//			} 
//			else //按键抬起
//			{ 
//				if(key1_holdon_ms>5)//按下时间大于50ms，生成单击标志
//				{  
//					key1_holdon_ms=0;
//					short_key1_flag=1;
//					key1_long_down =0;
//					key1_fall_flag=0;

//					//距离上次单击时间在100~500ms之间，则认为发生连击事件
////					if(key1upCnt>100 && key1upCnt<500)
////					{ 
////						doubleClick = True;
////						short_key1_flag=0;
////					} 
//					key1UpFlag = True;//单击抬起按键后，生成按键抬起标志 
//				} 
//				else  //按键持续时间小于50ms，忽略 
//				{    
//					key1_holdon_ms=0; 
//					short_key1_flag=0;
//					long_key1_flag=0;
//					key1_fall_flag=0;
//				} 
//			}
//		}
//		
//		if(key1UpFlag)//单击抬起后，启动计数，计数到500ms  
//			key1upCnt++;
//		if(key1upCnt>50)
//		{ 
//			key1upCnt = 0;
//			key1UpFlag = False;
//		}
//		
//		
//		if(key2_fall_flag==1)//发生按键按下事件
//		{
//			if(!HAL_GPIO_ReadPin(KEY_2_GPIO_Port,KEY_2_Pin))//按键持续按下
//			{         
//				if(key2_holdon_ms <= 200)  
//				{
//					key2_holdon_ms++; 
//				} 
//				else //按键按下到2000ms就判断长按时间成立，生成长按标志 
//				{ 
//					key2_holdon_ms = 0; 
//					short_key2_flag=0;//清短按键标志
//					key2_long_down = 1;//长按键标志置位
//					key2_fall_flag = 0;//清按键按下标志 
//				} 
//			} 
//			else //按键抬起
//			{ 
//				if(key2_holdon_ms>5)//按下时间大于50ms，生成单击标志
//				{  
//					key2_holdon_ms=0;
//					short_key2_flag=1;
//					key2_long_down =0;
//					key2_fall_flag=0;

//					//距离上次单击时间在100~500ms之间，则认为发生连击事件
////					if(key2upCnt>100 && key2upCnt<500)
////					{ 
////						doubleClick = True;
////						short_key2_flag=0;
////					} 
//					key2UpFlag = True;//单击抬起按键后，生成按键抬起标志 
//				} 
//				else  //按键持续时间小于50ms，忽略 
//				{    
//					key2_holdon_ms=0; 
//					short_key2_flag=0;
//					long_key2_flag=0;
//					key2_fall_flag=0;
//				} 
//			}
//		}
//		
//		if(key2UpFlag)//单击抬起后，启动计数，计数到500ms  
//			key2upCnt++;
//		if(key2upCnt>50)
//		{ 
//			key2upCnt = 0;
//			key2UpFlag = False;
//		}
		
		DampAutoPos(0);
	}
	if (htim->Instance == htim5.Instance)
	{
		oled.Calc_Color();
		Flag_Refrash = True;
	}
	if (htim->Instance == htim9.Instance)
	{
		oled.Set_Wheel(TimeRun++%96);
		sprintf(fpschar,"%d",fps);
		fps = 0;
//		HAL_RTC_GetTime(&RTC_Handler,&RTC_TimeStruct,RTC_FORMAT_BIN);
//		printf("Time:%02d:%02d:%02d\r\n",RTC_TimeStruct.Hours,RTC_TimeStruct.Minutes,RTC_TimeStruct.Seconds); 
//		HAL_RTC_GetDate(&RTC_Handler,&RTC_DateStruct,RTC_FORMAT_BIN);
//		printf("Date:20%02d-%02d-%02d\r\n",RTC_DateStruct.Year,RTC_DateStruct.Month,RTC_DateStruct.Date); 
//		printf("Week:%d\r\n",RTC_DateStruct.WeekDay); 
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
			printf("Time:%s\r\n",ds3231.Time); 
		}
		break;
	}
	
//		case KEY_1_Pin:
//		if(HAL_GPIO_ReadPin(KEY_1_GPIO_Port,KEY_1_Pin)==GPIO_PIN_RESET)  //LED1翻转
//		{
//				printf("Key1 Pressed!\r\n"); 
//				key1_fall_flag = 1;//开启按键标志
//		}
//		break;
//		case KEY_2_Pin:
//		if(HAL_GPIO_ReadPin(KEY_2_GPIO_Port,KEY_2_Pin)==GPIO_PIN_RESET)  //LED1翻转
//		{
//				printf("Key2 Pressed!\r\n");  
//				key2_fall_flag = 1;//开启按键标志
//		}
//		break;
//	}
}
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
//	HAL_ADC_Stop_DMA(&hadc1);	//停止ADC的DMA传输
	FFT_Stop();
	adc_dma_ok = 1;						//标记ADC_DMA传输完成
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
