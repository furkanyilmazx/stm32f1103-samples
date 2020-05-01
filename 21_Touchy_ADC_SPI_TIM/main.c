#include "AD5932.h"

int main(){
	int i;
	
	init_RCC_Sources();
	init_USART();
	init_SPI();

	init_AD5932_GPIO();
	
	
	AD5932_Set_CR(AD5932_BIT24,AD5932_SIN_OUT,AD5932_NULL, AD5932_EXT_INC,AD5932_SYNC_EOS);
	AD5932_Set_Start_Freq(0x14F);							// 1Khz yollandÄ±
	AD5932_Set_Delta_F(0x16F0,AD5932_positive_increment);	// 17.5 KHz aralÄ±klarla tara
	AD5932_Set_NumOfInc(200);								//	200 adÄ±m tara
	AD5932_Set_Inc_IntervalTime(AD5932_tINT_MCLK_DIV_5,1650);	// max 11 bit sayÄ±sÄ± yani 165us o frekansta kal
	
		init_ADC();

	while(1){
	//	AD5932_Reset();
		AD5932_Start_Sweep();
			
	}
}