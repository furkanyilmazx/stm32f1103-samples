#include "stm32f10x.h"
#include <stdio.h>

#define	USART1_Rx			GPIO_Pin_10
#define	USART1_Tx			GPIO_Pin_9

uint32_t duty = 0,encoder = 0,denemeduty = 0;
unsigned int sistem_cevap = 0;
char buffer[16];
volatile uint8_t uart_received_data,receiveText[8] = {'\0'},strIndex = 0,receiveTextFormatted[8] = {'\0'};
double kpDeg,kiDeg,kdDeg;

void delay_MS(uint32_t nCount)
{ 
	uint32_t i;
	nCount = nCount*(SystemCoreClock/5000);
	for(i=0;i<nCount;i++);
}

void RCC_Configuration(void)
{
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA |RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOB | RCC_APB2Periph_USART1, ENABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);
    /* Enable AFIO clock */
}

void init_PWM_GPIO(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 ;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
}


void init_PWM_TIM(uint16_t period, uint16_t duty)
{

	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;

	/* Time Base configuration */
	TIM_TimeBaseStructure.TIM_Prescaler = 72-1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_Period = period;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;

	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

	/* Channel 1, 2,3 and 4 Configuration in PWM mode */
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = duty;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Reset;

	TIM_OC1Init(TIM2, &TIM_OCInitStructure);


	/* TIM1 counter enable */
	TIM_Cmd(TIM2, ENABLE);

	/* TIM1 Main Output Enable */
	TIM_CtrlPWMOutputs(TIM2, ENABLE);
	
	//timer pwm duty cycle ayarlama
	//TIM_SetCompare1(TIM2,duty);
}

void init_USART()
{
	GPIO_InitTypeDef 	GPIO_InitStruct;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef   NVIC_InitStructure;
		
		
	GPIO_InitStruct.GPIO_Pin = USART1_Rx;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Pin = USART1_Tx;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	

	
	
	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Tx |USART_Mode_Rx;
	
	USART_Init(USART1,&USART_InitStructure);
	USART_Cmd(USART1,ENABLE);
	USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);
	
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0E;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0F;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

	NVIC_Init(&NVIC_InitStructure);
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
void USART1_IRQHandler()
{
	if(USART_GetFlagStatus(USART1,USART_FLAG_RXNE) == SET)
	{
		
		uart_received_data =  (USART1->DR );
		
		if(uart_received_data == 'A')
		{
			
			if(duty<255)
			duty++;
			EXTI->IMR |= EXTI_Line5;
			SysTick->CTRL |= (0x00000001);
			
			TIM_SetCompare1(TIM2,duty);
			
		}
		else if(uart_received_data == 'S')
		{
			if(duty>0)
			{
				duty--;
			}
			if (duty == 0)
			{
				encoder = 0;
				duty = 0;
				SysTick->CTRL &= ~(0x00000001);	//systicki pasif yapıyor
				EXTI->IMR &= ~EXTI_Line5;
			}
			TIM_SetCompare1(TIM2,duty);
			
		}
		else if(uart_received_data == 'Q')
		{
			
			if(duty<255)
			duty+=51;
			EXTI->IMR |= EXTI_Line5;
			SysTick->CTRL |= (0x00000001);
			
			TIM_SetCompare1(TIM2,duty);
			
		}
		else if(uart_received_data == 'W')
		{
			if(duty>0)
			{
				duty-=51;
			}
			if (duty == 0)
			{
				encoder = 0;
				duty = 0;
				SysTick->CTRL &= ~(0x00000001);	//systicki pasif yapıyor
				EXTI->IMR &= ~EXTI_Line5;
			}
			TIM_SetCompare1(TIM2,duty);
			
		}
		else if(uart_received_data == 'D')
		{
			duty=0;
			
			TIM_SetCompare1(TIM2,duty);
			SysTick->CTRL &= ~(0x00000001);	//systicki pasif yapıyor
			EXTI->IMR &= ~EXTI_Line5;
			encoder = 0;
			//sendString_USART("Stopped\n");
		}
		else if(uart_received_data == 'M')
		{
			duty=255;
			EXTI->IMR |= EXTI_Line5;
			SysTick->CTRL |= (0x00000001);
			TIM_SetCompare1(TIM2,duty);
			//sendString_USART("MaxSpeed\n");
		}
		else if(uart_received_data == 'F')
		{
			//PID set değerlerini set etme komutu
		}
		else
		{
			if(uart_received_data != '_' && strIndex < 7)
			{
				receiveText[strIndex++] = uart_received_data; //strindeks bir sonraki karakter için arttırılıyor
			}
			else
			{
				if(strIndex == 7) receiveText[strIndex] = '\0';

				strIndex = 0;
				if((receiveText[0]  == 'k' ) && (receiveText[1] == 'p'))
				{
					sendString_USART("\nKp geldi :\n");
					sendString_USART((const char *)&receiveText[2]);
					kpDeg = (double)(receiveText[2]-48) + (double)((receiveText[4]-48) / 10) + (double)((receiveText[5]-48 )/ 100);
					
					
				}
				else if((receiveText[0]  == 'k' ) && (receiveText[1] == 'd'))
				{
					sendString_USART("\nKd geldi :\n");
					sendString_USART((const char *)&receiveText[2]);
					kdDeg = (double)(receiveText[2]-48) + (double)((receiveText[4]-48) / 10) + (double)((receiveText[5]-48 )/ 100);
					
				}
				else if((receiveText[0]  == 'k' ) && (receiveText[1] == 'i'))
				{
					sendString_USART("\nKi geldi :\n");
					sendString_USART((const char *)&receiveText[2]);
					kiDeg = (double)(receiveText[2]-48) + (double)((receiveText[4]-48) / 10) + (double)((receiveText[5]-48 )/ 100);
					
				}
				else
					sendString_USART((const char *)receiveText);
			}
		}
		USART_ClearFlag(USART1,USART_FLAG_RXNE);
	}
}

void init_Encoder_Interrupt()
{
	EXTI_InitTypeDef   EXTI_InitStructure;
	GPIO_InitTypeDef   GPIO_InitStructure;
	NVIC_InitTypeDef   NVIC_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

	/* Configure PB.09 pin as input floating */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource5);

	/* Configure EXTI9 line */
	EXTI_InitStructure.EXTI_Line = EXTI_Line5;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
	
	  /* Enable and set EXTI9_5 Interrupt to the lowest priority */
	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

	NVIC_Init(&NVIC_InitStructure);
}

void init_Buton9_Interrupt()
{
	EXTI_InitTypeDef   EXTI_InitStructure;
	GPIO_InitTypeDef   GPIO_InitStructure;
	

	/* Configure PB.09 pin as input floating */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource9);

	/* Configure EXTI9 line */
	EXTI_InitStructure.EXTI_Line = EXTI_Line9;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
}

void init_DownButon_Interrupt()
{
	EXTI_InitTypeDef   EXTI_InitStructure;
	GPIO_InitTypeDef   GPIO_InitStructure;
	NVIC_InitTypeDef   NVIC_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

	/* Configure PB.09 pin as input floating */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource12);

	/* Configure EXTI9 line */
	EXTI_InitStructure.EXTI_Line = EXTI_Line12;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
	
	  /* Enable and set EXTI9_5 Interrupt to the lowest priority */
	NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x05;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

	NVIC_Init(&NVIC_InitStructure);
}

int main(void)
{
	GPIO_InitTypeDef   GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	
	// System initializing
	SystemInit();
	SystemCoreClockUpdate();
	
	RCC_Configuration();
	init_USART();
	init_PWM_GPIO();
	init_PWM_TIM(255,duty);
	init_Encoder_Interrupt();
	init_Buton9_Interrupt();
	init_DownButon_Interrupt();
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);
	SysTick_Config(100000);
	
	SysTick->CTRL &= ~(0x00000001);	//systicki pasif yapıyor
	EXTI->IMR &= ~EXTI_Line5;
	while (1)
	{

	}
}

void EXTI9_5_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line5) != RESET)
	{
		encoder++;
		EXTI_ClearITPendingBit(EXTI_Line5);
	}
	
	else if(EXTI_GetITStatus(EXTI_Line9) != RESET)
	{
		SysTick->CTRL |= (0x00000001);
		EXTI->IMR |= EXTI_Line5;
			if(duty<255)
			duty+=51;
			
		TIM_SetCompare1(TIM2,duty);
		EXTI_ClearITPendingBit(EXTI_Line9);
	}
}

void EXTI15_10_IRQHandler()
{
if(EXTI_GetITStatus(EXTI_Line12) != RESET)
	{
		if(duty>0)
		{
			duty-=51;
		}
		if (duty == 0)
		{
			encoder = 0;
			duty = 0;
			SysTick->CTRL &= ~(0x00000001);	//systicki pasif yapıyor
			EXTI->IMR &= ~EXTI_Line5;
		}
		TIM_SetCompare1(TIM2,duty);
		EXTI_ClearITPendingBit(EXTI_Line12);
	}

}


void SysTick_Handler()
{
	SysTick->CTRL &= ~(0x00000001);	//systicki pasif yapıyor
	GPIOB->ODR ^= 0x00000002;

	sistem_cevap= (unsigned int)((2*3.14*encoder/16)); 
	encoder = 0;
	sprintf(buffer,"%d",sistem_cevap);
	sendString_USART(buffer);
	sendByte_USART('\n');
	
	SysTick->CTRL |= (0x00000001);	//systicki aktif yapıyor
	
}
