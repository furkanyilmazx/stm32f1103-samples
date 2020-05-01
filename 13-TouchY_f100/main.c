#include "AD5932.h"

int main(){
	int i;
	delay_MS(100);
	init_RCC_Sources();
	init_USART();
	init_SPI();
	init_ADC();
	init_AD5932_GPIO();
	
	
	AD5932_Set_CR(AD5932_BIT24,AD5932_SIN_OUT,AD5932_NULL, AD5932_NULL,AD5932_SYNC_EOS);
//delay_MS(10);
	AD5932_Set_Start_Freq(0x14F);							// 1Khz yollandı
	//delay_MS(10);
	AD5932_Set_Delta_F(0x16F0,AD5932_positive_increment);	// 17.5 KHz aralıklarla tara
	//delay_MS(10);
	AD5932_Set_NumOfInc(200);								//	200 adım tara
	//delay_MS(10);
	AD5932_Set_Inc_IntervalTime(AD5932_tINT_MCLK_DIV_500,MAX_11bit);	// max 11 bit sayısı yani 20.5 ms o frekansta kal
	//delay_MS(10);
	//AD5932_Start_Sweep();									// taramayı başlat
	
	

	while(1){
		//delay_MS(4);
		//delay_MS(4);
		AD5932_Reset();
		AD5932_Start_Sweep();
//		delay_MS(100);
	}
}
