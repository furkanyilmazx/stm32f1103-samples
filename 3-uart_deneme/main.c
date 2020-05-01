#include <stm32f10x.h>
#include <stm32f10x_gpio.h>
#include <stm32f10x_rcc.h>
#include <stm32f10x_usart.h>
#include <stm32f10x_spi.h>


#define USART1_Rx GPIO_Pin_10
#define USART1_Tx GPIO_Pin_9


GPIO_InitTypeDef 	GPIO_InitStruct;
USART_InitTypeDef USART_InitStructure;
uint8_t TxBuffer[] = "AT+RST";
uint8_t TxCounter = 0;  
	
void USART1_Init(){
	
	
	
	GPIO_InitStruct.GPIO_Pin = USART1_Tx;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	USART_InitStructure.USART_BaudRate = 115200;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Tx;
	
	USART_Init(USART1,&USART_InitStructure);
	USART_Cmd(USART1,ENABLE);
	USART_HalfDuplexCmd(USART1, ENABLE);
}

int main(){
	int i;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO,ENABLE);
	
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOB, &GPIO_InitStruct);
	
	GPIO_SetBits(GPIOB, GPIO_Pin_9);
	
	USART1_Init();
	
	
	 while(1){
			USART_SendData(USART1,'A');
			while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
			for(i = 0; i< 555555;i++);
	}
}
