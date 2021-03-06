#ifndef OLED_LIB_H
#define OLED_LIB_H

#include <stm32f10x.h>
#include <stm32f10x_gpio.h>
#include <stm32f10x_rcc.h>
#include <stm32f10x_usart.h>
#include <stm32f10x_spi.h>

#define RST_PIN			GPIO_Pin_5
#define DC_PIN			GPIO_Pin_4
#define CS_PIN			GPIO_Pin_3
#define MOSI_PIN		GPIO_Pin_15
#define MISO_PIN		GPIO_Pin_14
#define SCK_PIN			GPIO_Pin_13
#define VCC_EN_PIN	GPIO_Pin_7
#define PMOD_EN_PIN	GPIO_Pin_6

#define OLED_CLEARWINDOW		0x25
#define OLED_DRAWLINE				0x21
#define OLED_DRAWRECTANGLE 	0x22



#define ESC 								0x1B
#define BRACKET							0x5B //[
#define CURSOR_POS_CMD			0x48 //H
#define CURSOR_SAVE_CMD			0x73 //s
#define CURSOR_RSTR_CMD			0x75 //u
#define DISP_CLR_CMD				0x6A //j
#define ERASE_INLINE_CMD		0x4B //K
#define ERASE_FIELD_CMD			0x4E //N
#define LSCROLL_CMD					0x40 //@
#define RSCROLL_CMD					0x41 //A
#define RST_CMD							0x2A //*
#define DISP_EN_CMD					0x65 //e
#define DISP_MODE_CMD				0x68 //h
#define CURSOR_MODE_CMD			0x63 //c
#define TWI_SAVE_ADDR_CMD		0x61 //a
#define BR_SAVE_CMD							0x62 //b
#define PRG_CHAR_CMD						0x70 //p
#define SAVE_RAM_TO_EEPROM_CMD	0x74 //t
#define LD_EEPROM_TO_RAM_CMD		0x6C //l
#define DEF_CHAR_CMD						0x64 //d
#define COMM_MODE_SAVE_CMD			0x6D //m
#define EEPROM_WR_EN_CMD				0x77 //w
#define CURSOR_MODE_SAVE_CMD		0x6E //n
#define DISP_MODE_SAVE_CMD			0x6F //o



void Delay_MS(uint32_t nCount);
void CLOCK_INIT(void);
void SPI_INIT(void);
uint16_t SPI_SEND_BYTE(uint8_t byte);
void SPI_SEND_BYTES(uint8_t *cData);
void OLED_Send_Command_byte(uint8_t Command);
void OLED_Send_Command_2byte(uint8_t Command,uint8_t CommandPara);
void OLED_CTRLPIN_INIT(void);
void OLED_CLEAR_WINDOW(uint8_t Command, uint8_t cStart, uint8_t rStart, uint8_t cStop, uint8_t rStop);
void OLED_Init(void);
void LCD_CTRLPIN_INIT(void);
void LCD_DISP_SET(uint8_t setDisplay, uint8_t setBckl);
void LCD_DisplayMode(uint8_t charNumber);
void LCD_DisplayClear(void);
void LCD_INIT(void);







#endif
