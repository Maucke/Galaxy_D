#include "i2c.h"
#include "main.h"

void SDA_OUT(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
  GPIO_InitStruct.Pin = DS_SDA_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

void SDA_IN(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
  GPIO_InitStruct.Pin = DS_SDA_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);		
}

void delay_us(uint16_t us)
{
	uint16_t i,j;
	for(i=0;i<us;i++)
		for(j=0;j<20;j++)
	;
}
void IIC_Start(void)
{
	SDA_OUT();     //sda线输出
	HAL_GPIO_WritePin(DS_SDA_GPIO_Port, DS_SDA_Pin, GPIO_PIN_SET);	  	  
	HAL_GPIO_WritePin(DS_SCL_GPIO_Port, DS_SCL_Pin, GPIO_PIN_SET);
	delay_us(4);
 	HAL_GPIO_WritePin(DS_SDA_GPIO_Port, DS_SDA_Pin, GPIO_PIN_RESET);//START:when CLK is high,DATA change form high to low 
	delay_us(4);
	HAL_GPIO_WritePin(DS_SCL_GPIO_Port, DS_SCL_Pin, GPIO_PIN_RESET);//钳住I2C总线，准备发送或接收数据 
}	  
//产生IIC停止信号
void IIC_Stop(void)
{
	SDA_OUT();//sda线输出
	HAL_GPIO_WritePin(DS_SCL_GPIO_Port, DS_SCL_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(DS_SDA_GPIO_Port, DS_SDA_Pin, GPIO_PIN_RESET);//STOP:when CLK is high DATA change form low to high
 	delay_us(4);
	HAL_GPIO_WritePin(DS_SCL_GPIO_Port, DS_SCL_Pin, GPIO_PIN_SET); 
	HAL_GPIO_WritePin(DS_SDA_GPIO_Port, DS_SDA_Pin, GPIO_PIN_SET);//发送I2C总线结束信号
	delay_us(4);							   	
}
//等待应答信号到来
//返回值：1，接收应答失败
//        0，接收应答成功
uint8_t IIC_Wait_Ack(void)
{
	uint8_t ucErrTime=0;
	SDA_IN();      //SDA设置为输入  
	HAL_GPIO_WritePin(DS_SDA_GPIO_Port, DS_SDA_Pin, GPIO_PIN_SET);delay_us(1);	   
	HAL_GPIO_WritePin(DS_SCL_GPIO_Port, DS_SCL_Pin, GPIO_PIN_SET);delay_us(1);	 
	while(HAL_GPIO_ReadPin(DS_SDA_GPIO_Port, DS_SDA_Pin))
	{
		ucErrTime++;
		if(ucErrTime>250)
		{
			IIC_Stop();
			return 1;
		}
	}
	HAL_GPIO_WritePin(DS_SCL_GPIO_Port, DS_SCL_Pin, GPIO_PIN_RESET);//时钟输出0 	   
	return 0;  
} 
//产生ACK应答
void IIC_Ack(void)
{
	HAL_GPIO_WritePin(DS_SCL_GPIO_Port, DS_SCL_Pin, GPIO_PIN_RESET);
	SDA_OUT();
	HAL_GPIO_WritePin(DS_SDA_GPIO_Port, DS_SDA_Pin, GPIO_PIN_RESET);
	delay_us(2);
	HAL_GPIO_WritePin(DS_SCL_GPIO_Port, DS_SCL_Pin, GPIO_PIN_SET);
	delay_us(2);
	HAL_GPIO_WritePin(DS_SCL_GPIO_Port, DS_SCL_Pin, GPIO_PIN_RESET);
}
//不产生ACK应答		    
void IIC_NAck(void)
{
	HAL_GPIO_WritePin(DS_SCL_GPIO_Port, DS_SCL_Pin, GPIO_PIN_RESET);
	SDA_OUT();
	HAL_GPIO_WritePin(DS_SDA_GPIO_Port, DS_SDA_Pin, GPIO_PIN_SET);
	delay_us(2);
	HAL_GPIO_WritePin(DS_SCL_GPIO_Port, DS_SCL_Pin, GPIO_PIN_SET);
	delay_us(2);
	HAL_GPIO_WritePin(DS_SCL_GPIO_Port, DS_SCL_Pin, GPIO_PIN_RESET);
}					

GPIO_PinState JudgePin(u8 in)
{
	if(in)
		return GPIO_PIN_SET;
	else
		return GPIO_PIN_RESET;
}
	
//IIC发送一个字节
//返回从机有无应答
//1，有应答
//0，无应答			  
void IIC_Send_Byte(uint8_t txd)
{                        
    uint8_t t;   
	  SDA_OUT(); 	    
    HAL_GPIO_WritePin(DS_SCL_GPIO_Port, DS_SCL_Pin, GPIO_PIN_RESET);//拉低时钟开始数据传输
    for(t=0;t<8;t++)
    {              
    HAL_GPIO_WritePin(DS_SDA_GPIO_Port, DS_SDA_Pin, JudgePin((txd&0x80)>>7));
    txd<<=1; 	  
		delay_us(2);   //对TEA5767这三个延时都是必须的
		HAL_GPIO_WritePin(DS_SCL_GPIO_Port, DS_SCL_Pin, GPIO_PIN_SET);
		delay_us(2); 
		HAL_GPIO_WritePin(DS_SCL_GPIO_Port, DS_SCL_Pin, GPIO_PIN_RESET);	
		delay_us(2);
    }	 
} 

//读1个字节，ack=1时，发送ACK，ack=0，发送nACK   
uint8_t IIC_Read_Byte(unsigned char ack)
{
	unsigned char i,receive=0;
	SDA_IN();//SDA设置为输入
    for(i=0;i<8;i++ )
	{
        HAL_GPIO_WritePin(DS_SCL_GPIO_Port, DS_SCL_Pin, GPIO_PIN_RESET); 
        delay_us(2);
		HAL_GPIO_WritePin(DS_SCL_GPIO_Port, DS_SCL_Pin, GPIO_PIN_SET);
        receive<<=1;
        if(HAL_GPIO_ReadPin(DS_SDA_GPIO_Port, DS_SDA_Pin))receive++;   
		delay_us(1); 
    }					 
    if (!ack)
        IIC_NAck();//发送nACK
    else
        IIC_Ack(); //发送ACK 
    return receive;
}

uint8_t read_IIC(uint8_t slave,uint8_t ReadAddr)
{				  
	  uint8_t temp=0;		  	    																 
    IIC_Start();  
    IIC_Send_Byte(slave);   //发送器件地址,写数据 	   
	  IIC_Wait_Ack(); 
    IIC_Send_Byte(ReadAddr);   //发送低地址
	  IIC_Wait_Ack();	    
	  IIC_Start();  	 	   
	  IIC_Send_Byte(slave|0x01);           //进入接收模式			   
	  IIC_Wait_Ack();	 
    temp=IIC_Read_Byte(0);		   
    IIC_Stop();//产生一个停止条件	    
	  return temp;
}

void write_IIC(uint8_t slave,uint8_t WriteAddr,uint8_t DataToWrite)
{				   	  	    																 
    IIC_Start();  
    IIC_Send_Byte(slave);   //发送器件地址,写数据 	 
	  IIC_Wait_Ack();	   
    IIC_Send_Byte(WriteAddr);   //发送低地址
	  IIC_Wait_Ack(); 	 										  		   
	  IIC_Send_Byte(DataToWrite);     //发送字节							   
	  IIC_Wait_Ack();  		    	   
    IIC_Stop();//产生一个停止条件 
	  HAL_Delay(10);	 
}
uint8_t IIC_Write_Byte(uint8_t dat)
{
	uint8_t i;
	uint8_t IIC_Ack=0;	

	SDA_OUT(); 	    

	for(i = 0;i < 8;i++)
	{
		if(dat & 0x80)	
			HAL_GPIO_WritePin(DS_SDA_GPIO_Port, DS_SDA_Pin, GPIO_PIN_SET);
		else
			HAL_GPIO_WritePin(DS_SDA_GPIO_Port, DS_SDA_Pin, GPIO_PIN_RESET);
			
		delay_us(2);
		HAL_GPIO_WritePin(DS_SCL_GPIO_Port, DS_SCL_Pin, GPIO_PIN_SET);
	  delay_us(2);
		HAL_GPIO_WritePin(DS_SCL_GPIO_Port, DS_SCL_Pin, GPIO_PIN_RESET);
		dat<<=1;
	}

	HAL_GPIO_WritePin(DS_SDA_GPIO_Port, DS_SDA_Pin, GPIO_PIN_SET);	//释放数据线

	SDA_IN();	//设置成输入

	delay_us(2);
	HAL_GPIO_WritePin(DS_SCL_GPIO_Port, DS_SCL_Pin, GPIO_PIN_SET);
	delay_us(2);
	
	IIC_Ack |= HAL_GPIO_ReadPin(DS_SDA_GPIO_Port, DS_SDA_Pin);	//读入应答位
	HAL_GPIO_WritePin(DS_SCL_GPIO_Port, DS_SCL_Pin, GPIO_PIN_RESET);
	return IIC_Ack;	//返回应答信号
}

uint8_t IIC_Read_Byte1(void)
{
	uint8_t i;
	uint8_t x=0;

	HAL_GPIO_WritePin(DS_SDA_GPIO_Port, DS_SDA_Pin, GPIO_PIN_SET);	//首先置数据线为高电平

	SDA_IN();	//设置成输入

	for(i = 0;i < 8;i++)
	{
		x <<= 1;	//读入数据，高位在前

		delay_us(2);
		HAL_GPIO_WritePin(DS_SCL_GPIO_Port, DS_SCL_Pin, GPIO_PIN_SET);	//突变
		delay_us(2);
		
		if(HAL_GPIO_ReadPin(DS_SDA_GPIO_Port, DS_SDA_Pin))	x |= 0x01;	//收到高电平

		HAL_GPIO_WritePin(DS_SCL_GPIO_Port, DS_SCL_Pin, GPIO_PIN_RESET);
		delay_us(2);
	}	//数据接收完成

	HAL_GPIO_WritePin(DS_SCL_GPIO_Port, DS_SCL_Pin, GPIO_PIN_RESET);

	return x;	//返回读取到的数据
}

