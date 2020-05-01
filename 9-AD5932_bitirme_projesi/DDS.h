#ifndef DDS_H
#define DDS_H

#include <stm32f10x.h>
#include <core_cm3.h>
#include <stm32f10x_spi.h>
#include <stm32f10x_usart.h>


//-------------------Control Register settings-----------------------------------------------------------------------------------------------------
#define DDS_CR_base				(uint16_t)0x00D3	// Control registerin ilk ve bos degeri diger tum ayarlamalar bununla "or"lanarak yapilacak.
													// B24 isnt active, 
													// DAC disable, 
													// sin/tri disable, 
													// MSBOUTEN disable, 
													// Auto increment is active, 
													// SyncOut disable and syncSel is 4xTclock increment output

#define DDS_SIN_OUT 			(uint16_t)0x0600	// Control register da cikisin sinus olmasini isteniyorsa DDS_CR_base ile "or" lanir.
#define DDS_TRI_OUT				(uint16_t)0x0400	// Control register da cikisin ucgen olmasini isteniyorsa DDS_CR_base ile "or" lanir.
#define DDS_EXT_INC				(uint16_t)0x0020	// Control register da frekans degisimlerinin disaridan yapilmasi isteniyorsa DDS_CR_base ile "or" lanir.
#define DDS_MSB_OUTEN			(uint16_t)0x0100	// Control register da cikisin kare dalga olmasini isteniyorsa DDS_CR_base ile "or" lanir.
#define DDS_BIT24				(uint16_t)0x0800	// Control register da 24bitlik registerlarin ayri ayri ayarlanmasi isteniyorsa DDS_CR_base ile "or" lanir.
#define DDS_SYNC_EOS			(uint16_t)0x000C	// Control register da her frekans taramasi bittiginde sync pininden cikis isteniyorsa DDS_CR_base ile "or" lanir.
#define DDS_SYNC_fINC			(uint16_t)0x0004	// Control register da her frekans arttiriminda sync pininden isteniyorsa DDS_CR_base ile "or" lanir.
													// NOT: Default deger olarak DDS_SYNC_fINC secili DDS_CR_base registerinda
//---------------------------------------------------------------------------------------------------------------------------------------------------------

//--------------------DeltaF settings------------------------------------------------------------------------------------------------------------------------	
#define DDS_DeltaF_LSB_base		(uint16_t)0x2000	// DeltaF degerinin düşük öncelikli bitlerni göndermek için bu register değerine 12 bitlik değer ile "or"lanmalıdır
#define DDS_P_Delta_F_MSB_base	(uint16_t)0x3000	// DeltaF degerinin FStart frekansından yukarı doğru(pozitif) arttırım aralığını ifade eder. 11 bitlik bir değer ile "or"lanmalııdr.
#define DDS_M_Delta_F_MSB_base	(uint16_t)0x3800	// DeltaF degerinin FStart frekansından aşağı doğru(negatif) arttırım aralığını ifade eder. 11 bitlik bir değer ile "or"lanmalııdr.
//---------------------------------------------------------------------------------------------------------------------------------------------------------

//--------------------Number of Increment settings---------------------------------------------------------------------------------------------------------
#define DDS_NUM_INC_base		(uint16_t)0x1002	// Frekans arttırım sayısıdır. 12 bitlik bir değer ile "or"lanmalıdır. max değeri 4095 olur. DDS_NUM_INC de minimum koşul gözetilmiştir.
//---------------------------------------------------------------------------------------------------------------------------------------------------------

//--------------------Increment Interval settings------------------------------------------------------------------------------------------------------------------------------
// 11bit lik bir sayı ile "or"lanmalıdır. Max değeri (2^11-1)4095 sayısı olabilir. Minimum değeri 2 olabilir.
#define DDS_tINT_fixWaveCycle_base	(uint16_t)0x4002	// Minimum değeri 2 olan ve 11 bitlik değeri 20ns cycle değeriyle çarparak frekans tarama süresinin değerini belirtiyor.
#define DDS_tINT_MCLK_DIV_1_base	(uint16_t)0x6002	// Minimum değeri 2 olan ve 11 bitlik değeri 20ns cycle değeriyle çarparak frekans tarama süresinin değerini belirtiyor.
#define DDS_tINT_MCLK_DIV_5_base	(uint16_t)0x6802	// Minimum değeri 2 olan ve 11 bitlik değeri 100ns cycle değeriyle çarparak frekans tarama süresinin değerini belirtiyor.
#define DDS_tINT_MCLK_DIV_100_base	(uint16_t)0x7002	// Minimum değeri 2 olan ve 11 bitlik değeri 2us cycle değeriyle çarparak frekans tarama süresinin değerini belirtiyor.
#define DDS_tINT_MCLK_DIV_500_base	(uint16_t)0x7802	// Minimum değeri 2 olan ve 11 bitlik değeri 10us cycle değeriyle çarparak frekans tarama süresinin değerini belirtiyor.

#define DDS_INC_TIME_MODE1			(uint8_t)0x01
#define DDS_INC_TIME_MODE2			(uint8_t)0x02
#define DDS_INC_TIME_MODE3			(uint8_t)0x03
#define DDS_INC_TIME_MODE4			(uint8_t)0x04
#define DDS_INC_TIME_MODE5			(uint8_t)0x05

//---------------------------------------------------------------------------------------------------------------------------------------------------------

#define SPI1_NSS_PIN	GPIO_Pin_4
#define SPI1_SCK_PIN	GPIO_Pin_5
#define SPI1_MISO_PIN	GPIO_Pin_6
#define SPI1_MOSI_PIN	GPIO_Pin_7
#define SPI1_RCC		RCC_APB2Periph_SPI1
#define SPI1_RCC_GPIO	RCC_APB2Periph_GPIOA
#define SPI1_GPIO		GPIOA

#define SPI2_NSS_PIN	GPIO_Pin_12
#define SPI2_SCK_PIN	GPIO_Pin_13
#define SPI2_MISO_PIN	GPIO_Pin_14
#define SPI2_MOSI_PIN	GPIO_Pin_15
#define SPI2_RCC		RCC_APB1Periph_SPI2
#define SPI2_RCC_GPIO	RCC_APB2Periph_GPIOB
#define SPI2_GPIO		GPIOB


#define __IS_B24_ENABLE(CREG)	(((CREG & 0x0800) == 0x0800) ? 1 : 0)


typedef enum{
	sine 		= 1,
	tri 		= 2,
	square		= 3
}wave_type;



// 16bit SPI haberleşme olmalıdır.

class DDS_class{

public:
	DDS_class(void);
	DDS_class(SPI_TypeDef *);
	DDS_class(USART_TypeDef *, uint32_t);
	DDS_class(wave_type);
	uint16_t	DDS_SPI_SendAndGet(uint16_t Data);
	int			DDS_Set_Start_Freq(uint32_t Freq_Data);
	int 		DDS_Set_Delta_F(uint32_t DeltaF_Data, uint8_t pORm);
	void		DDS_Set_CR(uint8_t B24_bit,uint8_t SinTri_bit, uint8_t MSBOUTEN_bit, uint8_t ext_INC_bit, uint8_t SYNCSEL_bit);
	int			DDS_Set_NumOfInc(uint16_t Inc_Num_Data);
	int		DDS_Set_Inc_IntervalTime(uint8_t Inc_int_type_bit, uint16_t Inc_IntTim_Data);
	
private:
	uint16_t USARTx_TX, USARTx_RX, SPIx_MOSI, SPIx_SCK, SPIx_NSS;
	uint32_t USARTx_BdRate;
	uint16_t DDS_Cr_data, DDS_fStart_data, DDS_NumOfInc_data, DDS_Inc_Interval_data, DDS_Inc_IntTim_data;
	GPIO_TypeDef *USARTx_GPIO, *SPIx_GPIO;
	SPI_TypeDef *SPI_Type;
	USART_TypeDef *USART_Type;
	

	void DDS_SPI_Init();
	void DDS_USART_Init();
	void DDS_USART_Send(uint8_t *Data_Buff);

};



#endif
