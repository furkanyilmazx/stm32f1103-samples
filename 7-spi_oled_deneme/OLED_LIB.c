#include "OLED_LIB.h"

void Delay_MS(uint32_t nCount)
{ 
	uint32_t i;
	nCount = nCount*(SystemCoreClock/5000);
  for(i=0;i<nCount;i++);
}
void CLOCK_INIT(){
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);
}

void SPI_INIT()
{

  GPIO_InitTypeDef GPIO_InitStructure;
  SPI_InitTypeDef  SPI_InitStructure;
	
  GPIO_InitStructure.GPIO_Pin = MOSI_PIN | SCK_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
	
  SPI_InitStructure.SPI_Direction = SPI_Direction_1Line_Tx;
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
  SPI_InitStructure.SPI_NSS = SPI_NSS_Hard;
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_32;
  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
  SPI_InitStructure.SPI_CRCPolynomial = 7;
	
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;

  SPI_Init(SPI2, &SPI_InitStructure);
	//SPI_NSSInternalSoftwareConfig(SPI2,SPI_NSSInternalSoft_Set);
	SPI_SSOutputCmd(SPI2,ENABLE);
  SPI_Cmd(SPI2, ENABLE);
  Delay_MS(100);
}

void SPI_SEND_BYTE(uint8_t byte)
{
  while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET);
	SPI_I2S_SendData(SPI2, byte);
}

void SPI_SEND_BYTES(uint8_t *cData)
{
	while(*(cData)) SPI_SEND_BYTE(*cData++);

}


void OLED_Send_Command_byte(uint8_t Command)
{
	SPI_SEND_BYTE(Command);
}
void OLED_Send_Command_2byte(uint8_t Command,uint8_t CommandPara)
{
	SPI_SEND_BYTE(Command);
	SPI_SEND_BYTE(CommandPara);

}

void OLED_CTRLPIN_INIT()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
GPIO_InitStructure.GPIO_Pin =  DC_PIN | VCC_EN_PIN | PMOD_EN_PIN | RST_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	
  GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_ResetBits(GPIOB,DC_PIN);
	GPIO_SetBits(GPIOB,PMOD_EN_PIN);
	GPIO_SetBits(GPIOB,RST_PIN);
	GPIO_ResetBits(GPIOB,VCC_EN_PIN);

}
void OLED_CLEAR_WINDOW(uint8_t Command,uint8_t cStart,uint8_t rStart,uint8_t cStop,uint8_t rStop)
{

	SPI_SEND_BYTE(Command);
	SPI_SEND_BYTE(cStart);
	SPI_SEND_BYTE(rStart);
	SPI_SEND_BYTE(cStop);
	SPI_SEND_BYTE(rStop);
}

void OLED_DRAW_LINE(uint8_t cStart,uint8_t rStart,uint8_t cStop,uint8_t rStop){

	SPI_SEND_BYTE(0x21);
	SPI_SEND_BYTE(cStart);
	SPI_SEND_BYTE(rStart);
	SPI_SEND_BYTE(cStop);
	SPI_SEND_BYTE(rStop);
	SPI_SEND_BYTE(35);
	SPI_SEND_BYTE(0);
	SPI_SEND_BYTE(0);
}
void OLED_Init()
{
	CLOCK_INIT();
	SPI_INIT();
	
	OLED_CTRLPIN_INIT();
		

	OLED_Send_Command_2byte(0xFD,0x12);
	
	OLED_Send_Command_byte(0xAE);				//SET DISPLAY OFF
	OLED_Send_Command_2byte(0xA0,0x72);
	OLED_Send_Command_2byte(0xA1,0x00);
	OLED_Send_Command_2byte(0xA2,0x00);
	OLED_Send_Command_byte(0xA4);
	OLED_Send_Command_2byte(0xA8,0x3F);
	OLED_Send_Command_2byte(0xAD,0x8E);
	OLED_Send_Command_2byte(0xB0,0x0B);
	OLED_Send_Command_2byte(0xB1,0x31);
	OLED_Send_Command_2byte(0xB3,0xF0);
	OLED_Send_Command_2byte(0x8A,0x64);
	OLED_Send_Command_2byte(0x8B,0x78);
	OLED_Send_Command_2byte(0x8C,0x64);
	OLED_Send_Command_2byte(0xBB,0x3A);
	OLED_Send_Command_2byte(0xBE,0x3E);
	OLED_Send_Command_2byte(0x87,0x06);
	OLED_Send_Command_2byte(0x81,0x91);
	OLED_Send_Command_2byte(0x82,0x50);
	OLED_Send_Command_2byte(0x83,0x7D);
	OLED_Send_Command_byte(0x2E);
	OLED_CLEAR_WINDOW(OLED_CLEARWINDOW,0,0,95,63);
	
	GPIO_SetBits(GPIOB,VCC_EN_PIN);

	
	OLED_Send_Command_byte(0xAF);
	Delay_MS(300);
	
	GPIO_SetBits(GPIOB,DC_PIN);	
	
	
}
