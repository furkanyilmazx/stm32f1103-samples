#include "DDS.h"

DDS_class::DDS_class(void){
	int SPI_En;
	DDS_Cr_data = DDS_CR_base;
	
	SPI_Type = SPI2;
	SPIx_NSS = SPI2_NSS_PIN;
	SPIx_SCK = SPI2_SCK_PIN;
	SPIx_MOSI = SPI2_MOSI_PIN;
	SPIx_GPIO = SPI2_GPIO;
	USARTx_BdRate = 230400;
	
	SPI_En = DDS_SPI_Init();
	
	//if(!SPI_En) 
	
}

DDS_class::DDS_class(USART_TypeDef *USARTx, uint32_t BdRate){

	USART_Type = USARTx;
	USARTx_BdRate = BdRate;
	
	if(USART_Type == USART1)
	{
		USARTx_TX = GPIO_Pin_9;
		USARTx_RX = GPIO_Pin_10;
		USARTx_GPIO = GPIOA;
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA ,ENABLE);
	}
	else if(USART_Type == USART2)
	{
		USARTx_TX = GPIO_Pin_2;
		USARTx_RX = GPIO_Pin_3;
		USARTx_GPIO = GPIOA;
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA ,ENABLE);
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	}
	else if(USART_Type == USART3)
	{
		USARTx_TX = GPIO_Pin_10;
		USARTx_RX = GPIO_Pin_11;
		USARTx_GPIO = GPIOB;
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB ,ENABLE);
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
	}
	
	DDS_USART_Init();
	
}


DDS_class::DDS_class(SPI_TypeDef *SPIx){

	SPI_Type = SPIx;
	
	if(SPI_Type == SPI1)
	{
		SPIx_NSS	= SPI1_NSS_PIN;
		SPIx_SCK	= SPI1_SCK_PIN;
		SPIx_MOSI	= SPI1_MOSI_PIN;
		SPIx_GPIO	= SPI1_GPIO;
		RCC_APB2PeriphClockCmd(SPI1_RCC_GPIO |SPI1_RCC,ENABLE);
	}
		
	else if(SPI_Type == SPI2)
	{
		SPIx_NSS = SPI2_NSS_PIN;
		SPIx_SCK = SPI2_SCK_PIN;
		SPIx_MOSI = SPI2_MOSI_PIN;
		SPIx_GPIO = SPI2_GPIO;
		RCC_APB2PeriphClockCmd(SPI2_RCC_GPIO,ENABLE);
		RCC_APB1PeriphClockCmd(SPI2_RCC, ENABLE);
	}
	
}

DDS_class::DDS_class(wave_type wv_type){
	
}

uint16_t DDS_class::DDS_SPI_SendAndGet(uint16_t Data){
	// burada nss pini ya elle ya da soft olarak yapılması gerek onu unutma!!!!
	while(SPI_I2S_GetFlagStatus(SPI_Type,SPI_I2S_FLAG_TXE) == RESET);
	SPI_I2S_SendData(SPI_Type,Data);
	while(SPI_I2S_GetFlagStatus(SPI_Type,SPI_I2S_FLAG_RXNE)== RESET);
	// burada nss pini ya elle ya da soft olarak yapılması gerek onu unutma!!!!
	return SPI_I2S_ReceiveData(SPI_Type);
}

int	DDS_class::DDS_Set_Start_Freq(uint32_t Freq_Data){

	uint16_t fLSB, fMSB;
	
	if(Freq_Data > 0x00FFFFFF || __IS_B24_ENABLE(DDS_Cr_data))	return -1; // 24 bitten büyük sayı yollanırsa veya B24biti aktif değilse hata kodu yolla
																	
	
	Freq_Data =  0x00FFFFFF & Freq_Data;		// 24 bit maskeleme
	
	fLSB = 0x00000FFF & Freq_Data;			// 24 bitin düşük anlamlı 12 bitini maskele 
	fMSB = (0x00FFF000 & Freq_Data) >> 12;	// 24 bitin yüksek anlamlı 12 bitini maskele
	
	fLSB = fLSB | 0xC000;							// Fstart registerının düşük anlamlı adresi ile "or"lanarak data paketleniyor.
	fMSB = fMSB | 0xD000;							// Fstart registerının yüksek anlamlı adresi ile "or"lanarak data paketleniyor.
	
	DDS_SPI_SendAndGet(fLSB);						// paketlenen data yollanıyor
	DDS_SPI_SendAndGet(fMSB);						// paketlenen data yollanıyor

	return 0;										// işlem başarılı
}

int	DDS_class::DDS_Set_Delta_F(uint32_t DeltaF_Data, uint8_t pORm){ // pORm: 1 ise pozitif, 0 ise negatif frekans artırımı

	uint16_t fLSB, fMSB;

	if(DeltaF_Data > 0x00FFFFFF || __IS_B24_ENABLE(DDS_Cr_data))	return -1; // 24 bitten büyük sayı yollanırsa veya B24biti aktif değilse hata kodu yolla
	
	DeltaF_Data =  0x00FFFFFF & DeltaF_Data;	// 24 bit maskeleme
	
	fLSB = 0x00000FFF & DeltaF_Data;			// 24 bitin düşük anlamlı 12 bitini maskele 
	fMSB = ((0x00FFF000 & DeltaF_Data) >> 12);	// 24 bitin yüksek anlamlı 12 bitini maskele
	
	fLSB = fLSB | 0x2000;							// deltaF registerının düşük anlamlı adresi ile "or"lanarak data paketleniyor.
	if(pORm == 1)									// deltaF artarak gidecekse 1
		fMSB = fMSB | 0x3000;						// deltaF registerının yüksek anlamlı adresi ile "or"lanarak data paketleniyor.
	else											// deltaF azalarak gidecekse 0
		fMSB = fMSB | 0x3800;						// deltaF registerının yüksek anlamlı adresi ile "or"lanarak data paketleniyor.
	
	DDS_SPI_SendAndGet(fLSB);						// paketlenen data yollanıyor
	DDS_SPI_SendAndGet(fMSB);						// paketlenen data yollanıyor
	
	return 0;										// işlem başarılı
}

void DDS_class::DDS_Set_CR(uint8_t B24_bit,uint8_t SinTri_bit, uint8_t MSBOUTEN_bit, uint8_t ext_INC_bit, uint8_t SYNCSEL_bit){
	
	DDS_Cr_data = DDS_CR_base;
	
	if(B24_bit == 1)
							DDS_Cr_data |= DDS_BIT24;		// B24 bitini aktif et
	if(SinTri_bit == 1)
							DDS_Cr_data |= DDS_SIN_OUT;		// Sinus çıkış ver	VOUT çıkışından
	else if(SinTri_bit == 2)
							DDS_Cr_data |= DDS_TRI_OUT;		// Ucgen çıkış ver	VOUT çıkışından
	if(MSBOUTEN_bit == 1)
							DDS_Cr_data |= DDS_MSB_OUTEN;	// Kare dalga  üret MSBOUT çıkışından
	if(ext_INC_bit == 1)
							DDS_Cr_data |= DDS_EXT_INC;		// Frekans arttırımını dışarıdan kontrol et
	if(SYNCSEL_bit == 1)
							DDS_Cr_data |= DDS_SYNC_EOS;	// SYNOUT çıkışından her frekans taraması bittiğinde pulse al
	else if(SYNCSEL_bit == 2)
							DDS_Cr_data |= DDS_SYNC_fINC;	// SYNCOUT çıkışından her frekans arttırımında 4xTclock süresinde pulse al
							
	DDS_SPI_SendAndGet(DDS_Cr_data);						// paketlenen data yollanıyor
							
}

int DDS_class::DDS_Set_NumOfInc(uint16_t Inc_Num_Data){

	DDS_NumOfInc_data = DDS_NUM_INC_base;
	if(Inc_Num_Data > 0xFFF)	return -1;
	
	Inc_Num_Data &= 0x0FFF;						//	12 bit maskeleme
	
	DDS_NumOfInc_data |= Inc_Num_Data;			// Paketle
	
	DDS_SPI_SendAndGet(DDS_NumOfInc_data);		// paketlenen data yollanıyor
	
	return 0;
}

int DDS_class::DDS_Set_Inc_IntervalTime(uint8_t Inc_int_type_bit, uint16_t Inc_IntTim_Data){
	
	if(Inc_IntTim_Data > 0x07FF || Inc_IntTim_Data < 0x0002)	return -1;
	
	if		(Inc_int_type_bit  == DDS_INC_TIME_MODE1)	DDS_Inc_IntTim_data = DDS_tINT_fixWaveCycle_base;
	else if	(Inc_int_type_bit  == DDS_INC_TIME_MODE2)	DDS_Inc_IntTim_data = DDS_tINT_MCLK_DIV_1_base;
	else if	(Inc_int_type_bit  == DDS_INC_TIME_MODE3)	DDS_Inc_IntTim_data = DDS_tINT_MCLK_DIV_5_base;
	else if	(Inc_int_type_bit  == DDS_INC_TIME_MODE4)	DDS_Inc_IntTim_data = DDS_tINT_MCLK_DIV_100_base;
	else if	(Inc_int_type_bit  == DDS_INC_TIME_MODE5)	DDS_Inc_IntTim_data = DDS_tINT_MCLK_DIV_500_base;
	else 
		return -1;
		
	Inc_IntTim_Data &= 0x7FF;
	
	DDS_Inc_IntTim_data |= Inc_IntTim_Data;
	
	DDS_SPI_SendAndGet(DDS_Inc_IntTim_data);		// paketlenen data yollanıyor
	
	return 0;
}

void DDS_class::DDS_SPI_Init(){
	SPI_InitTypeDef		SPI_InitStructure;
	GPIO_InitTypeDef	GPIO_InitStructure;
 
    GPIO_InitStructure.GPIO_Pin    = SPIx_NSS | SPIx_SCK | SPIx_MOSI;
    GPIO_InitStructure.GPIO_Mode   = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed  = GPIO_Speed_50MHz;
	
    GPIO_Init(SPIx_GPIO, &GPIO_InitStructure);
		
		
    SPI_InitStructure.SPI_Direction         = SPI_Direction_1Line_Tx;
    SPI_InitStructure.SPI_Mode              = SPI_Mode_Master;
    SPI_InitStructure.SPI_DataSize          = SPI_DataSize_16b;
    SPI_InitStructure.SPI_CPOL              = SPI_CPOL_High; //Bunları ayarlamayı unutma
    SPI_InitStructure.SPI_CPHA              = SPI_CPHA_1Edge; //bunuda
    SPI_InitStructure.SPI_NSS               = SPI_NSS_Hard;
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;
    SPI_InitStructure.SPI_FirstBit          = SPI_FirstBit_MSB; //bunuda
    SPI_InitStructure.SPI_CRCPolynomial     = 7;
 
    SPI_I2S_ITConfig(SPI_Type, SPI_I2S_IT_RXNE, DISABLE);
	SPI_I2S_ITConfig(SPI_Type, SPI_I2S_IT_TXE, ENABLE);
    SPI_Init(SPI_Type,&SPI_InitStructure);
    SPI_Cmd(SPI_Type,ENABLE);

}

void DDS_class::DDS_USART_Init(){
		
	GPIO_InitTypeDef	GPIO_InitStruct;
	USART_InitTypeDef USART_InitStructure;
	

	
	GPIO_InitStruct.GPIO_Pin = USARTx_RX;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(USARTx_GPIO, &GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Pin = USARTx_TX;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(USARTx_GPIO, &GPIO_InitStruct);
	
	USART_InitStructure.USART_BaudRate = USARTx_BdRate;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	
	USART_Init(USART_Type,&USART_InitStructure);
	USART_Cmd(USART_Type,ENABLE);
}

void DDS_class::DDS_USART_Send(uint8_t *Data_Buff){
		
	
}
