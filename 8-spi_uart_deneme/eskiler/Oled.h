/**************************************************************************
 * Name:  SEPS114A LCD driver library for OLED Click											*
 * Designer: Erhan YILMAZ																									*
 * Date: 28-03-2015																												*
 * Tested with stm32f4 discovery shield board including stm32f4 discovery *
 * It is a public and not guaranteed. All responsibilities are on you			*
 * ************************************************************************/

#ifndef __OLED_H
#define __OLED_H
 #include <stm32f10x.h>
 #include <stm32f10x_spi.h>
  #include <stm32f10x_rcc.h>

#define SEPS114A_SOFT_RESET 0x01
#define SEPS114A_DISPLAY_ON_OFF 0x02
#define SEPS114A_ANALOG_CONTROL 0x0F    //
#define SEPS114A_STANDBY_ON_OFF 0x14
#define SEPS114A_OSC_ADJUST 0x1A
#define SEPS114A_ROW_SCAN_DIRECTION 0x09
#define SEPS114A_DISPLAY_X1 0x30
#define SEPS114A_DISPLAY_X2 0x31
#define SEPS114A_DISPLAY_Y1 0x32
#define SEPS114A_DISPLAY_Y2 0x33
#define SEPS114A_DISPLAYSTART_X 0x38
#define SEPS114A_DISPLAYSTART_Y 0x39
#define SEPS114A_CPU_IF 0x0D
#define SEPS114A_MEM_X1 0x34
#define SEPS114A_MEM_X2 0x35
#define SEPS114A_MEM_Y1 0x36
#define SEPS114A_MEM_Y2 0x37
#define SEPS114A_MEMORY_WRITE_READ 0x1D
#define SEPS114A_DDRAM_DATA_ACCESS_PORT 0x08
#define SEPS114A_DISCHARGE_TIME 0x18
#define SEPS114A_PEAK_PULSE_DELAY 0x16
#define SEPS114A_PEAK_PULSE_WIDTH_R 0x3A
#define SEPS114A_PEAK_PULSE_WIDTH_G 0x3B
#define SEPS114A_PEAK_PULSE_WIDTH_B 0x3C
#define SEPS114A_PRECHARGE_CURRENT_R 0x3D
#define SEPS114A_PRECHARGE_CURRENT_G 0x3E
#define SEPS114A_PRECHARGE_CURRENT_B 0x3F
#define SEPS114A_COLUMN_CURRENT_R 0x40
#define SEPS114A_COLUMN_CURRENT_G 0x41
#define SEPS114A_COLUMN_CURRENT_B 0x42
#define SEPS114A_ROW_OVERLAP 0x48
#define SEPS114A_SCAN_OFF_LEVEL 0x49
#define SEPS114A_ROW_SCAN_ON_OFF 0x17
#define SEPS114A_ROW_SCAN_MODE 0x13
#define SEPS114A_SCREEN_SAVER_CONTEROL 0xD0
#define SEPS114A_SS_SLEEP_TIMER 0xD1
#define SEPS114A_SCREEN_SAVER_MODE 0xD2
#define SEPS114A_SS_UPDATE_TIMER 0xD3
#define SEPS114A_RGB_IF 0xE0
#define SEPS114A_RGB_POL 0xE1
#define SEPS114A_DISPLAY_MODE_CONTROL 0xE5

// Screen size
#define dispX 96
#define dispY 96

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

#define OLED_RST_PIN                	 GPIO_Pin_5                  
#define OLED_RST_GPIO_PORT          	 GPIOB                       
#define OLED_RST_GPIO_CLK           	 RCC_APB2Periph_GPIOB

#define OLED_RW_PIN                	   GPIO_Pin_6                 
#define OLED_RW_GPIO_PORT          	   GPIOB                      
#define OLED_RW_GPIO_CLK           	   RCC_APB2Periph_GPIOB

// Function prototypes
void OLEDInitSPI(void);
static uint8_t OLEDSendByte(uint8_t byte);
void OLEDCommand(unsigned char reg_index, unsigned char reg_value);
void OLEDData(unsigned char data_value);
void OLEDInit(void);
void OLEDDDRAMAccess(void);
void OLEDMemorySize(char X1, char X2, char Y1, char Y2);
void OLEDColor(uint16_t color);
void OLEDDelayMs(uint32_t nCount);
void OLEDClear(void);
void OLEDPoint(unsigned char X, unsigned char Y, uint16_t color);
void OLEDDrawLine(unsigned char x0, unsigned char y0, unsigned char x1, unsigned char y1, uint16_t color);
void OLEDDrawRectangle(unsigned char X, unsigned char Y, unsigned char width, unsigned char height, uint16_t color);
void OLEDDrawFilledRectangle(unsigned char X, unsigned char Y, unsigned char width, unsigned char height, uint16_t color);
void OLEDPutCh(unsigned char c, unsigned char X, unsigned char Y, uint16_t colorfore, uint16_t colorback);
void OLEDSendString(const char *c, unsigned char X, unsigned char Y, uint16_t colorfore, uint16_t colorback);

#endif 
