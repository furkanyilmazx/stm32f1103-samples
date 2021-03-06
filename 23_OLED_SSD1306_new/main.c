#include <stm32f10x.h>
#include <stm32f10x_rcc.h>
#include <stm32f10x_gpio.h>
#include <stm32f10x_spi.h>
#include <stm32f10x_tim.h>

#define CS_PIN 		GPIO_Pin_13	// GPIOC
#define DC_PIN 		GPIO_Pin_14	// GPIOC
#define RST_PIN 	GPIO_Pin_15	// GPIOC

#define VBAT_PIN 	GPIO_Pin_0  // GPIOB
#define VDD_PIN 	GPIO_Pin_1	// GPIOB


static uint8_t buffer[1024] = { 
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,

0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFC,0xFC,0xFC,0xFC,0xFC,0x3C,0xFC,0xF0,0xF0,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xE0,0xFC,0xFC,0x3C,0x3C,0xFC,0xFC,0xFC,
0xFC,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0xFC,0xFC,0xFC,0x00,0x00,0x00,0x00,0x00,0xE0,0xF0,0xFC,
0xFC,0x3C,0x0C,0x0C,0x00,0x00,0x00,0xFC,0xFC,0xFC,0xFC,0x00,0x1C,0x1C,0x1C,0x1C,
0x1C,0x1C,0xFC,0xFC,0xFC,0xFC,0xFC,0x1C,0x1C,0x1C,0x1C,0x1C,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,

0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x03,0x3F,0x3F,
0xFF,0xFF,0xF8,0xC0,0xC0,0xF8,0xFF,0xFF,0x3F,0x03,0x03,0x00,0x00,0xFF,0xFF,0xFF,
0xFF,0x00,0x00,0x03,0x1F,0x1F,0x1F,0xFF,0xFE,0xE0,0xFE,0xFF,0xFF,0x1F,0x03,0x03,
0x03,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0x1C,0x1C,0x1C,0x7E,0xFF,0xE3,0xC1,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,

0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3F,0x3F,0x3F,0x3F,0x3F,0x00,0x00,0x00,0x00,
0x03,0x03,0x3F,0x3F,0x3F,0x1F,0x03,0x03,0x00,0x00,0x00,0x00,0x00,0x3F,0x3F,0x3F,
0x3F,0x00,0x20,0x38,0x3E,0x3E,0x3E,0x3F,0x07,0x01,0x07,0x3F,0x3F,0x3E,0x38,0x38,
0x38,0x20,0x00,0x00,0x00,0x3F,0x3F,0x3F,0x00,0x00,0x00,0x00,0x01,0x03,0x1F,0x3E,
0x3E,0x3C,0x38,0x38,0x00,0x00,0x00,0x3F,0x3F,0x3F,0x3F,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x3F,0x3F,0x3F,0x3F,0x3F,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,

0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,

0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xF0,0xF0,0xF0,0x10,0x10,0x30,0x30,0xE0,
0xC0,0x00,0x80,0xC0,0x40,0xC0,0x80,0x00,0x00,0xC0,0xC0,0x00,0xC0,0xC0,0x00,0x80,
0xC0,0x40,0x40,0xC0,0x80,0x00,0x00,0xF0,0xF0,0x00,0x80,0xC0,0x40,0xC0,0x80,0x00,
0xC0,0xC0,0xC0,0x40,0xC0,0x80,0x80,0x00,0xC0,0xC0,0x40,0xC0,0x80,0x40,0xC0,0x80,
0x80,0x00,0x00,0x80,0xC0,0x40,0xC0,0x80,0x00,0xC0,0xC0,0xC0,0x40,0xC0,0xC0,0x80,
0x40,0xE0,0xE0,0x40,0x00,0x00,0x00,0x00,0xF0,0xF0,0xF0,0x90,0x90,0xF0,0xF0,0x60,
0x00,0x80,0xC0,0x40,0xC0,0x80,0x00,0x40,0x40,0x40,0x40,0xC0,0x80,0x00,0xC0,0xC0,
0x40,0x00,0x00,0x80,0xC0,0x40,0x40,0xF0,0xF0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,

0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1F,0x1F,0x1F,0x10,0x10,0x18,0x18,0x0F,
0x07,0x00,0x0F,0x1F,0x13,0x13,0x13,0x00,0x00,0x03,0x1F,0x18,0x1F,0x03,0x00,0x0F,
0x1F,0x13,0x13,0x13,0x13,0x00,0x00,0x1F,0x1F,0x00,0x0F,0x1F,0x10,0x1F,0x0F,0x00,
0x7F,0x7F,0x7F,0x10,0x1F,0x0F,0x0F,0x00,0x1F,0x1F,0x00,0x1F,0x1F,0x00,0x1F,0x1F,
0x1F,0x00,0x00,0x0F,0x1F,0x13,0x13,0x13,0x00,0x1F,0x1F,0x1F,0x00,0x1F,0x1F,0x1F,
0x00,0x0F,0x1F,0x10,0x00,0x00,0x00,0x00,0x1F,0x1F,0x1F,0x10,0x10,0x1F,0x1F,0x0F,
0x00,0x0F,0x1F,0x10,0x1F,0x0F,0x00,0x0C,0x0C,0x1F,0x13,0x1F,0x1F,0x00,0x1F,0x1F,
0x00,0x00,0x00,0x0F,0x1F,0x10,0x10,0x1F,0x1F,0x00,0x00,0x00,0x00,0x00,0x00,0x00,

0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
};
/*

static uint8_t buffer[512] = {
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0xF8, 0xF8, 0x38, 0xF0, 0xC0, 0x00, 0x00, 0xC0, 0xF0, 0x38, 0xF8,
0xF8, 0x00, 0x40, 0xC0, 0x80, 0x00, 0x80, 0xC0, 0x40, 0x00, 0xF8, 0xF8, 0x80, 0xC0, 0xE0, 0x70,
0x38, 0x18, 0x00, 0xF8, 0xF8, 0x00, 0x18, 0x18, 0x18, 0xF8, 0xF8, 0x18, 0x18, 0x18, 0x00, 0x3C,
0x3C, 0x00, 0x00, 0x80, 0x80, 0x80, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF0, 0xF0, 0x80,
0x80, 0x80, 0x00, 0x00, 0x00, 0x80, 0x80, 0x80, 0x80, 0x00, 0x00, 0x00, 0x80, 0x80, 0x80, 0x00,
0x00, 0x80, 0x80, 0x80, 0x80, 0x80, 0x00, 0x80, 0x80, 0x80, 0x80, 0x00, 0x00, 0xF0, 0xF0, 0x00,
0x00, 0x80, 0x80, 0x80, 0xF0, 0xF0, 0x00, 0xB0, 0xB0, 0x00, 0x80, 0x80, 0x80, 0x80, 0x80, 0x00,
0x00, 0xB0, 0xB0, 0x00, 0x80, 0x80, 0x80, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x1F, 0x1F, 0x00, 0x01, 0x07, 0x1E, 0x1E, 0x07, 0x01, 0x00, 0x1F,
0x1F, 0x00, 0x10, 0x1D, 0x0F, 0x02, 0x0F, 0x1D, 0x10, 0x00, 0x1F, 0x1F, 0x01, 0x03, 0x07, 0x0E,
0x1C, 0x18, 0x00, 0x1F, 0x1F, 0x00, 0x00, 0x00, 0x00, 0x1F, 0x1F, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x0F, 0x1F, 0x12, 0x12, 0x13, 0x13, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1F, 0x1F, 0x00,
0x00, 0x1F, 0x1F, 0x00, 0x0F, 0x1F, 0x10, 0x10, 0x1F, 0x0F, 0x00, 0x0B, 0x56, 0x7C, 0x0C, 0x00,
0x6B, 0x7F, 0x54, 0x77, 0x33, 0x00, 0x0F, 0x1F, 0x12, 0x12, 0x13, 0x13, 0x00, 0x1F, 0x1F, 0x00,
0x0F, 0x1F, 0x10, 0x10, 0x1F, 0x1F, 0x00, 0x1F, 0x1F, 0x00, 0x1F, 0x1F, 0x00, 0x00, 0x1F, 0x1F,
0x00, 0x1F, 0x1F, 0x00, 0x18, 0x1E, 0x17, 0x11, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
*/
void 	delay_MS(uint32_t nCount);
void	init_RCC(void);
void	init_SPI2(void);
void	init_CTRL_GPIO(void);
void	send_data_SPI2(uint8_t data);

void 	send_1Command_SSD1306(uint8_t command);
void 	send_2Command_SSD1306(uint8_t comm1,uint8_t comm2);
void 	send_Data_SSD1306(uint8_t data);
void 	init_SSD1306(void);
void	display_SSD1306(uint8_t *buff);
void	stopScroll_SSD1306(void);
void	startScrollRight_SSD1306(uint8_t start, uint8_t stop);
void	startDiagonalScrollRight_SSD1306(uint8_t start, uint8_t stop);
void	inverseDisplay_SSD1306(uint8_t inverseFlag);

int timer=0;

int main(){

	SystemInit ();
	SystemCoreClockUpdate();
	
	init_RCC();
	init_CTRL_GPIO();

	init_SPI2();

	init_SSD1306();
	display_SSD1306(buffer);
	
	while(1){}
}

void delay_MS(uint32_t nCount)
{ 
	uint32_t i;
	nCount = nCount*(SystemCoreClock/5000);
	for(i=0;i<nCount;i++);
}

void init_RCC()
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOB, ENABLE);
}

void init_SPI2()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	SPI_InitTypeDef  SPI_InitStructure;

	/* SPI2 GPIO configuration -------------------------------------------------------*/
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_15;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	
	/* SPI2 configuration -------------------------------------------------------*/
	SPI_InitStructure.SPI_Direction 		= SPI_Direction_1Line_Tx;
	SPI_InitStructure.SPI_DataSize 			= SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL 				= SPI_CPOL_High;
	SPI_InitStructure.SPI_CPHA 				= SPI_CPHA_2Edge;
	SPI_InitStructure.SPI_NSS 				= SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_32;
	SPI_InitStructure.SPI_FirstBit 			= SPI_FirstBit_MSB;
	
	SPI_InitStructure.SPI_Mode 				= SPI_Mode_Master;
	
	
	SPI_Init(SPI2, &SPI_InitStructure);
	
	SPI_Cmd(SPI2, ENABLE);
}

void send_data_SPI2(uint8_t data)
{
	GPIO_ResetBits(GPIOC, CS_PIN);
	SPI_I2S_SendData(SPI2, data);
	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET);
	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_BSY) == SET);
	GPIO_SetBits(GPIOC, CS_PIN);
}

void init_CTRL_GPIO()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	GPIO_InitStructure.GPIO_Pin = CS_PIN | DC_PIN | RST_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = VBAT_PIN | VDD_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
	
	GPIO_Init(GPIOB,&GPIO_InitStructure);

	
}
void send_1Command_SSD1306(uint8_t command)
{
	GPIO_ResetBits(GPIOC, DC_PIN);
	send_data_SPI2(command);
	GPIO_SetBits(GPIOC, DC_PIN);
}

void send_2Command_SSD1306(uint8_t comm1,uint8_t comm2)
{
	GPIO_ResetBits(GPIOC, DC_PIN);
	send_data_SPI2(comm1);
	send_data_SPI2(comm2);
	GPIO_SetBits(GPIOC, DC_PIN);
}

void send_Data_SSD1306(uint8_t data)
{
	GPIO_SetBits(GPIOC, CS_PIN);
	GPIO_SetBits(GPIOC, DC_PIN);
	send_data_SPI2(data);
}

void powerOnReset_SSD1306()
{
	GPIO_SetBits(GPIOC, RST_PIN);
	delay_MS(1);
	GPIO_ResetBits(GPIOC, RST_PIN);	
	delay_MS(10);
	GPIO_SetBits(GPIOC, RST_PIN);
}

void init_SSD1306()
{
	
	GPIO_SetBits(GPIOB,VDD_PIN);
			powerOnReset_SSD1306();
	send_1Command_SSD1306(0xAE);

	send_1Command_SSD1306(0xD5);
	send_1Command_SSD1306(0x80);
	
	send_1Command_SSD1306(0xA8);
	send_1Command_SSD1306(0x3F); //3f olmali
	
	send_1Command_SSD1306(0xD3);
	send_1Command_SSD1306(0x00);
		
	send_1Command_SSD1306(0x40);
	
	send_1Command_SSD1306(0x8D);
	//send_1Command_SSD1306(0x10);	//eXTERNAL VCC
	send_1Command_SSD1306(0x14);	//INTERNAL VCC
	
	send_1Command_SSD1306(0x20);
	send_1Command_SSD1306(0x00);
	
	send_1Command_SSD1306(0xA1);
	
	send_1Command_SSD1306(0xC8);
		
	send_1Command_SSD1306(0xDA);
	send_1Command_SSD1306(0x12); // 12 olmal1
		
	send_1Command_SSD1306(0x81);
	//send_1Command_SSD1306(0x9F);	//eXTERNAL VCC
	send_1Command_SSD1306(0xCF);	//INTERNAL VCC
		
	
	send_1Command_SSD1306(0xD9);
	//send_1Command_SSD1306(0x22);	//eXTERNAL VCC
	send_1Command_SSD1306(0xF1);	//INTERNAL VCC
	
	send_1Command_SSD1306(0xDB);
	send_1Command_SSD1306(0x40);
	
	send_1Command_SSD1306(0xA4);
	
	send_1Command_SSD1306(0xA6);

	GPIO_SetBits(GPIOB,VBAT_PIN);
	delay_MS(200);
		
	send_1Command_SSD1306(0xAF);	//DISPLAY ON
	
	//send_1Command_SSD1306(0x2E); 	//deactivescrool
}

void display_SSD1306(uint8_t *buff)
{
	uint16_t i;
	
	send_1Command_SSD1306(0x00);
	send_1Command_SSD1306(0x10);
	send_1Command_SSD1306(0x40);
	
	for(i = 0 ; i < 1024; i++)
	{
		send_Data_SSD1306(buff[i]);
	}
	
}
void stopScroll_SSD1306()
{
	send_1Command_SSD1306(0x2E);
}
void startScrollRight_SSD1306(uint8_t start, uint8_t stop)
{
	send_1Command_SSD1306(0x26);
	send_1Command_SSD1306(0x00);
	send_1Command_SSD1306(start);
	send_1Command_SSD1306(0x07);	//speed
	send_1Command_SSD1306(stop);
	send_1Command_SSD1306(0x00);
	send_1Command_SSD1306(0xFF);
	send_1Command_SSD1306(0x2F);
}

void startDiagonalScrollRight_SSD1306(uint8_t start, uint8_t stop)
{
	send_1Command_SSD1306(0xA3);
	send_1Command_SSD1306(0x00);
	send_1Command_SSD1306(64);
	send_1Command_SSD1306(0x29);
	send_1Command_SSD1306(0x00);
	send_1Command_SSD1306(start);
	send_1Command_SSD1306(0x00);
	send_1Command_SSD1306(stop);
	send_1Command_SSD1306(0x01);
	send_1Command_SSD1306(0x2F);
}

void inverseDisplay_SSD1306(uint8_t inverseFlag)
{
	if(inverseFlag == 1)
			send_1Command_SSD1306(0xA7);
	else
			send_1Command_SSD1306(0xA6);
}
