#include <stm32f10x.h>
#include <stm32f10x_gpio.h>
#include <stm32f10x_adc.h>
#include <stm32f10x_usart.h>
#include <stm32f10x_dma.h>


#define ADC1_DR_Address    ((uint32_t)0x4001244C)
#define USART1_DR_addr    ((uint32_t)0x40013804)

#define TxBufferSize1   (countof(TxBuffer1) - 1)
#define TxBufferSize2   (countof(TxBuffer2) - 1)


#define countof(a)   (sizeof(a) / sizeof(*(a)))


__IO uint16_t ADCConvertedValue;






void delay_MS(uint32_t nCount)
{ 
	uint32_t i;
	nCount = nCount*(SystemCoreClock/5000);
	for(i=0;i<nCount;i++);
}

void init_RCC_Sources()
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 | RCC_APB2Periph_GPIOA | RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOB |RCC_APB2Periph_AFIO, ENABLE);
	RCC_ADCCLKConfig(RCC_PCLK2_Div4);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
}


void init_ADC()
{
	ADC_InitTypeDef adc_struct;
	GPIO_InitTypeDef 	GPIO_InitStruct;
	
	  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_1;
	  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AIN;
	  GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	adc_struct.ADC_Mode = ADC_Mode_Independent;
	adc_struct.ADC_ContinuousConvMode = ENABLE;	// sürekli çevrim
	adc_struct.ADC_ScanConvMode = DISABLE;		// tek kanal ADC
	adc_struct.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;	// dış tetiklemeli olmayacak
	adc_struct.ADC_DataAlign = ADC_DataAlign_Right;
	adc_struct.ADC_NbrOfChannel = 1;
	
	ADC_Init(ADC1,&adc_struct);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_1,1,ADC_SampleTime_41Cycles5);
	
	ADC_DMACmd(ADC1, ENABLE);
	ADC_Cmd(ADC1,ENABLE);
	
	ADC_ResetCalibration(ADC1);
	while(ADC_GetResetCalibrationStatus(ADC1) == SET){}
	    /* Start ADC1 calibration */
	  ADC_StartCalibration(ADC1);
	  /* Check the end of ADC1 calibration */
	  while(ADC_GetCalibrationStatus(ADC1) == SET){}
	  
	   ADC_SoftwareStartConvCmd(ADC1, ENABLE);
	   
}

void init_DMA_for_ADC()
{
	DMA_InitTypeDef DMA_InitStructure;

	  /* DMA1 channel1 configuration ----------------------------------------------*/
  DMA_DeInit(DMA1_Channel1);
  DMA_InitStructure.DMA_PeripheralBaseAddr = ADC1_DR_Address;
  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&ADCConvertedValue;
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
  DMA_InitStructure.DMA_BufferSize = 1;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Disable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
  DMA_Init(DMA1_Channel1, &DMA_InitStructure);
  
    /* Enable DMA1 channel1 */
  DMA_Cmd(DMA1_Channel1, ENABLE);
}

void init_DMA_fo_USART(void)
{
  DMA_InitTypeDef DMA_InitStructure;


  DMA_InitStructure.DMA_PeripheralBaseAddr = USART1_DR_addr;
  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)ADCConvertedValue;
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
  DMA_InitStructure.DMA_BufferSize = 1;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
  DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
  DMA_Init(DMA1_Channel4, &DMA_InitStructure);
  
	DMA_Cmd(DMA1_Channel4,ENABLE);
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
	USART_Cmd(USART1,ENABLE);
	USART_DMACmd(USART1,USART_DMAReq_Tx,ENABLE);
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

//void ADC1_2_IRQHandler()
//{
//volatile uint16_t adc_value;
//	if(ADC_GetITStatus(ADC1,ADC_IT_EOC) == SET)
//	{
//	
//		adc_value = ADC_GetConversionValue(ADC1);

//		sendByte_USART((((adc_value)>>8)&0x000F)+65);
//		sendByte_USART((((adc_value)>>4)&0x000F)+65);
//		sendByte_USART((((adc_value))&0x000F)+65);
//		sendByte_USART('~');
//	}
//}

void init_blink_gpio()
{
	GPIO_InitTypeDef 	GPIO_InitStruct;
	
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_12;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStruct);
	
}
int main(){

	init_RCC_Sources();
	init_USART();
	init_ADC();
	init_blink_gpio();
	init_DMA_for_ADC();
	init_DMA_fo_USART();
	
	
	GPIO_SetBits(GPIOB,GPIO_Pin_12);
	while(1){
	GPIO_SetBits(GPIOB,GPIO_Pin_12);
	delay_MS(100);
	GPIO_ResetBits(GPIOB,GPIO_Pin_12);
	delay_MS(100);
	}
}
