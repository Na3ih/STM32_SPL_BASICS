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

unsigned int tim;
void SysTick_Handler()
{
	if(tim) --tim;
}

void Send_Char(char c)
{
	while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
	USART_SendData(USART2, c);
}

void Send_String(const char* s)
{
	while(*s)
	{
		Send_Char(*s++);
	}
}
int main(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	SysTick_Config(SystemCoreClock / 1000);

	GPIO_InitTypeDef gpio;
	GPIO_StructInit(&gpio);
	gpio.GPIO_Mode = GPIO_Mode_AF_PP;	// alternative push-pull
	gpio.GPIO_Pin = GPIO_Pin_2;			// TX2
	GPIO_Init(GPIOA, &gpio);

	USART_InitTypeDef uart;
	USART_StructInit(&uart);
	uart.USART_BaudRate = 115200;
	USART_Init(USART2, &uart);

	USART_Cmd(USART2, ENABLE);

	while(1)
	{
		Send_Char('a');
		tim = 1000; while(tim);
		Send_Char('b');
		tim = 1000; while(tim);
		Send_String("HELLO MADA FakA Krejz! sZ|T \r\n");
	}
}
