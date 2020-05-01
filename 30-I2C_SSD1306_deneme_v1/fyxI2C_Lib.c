#include "fyxI2C_lib.h"

void fyx_init_RCC()
{
	RCC_APB1PeriphClockCmd(I2Cx_RCC| RCC_APB1Periph_SPI2, ENABLE);
	RCC_APB2PeriphClockCmd(I2Cx_GPIO_RCC | RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOC, ENABLE);
}

void fyx_delay_MS(uint32_t nCount)
{ 
	uint32_t i;
	nCount = nCount*(SystemCoreClock/5000);
	for(i=0;i<nCount;i++);
}

void fyx_init_I2C()
{
	// Initialization struct
	I2C_InitTypeDef I2C_InitStruct;
	GPIO_InitTypeDef GPIO_InitStruct;
	
	// Step 1: Initialize I2C
	I2C_InitStruct.I2C_ClockSpeed = 100000;
	I2C_InitStruct.I2C_Mode = I2C_Mode_I2C;
	I2C_InitStruct.I2C_DutyCycle = I2C_DutyCycle_2;
	I2C_InitStruct.I2C_OwnAddress1 = 0x00;
	I2C_InitStruct.I2C_Ack = I2C_Ack_Disable;
	I2C_InitStruct.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	I2C_Init(I2Cx, &I2C_InitStruct);
	I2C_Cmd(I2Cx, ENABLE);
	
	// Step 2: Initialize GPIO as open drain alternate function
	GPIO_InitStruct.GPIO_Pin = I2Cx_PIN_SCL | I2Cx_PIN_SDA;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_OD;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(I2Cx_GPIO, &GPIO_InitStruct);
}

void fyx_init_CTRL_GPIO()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	GPIO_InitStructure.GPIO_Pin = CS_PIN | DC_PIN | RST_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

	GPIO_Init(GPIOC, &GPIO_InitStructure);
}	

void fyx_start_I2C()
{
	// Wait until I2Cx is not busy anymore
	while (I2C_GetFlagStatus(I2Cx, I2C_FLAG_BUSY));
	
	// Generate start condition
	I2C_GenerateSTART(I2Cx, ENABLE);
	
	// Wait for I2C EV5. 
	// It means that the start condition has been correctly released 
	// on the I2C bus (the bus is free, no other devices is communicating))
	while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT));
}

void fyx_stop_I2C()
{
	// Generate I2C stop condition
	I2C_GenerateSTOP(I2Cx, ENABLE);
	// Wait until I2C stop condition is finished
	while (I2C_GetFlagStatus(I2Cx, I2C_FLAG_STOPF));
}

void fyx_address_direction_I2C(uint8_t address, uint8_t direction)
{
	// Send slave address
	I2C_Send7bitAddress(I2Cx, address, direction);
	
	// Wait for I2C EV6
	// It means that a slave acknowledges his address
	if (direction == I2C_Direction_Transmitter)
	{
		while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
	}
	else if (direction == I2C_Direction_Receiver)
	{	
		while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));
	}
}

void fyx_transmit_I2C(uint8_t byte)
{
	// Send data byte
	I2C_SendData(I2Cx, byte);
	// Wait for I2C EV8_2.
	// It means that the data has been physically shifted out and 
	// output on the bus)
	while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
}

uint8_t fyx_receive_ack_I2C()
{
	// Enable ACK of received data
	I2C_AcknowledgeConfig(I2Cx, ENABLE);
	// Wait for I2C EV7
	// It means that the data has been received in I2C data register
	while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED));
	
	// Read and return data byte from I2C data register
	return I2C_ReceiveData(I2Cx);
}


uint8_t fyx_receive_nack_I2C()
{
	// Disable ACK of received data
	I2C_AcknowledgeConfig(I2Cx, DISABLE);
	// Wait for I2C EV7
	// It means that the data has been received in I2C data register
	while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED));
	
	// Read and return data byte from I2C data register
	return I2C_ReceiveData(I2Cx);
}
void fyx_write_I2C(uint8_t address, uint8_t data)
{
	fyx_start_I2C();
	fyx_address_direction_I2C(address << 1, I2C_Direction_Transmitter);
	fyx_transmit_I2C(data);
	fyx_stop_I2C();
}

void fyx_read_I2C(uint8_t address, uint8_t* data)
{
	fyx_start_I2C();
	fyx_address_direction_I2C(address << 1, I2C_Direction_Receiver);
	*data = fyx_receive_nack_I2C();	// adresi verilen data degiskeninin icine yaziyor
	fyx_stop_I2C();
}

void fyx_send_1Command_SSD1306(uint8_t address,uint8_t command)
{
	uint8_t control = 0x00;
	fyx_start_I2C();
	fyx_address_direction_I2C(address << 1, I2C_Direction_Transmitter);
	fyx_transmit_I2C(control);
	fyx_transmit_I2C(command);
	fyx_stop_I2C();
}

void fyx_send_2Command_SSD1306(uint8_t address,uint8_t comm1,uint8_t comm2)
{
	uint8_t control = 0x00;
	fyx_start_I2C();
	fyx_address_direction_I2C(address << 1, I2C_Direction_Transmitter);
	fyx_transmit_I2C(control);
	fyx_transmit_I2C(comm1);
	fyx_transmit_I2C(comm2);
	fyx_stop_I2C();
}

void fyx_send_Data_SSD1306(uint8_t address,uint8_t data)
{
	fyx_start_I2C();
	fyx_address_direction_I2C(address << 1, I2C_Direction_Transmitter);
	fyx_transmit_I2C(0x40);
	fyx_transmit_I2C(data);
	fyx_stop_I2C();
}

void fyx_powerOnReset_SSD1306()
{
	GPIO_SetBits(GPIOC, RST_PIN);
	fyx_delay_MS(1);
	GPIO_ResetBits(GPIOC, RST_PIN);	
	fyx_delay_MS(10);
	GPIO_SetBits(GPIOC, RST_PIN);
}


void fyx_init_SSD1306()
{

	fyx_powerOnReset_SSD1306();
	
		
	fyx_send_1Command_SSD1306(SLAVE_ADDRESS,0xAE);
	
	fyx_send_1Command_SSD1306(SLAVE_ADDRESS,0xD5);
	fyx_send_1Command_SSD1306(SLAVE_ADDRESS,0x80);
	
	fyx_send_1Command_SSD1306(SLAVE_ADDRESS,0xA8);
	fyx_send_1Command_SSD1306(SLAVE_ADDRESS,0x1F);
	
	fyx_send_1Command_SSD1306(SLAVE_ADDRESS,0xD3);
	fyx_send_1Command_SSD1306(SLAVE_ADDRESS,0x00);
		
	fyx_send_1Command_SSD1306(SLAVE_ADDRESS,0x40);
	
	fyx_send_1Command_SSD1306(SLAVE_ADDRESS,0x8D);
	//fyx_send_1Command_SSD1306(SLAVE_ADDRESS,0x10);	//eXTERNAL VCC
	fyx_send_1Command_SSD1306(SLAVE_ADDRESS,0x14);	//INTERNAL VCC
	
	fyx_send_1Command_SSD1306(SLAVE_ADDRESS,0x20);
	fyx_send_1Command_SSD1306(SLAVE_ADDRESS,0x00);
	
	fyx_send_1Command_SSD1306(SLAVE_ADDRESS,0xA1);
	
	fyx_send_1Command_SSD1306(SLAVE_ADDRESS,0xC8);
		
	fyx_send_1Command_SSD1306(SLAVE_ADDRESS,0xDA);
	fyx_send_1Command_SSD1306(SLAVE_ADDRESS,0x02);
	
	fyx_send_1Command_SSD1306(SLAVE_ADDRESS,0x81);
	//fyx_send_1Command_SSD1306(SLAVE_ADDRESS,0x9F);	//eXTERNAL VCC
	fyx_send_1Command_SSD1306(SLAVE_ADDRESS,0xCF);	//INTERNAL VCC
		
	
	fyx_send_1Command_SSD1306(SLAVE_ADDRESS,0xD9);
	//fyx_send_1Command_SSD1306(SLAVE_ADDRESS,0x22);	//eXTERNAL VCC
	fyx_send_1Command_SSD1306(SLAVE_ADDRESS,0xF1);	//INTERNAL VCC
	
	fyx_send_1Command_SSD1306(SLAVE_ADDRESS,0xDB);
	fyx_send_1Command_SSD1306(SLAVE_ADDRESS,0x40);
	
	fyx_send_1Command_SSD1306(SLAVE_ADDRESS,0xA4);
	
	fyx_send_1Command_SSD1306(SLAVE_ADDRESS,0xA6);
		
	fyx_send_1Command_SSD1306(SLAVE_ADDRESS,0xAF);	//DISPLAY ON
	
	fyx_send_1Command_SSD1306(SLAVE_ADDRESS,0x2E); 	//deactivescrool
}

void fyx_display_SSD1306(uint8_t *buff)
{
	uint16_t i,x;
	
	fyx_send_1Command_SSD1306(SLAVE_ADDRESS,0x21);
	fyx_send_1Command_SSD1306(SLAVE_ADDRESS,0x00);
	fyx_send_1Command_SSD1306(SLAVE_ADDRESS,0x7F);
	fyx_send_1Command_SSD1306(SLAVE_ADDRESS,0x22);
	fyx_send_1Command_SSD1306(SLAVE_ADDRESS,0x00);
	fyx_send_1Command_SSD1306(SLAVE_ADDRESS,3);
	
	for(i = 0 ; i < 512; i++)
	{
		fyx_start_I2C();
		fyx_address_direction_I2C(SLAVE_ADDRESS << 1, I2C_Direction_Transmitter);
		fyx_transmit_I2C(0x40);
		fyx_transmit_I2C(buff[i]);
		fyx_stop_I2C();
	}
	
}
void fyx_stopScroll_SSD1306()
{
	fyx_send_1Command_SSD1306(SLAVE_ADDRESS,0x2E);
}
void fyx_startScrollRight_SSD1306(uint8_t start, uint8_t stop)
{
	fyx_send_1Command_SSD1306(SLAVE_ADDRESS,0x26);
	fyx_send_1Command_SSD1306(SLAVE_ADDRESS,0x00);
	fyx_send_1Command_SSD1306(SLAVE_ADDRESS,start);
	fyx_send_1Command_SSD1306(SLAVE_ADDRESS,0x07);	//speed
	fyx_send_1Command_SSD1306(SLAVE_ADDRESS,stop);
	fyx_send_1Command_SSD1306(SLAVE_ADDRESS,0x00);
	fyx_send_1Command_SSD1306(SLAVE_ADDRESS,0xFF);
	fyx_send_1Command_SSD1306(SLAVE_ADDRESS,0x2F);
}

void fyx_startDiagonalScrollRight_SSD1306(uint8_t start, uint8_t stop)
{
	fyx_send_1Command_SSD1306(SLAVE_ADDRESS,0xA3);
	fyx_send_1Command_SSD1306(SLAVE_ADDRESS,0x00);
	fyx_send_1Command_SSD1306(SLAVE_ADDRESS,64);
	fyx_send_1Command_SSD1306(SLAVE_ADDRESS,0x29);
	fyx_send_1Command_SSD1306(SLAVE_ADDRESS,0x00);
	fyx_send_1Command_SSD1306(SLAVE_ADDRESS,start);
	fyx_send_1Command_SSD1306(SLAVE_ADDRESS,0x00);
	fyx_send_1Command_SSD1306(SLAVE_ADDRESS,stop);
	fyx_send_1Command_SSD1306(SLAVE_ADDRESS,0x01);
	fyx_send_1Command_SSD1306(SLAVE_ADDRESS,0x2F);
}

void fyx_inverseDisplay_SSD1306(uint8_t inverseFlag)
{
	if(inverseFlag == 1)
			fyx_send_1Command_SSD1306(SLAVE_ADDRESS,0xA7);
	else
			fyx_send_1Command_SSD1306(SLAVE_ADDRESS,0xA6);
}
