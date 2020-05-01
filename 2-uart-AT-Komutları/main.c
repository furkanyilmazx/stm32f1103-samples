#include <stm32f10x.h>
#include <stm32f10x_gpio.h>
#include <stm32f10x_rcc.h>
#include <stm32f10x_usart.h>

#define USART1_Rx GPIO_Pin_10
#define USART1_Tx GPIO_Pin_9


GPIO_InitTypeDef 	GPIO_InitStruct;
USART_InitTypeDef USART_InitStructure;
uint8_t TxBuffer[] = "AT+RST";
uint8_t TxCounter = 0;  
	
void USART1_Init(){
	RCC_APB2PeriphClockCmd(RCC_APB2ENR_USART1EN | RCC_APB2Periph_AFIO,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	
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

int main(){
	int i;
	SystemInit();
	SystemCoreClockUpdate();
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);	
	
  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_15;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;

  GPIO_Init(GPIOC, &GPIO_InitStruct);
   
	GPIO_SetBits(GPIOC, GPIO_Pin_15);
	USART1_Init();
		
	 while(1){
			USART_SendData(USART1,TxBuffer[TxCounter++]);
			while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET){
			GPIO_SetBits(GPIOC, GPIO_Pin_15);
			}
			if(TxCounter == 6){
			TxCounter = 0;
			GPIO_ResetBits(GPIOC, GPIO_Pin_15);
			for(i = 0; i< 355555;i++);
			
		}
	}
}
