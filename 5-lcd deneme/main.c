#include <stm32f10x.h>
#include <stm32f10x_gpio.h>
#include <stm32f10x_rcc.h>
#include <stm32f10x_usart.h>
#include <stm32f10x_spi.h>
#include <stdio.h>
#include <string.h>


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


#define LCDS_ERR_SUCCESS			0	// The action completed successfully
#define LCDS_ERR_ARG_ROW_RANGE		1	// The argument is not within 0, 2 range for rows
#define LCDS_ERR_ARG_COL_RANGE		2	// The argument is not within 0, 39 range
#define LCDS_ERR_ARG_ERASE_OPTIONS	3	// The argument is not within 0, 2 range for erase types
#define LCDS_ERR_ARG_BR_RANGE		4	// The argument is not within 0, 6 range
#define LCDS_ERR_ARG_TABLE_RANGE	5	// The argument is not within 0, 3 range for table selection
#define LCDS_ERR_ARG_COMM_RANGE		6	// The argument is not within 0, 7 range
#define LCDS_ERR_ARG_CRS_RANGE		7	// The argument is not within 0, 2 range for cursor modes
#define LCDS_ERR_ARG_DSP_RANGE		8	// The argument is not within 0, 3 range for display settings types
#define LCDS_ERR_ARG_POS_RANGE		9	// The argument is not within 0, 7 range for characters position in the memory




#define USART1_Rx GPIO_Pin_10
#define USART1_Tx GPIO_Pin_9

uint8_t         defChar[] = {0, 0x4, 0x2, 0x1F, 0x02, 0x4, 0, 0};
uint8_t         defChar1[] = {14, 31, 21, 31, 23, 16, 31, 14};
uint8_t         defChar2[] = {0x00, 0x1F, 0x11, 0x00, 0x00, 0x11, 0x1F, 0x00};
uint8_t         defChar3[] = {0x00, 0x0A, 0x15, 0x11, 0x0A, 0x04, 0x00, 0x00};



void Delay_MS(uint32_t nCount)
{ 
	uint32_t i;
	nCount = nCount*(SystemCoreClock/5000);
  for(i=0;i<nCount;i++);
}

void lcdInitSPI()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	SPI_InitTypeDef  SPI_InitStructure;
		
	Delay_MS(100);

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);


	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

	/* SPI SCK pin configuration */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_15;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	/* SPI configuration -------------------------------------------------------*/
	SPI_InitStructure.SPI_Direction = SPI_Direction_1Line_Tx;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_32;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 7;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	
	
	SPI_Init(SPI2, &SPI_InitStructure);
	SPI_Cmd(SPI2, DISABLE);

}

void GPIO_CS(){
	GPIO_InitTypeDef GPIO_InitStruct;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_5;
	GPIO_Init(GPIOB, &GPIO_InitStruct);
	GPIO_WriteBit(GPIOB,GPIO_Pin_5,Bit_SET);
}

void SPI_Send_byte(uint8_t byte)
{

	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET);
	SPI_I2S_SendData(SPI2, byte);
	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_BSY) != RESET);

}

void SPI_Send_bytes(uint8_t *data, uint8_t dataSize){
	int i;
	
	for(i = 0; i < dataSize ; i++)
	{
		SPI_Send_byte(data[i]);
	}

}

void LCD_SendCommand(uint8_t command){

	GPIO_WriteBit(GPIOB,GPIO_Pin_5,Bit_RESET);
	SPI_Cmd(SPI2, ENABLE);
	SPI_Send_byte(command);
	GPIO_WriteBit(GPIOB,GPIO_Pin_5,Bit_SET);
	SPI_Cmd(SPI2, DISABLE);
}

void LCD_SendCommands(uint8_t *commands,uint8_t comSize){

	GPIO_WriteBit(GPIOB,GPIO_Pin_5,Bit_RESET);
	SPI_Cmd(SPI2, ENABLE);
	SPI_Send_bytes(commands,comSize);
	GPIO_WriteBit(GPIOB,GPIO_Pin_5,Bit_SET);
	SPI_Cmd(SPI2, DISABLE);
}

void DisplaySet(uint8_t setDisplay, uint8_t setBckl){
	
	uint8_t dispBcklOff []  = {ESC, BRACKET, '0', DISP_EN_CMD};
	uint8_t dispOnBckl  []  = {ESC, BRACKET, '1', DISP_EN_CMD};
	uint8_t dispBcklOn  []  = {ESC, BRACKET, '2', DISP_EN_CMD};
	uint8_t dispOnBcklOn[]  = {ESC, BRACKET, '3', DISP_EN_CMD};
	
	if ((!setDisplay)&&(!setBckl))	{
		//send the command for both display and backlight off
		LCD_SendCommands(dispBcklOff, 5);
	}
	else if ((setDisplay)&&(!setBckl))	{
		//send the command for display on and backlight off
		LCD_SendCommands(dispOnBckl, 5);
	}
	else if ((!setDisplay)&&(setBckl))	{
			//send the command for backlight on and display off
			LCD_SendCommands(dispBcklOn, 5);
	}
	else {
				//send the command for both display and backlight on
				LCD_SendCommands(dispOnBcklOn, 5);
	}
}


void DisplayClear() {
	uint8_t dispClr[] =  {ESC, BRACKET, '0',DISP_CLR_CMD};

	LCD_SendCommands(dispClr, 5);

}

void DisplayMode(uint8_t charNumber){
	uint8_t dispMode16[] = {ESC, BRACKET, '0', DISP_MODE_CMD};
	uint8_t dispMode40[] = {ESC, BRACKET, '1', DISP_MODE_CMD};
	if (charNumber){
		//wrap line at 16 characters
		LCD_SendCommands(dispMode16, 5);
	}
	else{
		//wrap line at 40 characters
		LCD_SendCommands(dispMode40, 5);
	}
}

void WriteStringAtPos(uint8_t idxRow, uint8_t idxCol, uint8_t strLn[], uint8_t lengt_Data) {
		
		uint8_t comm[] = {ESC,BRACKET,0,';',0,0,CURSOR_POS_CMD};
		
		comm[4] = (idxCol % 10) + '0';
		comm[5] = (idxCol / 10) + '0';
		comm[2] = idxRow + '0';
		
		LCD_SendCommands(comm, 7);
		LCD_SendCommands(strLn, lengt_Data);

}


int main(){
	uint8_t name[] = "Furkan";
	uint8_t suname[] = "yilmaz";
	
	
//	lcdInitSPI();
//	GPIO_CS();
//	
//	DisplaySet(1,1);
//	DisplayMode(0);
//	DisplayClear();

//	WriteStringAtPos(0, 5,name,6);
//	WriteStringAtPos(0, 5,name,6);
////	WriteStringAtPos(1, 5,suname,6);
	

	while(1){
	
	LCD_SendCommand(0xAA);

	}
}
