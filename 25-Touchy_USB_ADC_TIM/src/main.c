#include "AD5932.h"

int main(void)
{
	//--------------------------AD5932 Programla---------------------------------------------------------------------
	init_RCC_AD5932();
	init_SPI_for_AD5932();
	init_AD5932_GPIO();
	
	
	AD5932_Set_CR(AD5932_BIT24,AD5932_SIN_OUT,AD5932_NULL, AD5932_NULL,AD5932_SYNC_EOS);
	AD5932_Set_Start_Freq(0x14F);							// 1Khz yollandÄ±
	AD5932_Set_Delta_F(0x16F0,AD5932_positive_increment);	// 17.5 KHz aralÄ±klarla tara
	AD5932_Set_NumOfInc(200);								//	200 adÄ±m tara
	AD5932_Set_Inc_IntervalTime(AD5932_tINT_MCLK_DIV_5,1650);	// max 11 bit sayÄ±sÄ± yani 165us o frekansta kal
	AD5932_Start_Sweep();
	//----------------------------------------------------------------------------------------------------------------
	//--------------------------USB Protokolunu baslat----------------------------------------------------------------
	Set_System();
	Set_USBClock();
	USB_Interrupts_Config();
	USB_Init();
	//----------------------------------------------------------------------------------------------------------------
	//--------------------------ADC ve TIMER baslat ve USB den gonder-------------------------------------------------
	init_ADC_for_AD5932();
	init_TIM_for_AD5932();
	init_NVIC_for_TIM();
	//----------------------------------------------------------------------------------------------------------------
	

	
	while(1){
		delay_MS(5);
		AD5932_Reset();
		AD5932_Start_Sweep();
	}
  
}

