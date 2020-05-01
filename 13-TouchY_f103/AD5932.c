#include "AD5932.h"

uint16_t 	AD5932_Cr = 0; 			//NULL Yaptık başlangıçta
uint16_t	AD5932_NumOfInc = 0; 
uint16_t	AD5932_Inc_IntTim;

void delay_MS(uint32_t nCount)
{ 
	uint32_t i;
	nCount = nCount*(SystemCoreClock/5000);
	for(i=0;i<nCount;i++);
}

void init_RCC_Sources()
{
	RCC_APB1PeriphClockCmd(SPI2_RCC, ENABLE);
	RCC_APB2PeriphClockCmd(SPI2_RCC_GPIO | USART1_RCC_GPIO | USART1_RCC | AD5932_GPIOs_RCC|RCC_APB2Periph_ADC1, ENABLE);
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
	//sendString_USART("[OK] init SPI\r\n");
}

void init_ADC()
{
	ADC_InitTypeDef adc_struct;
	NVIC_InitTypeDef nvic;
	GPIO_InitTypeDef 	GPIO_InitStruct;
	
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AIN;
	
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	
	adc_struct.ADC_Mode = ADC_Mode_Independent;
	adc_struct.ADC_ContinuousConvMode = ENABLE;	// sürekli çevrim
	adc_struct.ADC_ScanConvMode = DISABLE;		// tek kanal ADC
	adc_struct.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;	// dış tetiklemeli olmayacak
	adc_struct.ADC_DataAlign = ADC_DataAlign_Right;
	adc_struct.ADC_NbrOfChannel = 1;
	
	ADC_Init(ADC1,&adc_struct);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_1,1,ADC_SampleTime_239Cycles5);
	
	ADC_ITConfig(ADC1,ADC_IT_EOC,ENABLE);
	ADC_Cmd(ADC1,ENABLE);
	
	ADC_ResetCalibration(ADC1);
	while(ADC_GetResetCalibrationStatus(ADC1) == SET){}
	    /* Start ADC1 calibration */
	  ADC_StartCalibration(ADC1);
	  /* Check the end of ADC1 calibration */
	  while(ADC_GetCalibrationStatus(ADC1) == SET){}
	  
	   ADC_SoftwareStartConvCmd(ADC1, ENABLE);
	   
	   nvic.NVIC_IRQChannel = ADC1_2_IRQn;
	   nvic.NVIC_IRQChannelCmd = ENABLE;
	   nvic.NVIC_IRQChannelPreemptionPriority = 0;
	   nvic.NVIC_IRQChannelSubPriority = 0;
	   
	   NVIC_SetPriorityGrouping(NVIC_PriorityGroup_2);
	   
	   NVIC_Init(&nvic);
	   
}

void ADC1_2_IRQHandler()
{
volatile uint16_t adc_value;
	if(ADC_GetITStatus(ADC1,ADC_IT_EOC) == SET)
	{
	
		adc_value = ADC_GetConversionValue(ADC1);
		//ADC_ITConfig(ADC1,ADC_IT_EOC,DISABLE);
//		sendByte_USART((adc_value/1000));
//		sendByte_USART(((adc_value/100)%10));
//		sendByte_USART(((adc_value/10)%10));
//		sendByte_USART(((adc_value)%10));
		sendByte_USART((((adc_value)>>8)&0x000F)+65);
		sendByte_USART((((adc_value)>>4)&0x000F)+65);
		sendByte_USART((((adc_value))&0x000F)+65);
		sendByte_USART('~');
		//ADC_ITConfig(ADC1,ADC_IT_EOC,ENABLE);
	}
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
	USART_Cmd(USART1,ENABLE);
	//sendString_USART("[OK] init USART\r\n");
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
	//sendString_USART("\r\n\t-->[OK] LOW CTRL pin \r\n");
	GPIO_ResetBits(AD5932_GPIOs, STANDBY_pin);
	//sendString_USART("\t-->[OK] LOW STADNBY pin \r\n");
	GPIO_ResetBits(AD5932_GPIOs, INTERRUPT_pin);
	//sendString_USART("\t-->[OK] LOW INTERRUPT pin\r\n");
	GPIO_SetBits(SPI2_GPIO, SPI2_CS);
	//sendString_USART("\t-->[OK] High FSYNC/CS pin\r\n");
	
	//sendString_USART("[OK] initialized AD5932 control pins \r\n");
}

void AD5932_Set_CR(uint16_t B24_bit,uint16_t SinTri_bit, uint16_t MSBOUTEN_bit, uint16_t ext_INC_bit, uint16_t SYNCSEL_bit)
{
	
	AD5932_Cr = AD5932_CR_base;								// Control registerina default başlangıç değerini atadık
	//sendString_USART("\r\n");
	if(B24_bit == AD5932_BIT24)
	{
		AD5932_Cr |= AD5932_BIT24;		// B24 bitini aktif et
		//sendString_USART("\t --> [ENABLE] BIT24\r\n");
	}
	else 
		//sendString_USART("\t --> [DISABLE] BIT24\r\n");
		
	if(SinTri_bit == AD5932_SIN_OUT)
	{
		AD5932_Cr |= AD5932_SIN_OUT;		// Sinus çıkış ver	VOUT çıkışından
		//sendString_USART("\t --> [ENABLE] Sinus OUT\r\n");
	}
	else if(SinTri_bit == AD5932_TRI_OUT)
	{
		AD5932_Cr |= AD5932_TRI_OUT;		// Ucgen çıkış ver	VOUT çıkışından
		//sendString_USART("\t --> [ENABLE] Triangle OUT\r\n");
	}
	else
		//sendString_USART("\t --> [DISABLE] DAC\r\n");
		
	if(MSBOUTEN_bit == AD5932_MSB_OUTEN)
	{
		AD5932_Cr |= AD5932_MSB_OUTEN;	// Kare dalga  üret MSBOUT çıkışından
		//sendString_USART("\t --> [ENABLE] MSBOUT\r\n");
	}
	else
		//sendString_USART("\t --> [DISABLE] MSBOUT\r\n");
		
	if(ext_INC_bit == AD5932_EXT_INC)
	{
		AD5932_Cr |= AD5932_EXT_INC;		// Frekans arttırımını dışarıdan kontrol et
		//sendString_USART("\t --> [ENABLE] External increment\r\n");
	}
	else
		//sendString_USART("\t --> [ENABLE] Automatic increment\r\n");
		
	if(SYNCSEL_bit == AD5932_SYNC_EOS)
	{
		AD5932_Cr |= AD5932_SYNC_EOS;		// SYNOUT çıkışından her frekans taraması bittiğinde pulse al
		//sendString_USART("\t --> [ENABLE] End of Scan pulse\r\n");
	}
	else if(SYNCSEL_bit == AD5932_SYNC_fINC)
	{
		AD5932_Cr |= AD5932_SYNC_fINC;	// SYNCOUT çıkışından her frekans arttırımında 4xTclock süresinde pulse al
		//sendString_USART("\t --> [ENABLE] Frequency increment\r\n");
	}
	else
		//sendString_USART("\t --> [DISABLE] SYNC_OUT (three-state)\r\n");
	
	//sendString_USART("[OK] CR register configured\r\n");
							
	sendCommand_AD5932(AD5932_Cr);							// paketlenen data yollanıyor
							
}

int AD5932_Set_Start_Freq(uint32_t Freq_Data)
{
	uint16_t fLSB, fMSB;
	
	if(Freq_Data > MAX_24bit) // 24 bitten büyük sayı yollanırsa veya B24biti aktif değilse hata kodu yolla
	{
		//sendString_USART("[ERROR] Start Frekansi 24 bitten buyuk\r\n");
		//sendString_USART("[TEMINATE] Program\r\n");
		return -1; 
	}	
	else if (!__IS_B24_ENABLE(AD5932_Cr))
	{
		//sendString_USART("[FAIL] B24 biti aktif degil ve start frekansi yollanamadi\r\n");
		//sendString_USART("[TEMINATE] Program\r\n");
		return -1; 
	}
						
	
	Freq_Data =  MAX_24bit & Freq_Data;				// 24 bit maskeleme
	
	fLSB = 0x00000FFF 	& Freq_Data;				// 24 bitin düşük anlamlı 12 bitini maskele 
	fMSB = ((0x00FFF000 & Freq_Data) >> 12);		// 24 bitin yüksek anlamlı 12 bitini maskele
	
	fLSB = fLSB | 0xC000;							// Fstart registerının düşük anlamlı adresi ile "or"lanarak data paketleniyor.
	fMSB = fMSB | 0xD000;							// Fstart registerının yüksek anlamlı adresi ile "or"lanarak data paketleniyor.
	
	sendCommand_AD5932(fLSB);						// paketlenen data yollanıyor
	sendCommand_AD5932(fMSB);						// paketlenen data yollanıyor
	
	//sendString_USART("[OK] send start frequency\r\n");
	return 0;										// işlem başarılı

}

int	AD5932_Set_Delta_F(uint32_t DeltaF_Data, uint8_t pORm)	// pORm: 1 ise pozitif, 0 ise negatif frekans artırımı
{ 			

	uint16_t fLSB, fMSB;

	if(DeltaF_Data > MAX_24bit) // 24 bitten büyük sayı yollanırsa veya B24biti aktif değilse hata kodu yolla
	{
		//sendString_USART("[FAIL] deltaF 24 bitten buyuk\r\n");
		//sendString_USART("[TEMINATE] Program\r\n");
		return -1; 
		
	}	
	else if (!__IS_B24_ENABLE(AD5932_Cr))
	{
		//sendString_USART("[FAIL] B24 biti aktif degil ve delta F yollanamadi\r\n");
		//sendString_USART("[TEMINATE] Program\r\n");
		return -1; 
	}
	
	DeltaF_Data =  MAX_24bit & DeltaF_Data;			// 24 bit maskeleme
	
	fLSB =	0x00000FFF 	& DeltaF_Data;				// 24 bitin düşük anlamlı 12 bitini maskele 
	fMSB = ((0x00FFF000 & DeltaF_Data) >> 12);		// 24 bitin yüksek anlamlı 12 bitini maskele
	
	fLSB = fLSB | AD5932_DeltaF_LSB_base;			// deltaF registerının düşük anlamlı adresi ile "or"lanarak data paketleniyor.
	
	if(pORm == AD5932_positive_increment)
	{	
		fMSB = fMSB | AD5932_P_Delta_F_MSB_base;	// deltaF registerının yüksek anlamlı adresi ile "or"lanarak data paketleniyor.
		//sendString_USART("\r\n\t-->[ENABLE] Positive increment\r\n");
	}
	else if (pORm == AD5932_negative_increment)	
	{
		fMSB = fMSB | AD5932_M_Delta_F_MSB_base;	// deltaF registerının yüksek anlamlı adresi ile "or"lanarak data paketleniyor.
		//sendString_USART("\r\n\t-->[ENABLE] Negative increment\r\n");
	}
	else
		//sendString_USART("\r\n\t-->[FAIL] Unknown increment parameter\r\n");
		
	
	sendCommand_AD5932(fLSB);						// paketlenen data yollanıyor
	sendCommand_AD5932(fMSB);						// paketlenen data yollanıyor
	
	//sendString_USART("[OK] send deltaF\r\n");
	return 0;										// işlem başarılı
}

int AD5932_Set_NumOfInc(uint16_t Inc_Num_Data)
{

	AD5932_NumOfInc = AD5932_NUM_INC_base;
//	if(Inc_Num_Data > MAX_12bit)	
//	{
//		sendString_USART("[FAIL] Arttirim sayisi 12 bitten buyuk\r\n");
//		sendString_USART("[TEMINATE] Program\r\n");
//		return -1; 
//	}	
//	
	Inc_Num_Data &= MAX_12bit;						//	12 bit maskeleme
	
	AD5932_NumOfInc |= Inc_Num_Data;			// Paketle
	
	sendCommand_AD5932(AD5932_NumOfInc);		// paketlenen data yollanıyor
	//sendString_USART("[OK] send NumOfInc\r\n");
	return 0;
}

int AD5932_Set_Inc_IntervalTime(uint16_t Inc_int_type_bit, uint16_t Inc_IntTim_Data)
{
	
	if(Inc_IntTim_Data > MAX_11bit || Inc_IntTim_Data < 0x0002)
	{
		//sendString_USART("[FAIL] arttirim suresi 11 bitten fazla veya decimal 2 den kucuk\r\n");
		//sendString_USART("[TEMINATE] Program\r\n");
		return -1; 
	}	
	
	if		(Inc_int_type_bit  == AD5932_tINT_fixWaveCycle)		AD5932_Inc_IntTim = AD5932_tINT_fixWaveCycle;
	else if	(Inc_int_type_bit  == AD5932_tINT_MCLK_DIV_1)		AD5932_Inc_IntTim = AD5932_tINT_MCLK_DIV_1;
	else if	(Inc_int_type_bit  == AD5932_tINT_MCLK_DIV_5)		AD5932_Inc_IntTim = AD5932_tINT_MCLK_DIV_5;
	else if	(Inc_int_type_bit  == AD5932_tINT_MCLK_DIV_100)		AD5932_Inc_IntTim = AD5932_tINT_MCLK_DIV_100;
	else if	(Inc_int_type_bit  == AD5932_tINT_MCLK_DIV_500)		AD5932_Inc_IntTim = AD5932_tINT_MCLK_DIV_500;
	else 
	{
		//sendString_USART("[FAIL] Arttirim modu secim hatasi\r\n");
		return -1;
	}
	
		
	Inc_IntTim_Data &= MAX_11bit;				// 11 bitlik data maskeleme
	
	AD5932_Inc_IntTim |= Inc_IntTim_Data;			// maskelenen datanın incIntbase registeri ile paketleniyor
		
	sendCommand_AD5932(AD5932_Inc_IntTim);			// paketlenen data yollanıyor
	//sendString_USART("[OK] send Tint\r\n");
	return 0;
}

void AD5932_Start_Sweep()
{
	delay_MS(100);
	GPIO_SetBits(AD5932_GPIOs, CTRL_pin);
	delay_MS(100);
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
	GPIO_SetBits(AD5932_GPIOs, STANDBY_pin);
	//sendString_USART("[OK] AD5932 standby\r\n");
}

void AD5932_Wakeup()
{
	GPIO_ResetBits(AD5932_GPIOs, STANDBY_pin);
	//sendString_USART("[OK] AD5932 waked up\r\n");
}
