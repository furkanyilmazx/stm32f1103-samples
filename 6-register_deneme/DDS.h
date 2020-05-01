#ifndef DDS_H
#define DDS_H

#define DDS_CR_base				(uint16_t)0x00D3
#define DDS_SIN_OUT 			(uint16_t)0x0600
#define DDS_TRI_OUT				(uint16_t)0x0400
#define DDS_EXT_INC				(uint16_t)0x0020
#define DDS_MSB_OUTEN			(uint16_t)0x0100
#define DDS_BIT24					(uint16_t)0x0800
#define DDS_SYNC_EOS			(uint16_t)0x000C
#define DDS_SYNC_CLK			(uint16_t)0x0004

#define DDS_dF_LSB_base		(uint16_t)0x2000
#define DDS_PdF_MSB_base	(uint16_t)0x3000
#define DDS_MdF_MSB_base	(uint16_t)0x3800

#define DDS_NUM_INC				(uint16_t)0x1000

#define DDS_tINT_FIX			(uint16_t)0x4000
#define DDS_MCLK_DIV_1		(uint16_t)0x6000
#define DDS_MCLK_DIV_5		(uint16_t)0x6800
#define DDS_MCLK_DIV_100	(uint16_t)0x7000
#define DDS_MCLK_DIV_500	(uint16_t)0x7800


#include <stm32f10x.h>
#include <core_cm3.h>
#include <stm32f10x_spi.h>
#include <stm32f10x_usart.h>



typedef enum{
	sine 		= 1,
	tri 		= 2,
	squqre	= 3
}wave_type;



class DDS_class{
	public:
		void 	DDS();
		void 	DDS(SPI_TypeDef *);
		void	DDS(USART_TypeDef *, uint32_t);
		void 	DDS(wave_type);
		int		DDS_SPI_Send(uint16_t Data);
		int		DDS_Set_Start_Freq(uint16_t Freq_Data);
		int 	DDS_Set_Delta_F(uint16_t DeltaF_Data);
		int		DDS_Set_CR(uint16_t	Cr_Data);
		int		DDS_Set_Inc_Num(uint16_t Inc_Num_Data);
		
		
	private:
		uint16_t USARTx_TX, USARTx_RX, SPIx_MOSI, SPIx_SCK, SPIx_NSS;
	
		GPIO_TypeDef *USARTx_GPIO, *SPIx_GPIO;
	
		SPI_TypeDef *SPI_Type;
		USART_TypeDef *USART_Type;
	
		int	DDS_SPI_Init();
		void DDS_USART_Init(uint32_t);
		void DDS_USART_Send(uint8_t *Data_Buff);

};



#endif