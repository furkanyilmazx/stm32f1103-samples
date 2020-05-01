#ifndef FYX_INIT_PERIPH
#define FYX_INIT_PERIPH

#include <stm32f10x.h>
#include <stm32f10x_tim.h>
#include <stm32f10x_usart.h>


#define	USART1_Rx_pin			GPIO_Pin_10
#define	USART1_Tx_pin			GPIO_Pin_9

#define	USART2_Rx_pin			GPIO_Pin_3
#define	USART2_Tx_pin			GPIO_Pin_2

void Delay(__IO uint32_t nCount);
void init_blink_led(void);
void init_USART(void);
void sendByte_USART(USART_TypeDef* USARTx,uint8_t data);
void sendString_USART(USART_TypeDef* USARTx,const char data[]);
void init_PWMs(uint16_t duty);
void init_PWM_outputs_GPIO(void);
void init_Buton1_2(void);
void init_Buton3(void);


#endif
