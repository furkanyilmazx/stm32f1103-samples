#include <stm32f10x.h>
#include <stm32f10x_spi.h>
#include <stm32f10x_rcc.h>
#include <stm32f10x_gpio.h>
#include <stm32f10x_usart.h>
#include <misc.h>

uint8_t flag = 0;



void UART_send(USART_TypeDef *UARTx, uint8_t data){
	
	while (USART_GetFlagStatus(UARTx, USART_FLAG_TXE) == RESET);
	USART_SendData(UARTx,data);

}

void UART_send_bytes(USART_TypeDef *UARTx, uint8_t *data){
	while(*data) UART_send(USART1,*(data++));
}


void SPI2_IRQHandler(){
	volatile uint8_t rxData[6] = {0};
	volatile uint8_t buffSize = 6,count = 0;
	while(count < buffSize){
			SPI_I2S_ClearFlag(SPI2,SPI_I2S_FLAG_RXNE);
		while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET);
		rxData[count] = SPI_I2S_ReceiveData(SPI2);

		count++;
	}

	UART_send_bytes(USART1,(uint8_t *)rxData);
	UART_send_bytes(USART1,(uint8_t *)"geldi");
	if(flag)	{GPIOB->BSRR = 0x00000200; flag = 0;}
	else			{GPIOB->BSRR = 0x02000000; flag = 1;}
} 

int main(){

	GPIO_InitTypeDef GPIO_InitStructure;
	SPI_InitTypeDef  SPI_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef nvic;
	
	uint8_t hazir[] = "Hazir";

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB |RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);


	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_15;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	/* SPI configuration -------------------------------------------------------*/
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_RxOnly;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Hard;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 7;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Slave;
	
	SPI_Init(SPI2, &SPI_InitStructure);
	SPI_Cmd(SPI2, ENABLE);
	SPI_I2S_ITConfig(SPI2,SPI_I2S_IT_RXNE,ENABLE);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_SetBits(GPIOB,GPIO_Pin_9);
	
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	USART_InitStructure.USART_BaudRate = 9600;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Tx;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	
	USART_Init(USART1, &USART_InitStructure);

	USART_Cmd(USART1 , ENABLE);
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	nvic.NVIC_IRQChannel = SPI2_IRQn;
	nvic.NVIC_IRQChannelPreemptionPriority = 0x0;
	nvic.NVIC_IRQChannelSubPriority = 0x0;
	nvic.NVIC_IRQChannelCmd = ENABLE;
	
	NVIC_Init(&nvic);
	

	UART_send_bytes(USART1,hazir);
	
	while(1){
		
	
	}
}
