#ifndef _AD5932_H
#define _AD5932_H

#include <stm32f10x.h>
#include <stm32f10x_rcc.h>
#include <stm32f10x_adc.h>
#include <stm32f10x_gpio.h>
#include <stm32f10x_usart.h>
#include <stm32f10x_spi.h>
#include <stm32f10x_adc.h>
#include <stm32f10x_dma.h>


#define SPI2_GPIO			GPIOB
#define SPI2_RCC_GPIO		RCC_APB2Periph_GPIOB
#define SPI2_MOSI			GPIO_Pin_15
#define SPI2_MISO			GPIO_Pin_14
#define SPI2_SCK			GPIO_Pin_13
#define SPI2_CS				GPIO_Pin_5
#define SPI2_RCC			RCC_APB1Periph_SPI2


#define SPI1_GPIO			GPIOA
#define SPI1_RCC_GPIO		RCC_APB2Periph_GPIOA
#define SPI1_MOSI			GPIO_Pin_7
#define SPI1_MISO			GPIO_Pin_6
#define SPI1_SCK			GPIO_Pin_5
#define SPI1_RCC			RCC_APB2Periph_SPI1


#define USART1_GPIO			GPIOA
#define USART1_RCC_GPIO		RCC_APB2Periph_GPIOA
#define	USART1_Rx			GPIO_Pin_10
#define	USART1_Tx			GPIO_Pin_9
#define	USART1_RCC			RCC_APB2Periph_USART1


#define USART2_GPIO			GPIOA
#define USART2_RCC_GPIO		RCC_APB2Periph_GPIOA
#define	USART2_Rx			GPIO_Pin_3
#define	USART2_Tx			GPIO_Pin_2
#define	USART2_RCC			RCC_APB1Periph_USART2

#define STANDBY_pin			GPIO_Pin_4
#define CTRL_pin			GPIO_Pin_5
#define INTERRUPT_pin		GPIO_Pin_6
#define AD5932_GPIOs		GPIOA
#define AD5932_GPIOs_RCC	RCC_APB2Periph_GPIOA

//-------------------Control Register settings-----------------------------------------------------------------------------------------------------
#define AD5932_CR_base				(uint16_t)0x00D3	// Control registerin ilk ve bos degeri diger tum ayarlamalar bununla "or"lanarak yapilacak.
													// B24 isnt active, 
													// DAC disable, 
													// sin/tri disable, 
													// MSBOUTEN disable, 
													// Auto increment is active, 
													// SyncOut disable and syncSel is 4xTclock increment output

#define AD5932_SIN_OUT 			(uint16_t)0x0600	// Control register da cikisin sinus olmasini isteniyorsa AD5932_CR_base ile "or" lanir.
#define AD5932_TRI_OUT			(uint16_t)0x0400	// Control register da cikisin ucgen olmasini isteniyorsa AD5932_CR_base ile "or" lanir. DAC enable
#define AD5932_EXT_INC			(uint16_t)0x0020	// Control register da frekans degisimlerinin disaridan yapilmasi isteniyorsa AD5932_CR_base ile "or" lanir. DAC enable
#define AD5932_MSB_OUTEN		(uint16_t)0x0100	// Control register da cikisin kare dalga olmasini isteniyorsa AD5932_CR_base ile "or" lanir.
#define AD5932_BIT24			(uint16_t)0x0800	// Control register da 24bitlik registerlarin ayri ayri ayarlanmasi isteniyorsa AD5932_CR_base ile "or" lanir.
#define AD5932_SYNC_EOS			(uint16_t)0x000C	// Control register da her frekans taramasi bittiginde sync pininden cikis isteniyorsa AD5932_CR_base ile "or" lanir.
#define AD5932_SYNC_fINC		(uint16_t)0x0004	// Control register da her frekans arttiriminda sync pininden isteniyorsa AD5932_CR_base ile "or" lanir.
													// NOT: Default deger olarak AD5932_SYNC_fINC secili AD5932_CR_base registerinda
#define AD5932_NULL				(uint16_t)0x0000
//---------------------------------------------------------------------------------------------------------------------------------------------------------

//--------------------DeltaF settings------------------------------------------------------------------------------------------------------------------------	
#define AD5932_DeltaF_LSB_base		(uint16_t)0x2000	// DeltaF degerinin düşük öncelikli bitlerni göndermek için bu register değerine 12 bitlik değer ile "or"lanmalıdır
#define AD5932_P_Delta_F_MSB_base	(uint16_t)0x3000	// DeltaF degerinin FStart frekansından yukarı doğru(pozitif) arttırım aralığını ifade eder. 11 bitlik bir değer ile "or"lanmalııdr.
#define AD5932_M_Delta_F_MSB_base	(uint16_t)0x3800	// DeltaF degerinin FStart frekansından aşağı doğru(negatif) arttırım aralığını ifade eder. 11 bitlik bir değer ile "or"lanmalııdr.

#define AD5932_positive_increment	(uint8_t)0x01
#define AD5932_negative_increment	(uint8_t)0x02

//---------------------------------------------------------------------------------------------------------------------------------------------------------

//--------------------Number of Increment settings---------------------------------------------------------------------------------------------------------
#define AD5932_NUM_INC_base			(uint16_t)0x1002	// Frekans arttırım sayısıdır. 12 bitlik bir değer ile "or"lanmalıdır. max değeri 4095 olur. AD5932_NUM_INC de minimum koşul gözetilmiştir.
//---------------------------------------------------------------------------------------------------------------------------------------------------------

//--------------------Increment Interval settings------------------------------------------------------------------------------------------------------------------------------
// 11bit lik bir sayı ile "or"lanmalıdır. Max değeri (2^11-1)4095 sayısı olabilir. Minimum değeri 2 olabilir.
#define AD5932_tINT_fixWaveCycle		(uint16_t)0x4002	// Minimum değeri 2 olan ve 11 bitlik değeri 20ns cycle değeriyle çarparak frekans tarama süresinin değerini belirtiyor.
#define AD5932_tINT_MCLK_DIV_1			(uint16_t)0x6002	// Minimum değeri 2 olan ve 11 bitlik değeri 20ns cycle değeriyle çarparak frekans tarama süresinin değerini belirtiyor.
#define AD5932_tINT_MCLK_DIV_5			(uint16_t)0x6802	// Minimum değeri 2 olan ve 11 bitlik değeri 100ns cycle değeriyle çarparak frekans tarama süresinin değerini belirtiyor.
#define AD5932_tINT_MCLK_DIV_100		(uint16_t)0x7002	// Minimum değeri 2 olan ve 11 bitlik değeri 2us cycle değeriyle çarparak frekans tarama süresinin değerini belirtiyor.
#define AD5932_tINT_MCLK_DIV_500		(uint16_t)0x7802	// Minimum değeri 2 olan ve 11 bitlik değeri 10us cycle değeriyle çarparak frekans tarama süresinin değerini belirtiyor.


//---------------------------------------------------------------------------------------------------------------------------------------------------------
	

#define MAX_24bit	(uint32_t)0x00FFFFFF
#define MAX_12bit	(uint16_t)0x0FFF
#define MAX_11bit	(uint16_t)0x07FF

#define __IS_B24_ENABLE(CREG)	(((CREG & 0x0800) == 0x0800) ? 0x00010 : 0x0000)



void 	delay_MS(uint32_t nCount);
void 	init_RCC_Sources(void);
void 	sendByte_USART(uint8_t data);
void 	sendString_USART(const char data[]);
void 	init_SPI(void);
void	init_ADC(void);
void 	init_USART(void);
void 	init_AD5932_GPIO(void);
void 	sendCommand_AD5932(uint16_t comm);
void 	AD5932_Set_CR(uint16_t B24_bit,uint16_t SinTri_bit, uint16_t MSBOUTEN_bit, uint16_t ext_INC_bit, uint16_t SYNCSEL_bit);
int 	AD5932_Set_Start_Freq(uint32_t Freq_Data);
int		AD5932_Set_Delta_F(uint32_t DeltaF_Data, uint8_t pORm);
int 	AD5932_Set_NumOfInc(uint16_t Inc_Num_Data);
int 	AD5932_Set_Inc_IntervalTime(uint16_t Inc_int_type_bit, uint16_t Inc_IntTim_Data);
void 	AD5932_Start_Sweep(void);
void 	AD5932_Reset(void);
void	AD5932_Standby(void);
void	AD5932_Wakeup(void);

#endif
