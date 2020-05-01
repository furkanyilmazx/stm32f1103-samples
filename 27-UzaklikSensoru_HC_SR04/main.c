#include "stm32f10x.h"


#define SR04_OFFSET 0.8

#define	USART1_Rx			GPIO_Pin_10
#define	USART1_Tx			GPIO_Pin_9



void TIM2_Init(void) {
  uint16_t PrescalerValue = 0;
  
  TIM_TimeBaseInitTypeDef   TIM_TimeBaseStructure;
  TIM_OCInitTypeDef         TIM_OCInitStructure;

  /* TIM2 clock enable */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

  TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
  TIM_OCStructInit(&TIM_OCInitStructure);

  /* Compute the prescaler value */
  PrescalerValue = (uint16_t) (SystemCoreClock / 1000000) - 1;

  /* Time base configuration */
  TIM_TimeBaseStructure.TIM_Period = 65535 - 1;      //in uSecs
  TIM_TimeBaseStructure.TIM_Prescaler = PrescalerValue;
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

  TIM_Cmd(TIM2, ENABLE);
}

uint16_t SR04read(void) {
  TIM_SetCounter(TIM2, 0);
  GPIO_ResetBits(GPIOA, GPIO_Pin_4);
  while(TIM_GetCounter(TIM2) < 15);
  GPIO_SetBits(GPIOA, GPIO_Pin_4);
  //TIM_SetCounter(TIM2, 0);
  while(!GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_5));// & (TIM_GetCounter(TIM2) < 50000));
  TIM_SetCounter(TIM2, 0);
  while(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_5));// & (TIM_GetCounter(TIM2) < 50000));
  return (TIM_GetCounter(TIM2)-43);
}


void SR04_Init(void) {

	GPIO_InitTypeDef GPIO_InitStructure;

	// configuring clock sources for GPIOC
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA |RCC_APB2Periph_AFIO, ENABLE);

	/* Configure SR04 pins: PA4 - TRIGGER */
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_SetBits(GPIOA, GPIO_Pin_4);

	/* Configure SR04 pins: PA5 - ECHO */
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);



}


void init_USART()
{
	GPIO_InitTypeDef 	GPIO_InitStruct;
	USART_InitTypeDef USART_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA ,ENABLE);
	
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
}


void sendByte_USART(uint8_t data)
{
	while(USART_GetFlagStatus(USART1,USART_FLAG_TXE) == RESET);
	USART_SendData(USART1,data);

}
void sendString_USART(const char data[])
{
	while(*data)
	{
		sendByte_USART(*data++);
	}
}
int main()
{
	//int i;
	uint16_t data;
	
	init_USART();
	SR04_Init();
	TIM2_Init();

	sendString_USART("Basladik\n\r");
	while(1)
	{
		TIM_SetCounter(TIM2, 0);
		while(TIM_GetCounter(TIM2) < 65534);
		data = SR04read();
//		for(i = 0;i<20000000;i++);
		sendByte_USART((data/1000)+48);
		sendByte_USART(((data/100)%10)+48);
		sendByte_USART(((data/10)%10)+48);
		sendByte_USART(((data)%10)+48);
		sendByte_USART(126);
		sendByte_USART('\r');
		sendByte_USART('\n');
	}
}
