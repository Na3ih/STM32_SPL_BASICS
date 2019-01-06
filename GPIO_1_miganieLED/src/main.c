/**
  ******************************************************************************
  * @file    main.c
  * @author  Ac6
  * @version V1.0
  * @date    01-December-2013
  * @brief   Default main function.
  ******************************************************************************
*/

#include "stm32f10x.h"
#include "stm32f1xx_nucleo.h"
			
void delay(int time)
{
	for(int i = 0; i < time * 8000; i++);
}

int main(void)
{
	GPIO_InitTypeDef gpio;	// obiekt konfiguracji portu
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE); // taktowanie portu C

	GPIO_StructInit(&gpio); // domyœlna kunfiguracja
	gpio.GPIO_Pin = GPIO_Pin_13;	// pin 13
	gpio.GPIO_Mode = GPIO_Mode_Out_PP;	// jako wyjœcie push-pull
	GPIO_Init(GPIOC, &gpio);	// inicjalizacja portu C

	while(1)
	{
		GPIO_SetBits(GPIOC, GPIO_Pin_13);
		delay(100);
		GPIO_ResetBits(GPIOC, GPIO_Pin_13);
		delay(1000);
	}
}
