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

unsigned int tim = 0;
void SysTick_Handler()
{
	if(tim) --tim;
}

int main(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	SysTick_Config(SystemCoreClock / 1000);

	GPIO_InitTypeDef(gpio);
	GPIO_StructInit(&gpio);
	gpio.GPIO_Mode = GPIO_Mode_Out_PP;
	gpio.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_Init(GPIOB, &gpio);

	unsigned int slowo = 0x0800;

	while(1)
	{
		// Wpisz
		GPIO_Write(GPIOB, slowo);
		tim = 1000; while(tim);

		slowo = slowo >> 1;
		if(slowo == 0x0008) slowo = 0x0800;
	}

}
