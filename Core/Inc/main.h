/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

void SLEEP_Mode(void);
void OFFLINE_Mode(void);
void NORMAL_MainDisplay(void);
void NORMALUI_Init(void);
void NORMALUI_Out(void);
void NORMAL_Mode(void);
void GAME_MainDisplay(void);
void GAME_Out(void);
void GAME_Init(void);
void GAME_Mode(void);
void TIME_VFDDisplay(void);
void Display_Beat(void);
void DATE_Mode(void);
void Update_Pos(void);
void MUSIC_Mode(void);
void TIME_Mode(void);
extern uint8_t fall_pot[250];	//记录下落点的坐标
extern uint8_t flow_pot[250];

#define HIGH 1
#define LOW 0

#define True		1		
#define False		0		
 
/*!< STM32F10x Standard Peripheral Library old types (maintained for legacy purpose) */
typedef int32_t  s32;
typedef int16_t s16;
typedef int8_t  s8;

typedef const int32_t sc32;  /*!< Read Only */
typedef const int16_t sc16;  /*!< Read Only */
typedef const int8_t sc8;   /*!< Read Only */

typedef __IO int32_t  vs32;
typedef __IO int16_t  vs16;
typedef __IO int8_t   vs8;

typedef __I int32_t vsc32;  /*!< Read Only */
typedef __I int16_t vsc16;  /*!< Read Only */
typedef __I int8_t vsc8;   /*!< Read Only */

typedef uint32_t  u32;
typedef uint16_t u16;
typedef uint8_t  u8;

typedef const uint32_t uc32;  /*!< Read Only */
typedef const uint16_t uc16;  /*!< Read Only */
typedef const uint8_t uc8;   /*!< Read Only */

typedef __IO uint32_t  vu32;
typedef __IO uint16_t vu16;
typedef __IO uint8_t  vu8;

typedef __I uint32_t vuc32;  /*!< Read Only */
typedef __I uint16_t vuc16;  /*!< Read Only */
typedef __I uint8_t vuc8;   /*!< Read Only */
#define BITBAND(addr, bitnum) ((addr & 0xF0000000)+0x2000000+((addr &0xFFFFF)<<5)+(bitnum<<2)) 
#define MEM_ADDR(addr)  *((volatile unsigned long  *)(addr)) 
#define BIT_ADDR(addr, bitnum)   MEM_ADDR(BITBAND(addr, bitnum)) 
//IO口地址映射
#define GPIOA_ODR_Addr    (GPIOA_BASE+12) //0x4001080C 
#define GPIOB_ODR_Addr    (GPIOB_BASE+12) //0x40010C0C 
#define GPIOC_ODR_Addr    (GPIOC_BASE+12) //0x4001100C 
#define GPIOD_ODR_Addr    (GPIOD_BASE+12) //0x4001140C 
#define GPIOE_ODR_Addr    (GPIOE_BASE+12) //0x4001180C 
#define GPIOF_ODR_Addr    (GPIOF_BASE+12) //0x40011A0C    
#define GPIOG_ODR_Addr    (GPIOG_BASE+12) //0x40011E0C    

#define GPIOA_IDR_Addr    (GPIOA_BASE+8) //0x40010808 
#define GPIOB_IDR_Addr    (GPIOB_BASE+8) //0x40010C08 
#define GPIOC_IDR_Addr    (GPIOC_BASE+8) //0x40011008 
#define GPIOD_IDR_Addr    (GPIOD_BASE+8) //0x40011408 
#define GPIOE_IDR_Addr    (GPIOE_BASE+8) //0x40011808 
#define GPIOF_IDR_Addr    (GPIOF_BASE+8) //0x40011A08 
#define GPIOG_IDR_Addr    (GPIOG_BASE+8) //0x40011E08 
 
//IO口操作,只对单一的IO口!
//确保n的值小于16!
//#define PAout(n)   BIT_ADDR(GPIOA_ODR_Addr,n)  //输出 
//#define PAin(n)    BIT_ADDR(GPIOA_IDR_Addr,n)  //输入 

//#define PBout(n)   BIT_ADDR(GPIOB_ODR_Addr,n)  //输出 
//#define PBin(n)    BIT_ADDR(GPIOB_IDR_Addr,n)  //输入 

//#define PCout(n)   BIT_ADDR(GPIOC_ODR_Addr,n)  //输出 
//#define PCin(n)    BIT_ADDR(GPIOC_IDR_Addr,n)  //输入 

//#define PDout(n)   BIT_ADDR(GPIOD_ODR_Addr,n)  //输出 
//#define PDin(n)    BIT_ADDR(GPIOD_IDR_Addr,n)  //输入 

//#define PEout(n)   BIT_ADDR(GPIOE_ODR_Addr,n)  //输出 
//#define PEin(n)    BIT_ADDR(GPIOE_IDR_Addr,n)  //输入

//#define PFout(n)   BIT_ADDR(GPIOF_ODR_Addr,n)  //输出 
//#define PFin(n)    BIT_ADDR(GPIOF_IDR_Addr,n)  //输入

//#define PGout(n)   BIT_ADDR(GPIOG_ODR_Addr,n)  //输出 
//#define PGin(n)    BIT_ADDR(GPIOG_IDR_Addr,n)  //输入

//以下为汇编函数
void WFI_SET(void);		//执行WFI指令
void INTX_DISABLE(void);//关闭所有中断
void INTX_ENABLE(void);	//开启所有中断
void MSR_MSP(u32 addr);	//设置堆栈地址

#define MAKEWORD(low, high)		(((u8)(low)) | (((u8)(high)) << 8))
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
extern u8 flushok;
#define NPT 256									//样本数量
#define PI2 6.28318530717959
//采样率计算
//分辨率：Fs/NPT 
//#define Fs	10000							//FS FFT计算的频率 这个频率和ADC的采样频率绑定起来
#define Fs	9984								//取9984能出来整数的分辨率 9984/256 = 39Hz

extern uint8_t adc_dma_ok;			//adc的DMA传输完成标志
extern long lBufMagArray[NPT];	//每个频率对用的幅值
extern uint32_t adc_buf[NPT];		//adc原始数据

void FFT_Start(void);
void FFT_Stop(void);
void FFT_Pro(void);
/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define SYSLED_Pin GPIO_PIN_13
#define SYSLED_GPIO_Port GPIOC
#define OLED_RST_Pin GPIO_PIN_2
#define OLED_RST_GPIO_Port GPIOA
#define OLED_DC_Pin GPIO_PIN_3
#define OLED_DC_GPIO_Port GPIOA
#define OLED_CS_Pin GPIO_PIN_4
#define OLED_CS_GPIO_Port GPIOA
#define VFD_EN_Pin GPIO_PIN_12
#define VFD_EN_GPIO_Port GPIOB
#define VFD_STB_Pin GPIO_PIN_14
#define VFD_STB_GPIO_Port GPIOB
#define OLED_CS1_Pin GPIO_PIN_8
#define OLED_CS1_GPIO_Port GPIOA
#define DS_SDA_Pin GPIO_PIN_11
#define DS_SDA_GPIO_Port GPIOA
#define DS_SCL_Pin GPIO_PIN_12
#define DS_SCL_GPIO_Port GPIOA
#define DS_INT_Pin GPIO_PIN_15
#define DS_INT_GPIO_Port GPIOA
#define DS_INT_EXTI_IRQn EXTI15_10_IRQn
#define FL_CS_Pin GPIO_PIN_6
#define FL_CS_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

#define DMA_SPI 0
#define INTERFACE_4WIRE_SPI 1
#define INTERFACE_3WIRE_SPI !(INTERFACE_4WIRE_SPI)


#if INTERFACE_4WIRE_SPI
  #if INTERFACE_3WIRE_SPI
    #error "SPI Setting Error !!"
  #endif
#elif INTERFACE_3WIRE_SPI

#else
  #error "SPI Setting Error !!"
#endif
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
