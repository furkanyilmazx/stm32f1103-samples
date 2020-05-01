#ifndef FYXI2CLIB
#define FYXI2CLIB

#include "stm32f10x.h"

#define CS_PIN 	GPIO_Pin_13
#define DC_PIN 	GPIO_Pin_14
#define RST_PIN GPIO_Pin_15

#define I2Cx_RCC			RCC_APB1Periph_I2C2
#define I2Cx				I2C2
#define I2Cx_GPIO_RCC		RCC_APB2Periph_GPIOB
#define I2Cx_GPIO			GPIOB
#define I2Cx_PIN_SDA		GPIO_Pin_11
#define I2Cx_PIN_SCL		GPIO_Pin_10

#define SLAVE_ADDRESS		0x3C

void 	fyx_init_RCC(void);
void 	fyx_delay_MS(uint32_t nCount);
void 	fyx_init_CTRL_GPIO(void);
void 	fyx_init_I2C(void);
void 	fyx_start_I2C(void);
void 	fyx_stop_I2C(void);
void 	fyx_address_direction_I2C(uint8_t address, uint8_t direction);
void 	fyx_transmit_I2C(uint8_t byte);
uint8_t fyx_receive_ack_I2C(void);
uint8_t fyx_receive_nack_I2C(void);
void 	fyx_write_I2C(uint8_t address, uint8_t data);
void	fyx_read_I2C(uint8_t address, uint8_t* data);
void 	fyx_send_1Command_SSD1306(uint8_t address,uint8_t command);
void 	fyx_send_2Command_SSD1306(uint8_t address,uint8_t comm1,uint8_t comm2);
void 	fyx_send_Data_SSD1306(uint8_t address,uint8_t data);
void 	fyx_init_SSD1306(void);
void 	fyx_powerOnReset_SSD1306(void);
void 	fyx_display_SSD1306(uint8_t *buff);
void 	fyx_stopScroll_SSD1306(void);
void 	fyx_startScrollRight_SSD1306(uint8_t start, uint8_t stop);
void 	fyx_startDiagonalScrollRight_SSD1306(uint8_t start, uint8_t stop);
void 	fyx_inverseDisplay_SSD1306(uint8_t inverseFlag);


#endif
