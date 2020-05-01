/**************************************************************************
 * Name:  SEPS114A LCD driver library for OLED Click											*
 * Designer: Erhan YILMAZ																									*
 * Date: 28-03-2015																												*
 * Tested with stm32f4 discovery shield board including stm32f4 discovery *
 * It is not guaranteed. All responsibilities are on you									*
 * ************************************************************************/

#include "Oled.h"

// Init SPI function
void OLEDInitSPI()
{

  GPIO_InitTypeDef GPIO_InitStructure;
  SPI_InitTypeDef  SPI_InitStructure;

  /* Enable the SPI periph */
  RCC_APB1PeriphClockCmd(OLED_SPI_CLK, ENABLE);

  /* Enable SCK, MOSI and MISO GPIO clocks */
  RCC_APB1PeriphClockCmd(OLED_SPI_SCK_GPIO_CLK | OLED_SPI_MISO_GPIO_CLK | OLED_SPI_MOSI_GPIO_CLK, ENABLE);

  /* Enable CS  GPIO clock */
  RCC_APB1PeriphClockCmd(OLED_SPI_CS_GPIO_CLK, ENABLE);
	RCC_APB1PeriphClockCmd(OLED_DC_GPIO_CLK, ENABLE);
	RCC_APB2PeriphClockCmd(OLED_RST_GPIO_CLK, ENABLE);
	RCC_APB2PeriphClockCmd(OLED_RW_GPIO_CLK, ENABLE);
  

  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

  /* SPI SCK pin configuration */
  GPIO_InitStructure.GPIO_Pin = OLED_SPI_SCK_PIN | OLED_SPI_MOSI_PIN | OLED_SPI_MISO_PIN;
  GPIO_Init(OLED_SPI_SCK_GPIO_PORT, &GPIO_InitStructure);

  /* SPI configuration -------------------------------------------------------*/
  SPI_I2S_DeInit(OLED_SPI);
  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;
  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
  SPI_InitStructure.SPI_CRCPolynomial = 7;
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
  SPI_Init(OLED_SPI, &SPI_InitStructure);

  /* Enable SPI1  */
  SPI_Cmd(OLED_SPI, ENABLE);

	GPIO_InitStructure.GPIO_Pin = OLED_SPI_CS_PIN | OLED_DC_PIN | OLED_RST_PIN | OLED_RW_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

  GPIO_Init(OLED_SPI_CS_GPIO_PORT, &GPIO_InitStructure);

	GPIO_ResetBits(OLED_RW_GPIO_PORT, OLED_RW_PIN);
  OLEDDelayMs(100);
}

// Delay function
void OLEDDelayMs(uint32_t nCount)
{ uint32_t i;
	nCount = nCount*(SystemCoreClock/5000);
  for(i=0;i<nCount;i++);
}

// Send byte to LCD over SPI
static uint8_t OLEDSendByte(uint8_t byte)
{

  while (SPI_I2S_GetFlagStatus(OLED_SPI, SPI_I2S_FLAG_TXE) == RESET)
  {

  }
  
  /* Send a Byte through the SPI peripheral */
  SPI_I2S_SendData(OLED_SPI, byte);
  
  /* Wait to receive a Byte */
  while (SPI_I2S_GetFlagStatus(OLED_SPI, SPI_I2S_FLAG_RXNE) == RESET)
  {

  }
  
  /* Return the Byte read from the SPI bus */
  return (uint8_t)SPI_I2S_ReceiveData(OLED_SPI);
}

// Send command to OLED C display
void OLEDCommand(unsigned char reg_index, unsigned char reg_value)
{
//Select index addr
  GPIO_ResetBits(OLED_SPI_CS_GPIO_PORT, OLED_SPI_CS_PIN);
	GPIO_ResetBits(OLED_DC_GPIO_PORT, OLED_DC_PIN);
  OLEDSendByte(reg_index);
	GPIO_SetBits(OLED_SPI_CS_GPIO_PORT, OLED_SPI_CS_PIN);
// Write data to reg
	GPIO_ResetBits(OLED_SPI_CS_GPIO_PORT, OLED_SPI_CS_PIN);
  GPIO_SetBits(OLED_DC_GPIO_PORT, OLED_DC_PIN);
	OLEDSendByte(reg_value);
	GPIO_SetBits(OLED_SPI_CS_GPIO_PORT, OLED_SPI_CS_PIN);
}

// Send data to OLED C display
void OLEDData(unsigned char data_value)
{
	GPIO_ResetBits(OLED_SPI_CS_GPIO_PORT, OLED_SPI_CS_PIN);
	GPIO_SetBits(OLED_DC_GPIO_PORT, OLED_DC_PIN);
  OLEDSendByte(data_value);
	GPIO_SetBits(OLED_SPI_CS_GPIO_PORT, OLED_SPI_CS_PIN);
}

// Init sequence for 96x96 OLED color module
void OLEDInit()
{
	
	OLEDInitSPI();
	GPIO_ResetBits(OLED_RST_GPIO_PORT, OLED_RST_PIN);
  OLEDDelayMs(10);
	GPIO_SetBits(OLED_RST_GPIO_PORT, OLED_RST_PIN);
  OLEDDelayMs(10);
  /*  Soft reser */
  OLEDCommand(SEPS114A_SOFT_RESET,0x00);      
  /* Standby ON/OFF*/
  OLEDCommand(SEPS114A_STANDBY_ON_OFF,0x01);          // Standby on
  OLEDDelayMs(5);                                           // Wait for 5ms (1ms Delay Minimum)
  OLEDCommand(SEPS114A_STANDBY_ON_OFF,0x00);          // Standby off
  OLEDDelayMs(5);                                           // 1ms Delay Minimum (1ms Delay Minimum)
  /* Display OFF */
  OLEDCommand(SEPS114A_DISPLAY_ON_OFF,0x00);
  /* Set Oscillator operation */
  OLEDCommand(SEPS114A_ANALOG_CONTROL,0x00);          // using external resistor and internal OSC
  /* Set frame rate */
  OLEDCommand(SEPS114A_OSC_ADJUST,0x03);              // frame rate : 95Hz
  /* Set active display area of panel */
  OLEDCommand(SEPS114A_DISPLAY_X1,0x00);
  OLEDCommand(SEPS114A_DISPLAY_X2,0x5F);
  OLEDCommand(SEPS114A_DISPLAY_Y1,0x00);
  OLEDCommand(SEPS114A_DISPLAY_Y2,0x5F);
  /* Select the RGB data format and set the initial state of RGB interface port */
  OLEDCommand(SEPS114A_RGB_IF,0x00);                 // RGB 8bit interface
  /* Set RGB polarity */
  OLEDCommand(SEPS114A_RGB_POL,0x00);
  /* Set display mode control */
  OLEDCommand(SEPS114A_DISPLAY_MODE_CONTROL,0x80);   // SWAP:BGR, Reduce current : Normal, DC[1:0] : Normal
  /* Set MCU Interface */
  OLEDCommand(SEPS114A_CPU_IF,0x00);                 // MPU External interface mode, 8bits
  /* Set Memory Read/Write mode */
  OLEDCommand(SEPS114A_MEMORY_WRITE_READ,0x00);
  /* Set row scan direction */
  OLEDCommand(SEPS114A_ROW_SCAN_DIRECTION,0x00);     // Column : 0 --> Max, Row : 0 --> Max
  /* Set row scan mode */
  OLEDCommand(SEPS114A_ROW_SCAN_MODE,0x00);          // Alternate scan mode
  /* Set column current */
  OLEDCommand(SEPS114A_COLUMN_CURRENT_R,0x6E);
  OLEDCommand(SEPS114A_COLUMN_CURRENT_G,0x4F);
  OLEDCommand(SEPS114A_COLUMN_CURRENT_B,0x77);
  /* Set row overlap */
  OLEDCommand(SEPS114A_ROW_OVERLAP,0x00);            // Band gap only
  /* Set discharge time */
  OLEDCommand(SEPS114A_DISCHARGE_TIME,0x01);         // Discharge time : normal discharge
  /* Set peak pulse delay */
  OLEDCommand(SEPS114A_PEAK_PULSE_DELAY,0x00);
  /* Set peak pulse width */
  OLEDCommand(SEPS114A_PEAK_PULSE_WIDTH_R,0x02);
  OLEDCommand(SEPS114A_PEAK_PULSE_WIDTH_G,0x02);
  OLEDCommand(SEPS114A_PEAK_PULSE_WIDTH_B,0x02);
  /* Set precharge current */
  OLEDCommand(SEPS114A_PRECHARGE_CURRENT_R,0x14);
  OLEDCommand(SEPS114A_PRECHARGE_CURRENT_G,0x50);
  OLEDCommand(SEPS114A_PRECHARGE_CURRENT_B,0x19);
  /* Set row scan on/off  */
  OLEDCommand(SEPS114A_ROW_SCAN_ON_OFF,0x00);        // Normal row scan
  /* Set scan off level */
  OLEDCommand(SEPS114A_SCAN_OFF_LEVEL,0x04);         // VCC_C*0.75
  /* Set memory access point */
  OLEDCommand(SEPS114A_DISPLAYSTART_X,0x00);
  OLEDCommand(SEPS114A_DISPLAYSTART_Y,0x00);
  /* Display ON */
  OLEDCommand(SEPS114A_DISPLAY_ON_OFF,0x01);
}

// Set RAM access before writing data to memory
void OLEDDDRAMAccess()
{
	GPIO_ResetBits(OLED_SPI_CS_GPIO_PORT, OLED_SPI_CS_PIN);
	GPIO_ResetBits(OLED_DC_GPIO_PORT, OLED_DC_PIN);
  OLEDSendByte(0x08);
	GPIO_SetBits(OLED_SPI_CS_GPIO_PORT, OLED_SPI_CS_PIN);
}

// Set memory area(address) to write a display data
void OLEDMemorySize(char X1, char X2, char Y1, char Y2)
{
	OLEDCommand(SEPS114A_MEM_X1,X1);
	OLEDCommand(SEPS114A_MEM_X2,X2);
	OLEDCommand(SEPS114A_MEM_Y1,Y1);
	OLEDCommand(SEPS114A_MEM_Y2,Y2);
}

// Select color
void OLEDColor(uint16_t color )
{
	OLEDData((color>>8)&0xff);
	OLEDData(color&0xff);
}

// Fill All pixels as a white color
void OLEDClear(void)
{
	uint32_t i;
	OLEDCommand(0x1D,0x02);                //Set Memory Read/Write mode
  OLEDMemorySize(0x00,0x5F,0x00,0x3F);
  OLEDDDRAMAccess();
  for(i=0;i<dispX*dispY;i++)
	{
		OLEDColor(0xFFFF);
	}
}

// Set specified (X,Y) color of pixel as specified color(color)
void OLEDPoint(unsigned char X, unsigned char Y, uint16_t Color)
{
	OLEDMemorySize(X,X,Y,Y);
	OLEDDDRAMAccess();
	OLEDColor(Color);
}

// Draw line with start x0,y0, ends x1,y1 and specified color 
void OLEDDrawLine(unsigned char x0, unsigned char y0, unsigned char x1, unsigned char y1,uint16_t color)
{
#define abs(a)	(((a)> 0) ? (a) : -(a))
	int steep, t;
	int deltax, deltay, error;
	int x, y;
	int ystep;

	steep = (abs(y1 - y0) > abs(x1 - x0)); 
	if (steep)
	{ 
		t = x0;
		x0 = y0;
		y0 = t;
    t = x1;
    x1 = y1;
    y1 = t;
  }
  
	if (x0 > x1)
	{ 
    t = x0;
		x0 = x1;
    x1 = t;
    t = y0;
    y0 = y1;
    y1 = t;
  }
	deltax = x1 - x0;
	deltay = abs(y1 - y0);
  error = 0;
  y = y0;

	if (y0 < y1)
		ystep = 1;
  else
		ystep = -1;
    
	for (x = x0; x < x1 + 1; x++)
	{
		if (steep)
			OLEDPoint(y, x, color);
    else
			OLEDPoint(x, y, color);
    error += deltay;
		if ((error << 1) >= deltax)
		{
			y += ystep;
      error -= deltax;
    } 
  } 
} 

// Draw unfilled normal rectangle with start X,Y and has specified width, height, color
void OLEDDrawRectangle(unsigned char X, unsigned char Y, unsigned char width, unsigned char height, uint16_t color)
{
	OLEDDrawLine(X, Y, X+width-1, Y, color);
	OLEDDrawLine(X, Y, X,Y+height-1, color);
	OLEDDrawLine(X, Y+height-1, X+width-1, Y+height-1, color);
	OLEDDrawLine(X+width-1, Y, X+width-1, Y+height-1, color);
}

// Draw filled rectangle with start X,Y and has specified width, height, color
void OLEDDrawFilledRectangle(unsigned char X, unsigned char Y, unsigned char width, unsigned char height, uint16_t color)
{
uint16_t j;
    OLEDCommand(0x1D,0x02);                //Set Memory Read/Write mode
    
    OLEDMemorySize(X,X+width-1,Y,Y+height-1);
    OLEDDDRAMAccess();
    for(j=0;j<width*height;j++){
                        OLEDColor(color);
                       }
}

// Characters pixels value table
const char CharTable[] = 
{
        0x00,0x00,0x00,0x00,0x00,	// 0x20	??????
        0x00,0x00,0xF2,0x00,0x00,	// 0x21	!
        0x00,0xE0,0x00,0xE0,0x00,	// 0x22	"
        0x28,0xFE,0x28,0xFE,0x28,	// 0x23	#
        0x24,0x54,0xFE,0x54,0x48,	// 0x24	$
        0xC6,0xC8,0x10,0x26,0xC6,	// 0x25	%
        0x6C,0x92,0xAA,0x44,0x0A,	// 0x26	&
        0x00,0xA0,0xC0,0x00,0x00,	// 0x27	'
        0x38,0x44,0x82,0x00,0x00,	// 0x28	(
        0x00,0x00,0x82,0x44,0x38,	// 0x29	)
        0x28,0x10,0x7C,0x10,0x28,	// 0x2A	*
        0x10,0x10,0x7C,0x10,0x10,	// 0x2B	+
        0x00,0x00,0x0A,0x0C,0x00,	// 0x2C	,
        0x10,0x10,0x10,0x10,0x10,	// 0x2D	-
        0x00,0x00,0x06,0x06,0x00,	// 0x2E	.
        0x04,0x08,0x10,0x20,0x40,	// 0x2F	/

        0x7C,0x8A,0x92,0xA2,0x7C,	// 0x30	0
        0x00,0x42,0xFE,0x02,0x00,	// 0x31	1
        0x42,0x86,0x8A,0x92,0x62,	// 0x32	2
        0x84,0x82,0xA2,0xD2,0x8C,	// 0x33	3
        0x18,0x28,0x48,0xFE,0x08,	// 0x34	4
        0xE4,0xA2,0xA2,0xA2,0x9C,	// 0x35	5
        0x3C,0x52,0x92,0x92,0x0C,	// 0x36	6
        0x80,0x8E,0x90,0xA0,0xC0,	// 0x37	7
        0x6C,0x92,0x92,0x92,0x6C,	// 0x38	8
        0x60,0x92,0x92,0x94,0x78,	// 0x39	9
        0x00,0x00,0x6C,0x6C,0x00,	// 0x3A	:
        0x00,0x00,0x6A,0x6C,0x00,	// 0x3B	;
        0x10,0x28,0x44,0x82,0x00,	// 0x3C	<
        0x28,0x28,0x28,0x28,0x28,	// 0x3D	=
        0x00,0x82,0x44,0x28,0x10,	// 0x3E	>
        0x40,0x80,0x8A,0x90,0x60,	// 0x3F	?

        0x4C,0x92,0x9C,0x42,0x3C,	// 0x40	@
        0x7E,0x88,0x88,0x88,0x7E,	// 0x41	A
        0xFE,0x92,0x92,0x92,0x6C,	// 0x42	B
        0x7C,0x82,0x82,0x82,0x44,	// 0x43	C
        0xFE,0x82,0x82,0x44,0x38,	// 0x44	D
        0xFE,0x92,0x92,0x92,0x82,	// 0x45	E
        0xFE,0x90,0x90,0x90,0x80,	// 0x46	F
        0x7C,0x82,0x92,0x92,0x5E,	// 0x47	G
        0xFE,0x10,0x10,0x10,0xFE,	// 0x48	H
        0x00,0x82,0xFE,0x82,0x00,	// 0x49	I
        0x04,0x02,0x82,0xFC,0x80,	// 0x4A	J
        0xFE,0x10,0x28,0x44,0x82,	// 0x4B	K
        0xFE,0x02,0x02,0x02,0x02,	// 0x4C	L
        0xFE,0x40,0x20,0x40,0xFE,	// 0x4D	M
        0xFE,0x20,0x10,0x08,0xFE,	// 0x4E	N
        0x7C,0x82,0x82,0x82,0x7C,	// 0x4F	O

        0xFE,0x90,0x90,0x90,0x60,	// 0x50	P
        0x7C,0x82,0x8A,0x84,0x7A,	// 0x51	Q
        0xFE,0x90,0x98,0x94,0x62,	// 0x52	R
        0x62,0x92,0x92,0x92,0x8C,	// 0x53	S
        0x80,0x80,0xFE,0x80,0x80,	// 0x54	T
        0xFC,0x02,0x02,0x02,0xFC,	// 0x55	U
        0xF8,0x04,0x02,0x04,0xF8,	// 0x56	V
        0xFC,0x02,0x1C,0x02,0xFC,	// 0x57	W
        0xC6,0x28,0x10,0x28,0xC6,	// 0x58	X
        0xE0,0x10,0x1E,0x10,0xE0,	// 0x59	Y
        0x86,0x8A,0x92,0xA2,0xC2,	// 0x5A	Z
        0x00,0xFE,0x82,0x82,0x00,	// 0x5B	[
        0x18,0x24,0x7E,0x24,0x18,	// 0x5C	
        0x00,0x82,0x82,0xFE,0x00,	// 0x5D	]
        0x20,0x40,0x80,0x40,0x20,	// 0x5E	^
        0x02,0x02,0x02,0x02,0x02,	// 0x5F	_

        0x00,0x00,0x80,0x40,0x00,	// 0x60	'
        0x04,0x2A,0x2A,0x2A,0x1E,	// 0x61	a
        0xFE,0x12,0x22,0x22,0x1C,	// 0x62	b
        0x1C,0x22,0x22,0x22,0x04,	// 0x63	c
        0x1C,0x22,0x22,0x12,0xFE,	// 0x64	d
        0x1C,0x2A,0x2A,0x2A,0x18,	// 0x65	e
        0x10,0x7E,0x90,0x80,0x40,	// 0x66	f
        0x10,0x2A,0x2A,0x2A,0x3C,	// 0x67	g
        0xFE,0x10,0x20,0x20,0x1E,	// 0x68	h
        0x00,0x22,0xBE,0x02,0x00,	// 0x69	i
        0x04,0x02,0x22,0xBC,0x00,	// 0x6A	j
        0x00,0xFE,0x08,0x14,0x22,	// 0x6B	k
        0x00,0x82,0xFE,0x02,0x00,	// 0x6C	l
        0x3E,0x20,0x18,0x20,0x1E,	// 0x6D	m
        0x3E,0x10,0x20,0x20,0x1E,	// 0x6E	n
        0x1C,0x22,0x22,0x22,0x1C,	// 0x6F	o

        0x3E,0x28,0x28,0x28,0x10,	// 0x70	p
        0x10,0x28,0x28,0x28,0x3E,	// 0x71	q
        0x3E,0x10,0x20,0x20,0x10,	// 0x72	r
        0x12,0x2A,0x2A,0x2A,0x24,	// 0x73	s
        0x20,0xFC,0x22,0x02,0x04,	// 0x74	t
        0x3C,0x02,0x02,0x04,0x3E,	// 0x75	u
        0x38,0x04,0x02,0x04,0x38,	// 0x76	v
        0x3C,0x02,0x0C,0x02,0x3C,	// 0x77	w
        0x22,0x14,0x08,0x14,0x22,	// 0x78	x
        0x30,0x0A,0x0A,0x0A,0x3C,	// 0x79	y
        0x22,0x26,0x2A,0x32,0x22,	// 0x7A	z
        0x00,0x10,0x6C,0x82,0x00,	// 0x7B	{
        0x00,0x00,0xFE,0x00,0x00,	// 0x7C	|
        0x00,0x82,0x6C,0x10,0x00,	// 0x7D	}
        0x08,0x10,0x10,0x08,0x08,	// 0x7E	~
};

// Put a char on screen with X,Y position, specified foreground color(colorfore)
// and specified background color(colorback)
void OLEDPutCh(unsigned char c, unsigned char X, unsigned char Y, uint16_t colorfore, uint16_t colorback)
{
	uint8_t i,j,temp;
	uint16_t index;
	index=(c-' ')*5;

	for(i=0;i<5;i++)
	{
		temp = CharTable[index++];
		for(j=0;j<8;j++)
		{
			if((temp & 0x01)==1)
				OLEDPoint(X,Y,colorfore);
			else
				OLEDPoint(X,Y,colorback);
			temp>>=1;
			Y++;
		}
		Y-=8;
		X++;
	}
	
	for(j=0;j<8;j++)
	{
		OLEDPoint(X,Y,colorback);
		Y++;
	}

}

// Draw string on screen with X,Y position, specified foreground color(colorfore)
// and specified background color(colorback)
void OLEDSendString(const char *c, unsigned char X, unsigned char Y, uint16_t colorfore, uint16_t colorback)
{

	while(*c != '\0')
	{
		OLEDPutCh(*c, X, Y, colorfore, colorback);
		X+=6;
		c++;
	}
		

}

// End of library
