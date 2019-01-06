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
			
int main(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

	GPIO_InitTypeDef gpio;
	GPIO_StructInit(&gpio);
	gpio.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	gpio.GPIO_Pin = GPIO_Pin_3; 		// RX		!!! PRZEJSCIOWKA 5 -> 3.3
	GPIO_Init(GPIOA, &gpio);

	USART_InitTypeDef uart;
	USART_StructInit(&uart);
	uart.USART_BaudRate = 115200;
	USART_Init(USART2, &uart);

	USART_Cmd(USART2, ENABLE);

	while(1)
	{
		if(USART_GetFlagStatus(USART2, USART_FLAG_RXNE))
		{
			char c = USART_ReceiveData(USART2);

			/*switch(c) ... *
		}
	}
}
