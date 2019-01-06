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
			
#include <stdio.h>

void send_char(char c)
{
 while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
 USART_SendData(USART2, c);
}

int __io_putchar(int c)
{
    send_char(c);
    return c;
}

unsigned int tim = 0;

void SysTick_Handler()
{
	if(tim) --tim;
}

unsigned char stre[10] = "DUPAMARYNY";
int main(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	SysTick_Config(SystemCoreClock / 1000);

	GPIO_InitTypeDef gpio;
	GPIO_StructInit(&gpio);
	gpio.GPIO_Mode = GPIO_Mode_AF_PP;	// TX
	gpio.GPIO_Pin = GPIO_Pin_2;
	GPIO_Init(GPIOA, &gpio);

	USART_InitTypeDef uart;
	USART_StructInit(&uart);
	uart.USART_BaudRate = 9600;
	USART_Init(USART2, &uart);
	USART_Cmd(USART2, ENABLE);


	while(1)
	{
		printf("INT: %i \n", 2);
		printf("FLOAT: %u \n", 2.5);
		printf("CHAR: %c \n", 'a' );
		printf("%s \n", stre);
		printf("STRING: %s \n", "SPIERDALAJ");
		printf("NO i CZeœc");
		printf("A teraz wszystko razem : %i, %c, %s, %d i O! \n", 5, 'X', stre, 25.25);
		tim = 1000; while(tim);
	}

}
