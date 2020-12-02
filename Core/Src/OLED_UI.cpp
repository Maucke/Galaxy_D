#include "OLED_UI.h"
#include "main.h"
#include "stdlib.h"
#include "usart.h"

#ifdef __cplusplus
extern "C"  {
#endif


OLED_UI::OLED_UI(void) {
}

OLED_STATUS OLED_UI::MtRect(int x,int y,int w,int h,int step,uint16_t color)
{
	if(step<=w)
	{
		Draw_Line(x,y,x+step,y,color);return OLED_BUSY;
	}
	else
		Draw_Line(x,y,x+w,y,color);
		
	if(step>w&&step<=w+h)
	{
		Draw_Line(x+w,y,x+w,y+step-w,color);return OLED_BUSY;
	}
	else
		Draw_Line(x+w,y,x+w,y+h,color);
		
	if(step>w+h&&step<=2*w+h)
	{
		Draw_Line(x+w,y+h,x+w-(step-w-h),y+h,color);return OLED_BUSY;
	}
	else
		Draw_Line(x+w,y+h,x,y+h,color);
		
	if(step>2*w+h&&step<=2*w+2*h)
	{
		Draw_Line(x,y+h,x,y+h-(step-2*w-h),color);return OLED_BUSY;
	}
	else
		Draw_Line(x,y+h,x,y,color);
	
	if(step>2*w+2*h)
		return OLED_IDLE;
	else
		return OLED_BUSY;
}

OLED_STATUS OLED_UI::SUITitleShow(int step,uint16_t color)
{
	if(step>10)
	{
		OLED_SHFAny(69,42,"0",4,color);
		OLED_SHFAny(128+69,42,"0",4,color);
		OLED_SHFAny(2,6,"0",4,color);
		OLED_SHFAny(128+2,6,"0",4,color);
		OLED_SHFAny(128+2,68,"0",4,color);
	}
	if(step>20)
	{
		OLED_SHFAny(69+8*1,42,"1",4,color);
		OLED_SHFAny(128+69+8*1,42,"1",4,color);
		OLED_SHFAny(2+12*1,6,"1",4,color);
		OLED_SHFAny(128+12*1,6,"1",4,color);
		OLED_SHFAny(128+12*1,68,"1",4,color);
		Draw_Line(67,34,68,34,color_now);
		Draw_Line(67,34,67,35,color_now);
		Draw_Line(67+128,34,68+128,34,color_now);
		Draw_Line(67+128,34,67+128,35,color_now);
	}
	if(step>30)
	{
		OLED_SHFAny(69+8*2-1,42,"2",4,color);
		OLED_SHFAny(128+69+8*2-1,42,"2",4,color);
		OLED_SHFAny(2+12*2,6,"2",4,color);
		OLED_SHFAny(128+12*2,6,"2",4,color);
		OLED_SHFAny(128+12*2,68,"2",4,color);
	
	}
	if(step>40)
	{
		OLED_SHFAny(69+8*3-1,42,"3",4,color);
		OLED_SHFAny(128+69+8*3-1,42,"3",4,color);
		OLED_SHFAny(2+12*3,6,"3",4,color);
		OLED_SHFAny(128+12*3,6,"3",4,color);
		OLED_SHFAny(128+12*3,68,"3",4,color);
	}
	if(step>50)
	{
		OLED_SHFAny(69+8*4-1,42,"4",4,color);
		OLED_SHFAny(128+69+8*4-1,42,"4",4,color);
		OLED_SHFAny(2+12*4,6,"4",4,color);
		OLED_SHFAny(128+12*4,6,"4",4,color);
		OLED_SHFAny(128+12*4,68,"4",4,color);
		Draw_Line(67,58,68,58,color_now);
		Draw_Line(67,58,67,57,color_now);
		Draw_Line(67+128,58,68+128,58,color_now);
		Draw_Line(67+128,58,67+128,57,color_now);
	}
	if(step>60)
	{
		OLED_SHFAny(69+8*5-1,42,"5",4,color);
		OLED_SHFAny(128+69+8*5-1,42,"5",4,color);
		OLED_SHFAny(2+12*5,6,"5",4,color);
		OLED_SHFAny(128+12*5,6,"5",4,color);
		OLED_SHFAny(128+12*5,68,"5",4,color);
	}
	if(step>70)
	{
		OLED_SHFAny(69+8*6-1-1,42,"6",4,color);
		OLED_SHFAny(128+69+8*6-1-1,42,"6",4,color);
		OLED_SHFAny(2+12*6,6,"6",4,color);
		OLED_SHFAny(128+12*6,6,"6",4,color);
		OLED_SHFAny(128+12*6,68,"6",4,color);
	}
	if(step>80)
	{
		OLED_SHFAny(69+8*7-1-1,42,"7",4,color);
		OLED_SHFAny(128+69+8*7-1-1,42,"7",4,color);
		OLED_SHFAny(2+12*7,6,"7",4,color);
		OLED_SHFAny(128+12*7,6,"7",4,color);
		OLED_SHFAny(128+12*7,68,"7",4,color);
		Draw_Line(127,34,126,34,color_now);
		Draw_Line(127,34,127,35,color_now);
		Draw_Line(127+128,34,126+128,34,color_now);
		Draw_Line(127+128,34,127+128,35,color_now);
	}
	if(step>90)
	{
		OLED_SHFAny(2+12*8,6,"8",4,color);
		OLED_SHFAny(128+12*8,6,"8",4,color);
		OLED_SHFAny(128+12*8,68,"8",4,color);
	}
	if(step>100)
	{
		OLED_SHFAny(2+12*9,6,"9",4,color);
		OLED_SHFAny(128+12*9,6,"9",4,color);
		OLED_SHFAny(128+12*9,68,"9",4,color);
	}
	if(step>110)
	{
		OLED_SHFAny(2+12*10-4,6,"10",4,color);
		OLED_SHFAny(128+12*10-4,6,"10",4,color);
		OLED_SHFAny(128+12*10-4,68,"10",4,color);
		Draw_Line(127,58,126,58,color_now);
		Draw_Line(127,58,127,57,color_now);
		Draw_Line(127+128,58,126+128,58,color_now);
		Draw_Line(127+128,58,127+128,57,color_now);
		return OLED_IDLE;
	}
	else
		return OLED_BUSY;
}

extern const unsigned char Corn_SCPU[];
extern const unsigned char Corn_SGPU[];
extern const unsigned char Corn_SFANRAM[];
extern const unsigned char Corn_SRAM[];
extern const unsigned char Corn_SFAN[];
extern const unsigned char Corn_SFENQ[];
extern const unsigned char Corn_STEMP[];
extern const unsigned char Corn_SSTATE[];

OLED_STATUS OLED_UI::SUICornShow(void)
{
		Display_hbmp(49,0,13*2,5,Corn_SCPU,color_now,0);
		Display_hbmp(49+128,0,13*2,5,Corn_SGPU,color_now,0);
		
		Display_hbmp(13,34,18*2,5,Corn_SFANRAM,color_now,0);
		Display_hbmp(13+128,34,18*2,5,Corn_SFANRAM,color_now,0);
		
		Display_hbmp(86,34,12*2,5,Corn_SFAN,color_now,0);
		Display_hbmp(86+128,34,12*2,5,Corn_SFAN,color_now,0);
		
		Display_hbmp(12,62,20,5,Corn_SFENQ,color_now,0);
		Display_hbmp(53,62,22,5,Corn_STEMP,color_now,0);
		Display_hbmp(94,62,13*2,5,Corn_SSTATE,color_now,0);
		Display_hbmp(48+128,62,28,5,Corn_SRAM,color_now,0);
		return OLED_IDLE;
}

void OLED_UI::SUI_In()
{
	int i;
	for(i=0;i<12;i++)
		SetCurrent(i,0);
	
	SetTarget(0,314+4);
	SetTarget(1,176+4);
	SetTarget(2,148+4);
	SetTarget(3,320+4);
	SetTarget(4,125);
	SetTarget(5,14);
	SetTarget(6,11);
	
	
//	SetTarget(7,123);
//	SetTarget(8,64);
//	
//	SetTarget(9,58);
//	SetTarget(10,24);
	
//	SetTarget(11,123);//RAM
}

void OLED_UI::SUI_Out()
{
	int i;
	for(i=0;i<12;i++)
		SetTarget(i,0);
}


void OLED_UI::SUIDataPrss()
{
	SetTarget(7,Device_Msg.cpuload*123/1000);
	SetTarget(8,Device_Msg.gpuload*123/1000);
	
	SetTarget(9,Device_Msg.cpufan/142);
	SetTarget(10,Device_Msg.gpufan/142);
	
	SetTarget(11,Device_Msg.ramload*123/1000);//RAM
}

OLED_STATUS OLED_UI::SUIMainShow()
{
	int i;
	MtRect(0,0,127,30,pit[0].current,color_now);
	MtRect(0,34,64,57-33,pit[1].current,color_now);
	MtRect(0,62,41,95-62,pit[2].current,color_now);
	MtRect(43,62,41,95-62,pit[2].current,color_now);
	MtRect(86,62,41,95-62,pit[2].current,color_now);
	
	MtRect(0+128,0,127,30,pit[0].current,color_now);
	MtRect(0+128,34,64,57-33,pit[1].current,color_now);
	MtRect(0+128,62,127,95-62,pit[3].current,color_now);
	Draw_Line(2,2,pit[4].current,2,color_now);
	Draw_Line(2,28,pit[4].current,28,color_now);
	Draw_Line(2+128,2,pit[4].current+128,2,color_now);
	Draw_Line(2+128,28,pit[4].current+128,28,color_now);
	
	for(i=0;i<pit[6].current;i++)
	{
		Draw_Line(3+12*i,12,3+12*i,12+pit[5].current,color_half);
		Draw_Line(3+12*i+128,12,3+12*i+128,12+pit[5].current,color_half);
		Draw_Line(3+12*i+128,74,3+12*i+128,74+pit[5].current*18/13,color_half);
	}
	
	SUITitleShow(pit[2].current,color_now);
	SUICornShow();
	
	
	
	Fill_Rect(2,12,pit[7].current,14,color_half);
	Fill_Rect(2+128,12,pit[8].current,14,color_half);
	
	Fill_Rect(68,49,pit[9].current,7,color_half);
	Fill_Rect(68+128,49,pit[10].current,7,color_half);
	
	Fill_Rect(2+128,74,pit[11].current,19,color_half);
	
	OLED_SHFAny(12,42,Device_Str.cpufan,10,color_now);
	OLED_SHFAny(12+128,42,Device_Str.gpufan,10,color_now);
}

void OLED_UI::NUI_In(){}
void OLED_UI::NUI_Out(){}
void OLED_UI::NUIDataPrss(){}
OLED_STATUS OLED_UI::NUIMainShow(){}

void OLED_UI::TUI_In(){}
void OLED_UI::TUI_Out(){}
void OLED_UI::TUIDataPrss(){}
OLED_STATUS OLED_UI::TUIMainShow(){}

void OLED_UI::MUI_In(){}
void OLED_UI::MUI_Out(){}
void OLED_UI::MUIDataPrss(){}
OLED_STATUS OLED_UI::MUIMainShow(){}
#ifdef __cplusplus
}
#endif


