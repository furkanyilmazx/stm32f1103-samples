#include "AD5932.h"

uint16_t 	AD5932_Cr = 0; 			//NULL Yaptık başlangıçta
uint16_t	AD5932_NumOfInc = 0; 
uint16_t	AD5932_Inc_IntTim;

uint8_t		TxBuffer1[4] = {0,0,0,'~'};

volatile	uint16_t adc_value;

void delay_MS(uint32_t nCount)
{ 
	uint32_t i;
	nCount = nCount*(SystemCoreClock/5000);
	for(i=0;i<nCount;i++);
}

void init_RCC_Sources()
{
	RCC_APB1PeriphClockCmd(SPI2_RCC |RCC_APB1Periph_TIM2, ENABLE);
	RCC_APB2PeriphClockCmd(SPI2_RCC_GPIO | RCC_APB2Periph_GPIOA | USART1_RCC | RCC_APB2Periph_ADC1 | RCC_APB2Periph_AFIO , ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
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

void init_SPI()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	SPI_InitTypeDef  SPI_InitStructure;

	/* SPI2 MOSI SCK pin configuration */
	/* SPI MOSI SCK pin configuration */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_15;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
		/* SPI2 CS pin configuration */
	GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed 	= GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin 	= GPIO_Pin_5;
	
	GPIO_Init(GPIOB, &GPIO_InitStructure);	

	
	/* SPI2 configuration -------------------------------------------------------*/
	SPI_InitStructure.SPI_Direction 		= SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_DataSize 			= SPI_DataSize_16b;
	SPI_InitStructure.SPI_CPOL 				= SPI_CPOL_High;
	SPI_InitStructure.SPI_CPHA 				= SPI_CPHA_1Edge;
	SPI_InitStructure.SPI_NSS 				= SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_32;
	SPI_InitStructure.SPI_FirstBit 			= SPI_FirstBit_MSB;
	
	SPI_InitStructure.SPI_Mode 				= SPI_Mode_Master;
	
	
	SPI_Init(SPI2, &SPI_InitStructure);
	
	SPI_Cmd(SPI2, ENABLE);
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
	ADC_RegularChannelConfig(ADC1, ADC_Channel_1,1,ADC_SampleTime_239Cycles5);
	
	ADC_Cmd(ADC1,ENABLE);
	
	ADC_ResetCalibration(ADC1);
	
	while(ADC_GetResetCalibrationStatus(ADC1) == SET){}
	/* Start ADC1 calibration */
	ADC_StartCalibration(ADC1);
	/* Check the end of ADC1 calibration */
	while(ADC_GetCalibrationStatus(ADC1) == SET){}

	ADC_SoftwareStartConvCmd(ADC1, ENABLE);   
}

void sendCommand_AD5932(uint16_t comm)
{
	GPIO_ResetBits(GPIOB,GPIO_Pin_5);
	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET);
	SPI_I2S_SendData(SPI2, comm);
	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_BSY) == SET);
	GPIO_SetBits(GPIOB,GPIO_Pin_5);
}

void init_USART()
{
	GPIO_InitTypeDef 	GPIO_InitStruct;
	USART_InitTypeDef USART_InitStructure;
	
	GPIO_InitStruct.GPIO_Pin = USART1_Tx;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(USART1_GPIO, &GPIO_InitStruct);
	
	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Tx;
	
	USART_Init(USART1,&USART_InitStructure);

	USART_DMACmd(USART1, USART_DMAReq_Tx, ENABLE);
	
	USART_Cmd(USART1,ENABLE);
	
	DMA_Cmd(DMA1_Channel4, ENABLE);
		
	while (DMA_GetFlagStatus(DMA1_FLAG_TC4) == RESET)
	{
	}
}

void init_DMA_for_USART1(void)
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

void init_TIM2_for_ADC()
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

void init_NVIC_for_TIM2()
{
	
	NVIC_InitTypeDef	NVIC_DEF;
	NVIC_SetPriorityGrouping(NVIC_PriorityGroup_2);
	
	NVIC_DEF.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_DEF.NVIC_IRQChannelCmd = ENABLE;
	NVIC_DEF.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_DEF.NVIC_IRQChannelSubPriority = 0;
	
	NVIC_Init(&NVIC_DEF);
	
}

void TIM2_IRQHandler()
{

	if(TIM_GetFlagStatus(TIM2,TIM_FLAG_Update) == SET)
	{
		
		TIM_ClearFlag(TIM2,TIM_FLAG_Update);

		adc_value = ADC_GetConversionValue(ADC1);
		
		
		TxBuffer1[0] = ((((adc_value)>>8)&0x000F)+65);
		TxBuffer1[1] = ((((adc_value)>>4)&0x000F)+65);
		TxBuffer1[2] = ((((adc_value))&0x000F)+65);

	}
}

//Yarım kod bu KULLANMA!!!
void init_TIM1_for_AD5932_CTRL()
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	
	
	TIM_TimeBaseStructure.TIM_Prescaler = 72; //33ms
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_Period = 32999;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
	
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);
	
	  /* Channel 1, 2,3 and 4 Configuration in PWM mode */
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;
	TIM_OCInitStructure.TIM_Pulse = 1650;	// yuzde5 duty cycle
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
	TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;
	TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
	TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCIdleState_Reset;
  
	TIM_ITConfig(TIM1,TIM_IT_Update,ENABLE);
	
	TIM_Cmd(TIM1, ENABLE);
}

void init_AD5932_GPIO()
{
	GPIO_InitTypeDef 	GPIO_InitStruct;
	
	GPIO_InitStruct.GPIO_Pin = STANDBY_pin | CTRL_pin | INTERRUPT_pin;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	

	
	// AD5932 pin initialize values
	GPIO_ResetBits(AD5932_GPIOs, CTRL_pin);
	GPIO_ResetBits(AD5932_GPIOs, STANDBY_pin);
	GPIO_ResetBits(AD5932_GPIOs, INTERRUPT_pin);
	GPIO_SetBits(SPI2_GPIO, SPI2_CS);
}

void AD5932_Set_CR(uint16_t B24_bit,uint16_t SinTri_bit, uint16_t MSBOUTEN_bit, uint16_t ext_INC_bit, uint16_t SYNCSEL_bit)
{
	
	AD5932_Cr = AD5932_CR_base;								// Control registerina default başlangıç değerini atadık
	if(B24_bit == AD5932_BIT24)
	{
		AD5932_Cr |= AD5932_BIT24;		// B24 bitini aktif et
	}
		
	if(SinTri_bit == AD5932_SIN_OUT)
	{
		AD5932_Cr |= AD5932_SIN_OUT;		// Sinus çıkış ver	VOUT çıkışından
	}
	else if(SinTri_bit == AD5932_TRI_OUT)
	{
		AD5932_Cr |= AD5932_TRI_OUT;		// Ucgen çıkış ver	VOUT çıkışından
	}
	else
		
	if(MSBOUTEN_bit == AD5932_MSB_OUTEN)
	{
		AD5932_Cr |= AD5932_MSB_OUTEN;	// Kare dalga  üret MSBOUT çıkışından
	}
		
	if(ext_INC_bit == AD5932_EXT_INC)
	{
		AD5932_Cr |= AD5932_EXT_INC;		// Frekans arttırımını dışarıdan kontrol et
	}
		
	if(SYNCSEL_bit == AD5932_SYNC_EOS)
	{
		AD5932_Cr |= AD5932_SYNC_EOS;		// SYNOUT çıkışından her frekans taraması bittiğinde pulse al

	}
	else if(SYNCSEL_bit == AD5932_SYNC_fINC)
	{
		AD5932_Cr |= AD5932_SYNC_fINC;	// SYNCOUT çıkışından her frekans arttırımında 4xTclock süresinde pulse al
	}
						
	sendCommand_AD5932(AD5932_Cr);							// paketlenen data yollanıyor
							
}

int AD5932_Set_Start_Freq(uint32_t Freq_Data)
{
	uint16_t fLSB, fMSB;
	
	Freq_Data =  MAX_24bit & Freq_Data;				// 24 bit maskeleme
	
	fLSB = 0x00000FFF 	& Freq_Data;				// 24 bitin düşük anlamlı 12 bitini maskele 
	fMSB = ((0x00FFF000 & Freq_Data) >> 12);		// 24 bitin yüksek anlamlı 12 bitini maskele
	
	fLSB = fLSB | 0xC000;							// Fstart registerının düşük anlamlı adresi ile "or"lanarak data paketleniyor.
	fMSB = fMSB | 0xD000;							// Fstart registerının yüksek anlamlı adresi ile "or"lanarak data paketleniyor.
	
	sendCommand_AD5932(fLSB);						// paketlenen data yollanıyor
	sendCommand_AD5932(fMSB);						// paketlenen data yollanıyor
	
	return 0;										// işlem başarılı

}

int	AD5932_Set_Delta_F(uint32_t DeltaF_Data, uint8_t pORm)	// pORm: 1 ise pozitif, 0 ise negatif frekans artırımı
{ 			

	uint16_t fLSB, fMSB;

	
	DeltaF_Data =  MAX_24bit & DeltaF_Data;			// 24 bit maskeleme
	
	fLSB =	0x00000FFF 	& DeltaF_Data;				// 24 bitin düşük anlamlı 12 bitini maskele 
	fMSB = ((0x00FFF000 & DeltaF_Data) >> 12);		// 24 bitin yüksek anlamlı 12 bitini maskele
	
	fLSB = fLSB | AD5932_DeltaF_LSB_base;			// deltaF registerının düşük anlamlı adresi ile "or"lanarak data paketleniyor.
	
	if(pORm == AD5932_positive_increment)
	{	
		fMSB = fMSB | AD5932_P_Delta_F_MSB_base;	// deltaF registerının yüksek anlamlı adresi ile "or"lanarak data paketleniyor.
		
	}
	else if (pORm == AD5932_negative_increment)	
	{
		fMSB = fMSB | AD5932_M_Delta_F_MSB_base;	// deltaF registerının yüksek anlamlı adresi ile "or"lanarak data paketleniyor.
	}
		
	
	sendCommand_AD5932(fLSB);						// paketlenen data yollanıyor
	sendCommand_AD5932(fMSB);						// paketlenen data yollanıyor
	
	return 0;										// işlem başarılı
}

int AD5932_Set_NumOfInc(uint16_t Inc_Num_Data)
{

	AD5932_NumOfInc = AD5932_NUM_INC_base;

	Inc_Num_Data &= MAX_12bit;						//	12 bit maskeleme
	
	AD5932_NumOfInc |= Inc_Num_Data;			// Paketle
	
	sendCommand_AD5932(AD5932_NumOfInc);		// paketlenen data yollanıyor

	return 0;
}

int AD5932_Set_Inc_IntervalTime(uint16_t Inc_int_type_bit, uint16_t Inc_IntTim_Data)
{

	if		(Inc_int_type_bit  == AD5932_tINT_fixWaveCycle)		AD5932_Inc_IntTim = AD5932_tINT_fixWaveCycle;
	else if	(Inc_int_type_bit  == AD5932_tINT_MCLK_DIV_1)		AD5932_Inc_IntTim = AD5932_tINT_MCLK_DIV_1;
	else if	(Inc_int_type_bit  == AD5932_tINT_MCLK_DIV_5)		AD5932_Inc_IntTim = AD5932_tINT_MCLK_DIV_5;
	else if	(Inc_int_type_bit  == AD5932_tINT_MCLK_DIV_100)		AD5932_Inc_IntTim = AD5932_tINT_MCLK_DIV_100;
	else if	(Inc_int_type_bit  == AD5932_tINT_MCLK_DIV_500)		AD5932_Inc_IntTim = AD5932_tINT_MCLK_DIV_500;

		
	Inc_IntTim_Data &= MAX_11bit;				// 11 bitlik data maskeleme
	
	AD5932_Inc_IntTim |= Inc_IntTim_Data;			// maskelenen datanın incIntbase registeri ile paketleniyor
		
	sendCommand_AD5932(AD5932_Inc_IntTim);			// paketlenen data yollanıyor
	
	return 0;
}

void AD5932_Start_Sweep()
{
	GPIO_SetBits(AD5932_GPIOs, CTRL_pin);
	delay_MS(10);
	GPIO_ResetBits(AD5932_GPIOs, CTRL_pin);
}

void AD5932_Reset()
{
	GPIO_SetBits(AD5932_GPIOs, INTERRUPT_pin);
	delay_MS(10);
	GPIO_ResetBits(AD5932_GPIOs, INTERRUPT_pin);
}

void AD5932_Standby()
{
	AD5932_Reset();
	delay_MS(10);
	GPIO_SetBits(AD5932_GPIOs, STANDBY_pin);
}

void AD5932_Wakeup()
{
	GPIO_ResetBits(AD5932_GPIOs, STANDBY_pin);
}
