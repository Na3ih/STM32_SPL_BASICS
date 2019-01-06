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
			
volatile uint32_t timer = 0;

void SysTick_Handler()
{
	if(timer) --timer;
}

int main(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

	GPIO_InitTypeDef(gpio);
	GPIO_StructInit(&gpio);
	gpio.GPIO_Pin = GPIO_Pin_13;
	gpio.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOC, &gpio);

	SysTick_Config(SystemCoreClock / 1000);	// SystemCoreClock = 72MHz, :1000 = wywo³anie Handler co 1ms

	while(1)
	{
		GPIO_SetBits(GPIOC, GPIO_Pin_13);
		timer = 2000;
		while(timer);
		GPIO_ResetBits(GPIOC, GPIO_Pin_13);
		timer = 4000;
		while(timer);
		// ! dioda na PC13 dzia³a odwrotnie
	}
}
