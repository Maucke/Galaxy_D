#include "OLED_Driver.h"
#include "ASCII_Font.h"
#include "main.h"
#include "gpio.h"
#include "stdlib.h"
#include "stdio.h"


uint8_t color_byte[2],color_Damp_byte[2];

uint16_t wheel;
uint8_t OLED_GRAM[2][2*96*128];
uint16_t color_now,color_half,color_min;

#ifdef __cplusplus
extern "C"  {
#endif


OLED_Driver::OLED_Driver(void) {
}
  
u16 OLED_Driver::ConvertColor(u8 red,u8 green,u8 blue)
{
	return red<<11|green<<6|blue;
}


void OLED_Driver::Set_Wheel(u16 WheelPos)	{
	
	uint8_t red,green,blue;
	wheel = WheelPos;
	if(WheelPos < 32) 
	{
		red = 0;
		green = WheelPos;
		blue = (31 - WheelPos);
	}
	else if(WheelPos < (32*2)) 
	{          
		red = WheelPos-32;
		green = 32*2-1 - WheelPos;
		blue = 0;
	}
	else
	{
		red = (32*3)-1 - WheelPos;
		green = 0;
		blue = WheelPos-(32*2);
	}
	color_Damp_byte[0] = red<<3|(green>>2);
	color_Damp_byte[1] = green<<6|blue;
}
void OLED_Driver::Calc_Color(void){
				
	uint8_t red,green,blue;
	uint8_t red1,green1,blue1;
	red = color_byte[0]>>3;
	green = ((color_byte[0]&0x7)<<3)|color_byte[1]>>5;
	blue = color_byte[1]&0x1f;
	
	red1 = color_Damp_byte[0]>>3;
	green1 = ((color_Damp_byte[0]&0x7)<<3)|color_Damp_byte[1]>>5;
	blue1 = color_Damp_byte[1]&0x1f;
	
	if(red>red1)
	{
		red--;
	}
	else if(red<red1)
	{
		red++;
	}
	if(green>green1+1)
	{
		green-=2;
	}
	else if(green+1<green1)
	{
		green+=2;
	}
	if(blue>blue1)
	{
		blue--;
	}
	else if(blue<blue1)
	{
		blue++;
	}
	color_now = red<<11|green<<5|blue;
	color_byte[0] = color_now>>8;
	color_byte[1] = color_now&0xff;
	red >>=1;
	green >>=1;
	blue >>=1;
	color_half = red<<11|green<<5|blue;
	red >>=1;
	green >>=1;
	blue >>=1;
	color_min = red<<11|green<<5|blue;
	
}


uint16_t OLED_Driver::RandomColor(void){
	
	uint8_t red,green,blue;
	red = rand();
	__ASM("NOP");
	green = rand();
	__ASM("NOP");
	blue = rand();
	return (red<<11|green<<5|blue);
}

void OLED_Driver::Set_DampColor(uint16_t color)  {
  
  color_Damp_byte[0] = (uint8_t)(color >> 8);
  color_Damp_byte[1] = (uint8_t)(color & 0x00ff);
}




void OLED_Driver::Set_Color(uint16_t color)  {
  
  color_byte[0] = (uint8_t)(color >> 8);
  color_byte[1] = (uint8_t)(color & 0x00ff);
}


//void OLED_Driver::Set_FillColor(uint16_t color)  {
//  
//  color_fill_byte[0] = (uint8_t)(color >> 8);
//  color_fill_byte[1] = (uint8_t)(color & 0x00ff);
//}
//  

void OLED_Driver::Write_Command(uint8_t cmd,uint8_t typ)  {
  
	switch(typ)
	{
		case 0:OLED_CS(GPIO_PIN_RESET);break;
		case 1:OLED_CS1(GPIO_PIN_RESET);break;
		default:OLED_CS(GPIO_PIN_RESET);OLED_CS1(GPIO_PIN_RESET);break;
	}
  
  
#if  INTERFACE_4WIRE_SPI
  
  OLED_DC(GPIO_PIN_RESET);
  
  while(HAL_SPI_Transmit(&hspi1,&cmd,0x01,0x10) != HAL_OK);
  
  OLED_DC(GPIO_PIN_SET);
  
#elif INTERFACE_3WIRE_SPI
  
  uint8_t i;
	uint16_t hwData = 0;
	
  hwData = (uint16_t)cmd & ~0x0100;

	for(i = 0; i < 9; i ++) {
		OLED_SCK(GPIO_PIN_RESET);
    if(hwData & 0x0100) {
      OLED_DIN(GPIO_PIN_SET);
		}
    else  {
      OLED_DIN(GPIO_PIN_RESET);
		}
    OLED_SCK(GPIO_PIN_SET);
		hwData <<= 1;
	}

  
#endif
  
	switch(typ)
	{
		case 0:OLED_CS(GPIO_PIN_SET);break;
		case 1:OLED_CS1(GPIO_PIN_SET);break;
		default:OLED_CS(GPIO_PIN_SET);OLED_CS1(GPIO_PIN_SET);break;
	}
}


void OLED_Driver::Write_Data(uint8_t dat,uint8_t typ) {
  
	switch(typ)
	{
		case 0:OLED_CS(GPIO_PIN_RESET);break;
		case 1:OLED_CS1(GPIO_PIN_RESET);break;
		default:OLED_CS(GPIO_PIN_RESET);OLED_CS1(GPIO_PIN_RESET);break;
	}
  
#if  INTERFACE_4WIRE_SPI
  
  OLED_DC(GPIO_PIN_SET);
  
  while(HAL_SPI_Transmit(&hspi1,&dat,0x01,0x10) != HAL_OK);
  
  OLED_DC(GPIO_PIN_RESET);
  
#elif INTERFACE_3WIRE_SPI
  
  uint8_t i;
	uint16_t hwData = 0;
	
  hwData = (uint16_t)dat | 0x0100;
	
	for(i = 0; i < 9; i ++) {
    OLED_SCK(GPIO_PIN_RESET);
		if(hwData & 0x0100) {
      OLED_DIN(GPIO_PIN_SET);
		}
    else  {
      OLED_DIN(GPIO_PIN_RESET);
		}
    OLED_SCK(GPIO_PIN_SET);
		hwData <<= 1;
	}
  
#endif
  
	switch(typ)
	{
		case 0:OLED_CS(GPIO_PIN_SET);break;
		case 1:OLED_CS1(GPIO_PIN_SET);break;
		default:OLED_CS(GPIO_PIN_SET);OLED_CS1(GPIO_PIN_SET);break;
	}
  
}

uint8_t dmasendflag = 1;

void OLED_Driver::Write_Data(uint8_t* dat_p, long length,uint8_t typ) {
#if DMA_SPI
  while(!dmasendflag);
#endif
	switch(typ)
	{
		case 0:OLED_CS(GPIO_PIN_RESET);break;
		case 1:OLED_CS1(GPIO_PIN_RESET);break;
		default:OLED_CS(GPIO_PIN_RESET);OLED_CS1(GPIO_PIN_RESET);break;
	}
#if INTERFACE_4WIRE_SPI
  
  OLED_DC(GPIO_PIN_SET);
#if DMA_SPI
  HAL_SPI_Transmit_DMA(&hspi1,dat_p,length);
	dmasendflag = 0;
#else
  while(HAL_SPI_Transmit(&hspi1,dat_p,length,0x10) != HAL_OK);
	
  OLED_DC(GPIO_PIN_RESET);
	switch(typ)
	{
		case 0:OLED_CS(GPIO_PIN_SET);break;
		case 1:OLED_CS1(GPIO_PIN_SET);break;
		default:OLED_CS(GPIO_PIN_SET);OLED_CS1(GPIO_PIN_SET);break;
	}
#endif
  
#elif INTERFACE_3WIRE_SPI
  
  uint8_t i,j;
	uint16_t hwData = 0;
	

  for(i = 0; i < length; i++) {
    
    hwData = (uint16_t)dat_p[i] | 0x0100;
    
    for(j = 0; j < 9; j ++) {
      OLED_SCK(GPIO_PIN_RESET);
      if(hwData & 0x0100) {
        OLED_DIN(GPIO_PIN_SET);
      } else {
        OLED_DIN(GPIO_PIN_RESET);
      }
      OLED_SCK(GPIO_PIN_SET);
      hwData <<= 1;
    }
  }
	switch(typ)
	{
		case 0:OLED_CS(GPIO_PIN_SET);break;
		case 1:OLED_CS1(GPIO_PIN_SET);break;
		default:OLED_CS(GPIO_PIN_SET);OLED_CS1(GPIO_PIN_SET);break;
	}
#endif
}


void OLED_Driver::RAM_Address(uint8 typ)  {
  
  Write_Command(0x15,typ);
  Write_Data(0x0,typ);
  Write_Data(0x7f,typ);

  Write_Command(0x75,typ);
  Write_Data(0x20,typ);
  Write_Data(0x7f,typ);
}

int fpscount = 0;
char fpscountch[20];
void OLED_Driver::Refrash_Screen(void)  {
  fpscount++;
  OLED_SHFAny(0,0,fpscountch,19,0xffff);
  RAM_Address(0);
  Write_Command(0x5C,0);
//  for(i=0;i<;i++)  {
      Write_Data(OLED_GRAM[0],128*96*2,0);//RAM data clear
//  }
  RAM_Address(1);
  Write_Command(0x5C,1);
//  for(i=0;i<;i++)  {
      Write_Data(OLED_GRAM[1],128*96*2,1);//RAM data clear
//  }
}
  
void OLED_Driver::Clear_FpsCount(void)  {
  sprintf(fpscountch,"%d",fpscount);
	fpscount=0;
}

void OLED_Driver::Clear_Screen(void)  {
  
  int i,j;
  for(i=0;i<96;i++)  {
    for(j=0;j<128;j++)  {
      OLED_GRAM[0][2*j+i*128*2] = 0;
      OLED_GRAM[0][2*j+1+i*128*2] = 0;
      OLED_GRAM[1][2*j+i*128*2] = 0;
      OLED_GRAM[1][2*j+1+i*128*2] = 0;
    }
  }
}
  
void OLED_Driver::Set_Coordinate(uint16_t x, uint16_t y)  {

  if ((x >= SSD1351_WIDTH) || (y >= SSD1351_HEIGHT)) 
    return;
  //Set x and y coordinate
  Write_Command(SSD1351_CMD_SETCOLUMN,3);
  Write_Data(x,3);
  Write_Data(SSD1351_WIDTH-1,3);
  Write_Command(SSD1351_CMD_SETROW,3);
  Write_Data(y,3);
  Write_Data(SSD1351_HEIGHT-1,3);
  Write_Command(SSD1351_CMD_WRITERAM,3);
}
  
void OLED_Driver::Set_Address(uint8_t column, uint8_t row)  {
  
  Write_Command(SSD1351_CMD_SETCOLUMN,3);
  Write_Data(column,3);	//X start 
  Write_Data(column,3);	//X end 
  Write_Command(SSD1351_CMD_SETROW,3);
  Write_Data(row,3);	//Y start 
  Write_Data(row+7,3);	//Y end 
  Write_Command(SSD1351_CMD_WRITERAM,3);
}
  
  
void  OLED_Driver::Invert(bool v) {
  
  if (v)
    Write_Command(SSD1351_CMD_INVERTDISPLAY,3);
  else
    Write_Command(SSD1351_CMD_NORMALDISPLAY,3);
}

void OLED_Driver::Draw_Pixel(long x, long y)
{
  // Bounds check.
  if ((x >= SSD1351_WIDTH) || (y >= SSD1351_HEIGHT)) return;
  if ((x < 0) || (y < 0)) return;

	if(x<128)
	{
		OLED_GRAM[1][2*x+y*128*2] = color_byte[0];
		OLED_GRAM[1][2*x+1+y*128*2] = color_byte[1];
	}
	else
	{
		x-=128;
		OLED_GRAM[0][2*x+y*128*2] = color_byte[0];
		OLED_GRAM[0][2*x+1+y*128*2] = color_byte[1];
	}
  
}

void OLED_Driver::Draw_Pixel(long x, long y,uint16_t color)
{
  // Bounds check.
  if ((x >= SSD1351_WIDTH) || (y >= SSD1351_HEIGHT)) return;
  if ((x < 0) || (y < 0)) return;

	if(x<128)
	{
		OLED_GRAM[1][2*x+y*128*2] = (uint8_t)(color >> 8);
		OLED_GRAM[1][2*x+1+y*128*2] = (uint8_t)(color & 0x00ff);
	}
	else
	{
		x-=128;
		OLED_GRAM[0][2*x+y*128*2] = (uint8_t)(color >> 8);
		OLED_GRAM[0][2*x+1+y*128*2] = (uint8_t)(color & 0x00ff);
	}
  
  
}
  
  
void OLED_Driver::Device_Init(void) {

  OLED_CS(GPIO_PIN_RESET);
  OLED_CS1(GPIO_PIN_RESET);

  OLED_RST(GPIO_PIN_RESET);
  HAL_Delay(40);
  OLED_RST(GPIO_PIN_SET);
  HAL_Delay(20);
    
  Write_Command(0xfd,3);	// command lock
  Write_Data(0x12,3);
  Write_Command(0xfd,3);	// command lock
  Write_Data(0xB1,3);

  Write_Command(0xae,3);	// display off
  Write_Command(0xa4,3); 	// Normal Display mode

  Write_Command(0x15,3);	//set column address
  Write_Data(0x00,3);     //column address start 00
  Write_Data(0x7f,3);     //column address end 95
  Write_Command(0x75,3);	//set row address
  Write_Data(0x20,3);     //row address start 00
  Write_Data(0x7f,3);     //row address end 63	

  Write_Command(0xB3,3);
  Write_Data(0xF1,3);

  Write_Command(0xCA,3);	
  Write_Data(0x7F,3);

  Write_Command(0xa0,3);  //set re-map & data format
  Write_Data(0x74,3);     //Horizontal address increment

  Write_Command(0xa1,3);  //set display start line
  Write_Data(0x00,3);     //start 00 line

  Write_Command(0xa2,3);  //set display offset
  Write_Data(0x00,3);

  Write_Command(0xAB,3);	
  Write_Command(0x01,3);	

  Write_Command(0xB4,3);	
  Write_Data(0xA0,3);	  
  Write_Data(0xB5,3);  
  Write_Data(0x55,3);    

  Write_Command(0xC1,3);	
  Write_Data(0xC8,3);	
  Write_Data(0x80,3);
  Write_Data(0xC0,3);

  Write_Command(0xC7,3);	
  Write_Data(0x0F,3);

  Write_Command(0xB1,3);	
  Write_Data(0x32,3);

  Write_Command(0xB2,3);	
  Write_Data(0xA4,3);
  Write_Data(0x00,3);
  Write_Data(0x00,3);

  Write_Command(0xBB,3);	
  Write_Data(0x17,3);

  Write_Command(0xB6,3);
  Write_Data(0x01,3);

  Write_Command(0xBE,3);
  Write_Data(0x05,3);

  Write_Command(0xA6,3);

  Clear_Screen();
	Refrash_Screen();
  Write_Command(0xaf,3);	 //display on
}

  
// Draw a horizontal line ignoring any screen rotation.
void OLED_Driver::Draw_FastHLine(int16_t x, int16_t y, int16_t length) {
  // Bounds check
		int16_t x0=x;
    do
    {
        Draw_Pixel(x, y);   // 逐点显示，描出垂直线
        x++;
    }
    while(x0+length>=x);
}
// Draw a horizontal line ignoring any screen rotation.
void OLED_Driver::Draw_FastHLine(int16_t x, int16_t y, int16_t length,uint16_t color) {
  // Bounds check
		int16_t x0=x;
    do
    {
        Draw_Pixel(x, y,color);   // 逐点显示，描出垂直线
        x++;
    }
    while(x0+length>=x);
}
  
  // Draw a vertical line ignoring any screen rotation.
void OLED_Driver::Draw_FastVLine(int16_t x, int16_t y, int16_t length)  {
  // Bounds check
		int16_t y0=y;
    do
    {
        Draw_Pixel(x, y);   // 逐点显示，描出垂直线
        y++;
    }
    while(y0+length>=y);
}
  // Draw a vertical line ignoring any screen rotation.
void OLED_Driver::Draw_FastVLine(int16_t x, int16_t y, int16_t length,uint16_t color)  {
  // Bounds check
		int16_t y0=y;
    do
    {
        Draw_Pixel(x, y,color);   // 逐点显示，描出垂直线
        y++;
    }
    while(y0+length>=y);
}
  
void OLED_Driver::Display_hbmp(int x,int y,int w,int h,const u8 *ch,uint16_t color)
{
	u16 i,j;
	u16 red,green,blue;
	u16 red1,green1,blue1;
	u16 Factor;
//	color = 0xFFFF;
	red = color>>11;
	green = (color&0x7E0)>>5;
	blue = color&0x1F;
	for(j=0;j<h;j++)
		for(i=0;i<(w+1)/2;i++)
		{
			Factor = (ch[j*((w+1)/2)+i]&0xF0)>>4;
			red1 = red*(Factor+16)/32;
			green1 = green*(Factor+16)/32;
			blue1=blue*(Factor+16)/32;
			
			if(Factor)
				Draw_Pixel(x+i*2+0,y+j,red1<<11|(green1<<5)|(blue1));
			
			Factor = (ch[j*((w+1)/2)+i]&0xF);
			red1 = red*(Factor+16)/32;
			green1 = green*(Factor+16)/32;
			blue1=blue*(Factor+16)/32;
			
			if(Factor)
				Draw_Pixel(x+i*2+1,y+j,red1<<11|(green1<<5)|(blue1));
		}
}	
  
void OLED_Driver::Display_hbmp(int x,int y,int w,int h,const u8 *ch,uint16_t color,uint8_t bk)
{
	u16 i,j;
	u16 red,green,blue;
	u16 red1,green1,blue1;
	u16 Factor;
//	color = 0xFFFF;
	red = color>>11;
	green = (color&0x7E0)>>5;
	blue = color&0x1F;
	for(j=0;j<h;j++)
		for(i=0;i<(w+1)/2;i++)
		{
			Factor = (ch[j*((w+1)/2)+i]&0xF0)>>4;
			red1 = red*(Factor+16)/32;
			green1 = green*(Factor+16)/32;
			blue1=blue*(Factor+16)/32;
			
			if(Factor)
				Draw_Pixel(x+i*2+0,y+j,red1<<11|(green1<<5)|(blue1));
			else
				Draw_Pixel(x+i*2+0,y+j,bk);
			
			Factor = (ch[j*((w+1)/2)+i]&0xF);
			red1 = red*(Factor+16)/32;
			green1 = green*(Factor+16)/32;
			blue1=blue*(Factor+16)/32;
			
			if(Factor)
				Draw_Pixel(x+i*2+1,y+j,red1<<11|(green1<<5)|(blue1));
			else
				Draw_Pixel(x+i*2+1,y+j,bk);
		}
}	

void OLED_Driver::OLED_HFAny(int x,int y,int w,int h,u8 Num,const unsigned char *ch,uint16_t color)
{
	u16 i,j;
	u16 red,green,blue;
	u16 red1,green1,blue1;
	u16 Factor;
//	color = 0xFFFF;
	red = color>>11;
	green = (color&0x7E0)>>5;
	blue = color&0x1F;
	for(j=0;j<h;j++)
		for(i=0;i<(w+1)/2;i++)
		{
			Factor = (ch[Num*h*((w+1)/2)+j*((w+1)/2)+i]&0xF0)>>4;
			red1 = red*(Factor+16)/32;
			green1 = green*(Factor+16)/32;
			blue1=blue*(Factor+16)/32;
			
			if(Factor)
				Draw_Pixel(x+i*2+0,y+j,red1<<11|(green1<<5)|(blue1));
			
			Factor = (ch[Num*h*((w+1)/2)+j*((w+1)/2)+i]&0xF);
			red1 = red*(Factor+16)/32;
			green1 = green*(Factor+16)/32;
			blue1=blue*(Factor+16)/32;
			
			if(Factor)
				Draw_Pixel(x+i*2+1,y+j,red1<<11|(green1<<5)|(blue1));
		}
}	

extern const unsigned char Defu_XF4x5[];
extern const unsigned char GeForce_10x13[];
extern const unsigned char GeForce_19x23[];
extern const unsigned char GeForce_25x37[];
void OLED_Driver::OLED_SHFAny(int x,int y,char *ch,int w,uint16_t color)
{
	u8 c=0,j=0;
	while(ch[j]!='\0')
	{
		c=ch[j]-'0';
		switch(w)
		{
			case 4:OLED_HFAny(x,y,4,5,c,Defu_XF4x5,color);break;
			case 10:OLED_HFAny(x,y,10,13,c,GeForce_10x13,color);break;
			case 19:OLED_HFAny(x,y,19,23,c,GeForce_19x23,color);break;
			case 25:OLED_HFAny(x,y,25,37,c,GeForce_25x37,color);break;
			default:OLED_HFAny(x,y,10,13,c,GeForce_10x13,color);break;
		}
		x+=w; 
		j++;
	}
}


#ifdef __cplusplus
}
#endif


