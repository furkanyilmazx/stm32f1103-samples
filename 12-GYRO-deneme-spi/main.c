#include <stm32f10x.h>
#include <stm32f10x_gpio.h>
#include <stm32f10x_rcc.h>
#include <stm32f10x_usart.h>
#include <stm32f10x_spi.h>
#include <stdio.h>
#include <string.h>


#define CTRL_REG1 0x20
#define CTRL_REG2 0x21
#define CTRL_REG3 0x22
#define CTRL_REG4 0x23
#define CTRL_REG5 0x24

#define USART1_Tx GPIO_Pin_9

volatile uint16_t SPI_rx_data,x_Val;


void Delay_MS(uint32_t nCount)
{ 
	uint32_t i;
	nCount = nCount*(SystemCoreClock/5000);
  for(i=0;i<nCount;i++);
}

void init_SPI()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	SPI_InitTypeDef  SPI_InitStructure;
		
	Delay_MS(100);

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

	/* SPI MOSI SCK pin configuration */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_15;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	/* SPI MISO pin configuration */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	
	
	/* SPI configuration -------------------------------------------------------*/
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_16b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_32;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 7;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	
	
	SPI_Init(SPI2, &SPI_InitStructure);
	SPI_Cmd(SPI2, DISABLE);

}

void init_CSpin()
{
	GPIO_InitTypeDef GPIO_InitStruct;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_5;
	GPIO_Init(GPIOB, &GPIO_InitStruct);
	GPIO_WriteBit(GPIOB,GPIO_Pin_5,Bit_SET);
}

uint16_t SPI_Send_byte(uint16_t byte)
{

	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET);
	SPI_I2S_SendData(SPI2, byte);
	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET);
	SPI_rx_data = SPI_I2S_ReceiveData(SPI2);
	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_BSY) == SET);
	return SPI_rx_data;

}

void GYRO_Send_BYTES(uint16_t com, uint16_t comVal){

	GPIO_WriteBit(GPIOB,GPIO_Pin_5,Bit_RESET);
	SPI_Cmd(SPI2, ENABLE);
	SPI_Send_byte(com);
	SPI_Send_byte(comVal);
	GPIO_WriteBit(GPIOB,GPIO_Pin_5,Bit_SET);
	SPI_Cmd(SPI2, DISABLE);

}



void GYRO_Init()
{
	init_SPI();
	init_CSpin();
	GYRO_Send_BYTES(CTRL_REG1,0x0F);
	GYRO_Send_BYTES(CTRL_REG2,0x00);
	GYRO_Send_BYTES(CTRL_REG3,0x08);
	GYRO_Send_BYTES(CTRL_REG4,0x00);
	GYRO_Send_BYTES(CTRL_REG5,0x00);
	
}

uint16_t GYRO_getX(){
	
	uint16_t x_degeri = 0;
	
	GPIO_WriteBit(GPIOB,GPIO_Pin_5,Bit_RESET);
	SPI_Cmd(SPI2, ENABLE);
	SPI_Send_byte(0x7BE3);			// x high oku
	x_Val = SPI_Send_byte(0x0F0F);
	GPIO_WriteBit(GPIOB,GPIO_Pin_5,Bit_SET);
	SPI_Cmd(SPI2, DISABLE);
	
	x_degeri = x_Val << 8;
	
	
	GPIO_WriteBit(GPIOB,GPIO_Pin_5,Bit_RESET);
	SPI_Cmd(SPI2, ENABLE);
	SPI_Send_byte(0xA8);
	x_Val = SPI_Send_byte(0xFF);
	GPIO_WriteBit(GPIOB,GPIO_Pin_5,Bit_SET);
	SPI_Cmd(SPI2, DISABLE);
	
	x_degeri |= x_Val;
	
	return x_degeri;
}

void init_USART()
{
	GPIO_InitTypeDef 	GPIO_InitStruct;
	USART_InitTypeDef USART_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_USART1,ENABLE);
	
	GPIO_InitStruct.GPIO_Pin = USART1_Tx;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	USART_InitStructure.USART_BaudRate = 9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Tx;
	
	USART_Init(USART1,&USART_InitStructure);
	USART_Cmd(USART1,ENABLE);
}

int main(){
	uint16_t x_deg,mainful_data;
	
	GYRO_Init();
	
	Delay_MS(1500);
	
	init_USART();
	USART_SendData(USART1,'H');
	while(USART_GetFlagStatus(USART1,USART_FLAG_TXE) == RESET);
	Delay_MS(1000);

	while(1){
	
		x_deg = GYRO_getX();
		mainful_data = ((~x_deg) + 0x0001);

		USART_SendData(USART1,mainful_data);
		while(USART_GetFlagStatus(USART1,USART_FLAG_TXE) == RESET);


		Delay_MS(10);
	}
}
