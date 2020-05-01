#include <stm32f10x.h>
#include <stm32f10x_tim.h>
#include <stm32f10x_usart.h>
#include "fyx_init_periph.h"
#include "fyx_esp8266.h"
#include <string.h>



char espBuffer[ESP8266BUFFER_LENGHT];
uint16_t espBufferIndex = 0;
volatile uint8_t uart_received_data,receiveText[8] = {'\0'},strIndex = 0,qFlag = 0,wFlag = 0;

char dutyBuffer[16];
uint32_t cntr;
uint16_t dutyCycle1 = 1000,dutyCycle2 = 1000;
uint8_t kolFlag = 1;
int aa = 1,bb = 1;
uint8_t ESPInitCase=0;

uint8_t varzaman = 0;
uint8_t motorKonum = 1;

void init_RCC_for_PERIPH()
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA |RCC_APB2Periph_GPIOB| RCC_APB2Periph_AFIO | RCC_APB2Periph_USART1,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2 | RCC_APB1Periph_USART2,ENABLE);
}



int main()
{
	SystemInit();

	init_RCC_for_PERIPH();
	
	init_blink_led();
	init_PWM_outputs_GPIO();
	init_USART();
	SysTick_Config(SystemCoreClock / 1000000); // 1us ayarlandi
	init_PWMs(1000);
	init_Buton1_2();
	init_Buton3();
	
	
//	check_ESP8266(espBuffer);
//	setClientMode_ESP8266(espBuffer);
//	connect_Wifi_ESP8266("SUPERONLINE_WiFi_4016","RXUEVVJERWWW",espBuffer);
//	request_WebSite_ESP8266("ogr.kocaeli.edu.tr",espBuffer);
//	send_GET_ESP8266("ogr.kocaeli.edu.tr","KOUBS/Istatistik/index.cfm",espBuffer);


	while(1){
	}
}
void EXTI9_5_IRQHandler(void)
{
	char a[5];
	
	if(EXTI_GetITStatus(EXTI_Line8) != RESET) // buton 1
	{
		
		if(kolFlag == 1)
		{
			if(dutyCycle1 >= 700)
				dutyCycle1 -=100;
			ESPInitCase=8;
				aa = 1;
			TIM_SetCompare1(TIM2,dutyCycle1);
			sprintf(a,"%d",dutyCycle1);
			sendByte_USART(USART1,'Q');
			sendString_USART(USART1,a);
			sendByte_USART(USART1,'_');
			
		}
		else if(kolFlag == 2)
		{
			if(dutyCycle2 >= 700)
				dutyCycle2 -=100;
			ESPInitCase=8;
				aa = 1;
			TIM_SetCompare2(TIM2,dutyCycle2);
			sprintf(a,"%d",dutyCycle2);
			sendByte_USART(USART1,'W');
			sendString_USART(USART1,a);
			sendByte_USART(USART1,'_');
		}
		EXTI_ClearITPendingBit(EXTI_Line8);
	}
	
	else if(EXTI_GetITStatus(EXTI_Line9) != RESET) // buton 2
	{
		if(kolFlag == 1)
		{
			if(dutyCycle1 <=3000)
				dutyCycle1 +=100;
				ESPInitCase=8;
				aa = 1;
			TIM_SetCompare1(TIM2,dutyCycle1);
			sprintf(a,"%d",dutyCycle1);
			sendByte_USART(USART1,'Q');
			sendString_USART(USART1,a);
			sendByte_USART(USART1,'_');
			
		}
		else if(kolFlag == 2)
		{
			if(dutyCycle2 <= 3000)
				dutyCycle2 +=100;
			ESPInitCase=8;
				aa = 1;
			TIM_SetCompare2(TIM2,dutyCycle2);
			sprintf(a,"%d",dutyCycle2);
			sendByte_USART(USART1,'W');
			sendString_USART(USART1,a);
			sendByte_USART(USART1,'_');
		}
		EXTI_ClearITPendingBit(EXTI_Line9);
	}
}

void EXTI15_10_IRQHandler()
{
	if(EXTI_GetITStatus(EXTI_Line12) != RESET)
	{
		if(kolFlag == 1)
			kolFlag = 2;
		else if(kolFlag == 2)
			kolFlag = 1;
		EXTI_ClearITPendingBit(EXTI_Line12);
	}

}

void USART1_IRQHandler(void){
    
	if(USART_GetFlagStatus(USART1,USART_FLAG_RXNE) == SET)
	{
		
		uart_received_data =  (USART1->DR );
		
		if(uart_received_data != '_')
		{
			receiveText[strIndex++] = uart_received_data; //strindeks bir sonraki karakter için arttiriliyor
		}
		else
		{
			
			receiveText[strIndex] = '\0';

			
			if(receiveText[0]  == 'Q')
			{
				sendString_USART(USART1,"Q");
				sendString_USART(USART1,(const char *)receiveText);
				sendString_USART(USART1," geldi STM32");
				dutyCycle1 = ((receiveText[1]-48)*1000)+((receiveText[2]-48)*100)+((receiveText[3]-48)*10)+(receiveText[4]-48);
				TIM_SetCompare1(TIM2,dutyCycle1);
			}
			else if(receiveText[0]  == 'W')
			{
				sendString_USART(USART1,"W");
				sendString_USART(USART1,(const char *)receiveText);
				sendString_USART(USART1," geldi STM32");
				dutyCycle2 = ((receiveText[1]-48)*1000)+((receiveText[2]-48)*100)+((receiveText[3]-48)*10)+(receiveText[4]-48);
				TIM_SetCompare2(TIM2,dutyCycle2);
				
			}
			strIndex = 0;
		}
		USART_ClearFlag(USART1,USART_FLAG_RXNE);
	}
}

void SysTick_Handler()
{
	if(cntr == 500000) //500ms
	{
		GPIOA->ODR ^= 0x00000010;
		cntr = 0;
	}
	else
	{
		cntr++;
		
	}
	
	
}
