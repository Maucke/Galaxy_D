#ifndef _I2C_H
#define _I2C_H

#include "main.h"

//#define IIC_SCL    PAout(12) //SCL
//#define IIC_SDA    PAout(11) //SDA	 
//#define READ_SDA   PAin(11)  //输入SDA 


void delay_us(uint16_t us);
void IIC_Start(void);
void IIC_Stop(void);
uint8_t IIC_Wait_Ack(void);
void IIC_Ack(void);	    
void IIC_NAck(void);
void IIC_Send_Byte(uint8_t txd);
uint8_t IIC_Read_Byte(unsigned char ack);
uint8_t read_IIC(uint8_t slave,uint8_t ReadAddr);
void write_IIC(uint8_t slave,uint8_t WriteAddr,uint8_t DataToWrite);

void SDA_IN(void);  //浮空输入
void SDA_OUT(void);  //推挽输出

uint8_t IIC_Write_Byte(uint8_t dat);
uint8_t IIC_Read_Byte1(void);

#endif
