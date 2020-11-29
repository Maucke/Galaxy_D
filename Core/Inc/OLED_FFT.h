#ifndef __OLED_FFT_H
#define __OLED_FFT_H

#include "stm32f4xx_hal.h"
#include "OLED_GFX.h"
#include "ASCII_Font.h"
#include "stdlib.h"

#define TrumHeight   96
#define TrumWidth    1
#define TrumInterval 1
#define TrumNum	   	 128
	

extern OLED_GFX oled;
#ifdef __cplusplus
extern "C" {
#endif
	
  class OLED_FFT {
    
    public: 
			OLED_FFT(void);
		
			void Display_Style1(void);
			void Display_Style2(void);
			void Display_Style3(void);
			void Display_Style4(void);
			void Display_Style5(void);
			void Display_Style6(void);
    private:  
			
  };



#ifdef __cplusplus
}
#endif
  
#endif
