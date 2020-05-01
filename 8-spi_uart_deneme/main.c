#include <stm32f10x.h>
#include <misc.h>
#include <stm32f10x_gpio.h>
#include <stm32f10x_rcc.h>
#include <stm32f10x_usart.h>
#include <stm32f10x_spi.h>


#define USART1_Rx GPIO_Pin_10
#define USART1_Tx GPIO_Pin_9

#define SPI2_MOSI_PIN GPIO_Pin_15
#define SPI2_SCK_PIN GPIO_Pin_13

#define SPI1_MOSI_PIN GPIO_Pin_7
#define SPI1_SCK_PIN GPIO_Pin_5


	


uint8_t flag;
uint8_t TxBuffer[] = "AT+RST";
uint8_t RxSPI_DATA = 0,SPI_DATA;  
	
void COMM_Init(){
	
	GPIO_InitTypeDef		GPIO_InitStructure;
	SPI_InitTypeDef  		SPI_InitStructure;
	NVIC_InitTypeDef 		nvic;
	

// SPI1 AYALRAMALAR	 -MASTER

  GPIO_InitStructure.GPIO_Pin = SPI1_MOSI_PIN | SPI1_SCK_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	
  GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	
  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_128;
  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_LSB;
  SPI_InitStructure.SPI_CRCPolynomial = 7;
	
	
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_Init(SPI1, &SPI_InitStructure);
	SPI_Cmd(SPI1, ENABLE);
	

// SPI2 AYALRAMALAR	 -SLAVE
	
	GPIO_InitStructure.GPIO_Pin = SPI2_MOSI_PIN | SPI2_SCK_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	 SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Slave;
	SPI_Init(SPI2, &SPI_InitStructure);
	SPI_Cmd(SPI2, ENABLE);


	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	nvic.NVIC_IRQChannel = SPI2_IRQn;
	nvic.NVIC_IRQChannelPreemptionPriority = 0x0;
	nvic.NVIC_IRQChannelSubPriority = 0x0;
	nvic.NVIC_IRQChannelCmd = ENABLE;
	
	NVIC_Init(&nvic);
	
	SPI_I2S_ITConfig(SPI2,SPI_I2S_IT_RXNE,ENABLE);
	
	
	
}

void USART1_Init(){
	
	GPIO_InitTypeDef		GPIO_InitStruct;
	USART_InitTypeDef 	USART_InitStructure;
	
	GPIO_InitStruct.GPIO_Pin = USART1_Rx;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Pin = USART1_Tx;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	USART_InitStructure.USART_BaudRate = 9600;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	
	USART_Init(USART1,&USART_InitStructure);
	USART_Cmd(USART1,ENABLE);
}

void UART_send(USART_TypeDef *UARTx, uint8_t data){
	
	while (USART_GetFlagStatus(UARTx, USART_FLAG_TXE) == RESET);
	USART_SendData(UARTx,data);
	if(flag)	{GPIOB->BSRR = 0x00000200; flag = 0;}
	else			{GPIOB->BSRR = 0x02000000; flag = 1;}
}

void UART_SEND_BYTES(USART_TypeDef *UARTx, uint8_t *data){
	while(*data) UART_send(UARTx,*data++);

}
void SPI2_IRQHandler(){
	
	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET);
	RxSPI_DATA = SPI_I2S_ReceiveData(SPI2);
	SPI_I2S_ClearFlag(SPI2,SPI_I2S_FLAG_RXNE);
	UART_send(USART1,RxSPI_DATA);


}

uint8_t SPI_send(SPI_TypeDef *SPIx,uint8_t data){
	
	while (SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_TXE) == RESET);
	SPI_I2S_SendData(SPIx, data);
	while (SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_RXNE) == RESET);
	SPI_DATA =  SPI_I2S_ReceiveData(SPIx);
}

void Delay_MS(uint32_t nCount)
{ 
	uint32_t i;
	nCount = nCount*(SystemCoreClock/5000);
  for(i=0;i<nCount;i++);
}

int main(){
	GPIO_InitTypeDef GPIO_InitStruct;
	
	SystemInit();
	SystemCoreClockUpdate();
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_SPI1 |RCC_APB2Periph_AFIO,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2,ENABLE);
		
  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOB, &GPIO_InitStruct);
	
	GPIO_SetBits(GPIOB, GPIO_Pin_9);
	
	COMM_Init();
	USART1_Init();

	
	//UART_SEND_BYTES(USART1,TxBuffer);
	 while(1){
	SPI_send(SPI1,'A');
		 Delay_MS(1000);
	}
}
