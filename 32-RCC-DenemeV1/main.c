#include <stm32f10x.h>
#include <stm32f10x_tim.h>

uint32_t cntr;

void SysTick_Handler()
{
	if(cntr == 500000) //500ms
	{
	GPIOA->ODR ^= 0x00000010;
	cntr = 0;
	}
	else
		cntr++;
}

void init_RCC_for_PERIPH()
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);
}

void init_blink_led()
{
	GPIO_InitTypeDef myGPIO;
	
	myGPIO.GPIO_Pin = GPIO_Pin_4 |GPIO_Pin_8;
	myGPIO.GPIO_Speed = GPIO_Speed_50MHz;
	myGPIO.GPIO_Mode = GPIO_Mode_Out_PP;
	
	GPIO_Init(GPIOA,&myGPIO);
	
	GPIO_SetBits(GPIOA,GPIO_Pin_4);
}

void init_PWM(uint16_t duty)
{
	TIM_TimeBaseInitTypeDef timer;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	NVIC_InitTypeDef nvic;
	
	timer.TIM_Prescaler = 1440;
	timer.TIM_Period = 24999;
	timer.TIM_CounterMode = TIM_CounterMode_Up;
	timer.TIM_ClockDivision = TIM_CKD_DIV1;

	TIM_TimeBaseInit(TIM2,&timer);
	
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = duty;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Reset;

	TIM_OC1Init(TIM2, &TIM_OCInitStructure);

	TIM_CtrlPWMOutputs(TIM2, ENABLE);
	
	TIM_Cmd(TIM2,ENABLE);
	
	nvic.NVIC_IRQChannel = TIM2_IRQn;
	nvic.NVIC_IRQChannelPreemptionPriority = 0;
	nvic.NVIC_IRQChannelSubPriority = 0;
	nvic.NVIC_IRQChannelCmd = ENABLE;
	
	NVIC_Init(&nvic);
	
	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);
}

void init_PWM_output_GPIO()
{
	GPIO_InitTypeDef GPIO_InitStructure;

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 ;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
}

void TIM2_IRQHandler()
{
	if(TIM_GetFlagStatus(TIM2,TIM_FLAG_Update) == SET)
	{
		
		GPIOA->ODR ^= 0x00000100;
		TIM_ClearFlag(TIM2,TIM_FLAG_Update);
	}
	
}

int main()
{
	SystemInit();

	init_RCC_for_PERIPH();
	
	init_blink_led();
	init_PWM_output_GPIO();
	SysTick->CTRL &= (uint32_t)0xFFFFFFD;
	SysTick_Config(SystemCoreClock / 1000000);
	init_PWM(1000);

	while(1){
		
	}
}
