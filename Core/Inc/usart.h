/**
  ******************************************************************************
  * File Name          : USART.h
  * Description        : This file provides code for the configuration
  *                      of the USART instances.
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
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __usart_H
#define __usart_H
#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

extern UART_HandleTypeDef huart1;

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

void MX_USART1_UART_Init(void);

/* USER CODE BEGIN Prototypes */

#define Uart_Max_Length 250
#define Uart_FFT_Length 192
extern uint8_t Uart_Recv1_Buf[];
extern uint8_t Uart_Overflow1_Flag;
extern uint16_t Uart_Recv1_Length;

extern uint8_t Uart_Recv3_Buf[];
extern uint8_t Uart_Overflow3_Flag;
extern uint16_t Uart_Recv3_Length;
#define End_Frame_ADDR 0x55AA
extern char Device_Name[20];
extern u8 Uart_Recv3_Data;
typedef struct
{
	u8 hour;
	u8 minute;
	u8 cation;//通知
	u8 mute;//静音 NO use
	u8 alarm;//闹钟
	u8 tmsys;
	u8 wifi;//WiFi状态
	u8 aiwork;
} CLOCK_MSG;

extern CLOCK_MSG Clock_Msg;

#define CPU_Temp 						0x1
#define CPU_Clock 					0x2
#define CPU_Load 						0x3
#define CPU_Fan 						0x4

#define GPU_Temp 						0x11
#define GPU_Clock 					0x12
#define GPU_Load 						0x13
#define GPU_Fan 						0x14

#define Main_Temp 					0x21
#define Main_Fan 						0x22
#define Main_Vol						0x23

#define RAM_Load 	  				0x31
#define RAM_Data 						0x32

#define HDD_Load 	  				0x41
#define HDD_Temp 						0x42

#define Uart_Year 	 			 	0x201
#define Uart_Month 					0x202
#define Uart_Day 	  				0x203
#define Uart_Week 					0x204
#define Uart_Hour 	  			0x205
#define Uart_Minute 				0x206
#define Uart_Second 	  		0x207
#define CPU_POWER 	  			0x208

#define CPU_NAME 	 		 		0x101
#define GPU_NAME 	 		 		0x102
#define Main_NAME 	 		 	0x103
#define Uart_END	 	  		0x55AA

#define Command_Mode      0x8001
#define Command_Motion    0x8002
#define Command_Style     0x8003
#define Command_Speed     0x8004
#define Command_Brightness 0x8005
#define Command_Set       0x8006
#define Command_RGBMode   0x8007
#define Command_RGBColor  0x8008
#define Command_RGBBrightness 0x8009

#define Command_TOPTHEME 	0x800A
#define Command_LOGOTHEME 0x800B
#define Command_GAMETYPE 0x800C
#define Command_DEVICENAME 0x400A

//#define FLASH_SAVE_ADDR  0X0807f000 	//设置FLASH 保存地址(必须为偶数，且其值要大于本代码所占用FLASH的大小+0X08000000)
#define FLASH_NAME_ADDR  0x0807D000 
#define FLASH_SAVE_ADDR  0x0803D000 

#define FFT_Data 0x0301
#define Left_Vol 0x0302
#define Right_Vol 0x0303

//			SendValue(0x8001, Convert.ToUInt16(Ctl.Mode)); // 显示模式
//			Thread.Sleep(2);
//			SendValue(0x8002, Convert.ToUInt16(Ctl.Motion)); // 显示模式
//			Thread.Sleep(2);
//			SendValue(0x8003, Convert.ToUInt16(Ctl.Style)); // 显示模式
//			Thread.Sleep(2);
//			SendValue(0x8004, Convert.ToUInt16(Ctl.Speed)); // 显示模式
//			Thread.Sleep(2);
typedef struct
{
	u16 cputemp;
	u16 cpuclock;
	u16 cpuload;
	u16 cpufan;
	u16 cpupower;

	u16 gputemp;
	u16 gpuclock;
	u16 gpuload;
	u16 gpufan;

	u16 maintemp;
	u16 mainfan;
	u16 mainvoltage;

	u16 ramload;
	u16 ramusrdata;

	u16 hddload;
	u16 hddtemp;

	u16 uartyear;
	u16 uartmonth;
	u16 uartday;
	u16 uartweek;
	u16 uarthour;
	u16 uartminute;
	u16 uartsecond;

	char cpuname[40];
	char gpuname[40];
	char mainname[40];
	u8 fft[Uart_FFT_Length];
	u16 leftvol;
	u16 rightvol;
}DEVICE_MSG;

typedef struct
{
	u16 commandmode;
	u16 commandmotion;
	u16 commandstyle;
	u16 commandspeed;
	u16 commandset;
	u16 commandbrightness;
	u16 commandrgbmode;
	u16 commandrgbcolor;
	u16 commandrgbbrightness;
	u16 commandtoptheme;
	u16 commandlogotheme;
	u16 commandgametype;
}DEVICE_CMD;


typedef struct
{
	char cputemp[10];	
	char cputempf[10];	
	char cpuclock[10];	
	char cpuload[10];	
	char cpufan[10];	

	char gputemp[10];	
	char gpuclock[10];	
	char gpuload[10];	
	char gpufan[10];		

	char maintemp[10];		
	char mainfan[10];

	char ramload[10];	
	char ramusrdata[10];
	
	char ngputemp[10];	
	char ncpuclock[10];	
	char ngpufan[10];		
	char ncpufan[10];	
	
	char vcputemp[10];	
	char vgputemp[10];	
		
//	char vtime[10];
//	char vhour[10];
//	char vmin[10];	
//	char vsec[10];	
//	char date[10];	
	
}DEVICE_STR;


#define ESP_WeatherNum			0x400
#define ESP_Weather				0x401
#define ESP_Temp				0x402
#define ESP_Humi				0x403
#define ESP_Wind_Dir			0x404
#define ESP_Wind_Pw				0x405
#define ESP_Report_Tm			0x406

#define ESP_D1_WeatherNum		0x410
#define ESP_D1_Weeek			0x411
#define ESP_D1_Weather			0x412
#define ESP_D1_Temp				0x413
#define ESP_D1_NTemp			0x414

#define ESP_D2_WeatherNum		0x420
#define ESP_D2_Weeek			0x421
#define ESP_D2_Weather			0x422
#define ESP_D2_Temp				0x423
#define ESP_D2_NTemp			0x424

#define ESP_D3_WeatherNum		0x430
#define ESP_D3_Weeek			0x431
#define ESP_D3_Weather			0x432
#define ESP_D3_Temp				0x433
#define ESP_D3_NTemp			0x434

#define ESP_Year 	 			0x501
#define ESP_Month 				0x502
#define ESP_Day 	  			0x503
#define ESP_Week 				0x504
#define ESP_Hour 	  			0x505
#define ESP_Minute 				0x506
#define ESP_Second 	  			0x507

#define ESP_Bili_Msg 	  		0x511
#define ESP_Bili_Fow   			0x512

#define ESP_KEY_MODE 				0x601
#define ESP_KEY_MENU 				0x602
#define ESP_KEY_SW 					0x603

#define ESP_Set_Addr  			0x8011
#define ESP_Set_Uid  			0x8012
typedef struct
{
	u16 weathernum;
	char weather[10];
	char temp[10];
	char utemp[10];
	char uhumidity[10];
	char humidity[10];
	char winddir[10];
	char windpw[20];
	char reporttime[20];
	
	u16 weather1num;
	u16 week1num;
	char week1[10];
	char weather1[10];
	char temp1[10];
	char ntemp1[10];
	char utemp1[10];
	char untemp1[10];
	
	u16 weather2num;
	u16 week2num;
	char week2[10];
	char weather2[10];
	char temp2[10];
	char ntemp2[10];
	char utemp2[10];
	char untemp2[10];
	
	u16 weather3num;
	u16 week3num;
	char week3[10];
	char weather3[10];
	char temp3[10];
	char ntemp3[10];
	char utemp3[10];
	char untemp3[10];
	
	u16 year;
	u16 month;
	u16 day;
	u16 week;
	u16 hour;
	u16 minute;
	u16 second;
	
	u16 bilimsg;
	char bilifans[10];
	u16 mode;
}WIFI_MSG;

extern u8 SaveFlag;
extern DEVICE_STR Device_Str;
extern DEVICE_MSG Device_Msg;
extern DEVICE_CMD Device_Cmd;
extern WIFI_MSG WiFi_Msg;
extern uint8_t OfflineCount;
extern uint8_t WiFiOfflineCount;
extern u8 WifiActive;
/* USER CODE END Private defines */

void MX_USART1_UART_Init(void);
void MX_USART3_UART_Init(void);

/* USER CODE BEGIN Prototypes */
extern uint8_t Uart_Recv1_Data;
extern uint8_t Uart_Recv3_Data;
extern u16 own_hour,own_min,own_sec;
void ConvertData(void);
void InitData(void);
void InitWifi(void);
void AnalysisComputermsg(uint8_t *Buf);
void AnalysisComputername(uint8_t *Buf);
void AnalysisMiStaus(uint8_t Data);
void AnalysisFFT(uint8_t *Buf);
void AnalysisCommand(uint8_t *Buf);
void ClearFFT(void);
void UsartCommand(UART_HandleTypeDef *huart,u16 Addr,u16 Data);

void UsartPrint(UART_HandleTypeDef *huart,u16 Addr,char *Data);
void UsartPrintuid(UART_HandleTypeDef *huart,u16 Addr,char *Data);
void UsartClear(UART_HandleTypeDef *huart);
void Tranfcmd(void);
void Recvcmd(void);
void AnalysisWiFiString(uint8_t *Buf);
void AnalysisWiFiInter(uint8_t *Buf);
void AnalysisKey(uint8_t *Buf);

void SendCommand(void);
int cont_str(char *s);
void delay_ms(unsigned int Ms);
/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif
#endif /*__ usart_H */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
