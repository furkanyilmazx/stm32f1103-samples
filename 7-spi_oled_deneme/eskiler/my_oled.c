#include "my_oled.h"


void OLEDInitSPI()
{

  GPIO_InitTypeDef GPIO_InitStructure;
  SPI_InitTypeDef  SPI_InitStructure;

  /* Enable the SPI periph */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);

  /* Enable SCK, MOSI and MISO GPIO clocks */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

  
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

  /* SPI SCK pin configuration */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_15;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  /* SPI configuration -------------------------------------------------------*/
  SPI_InitStructure.SPI_Direction = SPI_Direction_Tx;
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_LSB;
  SPI_InitStructure.SPI_CRCPolynomial = 7;
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
  SPI_Init(SPI2, &SPI_InitStructure);

  /* Enable SPI2  */
  SPI_Cmd(SPI2, ENABLE);

	GPIO_InitStructure.GPIO_Pin = OLED_SPI_CS_PIN | OLED_DC_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

  GPIO_Init(OLED_SPI_CS_GPIO_PORT, &GPIO_InitStructure);

  OLEDDelayMs(1000);
}

// Delay function
void OLEDDelayMs(uint32_t nCount)
{ uint32_t i;
	nCount = nCount*(SystemCoreClock/5000);
  for(i=0;i<nCount;i++);
}

// Send byte to LCD over SPI
void OLEDSendByte(uint8_t byte)
{
  while (SPI_I2S_GetFlagStatus(OLED_SPI, SPI_I2S_FLAG_TXE) == RESET);
  
  /* Send a Byte through the SPI peripheral */
  SPI_I2S_SendData(OLED_SPI, byte);
  
}

// Send command to OLED C display
void OLEDCommand(uint8_t reg_index, uint8_t reg_value)
{
//Select index addr
  GPIO_ResetBits(OLED_SPI_CS_GPIO_PORT, OLED_SPI_CS_PIN);
	GPIO_ResetBits(OLED_DC_GPIO_PORT, OLED_DC_PIN);
  OLEDSendByte(reg_index);
	
	OLEDSendByte(reg_value);
	GPIO_SetBits(OLED_SPI_CS_GPIO_PORT, OLED_SPI_CS_PIN);
}

// Send command to OLED C display
void OLEDCommand_byte(uint8_t reg_index)
{
//Select index addr
  GPIO_ResetBits(OLED_SPI_CS_GPIO_PORT, OLED_SPI_CS_PIN);
	GPIO_ResetBits(OLED_DC_GPIO_PORT, OLED_DC_PIN);
  OLEDSendByte(reg_index);
	GPIO_SetBits(OLED_SPI_CS_GPIO_PORT, OLED_SPI_CS_PIN);
}

void OLEDCommand_3byte(uint8_t reg_index, uint8_t colST, uint8_t colSTP)
{
//Select index addr
  GPIO_ResetBits(OLED_SPI_CS_GPIO_PORT, OLED_SPI_CS_PIN);
	GPIO_ResetBits(OLED_DC_GPIO_PORT, OLED_DC_PIN);
  OLEDSendByte(reg_index);
  OLEDSendByte(colST);
  OLEDSendByte(colSTP);
	GPIO_SetBits(OLED_SPI_CS_GPIO_PORT, OLED_SPI_CS_PIN);
}
// Send command to OLED C display
void OLED_clear(uint8_t cmd, uint8_t colStart,uint8_t rowStart,  uint8_t colStop, uint8_t rowStop)
{
//Select index addr
  GPIO_ResetBits(OLED_SPI_CS_GPIO_PORT, OLED_SPI_CS_PIN);
	GPIO_ResetBits(OLED_DC_GPIO_PORT, OLED_DC_PIN);
  OLEDSendByte(cmd);
	OLEDSendByte(colStart);
	OLEDSendByte(rowStart);
	OLEDSendByte(colStop);
	OLEDSendByte(rowStop);
	GPIO_SetBits(OLED_SPI_CS_GPIO_PORT, OLED_SPI_CS_PIN);
}

void VCC_EN(){
	GPIO_InitTypeDef	GPIO_InitStruct;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB |RCC_APB2Periph_GPIOC ,ENABLE);	
	
  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_7 |GPIO_Pin_6;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;

  GPIO_Init(GPIOB, &GPIO_InitStruct);
	
	  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_13;
	  GPIO_Init(GPIOC, &GPIO_InitStruct);
   
	GPIO_SetBits(GPIOB, GPIO_Pin_7|GPIO_Pin_6);
	GPIO_SetBits(GPIOC, GPIO_Pin_13);
	

}
void OLED_Init()
{
	OLEDInitSPI();
	OLEDCommand_byte(0xAE);	//SET DISPLAY OFF
	OLEDCommand(0xA0,0x74);
	OLEDCommand(0xA1,0x00);
	OLEDCommand(0xA2,0x00);
	OLEDCommand_byte(0xA4);
	OLEDCommand(0xA8,0x3F);
	OLEDCommand(0xAD,0x8E);
	OLEDCommand(0xB0,0x0B);
	OLEDCommand(0xB1,0x31);
	OLEDCommand(0xB3,0xF0);
	OLEDCommand(0x8A,0x64);
	OLEDCommand(0x8B,0x78);
	OLEDCommand(0x8C,0x64);
	OLEDCommand(0xBB,0x3A);
	OLEDCommand(0xBE,0x3E);
	OLEDCommand(0x87,0x06);
	OLEDCommand(0x81,0x91);
	OLEDCommand(0x82,0x50);
	OLEDCommand(0x83,0x7D);
	
	OLED_clear(0x25,0,0,COL,ROW);
	VCC_EN();
	OLEDDelayMs(100);
	OLEDCommand_byte(0xAF); 	//SET DISPLAY ON
	
	OLEDCommand_3byte(0x15,0,95);
	OLEDCommand_3byte(0x75,0,63);

	
}

void Draw_line(uint8_t cSTR,uint8_t rSTR, uint8_t cSTP, uint8_t rSTP, uint8_t BLUE, uint8_t GREEN, uint8_t RED){
	
	GPIO_ResetBits(OLED_SPI_CS_GPIO_PORT, OLED_SPI_CS_PIN);
	GPIO_ResetBits(OLED_DC_GPIO_PORT, OLED_DC_PIN);
  OLEDSendByte(0X21);
	OLEDSendByte(cSTR);
	OLEDSendByte(rSTR);
	OLEDSendByte(cSTP);
	OLEDSendByte(rSTP);
	OLEDSendByte(BLUE);
	OLEDSendByte(GREEN);
	OLEDSendByte(RED);
	GPIO_SetBits(OLED_SPI_CS_GPIO_PORT, OLED_SPI_CS_PIN);
	GPIO_SetBits(OLED_DC_GPIO_PORT, OLED_DC_PIN);
}


