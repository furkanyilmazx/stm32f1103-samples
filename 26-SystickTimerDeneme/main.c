#include <stm32f10x.h>

void SysTick_Handler()
{
	GPIOB->ODR ^= 0x00000003;
	//GPIOB->ODR ^= 0x00000200;
}

int main(){

	GPIO_InitTypeDef myGPIO;
	
	
	SystemInit ();
	SystemCoreClockUpdate();
	SysTick_Config(4500000);
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	
	myGPIO.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
	myGPIO.GPIO_Speed = GPIO_Speed_50MHz;
	myGPIO.GPIO_Mode = GPIO_Mode_Out_PP;
	
	GPIO_Init(GPIOB,&myGPIO);
	
	GPIO_SetBits(GPIOB,GPIO_Pin_0);
	

	
	while(1){
		
	}
}
