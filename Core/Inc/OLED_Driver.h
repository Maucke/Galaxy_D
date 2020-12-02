#ifndef __OLED_DRIVER_H
#define __OLED_DRIVER_H



#include "stm32f4xx_hal.h"
#include "spi.h"

extern uint16_t color_now,color_half,color_min;
#define UT_RPM 0
#define UT_DEG 1
#define UT_MHZ 2
#define UT_PREC 3

#define CU_CPU 0
#define CU_GPU 1
#define CU_RAM 2

#define TP_RAD 0
#define TP_NVI 1

#define OCX 64
#define OCY 48
#define PI 3.14159f

#define SSD1351_WIDTH   256
#define SSD1351_HEIGHT  96

#define SSD1351_CMD_SETCOLUMN       0x15
#define SSD1351_CMD_SETROW          0x75
#define SSD1351_CMD_WRITERAM        0x5C
#define SSD1351_CMD_READRAM         0x5D
#define SSD1351_CMD_SETREMAP        0xA0
#define SSD1351_CMD_STARTLINE       0xA1
#define SSD1351_CMD_DISPLAYOFFSET 	0xA2
#define SSD1351_CMD_DISPLAYALLOFF 	0xA4
#define SSD1351_CMD_DISPLAYALLON  	0xA5
#define SSD1351_CMD_NORMALDISPLAY 	0xA6
#define SSD1351_CMD_INVERTDISPLAY 	0xA7
#define SSD1351_CMD_FUNCTIONSELECT 	0xAB
#define SSD1351_CMD_DISPLAYOFF      0xAE
#define SSD1351_CMD_DISPLAYON     	0xAF
#define SSD1351_CMD_PRECHARGE       0xB1
#define SSD1351_CMD_DISPLAYENHANCE  0xB2
#define SSD1351_CMD_CLOCKDIV        0xB3
#define SSD1351_CMD_SETVSL          0xB4
#define SSD1351_CMD_SETGPIO 		    0xB5
#define SSD1351_CMD_PRECHARGE2      0xB6
#define SSD1351_CMD_SETGRAY         0xB8
#define SSD1351_CMD_USELUT          0xB9
#define SSD1351_CMD_PRECHARGELEVEL 	0xBB
#define SSD1351_CMD_VCOMH           0xBE
#define SSD1351_CMD_CONTRASTABC     0xC1
#define SSD1351_CMD_CONTRASTMASTER  0xC7
#define SSD1351_CMD_MUXRATIO        0xCA
#define SSD1351_CMD_COMMANDLOCK     0xFD
#define SSD1351_CMD_HORIZSCROLL     0x96
#define SSD1351_CMD_STOPSCROLL      0x9E
#define SSD1351_CMD_STARTSCROLL     0x9F


#define swap(a, b) { uint16_t t = a; a = b; b = t; }

#define OLED_RST(x)   HAL_GPIO_WritePin(OLED_RST_GPIO_Port, OLED_RST_Pin, x)
#define OLED_DC(x)    HAL_GPIO_WritePin(OLED_DC_GPIO_Port, OLED_DC_Pin, x)
#define OLED_CS(x)    HAL_GPIO_WritePin(OLED_CS_GPIO_Port, OLED_CS_Pin, x)
#define OLED_CS1(x)    HAL_GPIO_WritePin(OLED_CS1_GPIO_Port, OLED_CS1_Pin, x)

#if INTERFACE_3WIRE_SPI

#define OLED_SCK(x)   HAL_GPIO_WritePin(OLED_SCK_GPIO_Port, OLED_SCK_Pin, x)
#define OLED_DIN(x)   HAL_GPIO_WritePin(OLED_DIN_GPIO_Port, OLED_DIN_Pin, x)

#endif

#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

typedef unsigned short uint16_t;
typedef unsigned char  uint8_t;
typedef unsigned char  uint8;                   /* defined for unsigned 8-bits integer variable 	无符号8位整型变量  */
typedef signed   char  int8;                    /* defined for signed 8-bits integer variable		有符号8位整型变量  */
typedef unsigned short uint16;                  /* defined for unsigned 16-bits integer variable 	无符号16位整型变量 */
typedef signed   short int16;                   /* defined for signed 16-bits integer variable 		有符号16位整型变量 */
typedef unsigned int   uint32;                  /* defined for unsigned 32-bits integer variable 	无符号32位整型变量 */
typedef signed   int   int32;                   /* defined for signed 32-bits integer variable 		有符号32位整型变量 */
typedef float          fp32;                    /* single precision floating point variable (32bits) 单精度浮点数（32位长度） */
typedef double         fp64;                    /* double precision floating point variable (64bits) 双精度浮点数（64位长度） */

extern uint8_t color_byte[2];
extern uint8_t color_fill_byte[2];
extern uint8_t dmasendflag;
#ifdef __cplusplus
extern "C" {
#endif

  class OLED_Driver {
    
    public: 
      OLED_Driver(void);
		
			void Calc_Color(void);
			void MOLED_Fill(uint16_t data);
		
			uint16_t ConvertColor(uint8_t red,uint8_t green,uint8_t blue);
      void Device_Init(void);
      void Clear_Screen(void);
      void Refrash_Screen(void);
      void Fill_Color(uint16_t color);
      void Fill_Color(void);
      void Set_Coordinate(uint16_t x, uint16_t y);
			uint16_t RandomColor(void);

      void Write_text(uint8_t data1);
      void Set_Address(uint8_t column, uint8_t row);
      
      void Set_Color(uint16_t color);
      void Set_FillColor(uint16_t color);
    
      void Invert(bool v);
      void Draw_Pixel(long x, long y);
      void Draw_Pixel(long x, long y,uint16_t color);
    
      void Write_Data(uint8_t dat,uint8_t typ);
      void Write_Data(uint8_t* dat_p, long length,uint8_t typ);
    
      void Draw_FastHLine(int16_t x, int16_t y, int16_t length);
      void Draw_FastHLine(int16_t x, int16_t y, int16_t length,uint16_t color);
			
      void Draw_FastVLine(int16_t x, int16_t y, int16_t length);
      void Draw_FastVLine(int16_t x, int16_t y, int16_t length,uint16_t color);
      void Write_Command(uint8_t data1,uint8_t typ);
			
			void Display_hbmp(int x,int y,int w,int h,const uint8_t *ch,uint16_t color);
			void Display_hbmp(int x,int y,int w,int h,const uint8_t *ch,uint16_t color,uint8_t bk);
			
			void Set_DampColor(uint16_t color);
			void Set_Wheel(uint16_t WheelPos);
			void Set_Wheelf(uint16_t WheelPos);
			uint16_t WheelP(uint16_t WheelPos);
			
			void Clear_FpsCount(void);
			void OLED_HFAny(int x,int y,int w,int h,uint8_t Num,const unsigned char *ch,uint16_t color);
			void OLED_SHFAny(int x,int y,char *ch,int w,uint16_t color);
    private:  
      uint8_t Float2uint8_t(float Input);
      void RAM_Address(uint8_t typ);
  };



#ifdef __cplusplus
}
#endif
  
#endif

