#include "vfd.h"	 
#include <stdio.h>
#include <stdlib.h> 
#include "main.h"
#include "ds3231.h"
#include "usart.h"

uint8_t VRAM[24*7][2];
uint8_t VBAK[24*7][2];
uint8_t VCORN[5] = {0};
uint8_t VPNT[2] = {0};

//G1,G2,G3,G4,G5,G6,G7,G8,G9,G10,G11,G12,G13,G14
unsigned char VFD_DCRAM[14] = {0x07,'1','3','-','S','T','-','8','4','G','I','N','K',0x07};	//DCRAM
unsigned char VFD_CGRAM[8][5] = {				//CGRAM
	0x08,0x1c,0x3e,0x00,0x3e,	//left arrow
	0x3e,0x00,0x3e,0x1c,0x08,	//right arrow
	0x28,0x2c,0x2e,0x2c,0x28,	//up arrow
	0x0a,0x1a,0x2a,0x1a,0x0a,	//down arrow
	0x55,0x55,0x55,0x55,0x55,
	0x55,0x55,0x55,0x55,0x55,
	0x55,0x55,0x55,0x55,0x55,
	0x55,0x55,0x55,0x55,0x55
};
//NC,TIME_D1,SHIFT_D1,CLOCK_D1,HD_D1,USB_D1,LOCK_D1,DOLBY_D1,MUTE_D1,TU1_D1,TU2_D1,MP3_D1,LOOP_D1,NC,NC,NC,:1_D0,:2_D0,:3_d0
unsigned char VFD_ADRAM[19] = {0x00,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x00,0x00,0x00,0x00,0x00,0x00};			//ADRAM


const uint8_t VFD_Font[]=
{
	0x7f,0x41,0x41,0x7f,0x7f, // 0
	0x00,0x00,0x7f,0x7f,0x00, // 1
	0x79,0x79,0x49,0x49,0x4f, // 2
	0x41,0x49,0x49,0x7f,0x7f, // 3
	0x0e,0x08,0x08,0x7f,0x7f, // 4
	0x4f,0x49,0x49,0x79,0x79, // 5
	0x7f,0x49,0x49,0x79,0x78, // 6
	0x01,0x01,0x01,0x7f,0x7f, // 7
	0x7f,0x49,0x49,0x7f,0x7f, // 8
	0x0f,0x49,0x49,0x7f,0x7f, // 9
	0x7F,0x7F,0x09,0x09,0x7F, // A
	0x7F,0x7F,0x49,0x4F,0x78, // B
	0x7f,0x7f,0x41,0x41,0x41, // C
	0x7F,0x7F,0x41,0x41,0x7E, // D
	0x7F,0x7F,0x49,0x49,0x41, // E
	0x7F,0x7F,0x09,0x09,0x01, // F
	0x7F,0x7F,0x41,0x49,0x79, // G
	0x7F,0x7F,0x08,0x08,0x7F, // H
	0x41,0x7F,0x7F,0x41,0x00, // I
	0x61,0x41,0x7F,0x7F,0x01, // J
	0x7F,0x7F,0x1C,0x36,0x63, // K
	0x7F,0x7F,0x40,0x40,0x60, // L
	0x7F,0x03,0x07,0x03,0x7F, // M
	0x7F,0x7F,0x04,0x08,0x7F, // N
	0x7F,0x7F,0x41,0x41,0x7F, // O
	0x7f,0x7f,0x11,0x11,0x1f, // P
	0x7F,0x7F,0x41,0x61,0x7F, // Q
	0x7F,0x7F,0x09,0x79,0x4F, // R
	0x4F,0x4F,0x49,0x49,0x79, // S
	0x01,0x7F,0x7F,0x01,0x01, // T
	0x7f,0x7f,0x40,0x40,0x7f, // U
	0x7F,0x7F,0x30,0x0C,0x03, // V
	0x7F,0x60,0x70,0x60,0x7F, // W
	0x71,0x19,0x7F,0x4C,0x47, // X
	0x0F,0x0F,0x78,0x08,0x0F, // Y
	0x63,0x51,0x49,0x45,0x63, // Z
	0x00,0x00,0x36,0x36,0x00, // :
	0x40,0x40,0x40,0x40,0x40, // _
	0x00,0x1F,0x13,0x13,0x03, // ℃
	0x00,0x00,0x00,0x00,0x00, //  
	0x10,0x18,0x18,0x18,0x08, // -
	0x55,0xAA,0x55,0xAA,0x55, // 
};

const uint8_t VFD_Fonts[][91*5]=
{
{0x00,0x00,0x00,0x00,0x00,/*" ",0*/

0x00,0x00,0x5F,0x00,0x00,/*"!",1*/

0x01,0x00,0x00,0x01,0x00,/*""",2*/

0x7C,0x17,0x14,0x7C,0x17,/*"#",3*/

0x22,0x45,0x7F,0x4B,0x32,/*"$",4*/

0x49,0x37,0x0C,0x72,0x49,/*"%",5*/

0x4F,0x59,0x67,0x78,0x48,/*"&",6*/

0x00,0x01,0x00,0x00,0x00,/*"'",7*/

0x00,0x00,0x3E,0x41,0x00,/*"(",8*/

0x00,0x61,0x1E,0x00,0x00,/*")",9*/

0x14,0x0C,0x3E,0x0C,0x14,/*"*",10*/

0x08,0x08,0x3E,0x08,0x08,/*"+",11*/

0x40,0x40,0x00,0x00,0x00,/*",",12*/

0x08,0x08,0x08,0x08,0x08,/*"-",13*/

0x40,0x40,0x00,0x00,0x00,/*".",14*/

0x40,0x30,0x08,0x06,0x01,/*"/",15*/

0x3E,0x51,0x49,0x45,0x3E,/*"0",16*/

0x00,0x00,0x02,0x7F,0x00,/*"1",17*/

0x42,0x61,0x51,0x49,0x46,/*"2",18*/

0x21,0x41,0x45,0x4B,0x31,/*"3",19*/

0x18,0x14,0x12,0x7F,0x10,/*"4",20*/

0x2F,0x45,0x45,0x45,0x39,/*"5",21*/

0x3E,0x49,0x49,0x49,0x31,/*"6",22*/

0x01,0x01,0x79,0x05,0x03,/*"7",23*/

0x36,0x49,0x49,0x49,0x36,/*"8",24*/

0x26,0x49,0x49,0x49,0x3E,/*"9",25*/

0x00,0x00,0x24,0x00,0x00,/*":",26*/

0x00,0x00,0x44,0x00,0x00,/*";",27*/

0x08,0x14,0x22,0x22,0x41,/*"<",28*/

0x14,0x14,0x14,0x14,0x14,/*"=",29*/

0x41,0x22,0x22,0x14,0x08,/*">",30*/

0x07,0x01,0x51,0x09,0x07,/*"?",31*/

0x3E,0x5D,0x63,0x5F,0x61,/*"@",32*/

0x60,0x1C,0x13,0x1C,0x60,/*"A",33*/

0x7F,0x49,0x49,0x49,0x76,/*"B",34*/

0x3f,0x41,0x41,0x41,0x22,/*"C",35*/

0x7F,0x41,0x41,0x41,0x3E,/*"D",36*/

0x7F,0x49,0x49,0x49,0x41,/*"E",37*/

0x7F,0x09,0x09,0x09,0x01,/*"F",38*/

0x3E,0x41,0x41,0x51,0x32,/*"G",39*/

0x7F,0x08,0x08,0x08,0x7F,/*"H",40*/

0x00,0x41,0x7F,0x41,0x00,/*"I",41*/

0x20,0x40,0x41,0x3F,0x01,/*"J",42*/

0x7F,0x08,0x0C,0x32,0x41,/*"K",43*/

0x7F,0x40,0x40,0x40,0x40,/*"L",44*/

0x7F,0x0C,0x30,0x0C,0x7F,/*"M",45*/

0x7F,0x02,0x0C,0x10,0x7F,/*"N",46*/

0x3E,0x41,0x41,0x41,0x3E,/*"O",47*/

0x7F,0x09,0x09,0x09,0x06,/*"P",48*/

0x3E,0x41,0x41,0x61,0x7E,/*"Q",49*/

0x7F,0x09,0x09,0x39,0x46,/*"R",50*/

0x46,0x49,0x49,0x49,0x31,/*"S",51*/

0x01,0x01,0x7F,0x01,0x01,/*"T",52*/

0x3F,0x40,0x40,0x40,0x3F,/*"U",53*/

0x03,0x1C,0x60,0x1C,0x03,/*"V",54*/

0x1F,0x60,0x1F,0x60,0x1F,/*"W",55*/

0x41,0x36,0x08,0x36,0x41,/*"X",56*/

0x03,0x04,0x78,0x04,0x03,/*"Y",57*/

0x61,0x51,0x49,0x45,0x43,/*"Z",58*/

0x00,0x00,0x7F,0x00,0x00,/*"[",59*/

0x01,0x06,0x08,0x30,0x40,/*"\",60*/

0x00,0x00,0x7F,0x00,0x00,/*"]",61*/

0x00,0x00,0x00,0x00,0x00,/*"^",62*/

0x08,0x08,0x08,0x08,0x08,/*"-",63*/

0x00,0x01,0x00,0x00,0x00,/*"'",64*/

0x68,0x54,0x54,0x54,0x78,/*"a",65*/

0x7F,0x48,0x44,0x44,0x38,/*"b",66*/

0x38,0x44,0x44,0x44,0x48,/*"c",67*/

0x38,0x44,0x44,0x44,0x7F,/*"d",68*/

0x38,0x54,0x54,0x54,0x18,/*"e",69*/

0x04,0x7E,0x45,0x05,0x05,/*"f",70*/

0x78,0x24,0x24,0x54,0x48,/*"g",71*/

0x7F,0x04,0x04,0x04,0x78,/*"h",72*/

0x04,0x44,0x7D,0x40,0x00,/*"i",73*/

0x00,0x00,0x04,0x05,0x79,/*"j",74*/

0x7F,0x10,0x10,0x2C,0x44,/*"k",75*/

0x00,0x41,0x7F,0x40,0x00,/*"l",76*/

0x78,0x04,0x7C,0x04,0x04,/*"m",77*/

0x7C,0x08,0x04,0x04,0x78,/*"n",78*/

0x38,0x44,0x44,0x44,0x38,/*"o",79*/

0x7C,0x44,0x44,0x44,0x38,/*"p",80*/

0x38,0x44,0x44,0x44,0x7C,/*"q",81*/

0x44,0x7C,0x08,0x04,0x04,/*"r",82*/

0x68,0x54,0x54,0x54,0x6C,/*"s",83*/

0x04,0x3C,0x44,0x44,0x40,/*"t",84*/

0x3C,0x40,0x40,0x44,0x7C,/*"u",85*/

0x0C,0x34,0x40,0x34,0x0C,/*"v",86*/

0x3C,0x60,0x1C,0x60,0x1C,/*"w",87*/

0x44,0x2C,0x10,0x2C,0x44,/*"x",88*/

0x04,0x1C,0x60,0x1C,0x04,/*"y",89*/

0x44,0x64,0x54,0x4C,0x44},/*"z",90*/
{
0x00,0x00,0x00,0x00,0x00,/*" ",0*/

0x00,0x00,0x5F,0x5F,0x00,/*"!",1*/

0x00,0x00,0x01,0x01,0x00,/*""",2*/

0x30,0x3E,0x7E,0x7E,0x10,/*"#",3*/

0x00,0x47,0x7F,0x79,0x00,/*"$",4*/

0x0F,0x3F,0x7F,0x72,0x70,/*"%",5*/

0x36,0x7F,0x7F,0x7A,0x40,/*"&",6*/

0x00,0x01,0x01,0x00,0x00,/*"'",7*/

0x00,0x00,0x7F,0x7F,0x00,/*"(",8*/

0x00,0x77,0x77,0x00,0x00,/*")",9*/

0x14,0x1C,0x1C,0x1C,0x14,/*"*",10*/

0x08,0x08,0x1C,0x1C,0x08,/*"+",11*/

0x00,0x40,0x40,0x00,0x00,/*",",12*/

0x00,0x08,0x08,0x08,0x00,/*"-",13*/

0x00,0x40,0x40,0x00,0x00,/*".",14*/

0x40,0x78,0x3E,0x07,0x01,/*"/",15*/

0x3E,0x7F,0x41,0x41,0x3E,/*"0",16*/

0x00,0x06,0x7F,0x7F,0x00,/*"1",17*/

0x62,0x73,0x79,0x6F,0x66,/*"2",18*/

0x21,0x61,0x4D,0x7F,0x33,/*"3",19*/

0x1C,0x12,0x7F,0x7F,0x10,/*"4",20*/

0x26,0x67,0x45,0x7D,0x38,/*"5",21*/

0x3E,0x7F,0x4D,0x7D,0x38,/*"6",22*/

0x01,0x71,0x7F,0x0F,0x03,/*"7",23*/

0x36,0x7F,0xc9,0x49,0x36,/*"8",24*/

0x0E,0x5B,0x79,0x7F,0x1E,/*"9",25*/

0x00,0x00,0x00,0x00,0x00,/*":",26*/

0x00,0x00,0x44,0x44,0x00,/*";",27*/

0x08,0x1C,0x36,0x22,0x00,/*"<",28*/

0x14,0x14,0x14,0x14,0x14,/*"=",29*/

0x00,0x22,0x36,0x1C,0x08,/*">",30*/

0x02,0x03,0x09,0x0F,0x06,/*"?",31*/

0x3E,0x7F,0x5F,0x7F,0x3E,/*"@",32*/

0x7E,0x7F,0x09,0x09,0x7E,/*"A",33*/

	0x7F,0x7F,0x49,0x4F,0x70, // B

0x3E,0x7F,0x41,0x63,0x62,/*"C",35*/

0x7F,0x7F,0x41,0x41,0x3E,/*"D",36*/

0x7F,0x7F,0x49,0x49,0x41,/*"E",37*/

0x7F,0x7F,0x09,0x09,0x01,/*"F",38*/

0x3E,0x7F,0x41,0x53,0x32,/*"G",39*/

0x7F,0x7F,0x08,0x08,0x7F,/*"H",40*/

	0x41,0x7F,0x7F,0x41,0x00, // I
	0x61,0x41,0x7F,0x7F,0x01, // J

	0x7F,0x7F,0x1C,0x36,0x63, // K

0x7F,0x7F,0x40,0x40,0x40,/*"L",44*/

0x7F,0x0F,0x3C,0x0F,0x7F,/*"M",45*/

	0x7F,0x7F,0x04,0x08,0x7F, // N

	0x3E,0x7F,0x41,0x41,0x3E, // O

0x7F,0x7F,0x09,0x0F,0x06,/*"P",48*/

	0x3E,0x7F,0x41,0x61,0x7E, // Q

	0x7F,0x7F,0x09,0x79,0x4E, // R

0x66,0x6F,0x49,0x79,0x32,/*"S",51*/

0x01,0x7F,0x7F,0x01,0x01,/*"T",52*/

0x3F,0x7F,0x40,0x40,0x3F,/*"U",53*/

	0x7F,0x7F,0x30,0x0C,0x03, // V

	0x3F,0x78,0x1E,0x78,0x3F, // W
	
	0x71,0x19,0x7F,0x4C,0x47, // X
	0x07,0x0F,0x78,0x0C,0x07, // Y
	0x63,0x73,0x49,0x45,0x63, // Z

0x00,0x00,0x7F,0x7F,0x00,/*"[",59*/

0x00,0x0E,0x7E,0x70,0x00,/*"\",60*/

0x00,0x00,0x00,0x00,0x00,/*"]",61*/

0x00,0x00,0x00,0x00,0x00,/*"^",62*/

0x08,0x08,0x08,0x08,0x08,/*"-",63*/

0x00,0x01,0x01,0x00,0x00,/*"'",64*/

0x68,0x7C,0x7C,0x7C,0x40,/*"a",65*/

0x3F,0x7F,0x44,0x7C,0x38,/*"b",66*/

0x38,0x7C,0x44,0x44,0x00,/*"c",67*/

0x38,0x7C,0x44,0x7F,0x7F,/*"d",68*/

0x28,0x7C,0x54,0x5C,0x18,/*"e",69*/

0x04,0x7E,0x7F,0x05,0x01,/*"f",70*/

0x78,0x7C,0x7C,0x5C,0x04,/*"g",71*/

0x7F,0x7F,0x44,0x7C,0x78,/*"h",72*/

0x00,0x7C,0x7C,0x00,0x00,/*"i",73*/

0x00,0x00,0x7D,0x7D,0x00,/*"j",74*/

0x6F,0x7F,0x7C,0x6C,0x44,/*"k",75*/

0x00,0x7F,0x7F,0x00,0x00,/*"l",76*/

0x7C,0x7C,0x7C,0x78,0x78,/*"m",77*/

0x7C,0x7C,0x44,0x7C,0x78,/*"n",78*/

0x38,0x7C,0x44,0x7C,0x38,/*"o",79*/

0x3C,0x7C,0x44,0x7C,0x38,/*"p",80*/

0x38,0x7C,0x44,0x7C,0x38,/*"q",81*/

0x04,0x7C,0x7C,0x04,0x00,/*"r",82*/

0x00,0x7C,0x7C,0x54,0x00,/*"s",83*/

0x04,0x7C,0x7C,0x44,0x00,/*"t",84*/

0x3C,0x7C,0x44,0x7C,0x78,/*"u",85*/

0x04,0x3C,0x3C,0x1C,0x04,/*"v",86*/

0x1C,0x3C,0x7C,0x7C,0x14,/*"w",87*/

0x44,0x6C,0x7C,0x7C,0x04,/*"x",88*/

0x04,0x1C,0x7C,0x7C,0x04,/*"y",89*/

0x00,0x74,0x7C,0x4C,0x00,}/*"z",90*/
};

const uint8_t VFD_NumMt[] = 
{
	0x6F,0x49,0x41,0x7F,0x7F,
	0x7F,0x41,0x41,0x7F,0x7F,
	0x7f,0x41,0x41,0x7f,0x7f, // 0
	0x63,0x41,0x41,0x7F,0x7F,
	0x00,0x41,0x4D,0x7F,0x73,
	0x00,0x00,0x7f,0x7f,0x00, // 1
	0x00,0x30,0x79,0x4F,0x06,
	0x00,0x70,0x79,0x4F,0x07,
	0x71,0x71,0x59,0x4D,0x47,
	0x79,0x79,0x49,0x49,0x4f, // 2
	0x49,0x79,0x79,0x49,0x4F,
	0x41,0x49,0x79,0x79,0x4F,
	0x41,0x49,0x49,0x7D,0x7F,
	0x41,0x49,0x49,0x7f,0x7f, // 3
	0x43,0x49,0x48,0x7F,0x7F,
	0x03,0x49,0x48,0x7F,0x7F,
	0x07,0x08,0x48,0x7F,0x7F,
	0x0e,0x08,0x08,0x7f,0x7f, // 4
	0x0E,0x08,0x49,0x7B,0x7B,
	0x0E,0x49,0x49,0x79,0x7B,
	0x4f,0x49,0x49,0x79,0x79, // 5
	0x5F,0x49,0x49,0x79,0x78,
	0x7f,0x49,0x49,0x79,0x78, // 6
	0x7D,0x49,0x41,0x79,0x7C,
	0x61,0x41,0x41,0x7D,0x7E,
	0x01,0x41,0x41,0x7D,0x7F,
	0x01,0x01,0x01,0x7f,0x7f, // 7
	0x01,0x41,0x49,0x7F,0x7F,
	0x63,0x49,0x49,0x7F,0x7F,
	0x7f,0x49,0x49,0x7f,0x7f, // 8
	0x1F,0x49,0x49,0x7F,0x7F,
	0x0f,0x49,0x49,0x7f,0x7f, // 9
};

void VFD_Write(uint8_t w_data)
{   
	HAL_SPI_Transmit(&hspi2,&w_data,1,2000);
}  

void VFD_WR_WORD(uint16_t DATA) 
{ 
	HAL_GPIO_WritePin(GPIOB,VFD_STB_Pin, GPIO_PIN_RESET);
	__ASM("NOP");    
	VFD_Write(DATA>>8);  
	VFD_Write(DATA&0xFF);    
	HAL_GPIO_WritePin(GPIOB,VFD_STB_Pin, GPIO_PIN_SET);    
	__ASM("NOP");    
}  

void VFD_WR_BYTE(uint8_t DATA)
{
	HAL_GPIO_WritePin(GPIOB,VFD_STB_Pin, GPIO_PIN_RESET);
	__ASM("NOP");    
	VFD_Write(DATA&0xFF);    
	HAL_GPIO_WritePin(GPIOB,VFD_STB_Pin, GPIO_PIN_SET);    
	__ASM("NOP"); 
}	

void VFD_Clear(void)
{
	uint8_t i,j;
	for(i=0;i<24*7;i++)
		for(j=0;j<2;j++)
		{
			VBAK[i][j] = VRAM[i][j];
			VRAM[i][j] = 0x00;
		}
//	for(i=0;i<5;i++)
//		VCORN[i] = 0;
}

void VFD_ClearPart(int x,uint8_t y,uint8_t Wid,uint8_t He)
{
	uint8_t i,j;
	x+=Delt*7;
	for(j=0;j<He;j++)
		for(i=0;i<Wid;i++)
			VRAM[x+i][y] = 0;
}
void VFD_Fill(uint8_t Color)
{
	uint8_t i,j;
	for(i=0;i<24*7;i++)
		for(j=0;j<2;j++)
			VRAM[i][j] = Color;
}


void VFD_Write_CGRAM(unsigned char addr , unsigned char *dat)
{
	unsigned char i;
	HAL_GPIO_WritePin(GPIOB,VFD_STB_Pin, GPIO_PIN_RESET);
	VFD_Write(0x40 | addr); 	//write CGRAM
	for(i=0;i<5;i++)
	{
		VFD_Write(*dat++); //CGRAM
	}
	HAL_GPIO_WritePin(GPIOB,VFD_STB_Pin, GPIO_PIN_SET);   
}
void VFD_Init(void)
{ 
#if GEMINI == 1
//	VFD_WR_BYTE(0x6c);    //设置位
//	VFD_WR_WORD(0x53ff);	//亮度设置FF
//	VFD_WR_BYTE(0x70);    //开显示
//	VFD_Clear();
//	VFD_Refresh_Vram();

	HAL_Delay(5);
	VFD_WR_BYTE(0x6c);    //设置位
	VFD_Clear();
	VFD_Refresh_Vram();
	VFD_WR_WORD(0x5300);	//亮度设置FF
	VFD_WR_BYTE(0x70);    //开显示
#elif GALAXY_D == 1
	VFD_WR_WORD(0xe006); //设置位
	VFD_WR_BYTE(0xa6);    //升压
	VFD_WR_WORD(0xe400);	//亮度
	VFD_Clear();
	VFD_Refresh_Vram();
	VFD_WR_WORD(0xe800);	//开显示
#elif TEACH == 1
	VFD_WR_WORD(0xe00d); //设置位
	VFD_WR_WORD(0xe400);	//亮度
	VFD_Refresh_Vram();
	VFD_WR_WORD(0xe800);    //开显示
	VFD_Write_CGRAM(0x00,VFD_CGRAM[2]);
	
#endif
	
} 

void VFD_Brightness(uint16_t Bright)
{
	if(Bright>0xFF)
		Bright = 0xFF;
#if GEMINI == 1
	VFD_WR_WORD(0x5300|Bright);	//亮度设置FF
#elif GALAXY_D == 1
	VFD_WR_WORD(0xe400|Bright);	//亮度
#elif TEACH == 1
	VFD_WR_WORD(0xe400|Bright);	//亮度
#endif
	

}

void VFD_Refresh_Vram(void)
{
#if GEMINI == 1
	int i, j;
	HAL_GPIO_WritePin(GPIOB,VFD_STB_Pin, GPIO_PIN_RESET);
	VFD_Write(0xa0);
	for (j = 6; j < 18; j++)
		for (i = 0; i < 5; i++)
			VFD_Write(VRAM[j*7 + i][0]);  
	for (i = 0; i < 5; i++)
		VFD_Write(VCORN[i]);
	HAL_GPIO_WritePin(GPIOB,VFD_STB_Pin, GPIO_PIN_SET);   

	HAL_GPIO_WritePin(GPIOB,VFD_STB_Pin, GPIO_PIN_RESET);
	VFD_Write(0x20);
	for (j = 6; j < 18; j++)
		for (i = 0; i < 5; i++)
			VFD_Write(VRAM[j*7 + i][1]);  
	HAL_GPIO_WritePin(GPIOB,VFD_STB_Pin, GPIO_PIN_SET);   
	
	HAL_GPIO_WritePin(GPIOB,VFD_STB_Pin, GPIO_PIN_RESET);
	VFD_Write(0x90);
	for (j = 0; j < 13; j++)
		VFD_Write(j);
	HAL_GPIO_WritePin(GPIOB,VFD_STB_Pin, GPIO_PIN_SET);   
	
	HAL_GPIO_WritePin(GPIOB,VFD_STB_Pin, GPIO_PIN_RESET);
	VFD_Write(0x10);
	for (j = 0; j < 12; j++)
		VFD_Write(j);
	HAL_GPIO_WritePin(GPIOB,VFD_STB_Pin, GPIO_PIN_SET);   
#elif GALAXY_D == 1
	int i,j;
	for(j=0;j<7;j++)
	{
		HAL_GPIO_WritePin(GPIOB,VFD_STB_Pin, GPIO_PIN_RESET);   
		VFD_Write(0x40+j); 	 //向地址0x00-0x06写入数据
		for(i=0;i<5;i++)
			VFD_Write(VRAM[j*7+42+i][0]);
		
		HAL_GPIO_WritePin(GPIOB,VFD_STB_Pin, GPIO_PIN_SET);  
		VFD_WR_WORD(((0x20+j)<<8)|(0x00+j));	//在地址0x00-0x06读取数据显示
	}
	HAL_GPIO_WritePin(GPIOB,VFD_STB_Pin, GPIO_PIN_RESET);   
	VFD_Write(0x62);	//左边的冒号
	VFD_Write(VPNT[0]);	//左边的冒号
	VFD_Write(0x64);
	VFD_Write(VPNT[1]);	//左边的冒号
	HAL_GPIO_WritePin(GPIOB,VFD_STB_Pin, GPIO_PIN_SET);  
#elif TEACH == 1
	int i,j;
	HAL_GPIO_WritePin(GPIOB,VFD_STB_Pin, GPIO_PIN_RESET);
	VFD_Write(0x40); 	//write CGRAM
	for (i=0;i<8;i++)				
	{
		for(j=0;j<5;j++)
		{
			VFD_Write(VFD_CGRAM[i][j]); //CGRAM
		}
	}
HAL_GPIO_WritePin(GPIOB,VFD_STB_Pin, GPIO_PIN_SET); 
//for(j=0;j<7;j++)
//{
//	HAL_GPIO_WritePin(GPIOB,VFD_STB_Pin, GPIO_PIN_RESET);   
//	VFD_Write(0x40+j); 	 //向地址0x00-0x06写入数据
//	for(i=0;i<5;i++)
//		VFD_Write(0x5A);
//	
//	HAL_GPIO_WritePin(GPIOB,VFD_STB_Pin, GPIO_PIN_SET);  
//	VFD_WR_WORD(((0x20+j)<<8)|(0x00+j));	//在地址0x00-0x06读取数据显示
//}  
	
//	VFD_Write(0x20); 	//write DCRAM
//	for(i=0;i<24;i++)				
//	{
//		VFD_Write(VFD_DCRAM[i]); //DCRAM
//	}
//	HAL_GPIO_WritePin(GPIOB,VFD_STB_Pin, GPIO_PIN_SET);  
	HAL_GPIO_WritePin(GPIOB,VFD_STB_Pin, GPIO_PIN_RESET);
	VFD_Write(0X60);	//write ADRAM
	for(i=0;i<19;i++)				
	{
		VFD_Write(VFD_ADRAM[i]); //ADRAM
	}
	HAL_GPIO_WritePin(GPIOB,VFD_STB_Pin, GPIO_PIN_SET);  
#endif
//HAL_Delay(500);
}

void VFD_PNTTIME(void)
{
	static uint8_t step = 0;
	step++;
	switch(step/4)
	{
		case 0:VPNT[0] = 0x4;VPNT[1] = 0x0;break;
		case 1:VPNT[0] = 0x6;VPNT[1] = 0x4;break;
		case 2:VPNT[0] = 0x6;VPNT[1] = 0x6;break;
		case 3:VPNT[0] = 0x6;VPNT[1] = 0x6;break;
		case 4:VPNT[0] = 0x6;VPNT[1] = 0x6;break;
		case 5:VPNT[0] = 0x2;VPNT[1] = 0x6;break;
		case 6:VPNT[0] = 0x0;VPNT[1] = 0x2;break;
		case 7:VPNT[0] = 0x0;VPNT[1] = 0x0;break;
		case 12:step = 0;break;
	}
}

void VFD_PNTMSG(void)
{
	static uint8_t step = 0;
	step++;
	switch(step/4)
	{
		case 0:VPNT[0] = 0x4;VPNT[1] = 0x1;break;
		case 1:VPNT[0] = 0x6;VPNT[1] = 0x1;break;
		case 2:VPNT[0] = 0x6;VPNT[1] = 0x1;break;
		case 3:VPNT[0] = 0x6;VPNT[1] = 0x1;break;
		case 4:VPNT[0] = 0x6;VPNT[1] = 0x1;break;
		case 5:VPNT[0] = 0x2;VPNT[1] = 0x1;break;
		case 6:VPNT[0] = 0x0;VPNT[1] = 0x1;break;
		case 7:VPNT[0] = 0x0;VPNT[1] = 0x1;break;
		case 8:step = 0;VPNT[1] = 0x1;break;
	}
}

void VFD_PNTCls(void)
{
	VPNT[0] = 0;VPNT[1] = 0;
}

#define PIXNUM 10
#define PIXCH 5
uint8_t OneX[PIXNUM][PIXCH],OneY[PIXNUM][PIXCH];
uint8_t MovX[PIXNUM][PIXCH],MovY[PIXNUM][PIXCH];
uint8_t OneStaus[PIXNUM][PIXCH] = {0};
uint8_t OutFlag[PIXNUM][PIXCH] = {0};
uint8_t StopTime[PIXNUM][PIXCH] = {0};

void VFD_Bow(uint8_t X,uint8_t H,uint8_t Y,uint8_t PixNum,uint8_t Ch)
{
	uint8_t Temp[PIXNUM][PIXCH];
	uint8_t i;
	for(i=0;i<PixNum;i++)
	{
		if(VFD_ReadPoint(MovX[i][Ch],MovY[i][Ch]))
			VFD_Point(MovX[i][Ch],MovY[i][Ch],0); 
		else
			VFD_Point(MovX[i][Ch],MovY[i][Ch],1); 
		switch(OneStaus[i][Ch])
		{
			case 0:if(StopTime[i][Ch]<4) StopTime[i][Ch]++; else{Temp[i][Ch] = OneX[i][Ch] ; OneX[i][Ch] = rand()%H+X; if(Temp[i][Ch] == OneX[i][Ch]) OneX[i][Ch] = rand()%H+X; OneStaus[i][Ch] = 2;StopTime[i][Ch] = 0;}
				OutFlag[i][Ch] = False;break;
			case 1:if(StopTime[i][Ch]<4) StopTime[i][Ch]++; else{Temp[i][Ch] = OneY[i][Ch] ; OneY[i][Ch] = rand()%Y; if(Temp[i][Ch] == OneY[i][Ch]) OneY[i][Ch] = rand()%Y; OneStaus[i][Ch] = 3;StopTime[i][Ch] = 0;}
				OutFlag[i][Ch] = False;break;
			case 2:if(MovX[i][Ch]>OneX[i][Ch]) {MovX[i][Ch]--;OutFlag[i][Ch] = True;}
				else if(MovX[i][Ch]<OneX[i][Ch]) {MovX[i][Ch]++;OutFlag[i][Ch] = True;}
				else {OneStaus[i][Ch] = 1;OutFlag[i][Ch] = False;} break;
				
			case 3:if(MovY[i][Ch]>OneY[i][Ch]) {MovY[i][Ch]--;OutFlag[i][Ch] = True;}
				else if(MovY[i][Ch]<OneY[i][Ch]) {MovY[i][Ch]++;OutFlag[i][Ch] = True;}
				else {OneStaus[i][Ch] = 0;OutFlag[i][Ch] = False;} break;
		}
		
		if(OutFlag[i][Ch] == True)//防止自己消除
		{
			if(VFD_ReadPoint(MovX[i][Ch],MovY[i][Ch]))
				VFD_Point(MovX[i][Ch],MovY[i][Ch],0); 
			else
				VFD_Point(MovX[i][Ch],MovY[i][Ch],1); 
		}
	}
}

uint8_t VFD_NumMtIndex[] = {0+2,3+2,7+2,11+2,15+2,18+2,20+2,24+2,27+2,29+2};
uint8_t VFD_DPointMt[][5] = 
{
	0x00,0x00,0x02,0x00,0x00,
	0x00,0x00,0x02,0x02,0x00,
	0x00,0x00,0x02,0x06,0x00,
	0x00,0x00,0x02,0x16,0x00,
	0x00,0x00,0x02,0x36,0x00,
	0x00,0x00,0x22,0x36,0x00,
	0x00,0x00,0x32,0x36,0x00,
	0x00,0x00,0x36,0x36,0x00,
	0x00,0x00,0x34,0x36,0x00,
	0x00,0x00,0x34,0x34,0x00,
	0x00,0x00,0x34,0x30,0x00,
	0x00,0x00,0x34,0x20,0x00,
	0x00,0x00,0x34,0x00,0x00,
	0x00,0x00,0x14,0x00,0x00,
	0x00,0x00,0x04,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,
};


void VFD_F5x7(int x,uint8_t y,uint8_t Num,uint8_t Mtflag)
{
	uint8_t i;
	static uint8_t RunS = 0;
	static uint8_t FNum[12][2];
	static uint8_t BCount[12][2];
	
	if(Mtflag == True)
	{
		if(Num<=9)
		{
			if(FNum[x/7][y]<VFD_NumMtIndex[Num])
				FNum[x/7][y]++;
			else if(FNum[x/7][y]>VFD_NumMtIndex[Num])
				FNum[x/7][y]=0;
			
			for(i=0;i<5;i++)
				VRAM[x+i][y] = VFD_NumMt[FNum[x/7][y]*5+i];
		}
		else if(Num<=35||Num==38)
		{
			if(VBAK[x][y]!=VFD_Font[Num*5]||VBAK[x+2][y]!=VFD_Font[Num*5+2]||VBAK[x+4][y]!=VFD_Font[Num*5+4])
			{
				for(i=0;i<5;i++)
				{
					if(i%2)
						VRAM[x+i][y] = VBAK[x+i][y]<<1|(VFD_Font[Num*5+i]>>(7-(BCount[x/7][y])%8)&1);
					else
						VRAM[x+i][y] = VBAK[x+i][y]>>1|(VFD_Font[Num*5+i]<<(8-(BCount[x/7][y])%8)&0x80);
				}
				BCount[x/7][y]++;
			}
			else
			{
				for(i=0;i<5;i++)
					VRAM[x+i][y] = VFD_Font[Num*5+i];
				BCount[x/7][y] = 0;
			}
		}
		else if(Num==36)
		{
			RunS++;
			for(i=0;i<5;i++)
				VRAM[x+i][y] = VFD_DPointMt[(RunS/8)%16][i];
		}
		else if(Num==39)
		{
//			if(VBAK[x][y]!=VFD_Font[Num*5]||VBAK[x+2][y]!=VFD_Font[Num*5+2]||VBAK[x+4][y]!=VFD_Font[Num*5+4])
			{
				for(i=0;i<5;i++)
					VRAM[x+i][y] = VFD_Font[Num*5+i];
			}
		}
		else
		{
			if(VBAK[x][y]!=VFD_Font[Num*5]||VBAK[x+2][y]!=VFD_Font[Num*5+2]||VBAK[x+4][y]!=VFD_Font[Num*5+4])
			{
				for(i=0;i<5;i++)
				{
					VRAM[x+i][y] = VBAK[x+i][y]<<1|(VFD_Font[Num*5+i]>>(7-(BCount[x/7][y])%8)&1);
				}
				BCount[x/7][y]++;
			}
			else
			{
				for(i=0;i<5;i++)
					VRAM[x+i][y] = VFD_Font[Num*5+i];
				BCount[x/7][y] = 0;
			}
		}
	}
	else
	{
		for(i=0;i<5;i++)
			VRAM[x+i][y] = VFD_Font[Num*5+i];
	}
}
	

void VFD_SF5x7(int x,uint8_t y,char *ch,uint8_t Mtflag)
{
	uint8_t c=0,j=0;
	x+=Delt*7;
	while(ch[j]!='\0')
	{
		if(ch[j]>='0'&&ch[j]<='9')
			c=ch[j]-'0';
		else if(ch[j]>='A'&&ch[j]<='Z')
			c=ch[j]-'A'+10;
		else if(ch[j]>='a'&&ch[j]<='z')
			c=ch[j]-'a'+10;
		else if(ch[j]==':')
			c=36;
		else if(ch[j]=='#')
			c=38;
		else if(ch[j]==' ')
			c=39;
		else if(ch[j]=='-')
			c=40;
		VFD_F5x7(x,y,c,Mtflag);
		x+=7;
		j++;
	}
}

void VFD_Fs5x7(int x,uint8_t y,uint8_t Num,uint8_t Font)
{
	uint8_t i;
	static uint8_t RunS = 0;
//	static uint8_t FNum[24][2];
	static uint8_t BCount[24][2];
	
	if(Num==':'-' ')
	{
		RunS++;
		for(i=2;i<4;i++)
			VRAM[x+i][y] = VFD_DPointMt[(RunS/8)%16][i];
	}
	else if(Num==' '-' ')
	{
		if(VBAK[x][y]!=0||VBAK[x+1][y]!=0||VBAK[x+2][y]!=0||VBAK[x+3][y]!=0||VBAK[x+4][y]!=0)
		{
			for(i=0;i<5;i++)
				VRAM[x+i][y] = rand()&VBAK[x+i][y];
		}
	}
	else if(Num<='z'-' ')
	{
		if(VBAK[x][y]!=VFD_Fonts[Font][Num*5]||VBAK[x+2][y]!=VFD_Fonts[Font][Num*5+2])
		{
			for(i=0;i<5;i++)
			{
				if(i%2)
					VRAM[x+i][y] = VBAK[x+i][y]<<1|(VFD_Fonts[Font][Num*5+i]>>(7-(BCount[x/7][y])%8)&1);
				else
					VRAM[x+i][y] = VBAK[x+i][y]>>1|(VFD_Fonts[Font][Num*5+i]<<(8-(BCount[x/7][y])%8)&0x80);
			}
			BCount[x/7][y]++;
		}
		else
		{
			for(i=0;i<5;i++)
				VRAM[x+i][y] = VFD_Fonts[Font][Num*5+i];
			BCount[x/7][y] = 0;
		}
	}
	else
		for(i=0;i<5;i++)
			VRAM[x+i][y] = VFD_Fonts[Font][Num*5+i];
}

void VFD_SFs5x7(int x,uint8_t y,char *ch,uint8_t Font)
{
	uint8_t c=0,j=0;
	x+=Delt*7;
	while(ch[j]!='\0')
	{
		c=ch[j]-' ';
		VFD_Fs5x7(x,y,c,Font);
		x+=7;
		j++;
	}
}

void VFD_Part(int x,uint8_t y,uint8_t Wid,uint8_t He,uint8_t *ch)
{
	uint8_t i,j;
	x+=Delt*7;
	for(j=0;j<He;j++)
		for(i=0;i<Wid;i++)
			VRAM[x+i][y] = ch[j*Wid+i];
}

//0/20,2/08d,3/08,4/10,4/40,
void VFD_Corn(VFDCORN corn, uint8_t State)
{
	switch (corn)
	{
		case DOLBY:if (State) VCORN[2] = VCORN[2] | 0x08; else VCORN[2] = VCORN[2] & ~0x08; break;
		case TIME:if (State) VCORN[4] = VCORN[4] | 0x10; else VCORN[4] = VCORN[4] & ~0x10;  break;
		case CIRCLE:if (State) VCORN[0] = VCORN[0] | 0x20; else VCORN[0] = VCORN[0] & ~0x20; break;
		case MESSG:if (State) VCORN[4] = VCORN[4] | 0x40; else VCORN[4] = VCORN[4] & ~0x40; break;
		case HDCORN:if (State) VCORN[3] = VCORN[3] | 0x08; else VCORN[3] = VCORN[3] & ~0x08; break;
	}
}

//显示范围:X(0-81),Y(0-14)

void VFD_Point(int x,uint8_t y,uint8_t t)
{
	x+=Delt*7;
	if(x>83+Delt*7||y>15)return;//超出范围了.
	if(t) VRAM[x][y/8]|=1<<(y%8);
	else VRAM[x][y/8]&=~(1<<(y%8));	  
//	VRAM[x][0] =0xff;
}

uint8_t VFD_ReadPoint(int x,uint8_t y)
{
	x+=Delt*7;
	if(x>83+Delt*7||y>15)return 0;//超出范围了.
	if(VRAM[x][y/8]&(1<<(y%8))) return 1;
	else return 0;
}

/* 定义LCM像素数宏 */
#define  GUI_VFD_LCM_XMAX		84     /* 定义液晶x轴的像素数 */
#define  GUI_VFD_LCM_YMAX		16      /* 定义液晶y轴的像素数 */


#define BLINMAX 5
uint16_t FucBlin[3][BLINMAX];

VFD_STATUS VFD_FucBlin(uint8_t Index)
{
	uint8_t i;
	static float Motion[BLINMAX]={0};
	static uint8_t Rand[2][12];
	
	for(i=0;i<12;i++)
	{
		if(rand()%200<=Rand[1][i])
		{
			Rand[0][i] = rand()%(GUI_VFD_LCM_XMAX);
			Rand[1][i] = rand()%7;
		}
//		VFD_Point(Rand[0][i],Rand[1][i],1);
	}
	
	switch((uint8_t)Motion[Index])
	{
		case 0:
		VFD_Point(FucBlin[0][Index],FucBlin[1][Index],1);
		break;
		case 1:
		VFD_Point(FucBlin[0][Index],FucBlin[1][Index],1);
		VFD_Point(FucBlin[0][Index]+1,FucBlin[1][Index],1);
		break;
		case 2:
		VFD_Point(FucBlin[0][Index],FucBlin[1][Index],1);
		VFD_Point(FucBlin[0][Index]+1,FucBlin[1][Index],1);
		VFD_Point(FucBlin[0][Index],FucBlin[1][Index]+1,1);
		break;
		case 3:
		VFD_Point(FucBlin[0][Index],FucBlin[1][Index],1);
		VFD_Point(FucBlin[0][Index]+1,FucBlin[1][Index],1);
		VFD_Point(FucBlin[0][Index],FucBlin[1][Index]+1,1);
		VFD_Point(FucBlin[0][Index]-1,FucBlin[1][Index],1);
		break;
		case 4:
		VFD_Point(FucBlin[0][Index],FucBlin[1][Index],1);
		VFD_Point(FucBlin[0][Index]+1,FucBlin[1][Index],1);
		VFD_Point(FucBlin[0][Index],FucBlin[1][Index]+1,1);
		VFD_Point(FucBlin[0][Index]-1,FucBlin[1][Index],1);
		VFD_Point(FucBlin[0][Index],FucBlin[1][Index]-1,1);
		break;
		case 5:
		VFD_Point(FucBlin[0][Index],FucBlin[1][Index],1);
		VFD_Point(FucBlin[0][Index]+1,FucBlin[1][Index],1);
		VFD_Point(FucBlin[0][Index],FucBlin[1][Index]+1,1);
		VFD_Point(FucBlin[0][Index]-1,FucBlin[1][Index],1);
		VFD_Point(FucBlin[0][Index],FucBlin[1][Index]-1,1);
		break;
		case 6:
		VFD_Point(FucBlin[0][Index],FucBlin[1][Index],1);
		VFD_Point(FucBlin[0][Index]+1,FucBlin[1][Index],1);
		VFD_Point(FucBlin[0][Index],FucBlin[1][Index]+1,1);
		VFD_Point(FucBlin[0][Index]-1,FucBlin[1][Index],1);
		VFD_Point(FucBlin[0][Index],FucBlin[1][Index]-1,1);
		break;
		case 7:
		VFD_Point(FucBlin[0][Index],FucBlin[1][Index],1);
		VFD_Point(FucBlin[0][Index]+1,FucBlin[1][Index],1);
		VFD_Point(FucBlin[0][Index],FucBlin[1][Index]+1,1);
		VFD_Point(FucBlin[0][Index]-1,FucBlin[1][Index],1);
		VFD_Point(FucBlin[0][Index],FucBlin[1][Index]-1,1);
		break;
		case 8:
		VFD_Point(FucBlin[0][Index],FucBlin[1][Index],1);
		VFD_Point(FucBlin[0][Index]+1,FucBlin[1][Index],1);
		VFD_Point(FucBlin[0][Index],FucBlin[1][Index]+1,1);
		VFD_Point(FucBlin[0][Index]-1,FucBlin[1][Index],1);
		VFD_Point(FucBlin[0][Index],FucBlin[1][Index]-1,1);
		break;
		case 9:
		VFD_Point(FucBlin[0][Index],FucBlin[1][Index],1);
		VFD_Point(FucBlin[0][Index]+1,FucBlin[1][Index],1);
		VFD_Point(FucBlin[0][Index],FucBlin[1][Index]+1,1);
		VFD_Point(FucBlin[0][Index]-1,FucBlin[1][Index],1);
		VFD_Point(FucBlin[0][Index],FucBlin[1][Index]-1,1);
		break;
		case 10:
		VFD_Point(FucBlin[0][Index],FucBlin[1][Index],1);
		VFD_Point(FucBlin[0][Index]+1,FucBlin[1][Index],1);
		VFD_Point(FucBlin[0][Index],FucBlin[1][Index]+1,1);
		VFD_Point(FucBlin[0][Index]-1,FucBlin[1][Index],1);
		break;
		case 11:
		VFD_Point(FucBlin[0][Index],FucBlin[1][Index],1);
		VFD_Point(FucBlin[0][Index]+1,FucBlin[1][Index],1);
		VFD_Point(FucBlin[0][Index],FucBlin[1][Index]+1,1);
		break;
		case 12:
		VFD_Point(FucBlin[0][Index],FucBlin[1][Index],1);
		VFD_Point(FucBlin[0][Index]+1,FucBlin[1][Index],1);
		break;
		case 13:
		VFD_Point(FucBlin[0][Index],FucBlin[1][Index],1);
		VFD_Point(FucBlin[0][Index],FucBlin[1][Index],1);
		break;
//		default:
//		VFD_Point(FucBlin[0][Index]+1,FucBlin[1][Index],1);
//		VFD_Point(FucBlin[0][Index],FucBlin[1][Index]+1,1);
//		VFD_Point(FucBlin[0][Index]-1,FucBlin[1][Index],1);
//		VFD_Point(FucBlin[0][Index],FucBlin[1][Index]-1,1);
//		break;
	}
	Motion[Index]+=0.34f;
	if(Motion[Index] > FucBlin[0][Index]%8+13)
	{
		Motion[Index] = 0;
		return VFD_IDLE;
	}
	return VFD_BUSY;
}

void Motion_Blin(void)
{
	int i;
	for(i=0;i<BLINMAX;i++)
	{
		if(VFD_FucBlin(i) == VFD_IDLE||FucBlin[0][i] == 0)
		{
			FucBlin[0][i] = (rand()%3+1+(rand()%10+1)*7);
			__ASM("NOP");
			FucBlin[1][i] = rand()%13;
		}
	}
}

void  GUI_VFD_HLine(uint16_t x0, uint8_t y0, uint16_t x1, u8 color)
{
    uint16_t  temp;
    if(x0>x1)               // 对x0、x1大小进行排列，以便画图
    {
        temp = x1;
        x1 = x0;
        x0 = temp;
    }
    do
    {
        VFD_Point(x0, y0, color);   // 逐点显示，描出垂直线
        x0++;
    }
    while(x1>=x0);
}

void  GUI_VFD_RLine(uint16_t x0, uint8_t y0, uint8_t y1, u8 color)
{
    uint8_t  temp;
    if(y0>y1)       // 对y0、y1大小进行排列，以便画图
    {
        temp = y1;
        y1 = y0;
        y0 = temp;
    }
    do
    {
        VFD_Point(x0, y0, color);   // 逐点显示，描出垂直线
        y0++;
    }
    while(y1>=y0);
}
/****************************************************************************
* 名称：GUI_VFD_RectangleFill()
* 功能：填充矩形。画一个填充的矩形，填充色与边框色一样。
* 入口参数： x0		矩形左上角的x坐标值
*           y0		矩形左上角的y坐标值
*           x1      矩形右下角的x坐标值
*           y1      矩形右下角的y坐标值
*           color	填充颜色
* 出口参数：无
* 说明：操作失败原因是指定地址超出有效范围。
****************************************************************************/
void  GUI_VFD_RectangleFill(uint32_t x0, uint32_t y0, uint32_t x1, uint32_t y1, u8 color)
{  uint32_t  i;

   /* 先找出矩形左上角与右下角的两个点，保存在(x0,y0)，(x1,y1) */
   if(x0>x1) 						// 若x0>x1，则x0与x1交换
   {  i = x0;
      x0 = x1;
      x1 = i;
   }
   if(y0>y1)						// 若y0>y1，则y0与y1交换
   {  i = y0;
      y0 = y1;
      y1 = i;
   }
   
   /* 判断是否只是直线 */
   if(y0==y1) 
   {  GUI_VFD_HLine(x0, y0, x1, color);
      return;
   }
   if(x0==x1) 
   {  GUI_VFD_RLine(x0, y0, y1, color);
      return;
   }

   while(y0<=y1)						
   {  GUI_VFD_HLine(x0, y0, x1, color);	// 当前画水平线
      y0++;							// 下一行
   }
}

u8 MovRight[] = {0xFF,0xFF,0x40,0x00};
u8 MovLeft[] = {0x00,0x40,0xFF,0xFF};

void VFD_Load()
{
	static u8 Flowleft;
	if(Display_Mode == MODE_MUSIC)
	{
		if(Flowleft < Device_Msg.leftvol*90/65535)
			Flowleft += 1;
		else if(Flowleft > Device_Msg.leftvol*90/65535)
			Flowleft -= 1;
		if(Flowleft>47) Flowleft = 47;
		
		GUI_VFD_RectangleFill(0,1,Flowleft+1,5,1);	
	}
}

char VFDTip[12];
u16 VFDTipCount = 0;

void VFD_Display(void)
{
#if GEMINI == 1
	static u8 Flowleft,FlowRight;
	static u16 Step = 0;
	static u16 MotionStep[3] = {0};
	Step++;
	if(Display_Mode == MODE_MUSIC)
	{
		if(Flowleft < Device_Msg.leftvol*70/65535)
			Flowleft += 1;
		else if(Flowleft > Device_Msg.leftvol*70/65535)
			Flowleft -= 1;
		
		if(FlowRight < Device_Msg.rightvol*70/65535)
			FlowRight += 1;
		else if(FlowRight > Device_Msg.rightvol*70/65535)
			FlowRight -= 1;
			
//		GUI_VFD_HLine(0,0,2,1);
//		GUI_VFD_HLine(79,0,81,1);
		MotionStep[1] = Step%60;
		if(MotionStep[1]<=14)
		{
			GUI_VFD_RLine(1,0,MotionStep[1],1);
			GUI_VFD_RLine(0,0,MotionStep[1],1);
			
			GUI_VFD_RLine(80,0,MotionStep[1],1);
			GUI_VFD_RLine(81,0,MotionStep[1],1);
		}
		else if(MotionStep[1]<=60-7)
		{
			GUI_VFD_RLine(1,0,14,1);
			GUI_VFD_RLine(0,0,14,1);
			
			GUI_VFD_RLine(80,0,14,1);
			GUI_VFD_RLine(81,0,14,1);
			GUI_VFD_HLine(0,14,2,1);
			GUI_VFD_HLine(79,14,81,1);
		}
		else
		{
			
			GUI_VFD_RLine(1,14-(60-MotionStep[1])*2,14,1);
			GUI_VFD_RLine(0,14-(60-MotionStep[1])*2,14,1);
			
			GUI_VFD_RLine(81,14-(60-MotionStep[1])*2,14,1);
			GUI_VFD_RLine(80,14-(60-MotionStep[1])*2,14,1);
			GUI_VFD_HLine(0,14,2,1);
			GUI_VFD_HLine(79,14,81,1);
		}
		
		
		GUI_VFD_RectangleFill(3,1,Flowleft+3,5,1);	
		GUI_VFD_RectangleFill(78,1,81-FlowRight-3,5,1);	
	}
	MotionStep[2] = Step%60;
	if(MotionStep[2]<10)
	{
		VFD_Part(MotionStep[2],1,4,1,MovRight);
		VFD_Part(81-MotionStep[2]-3,1,4,1,MovLeft);
	}
	else if(MotionStep[2]<30)
	{
//		if(MotionStep[2]/6%2)
		{
			VFD_Part(9,1,4,1,MovRight);
			VFD_Part(81-9-3,1,4,1,MovLeft);
		}
	}
	else if(MotionStep[2]<35)
	{
		VFD_Part(9-(MotionStep[2]-30)*2,1,4,1,MovRight);
		VFD_Part(81-(9-(MotionStep[2]-30)*2)-3,1,4,1,MovLeft);
	}
		
//	HAL_Delay(100);
	Motion_Blin();
	if(Display_Mode != MODE_MUSIC)
	{
		VFD_Bow(0,5,7,2,1);
		VFD_Bow(77,5,7,2,2);
	}
	if(VFDTipCount>0&&Display_Mode != MODE_MUSIC)
	{
		VFDTipCount --;
		
		if(VFDTipCount>42)
			VFD_SF5x7((11-cont_str(VFDTip)%12)/2*7,0,VFDTip,True);
		else
			VFD_SF5x7(0-(85-VFDTipCount*2)+(11-cont_str(VFDTip)%12)/2*7,0,VFDTip,False);
	}
	else if(Display_Mode != MODE_MUSIC)
	{
		sprintf(VFDTip,"%s-%s %s",ds3231.Monm,ds3231.Day,ds3231.Weekm);
		VFD_SF5x7((12-cont_str(VFDTip)%12)/2*7,0,VFDTip,True);
	}
	VFD_SF5x7(2*7,1,ds3231.Time,True);
#elif GALAXY_D == 1
	if(Current_Mode!=MODE_OFFLINE)
		VFD_Brightness(0x55);
	else
		VFD_Brightness(0x1);
		
	switch(Current_Mode)
	{
		case MODE_DEFALUT:
		VFD_SF5x7(7,0,ds3231.Hour,True);
		VFD_SF5x7(21,0,ds3231.Min,True);
		VFD_SF5x7(35,0,ds3231.Sec,True);
		VFD_PNTTIME();
		VFD_Bow(0,5,7,2,0);break;
		case MODE_DATE:
		VFD_SF5x7(7,0,ds3231.Hour,True);
		VFD_SF5x7(21,0,ds3231.Min,True);
		VFD_SF5x7(35,0,ds3231.Sec,True);
		VFD_PNTTIME();
		VFD_Bow(0,5,7,2,0);break;
		case MODE_NORMAL:VFD_SF5x7(0,0,"CPU",True); VFD_SF5x7(7*3,0,Device_VStr.cputemp,True);VFD_PNTMSG();break;
		case MODE_GAME:VFD_SF5x7(0,0,"GPU",True); VFD_SF5x7(7*3,0,Device_VStr.gputemp,True);VFD_PNTMSG();break;
		case MODE_OFFLINE:
		VFD_SF5x7(7,0,ds3231.Hour,True);
		VFD_SF5x7(21,0,ds3231.Min,True);
		VFD_SF5x7(35,0,ds3231.Sec,True);
		VFD_PNTTIME();
		VFD_Bow(0,5,7,2,0);break;
//		case MODE_SLEEP:VFD_Bow(0,47,5,5,1);break;
		case MODE_SHOW:VFD_Bow(0,47,5,5,1);break;
		case MODE_MUSIC:VFD_Load();VFD_PNTCls();VFD_Bow(0,47,5,5,1);break;
		default:
		VFD_SF5x7(7,0,ds3231.Hour,True);
		VFD_SF5x7(21,0,ds3231.Min,True);
		VFD_SF5x7(35,0,ds3231.Sec,True);
		VFD_PNTTIME();
		VFD_Bow(0,5,7,2,0);break;
	}
#endif
}
