#include <stm32f10x.h>
#include <stm32f10x_gpio.h>
#include <stm32f10x_adc.h>
#include <stm32f10x_usart.h>
#include <stm32f10x_tim.h>
#include <stm32f10x_dma.h>

#define USART1_DR_Address    ((uint32_t)0x40013804)
#define ADC1_DR_Address    	((uint32_t)0x4001244C)

uint8_t TxBuffer1[4] = {0,0,0,'~'};
volatile uint16_t adc_value;

void delay_MS(uint32_t nCount)
{ 
	uint32_t i;
	nCount = nCount*(SystemCoreClock/5000);
	for(i=0;i<nCount;i++);
}

void init_RCC_Sources()
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 | RCC_APB2Periph_GPIOA | RCC_APB2Periph_USART1 | RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOC, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);
//	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
}

void init_timer()
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

	TIM_TimeBaseStructure.TIM_Prescaler = 72; //200kHz e ayarlı
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_Period = 4;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
	
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);
	
	TIM_Cmd(TIM2, ENABLE);
	
	
}

void init_Nvic_for_TIM2()
{
	
	NVIC_InitTypeDef	NVIC_DEF;
	
	NVIC_DEF.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_DEF.NVIC_IRQChannelCmd = ENABLE;
	NVIC_DEF.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_DEF.NVIC_IRQChannelSubPriority = 2;
	
	NVIC_Init(&NVIC_DEF);
	
}

void init_ADC()
{
	ADC_InitTypeDef adc_struct;
	GPIO_InitTypeDef 	GPIO_InitStruct;
	
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AIN;
	
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	adc_struct.ADC_Mode = ADC_Mode_Independent;
	adc_struct.ADC_ContinuousConvMode = ENABLE;	// sürekli çevrim
	adc_struct.ADC_ScanConvMode = DISABLE;		// tek kanal ADC
	adc_struct.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;	// dış tetiklemeli olmayacak
	adc_struct.ADC_DataAlign = ADC_DataAlign_Right;
	adc_struct.ADC_NbrOfChannel = ADC_Channel_1;
	
	ADC_Init(ADC1,&adc_struct);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_1,1,ADC_SampleTime_1Cycles5);
	
//	ADC_ITConfig(ADC1,ADC_IT_EOC,ENABLE);
	ADC_Cmd(ADC1,ENABLE);
	
	ADC_ResetCalibration(ADC1);
	while(ADC_GetResetCalibrationStatus(ADC1) == SET){}
	    /* Start ADC1 calibration */
	  ADC_StartCalibration(ADC1);
	  /* Check the end of ADC1 calibration */
	  while(ADC_GetCalibrationStatus(ADC1) == SET){}
	  
	   ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}

void init_USART()
{
	GPIO_InitTypeDef 	GPIO_InitStruct;
	USART_InitTypeDef USART_InitStructure;
	
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9;
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

//	USART_DMACmd(USART1, USART_DMAReq_Tx, ENABLE);

	USART_Cmd(USART1,ENABLE);
	
//	DMA_Cmd(DMA1_Channel4, ENABLE);
//		
//	while (DMA_GetFlagStatus(DMA1_FLAG_TC4) == RESET)
//	{
//	}
}

void DMA_Configuration_for_USART1(void)
{
	DMA_InitTypeDef DMA_InitStructure;
	
	DMA_InitStructure.DMA_PeripheralBaseAddr = USART1_DR_Address;
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)TxBuffer1;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
	DMA_InitStructure.DMA_BufferSize = 4;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	
	DMA_Init(DMA1_Channel4, &DMA_InitStructure);

}

void sendByte_USART(uint16_t data)
{
	while(USART_GetFlagStatus(USART1,USART_FLAG_TXE) == RESET);
	USART_SendData(USART1,data);

}
void sendString_USART(const uint16_t data[])
{
	while(*data)
	{
		sendByte_USART(*data++);
	}
}

void TIM2_IRQHandler()
{

	if(TIM_GetFlagStatus(TIM2,TIM_FLAG_Update) == SET)
	{
		
		TIM_ClearFlag(TIM2,TIM_FLAG_Update);

		adc_value = ADC_GetConversionValue(ADC1);
		
		
		sendByte_USART((((adc_value)>>8)&0x000F)+65);
		sendByte_USART((((adc_value)>>4)&0x000F)+65);
		sendByte_USART((((adc_value))&0x000F)+65);
		sendByte_USART('~');

	}
}

void init_GPIO_LED()
{
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_15;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOC, &GPIO_InitStruct);
}


int main()
{
	
	init_RCC_Sources();
	init_Nvic_for_TIM2();
//	DMA_Configuration_for_USART1();
	init_USART();
	init_ADC();
	init_timer();

//	init_GPIO_LED();
	
	
	while(1){
	
//		delay_MS(100);
//	
//		if(GPIO_ReadOutputDataBit(GPIOC,GPIO_Pin_15)  == 1)
//		{
//			GPIO_ResetBits(GPIOC, GPIO_Pin_15);

//		}
//		else
//		{
//			GPIO_SetBits(GPIOC, GPIO_Pin_15);

//		}
	}

}
