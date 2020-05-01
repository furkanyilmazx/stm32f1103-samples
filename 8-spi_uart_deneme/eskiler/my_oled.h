/**************************************************************************
 * Name:  SEPS114A LCD driver library for OLED Click											*
 * Designer: Erhan YILMAZ																									*
 * Date: 28-03-2015																												*
 * Tested with stm32f4 discovery shield board including stm32f4 discovery *
 * It is a public and not guaranteed. All responsibilities are on you			*
 * ************************************************************************/

#ifndef __MY_OLED_H
#define __MY_OLED_H

#include <stm32f10x.h>
#include <stm32f10x_spi.h>
#include <stm32f10x_rcc.h>


// Screen size
#define COL 95
#define ROW 63

// Some 16 bit color values
#define Black           0x0000      /*   0,   0,   0 */
#define Navy            0x000F      /*   0,   0, 128 */
#define DarkGreen       0x03E0      /*   0, 128,   0 */
#define DarkCyan        0x03EF      /*   0, 128, 128 */
#define Maroon          0x7800      /* 128,   0,   0 */
#define Purple          0x780F      /* 128,   0, 128 */
#define Olive           0x7BE0      /* 128, 128,   0 */
#define LightGrey       0xC618      /* 192, 192, 192 */
#define DarkGrey        0x7BEF      /* 128, 128, 128 */
#define Blue            0x001F      /*   0,   0, 255 */
#define Green           0x07E0      /*   0, 255,   0 */
#define Cyan            0x07FF      /*   0, 255, 255 */
#define Red             0xF800      /* 255,   0,   0 */
#define Magenta         0xF81F      /* 255,   0, 255 */
#define Yellow          0xFFE0      /* 255, 255,   0 */
#define White           0xFFFF      /* 255, 255, 255 */
#define Orange          0xFD20      /* 255, 165,   0 */
#define GreenYellow     0xAFE5      /* 173, 255,  47 */
#define Pink            0xF81F

// Port definitions
#define OLED_SPI                       SPI2
#define OLED_SPI_CLK                   RCC_APB1Periph_SPI2

#define OLED_SPI_SCK_PIN               GPIO_Pin_13                  
#define OLED_SPI_SCK_GPIO_PORT         GPIOB                      
#define OLED_SPI_SCK_GPIO_CLK          RCC_APB2Periph_GPIOB

#define OLED_SPI_MISO_PIN              GPIO_Pin_14                 
#define OLED_SPI_MISO_GPIO_PORT        GPIOB                     
#define OLED_SPI_MISO_GPIO_CLK         RCC_APB2Periph_GPIOB

#define OLED_SPI_MOSI_PIN              GPIO_Pin_15                 
#define OLED_SPI_MOSI_GPIO_PORT        GPIOB                      
#define OLED_SPI_MOSI_GPIO_CLK         RCC_APB2Periph_GPIOB

#define OLED_SPI_CS_PIN                GPIO_Pin_3                 
#define OLED_SPI_CS_GPIO_PORT          GPIOB                     
#define OLED_SPI_CS_GPIO_CLK           RCC_APB2Periph_GPIOB

#define OLED_DC_PIN                		 GPIO_Pin_4                 
#define OLED_DC_GPIO_PORT          		 GPIOB                       
#define OLED_DC_GPIO_CLK           		 RCC_APB2Periph_GPIOB


void OLEDInitSPI(void);
void OLEDDelayMs(uint32_t);
void OLEDSendByte(uint8_t);
void OLEDCommand(uint8_t, uint8_t);
void OLEDCommand_byte(uint8_t reg_index);
void OLEDCommand_3byte(uint8_t reg_index, uint8_t colST, uint8_t colSTP);
void OLED_clear(uint8_t, uint8_t ,uint8_t, uint8_t, uint8_t);
void OLED_Init(void);
void Draw_line(uint8_t ,uint8_t , uint8_t , uint8_t , uint8_t , uint8_t , uint8_t);




#endif 
