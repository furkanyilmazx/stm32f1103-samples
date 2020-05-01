#include "misc.h"
#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_tim.h"

void init_RCC()
{
  /* TIM2, GPIOC, AFIO clocks enable */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC |RCC_APB2Periph_AFIO, ENABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);
}

void delay_MS(uint32_t nCount)
{ 
	uint32_t i;
	nCount = nCount*(SystemCoreClock/5000);
	for(i=0;i<nCount;i++);
}

void init_timer()
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	
	
	TIM_TimeBaseStructure.TIM_Prescaler = 1440;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_Period = 50000;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
	
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
	
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_Active;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = 40;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
	
	TIM_OC1Init(TIM2, &TIM_OCInitStructure);
	TIM_ITConfig(TIM2,TIM_IT_CC1,ENABLE);
	TIM_Cmd(TIM2, ENABLE);
	
	
}

void TIM2_IRQHandler()
{
	if(TIM_GetFlagStatus(TIM2,TIM_FLAG_CC1) == SET)
	{
		TIM_ClearFlag(TIM2,TIM_FLAG_CC1);
//		TIM_ClearITPendingBit(TIM2,TIM_IT_CC1);

		if(GPIO_ReadOutputDataBit(GPIOC,GPIO_Pin_15)  == 1)
		{
			GPIO_ResetBits(GPIOC, GPIO_Pin_15);

		}
		else
		{
			GPIO_SetBits(GPIOC, GPIO_Pin_15);

		}
	}
}

void init_Nvic()
{
	
	NVIC_InitTypeDef	NVIC_DEF;
	NVIC_SetPriorityGrouping(NVIC_PriorityGroup_2);
	
	NVIC_DEF.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_DEF.NVIC_IRQChannelCmd = ENABLE;
	NVIC_DEF.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_DEF.NVIC_IRQChannelSubPriority = 0;
	
	NVIC_Init(&NVIC_DEF);
	
}

void init_GPIO_LED()
{
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_15;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOC, &GPIO_InitStruct);
}

int main(){

	init_RCC();
	init_GPIO_LED();
	init_Nvic();
	init_timer();
	
	
	
	while(1){

	}
}
