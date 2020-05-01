#include "DDS.h"

void 	DDS_class::DDS(){
	int SPI_En;
	SPI_Type = SPI2;
	SPIx_NSS = GPIO_Pin_12;
	SPIx_SCK = GPIO_Pin_13;
	SPIx_MOSI = GPIO_Pin_15;
	SPI_En = DDS_SPI_Init();
	
	//if(!SPI_En) 
	
}

void 	DDS_class::DDS(USART_TypeDef *USARTx, uint32_t BdRate){

	USART_Type = USARTx;
	if(USART_Type == USART1)
	{
		USARTx_TX = GPIO_Pin_9;
		USARTx_RX = GPIO_Pin_10;
		USARTx_GPIO = GPIOA;
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA ,ENABLE);
	}
	else if(USART_Type == USART2)
	{
		USARTx_TX = GPIO_Pin_2;
		USARTx_RX = GPIO_Pin_3;
		USARTx_GPIO = GPIOA;
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA ,ENABLE);
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	}
	else if(USART_Type == USART3)
	{
		USARTx_TX = GPIO_Pin_10;
		USARTx_RX = GPIO_Pin_11;
		USARTx_GPIO = GPIOB;
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB ,ENABLE);
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
	}
	
	DDS_USART_Init(BdRate);
	
}


void 	DDS_class::DDS(SPI_TypeDef *SPIx){

	SPI_Type = SPIx;
	
	if(SPI_Type == SPI1)
	{
		SPIx_NSS = GPIO_Pin_4;
		SPIx_SCK = GPIO_Pin_5;
		SPIx_MOSI = GPIO_Pin_7;
		SPIx_GPIO = GPIOB;
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB |RCC_APB2Periph_SPI1,ENABLE);
	}
		
	else if(SPI_Type == SPI2)
	{
		SPIx_NSS = GPIO_Pin_12;
		SPIx_SCK = GPIO_Pin_13;
		SPIx_MOSI = GPIO_Pin_15;
		SPIx_GPIO = GPIOB;
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
	}
	
	
}

void 	DDS_class::DDS(wave_type wv_type){
	
}

int	DDS_class::DDS_SPI_Send(uint16_t Data){
		
	return 1;
}

int	DDS_class::DDS_Set_Start_Freq(uint16_t Freq_Data){
	
	return 1;
}

int	DDS_class::DDS_Set_Delta_F(uint16_t DeltaF_Data){
	
	return 1;
}

int	DDS_class::DDS_Set_CR(uint16_t	Cr_Data){
	
	return 1;
}

int DDS_class::DDS_Set_Inc_Num(uint16_t Inc_Num_Data){
	
	return 1;
}

int DDS_class::DDS_SPI_Init(){
		

		SPI_InitTypeDef		SPI_InitStructure;
		GPIO_InitTypeDef	GPIO_InitStructure;
 
    GPIO_InitStructure.GPIO_Pin    = SPIx_NSS | SPIx_SCK | SPIx_MOSI;
    GPIO_InitStructure.GPIO_Mode   = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed  = GPIO_Speed_50MHz;
	
    GPIO_Init(SPIx_GPIO, &GPIO_InitStructure);
		
		
    SPI_InitStructure.SPI_Direction         = SPI_Direction_Tx;
    SPI_InitStructure.SPI_Mode              = SPI_Mode_Master;
    SPI_InitStructure.SPI_DataSize          = SPI_DataSize_16b;
    SPI_InitStructure.SPI_CPOL              = SPI_CPOL_High;
    SPI_InitStructure.SPI_CPHA              = SPI_CPHA_1Edge;
    SPI_InitStructure.SPI_NSS               = SPI_NSS_Hard;
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
    SPI_InitStructure.SPI_FirstBit          = SPI_FirstBit_MSB;
    SPI_InitStructure.SPI_CRCPolynomial     = 7;
 
    SPI_I2S_ITConfig(SPI_Type, SPI_I2S_IT_RXNE, DISABLE);
		SPI_I2S_ITConfig(SPI_Type, SPI_I2S_IT_TXE, ENABLE);
    SPI_Init(SPI_Type,&SPI_InitStructure);
    SPI_Cmd(SPI_Type,ENABLE);
 
		if((SPI_Type->CR1 & 0x00000080) != 0x00000080) return 0;		//SPI aktif ise 1 degilse 0
		else return 1;
}

void DDS_class::DDS_USART_Init(uint32_t BDRATE){
		
		GPIO_InitTypeDef	GPIO_InitStruct;
		USART_InitTypeDef USART_InitStructure;
		

		
		GPIO_InitStruct.GPIO_Pin = USARTx_RX;
		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
		GPIO_Init(USARTx_GPIO, &GPIO_InitStruct);
		
		GPIO_InitStruct.GPIO_Pin = USARTx_TX;
		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
		GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(USARTx_GPIO, &GPIO_InitStruct);
		
		USART_InitStructure.USART_BaudRate = BDRATE;
		USART_InitStructure.USART_WordLength = USART_WordLength_8b;
		USART_InitStructure.USART_StopBits = USART_StopBits_1;
		USART_InitStructure.USART_Parity = USART_Parity_No;
		USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
		USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
		
		USART_Init(USART_Type,&USART_InitStructure);
		USART_Cmd(USART_Type,ENABLE);
}

void DDS_USART_Send(uint8_t *Data_Buff){
		
	
}
