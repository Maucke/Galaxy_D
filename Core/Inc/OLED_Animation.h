#ifndef __OLED_ANIMATION_H
#define __OLED_ANIMATION_H

#include "stm32f4xx_hal.h"
#include "OLED_GFX.h"
#include "ASCII_Font.h"
#include "stdlib.h"

#define MINDMAX 28
#define CIRCLEMAX 16
#define STARMAX 40
#define METEORMAX 40
#define DEFLINEMAX 60
#define PLANETMAX 5
#define TRIANGLEMAX 16
#define FULLTRIANGLEMAX 16

typedef enum
{
  OLED_OK       = 0x00U,
  OLED_ERROR    = 0x01U,
  OLED_BUSY     = 0x02U,
  OLED_IDLE     = 0x03U,
} OLED_STATUS;

extern OLED_GFX oled;
#ifdef __cplusplus
extern "C" {
#endif

  class OLED_Animation {
    
    public: 
			OLED_Animation(void);
			void Motion_MindInit(void);
			void Motion_Mind(void);
			void Motion_CircleInit(void);
			void Motion_Circle(void);
			void Motion_SnowflakeInit(void);
			void Motion_Snowflake(void);
			void Motion_MovmeteorInit(void);
			void Motion_Movmeteor(void);
			void Motion_PlanetInit(void);
			void Motion_Planet(void);
			void Motion_TriangleInit(void);
			void Motion_Triangle(void);
			void Motion_TriangleF(void);
			void OLED_AllMotion_Init(void);
			void OLED_CustormMotion(u8 Channel);
			void OLED_AllMotion(u8 Type,u16 Speed);
    private:  
      OLED_STATUS OLED_MovMind(u8 Index);
			OLED_STATUS OLED_FucCircle(u8 Index);
			OLED_STATUS OLED_MovSnowflake(u8 Index);
			OLED_STATUS OLED_Movmeteor(u8 Index);
			OLED_STATUS OLED_Planet(u8 Index);
			OLED_STATUS OLED_Triangle(u8 Index);

  };





#ifdef __cplusplus
}
#endif
  
#endif
