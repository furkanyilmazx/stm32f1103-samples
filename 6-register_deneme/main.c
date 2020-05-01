#include <stm32f10x.h>
#include <stm32f10x_rcc.h>
#include <stm32f10x_usart.h>
       

int main(){
	int i;
	
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);
	
	GPIOC->CRH = 0x33344444;
	GPIOC->BSRR = 0x00002000;

	
//	 *((uint32_t *) 0x40011004) = 0x33344444;
//	 *((uint32_t *) 0x40011010) = 0x00008000;
	
	
	while(1){
		
		if(((GPIOC->ODR & 0x0000FFFF) > 0x0000E000) || ((GPIOC->ODR & 0x0000FFFF)  < 0x00002000)) GPIOC->BSRR = 0xC0002000;
		for(i= 0; i < 555555; i++);
		GPIOC->BSRR = ((GPIOC->ODR & 0x0000FFFF) << 16) | ((GPIOC->ODR & 0x0000FFFF)  << 1);
		

		
//					*(p+4) = 0x80002000;
//		for(i= 0; i< 555555;i++);
//					*(p+4) = 0x20004000;
//		for(i= 0; i< 555555;i++);
//					*(p+4)= 0x40008000;
//		for(i= 0; i< 555555;i++);

		
		
	}

}
