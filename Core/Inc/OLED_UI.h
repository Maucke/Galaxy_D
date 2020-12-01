#ifndef __OLED_UI_H
#define __OLED_UI_H

#include "stm32f4xx_hal.h"
#include "OLED_GFX.h"
#include "OLED_Driver.h"
#include "OLED_Animation.h"
#include "motion.h"

#ifdef __cplusplus
extern "C" {
#endif

  class OLED_UI : public virtual OLED_GFX {
    
    public: 
      OLED_UI(void);
			OLED_STATUS MtRect(int x,int y,int w,int h,int step,uint16_t color);
			OLED_STATUS SUITitleShow(int step,uint16_t color);
			OLED_STATUS SUICornShow(void);
			void SUI_In();
			void SUI_Out();
			OLED_STATUS SUIMainShow();
			void SUIDataPrss();
		
    private:  
  };



#ifdef __cplusplus
}
#endif
  
#endif

