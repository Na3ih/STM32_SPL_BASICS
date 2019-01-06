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

int main(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

	GPIO_InitTypeDef gpio;
	GPIO_StructInit(&gpio);
	gpio.GPIO_Mode = GPIO_Mode_Out_PP;
	gpio.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6;
	GPIO_Init(GPIOB, &gpio);

	gpio.GPIO_Mode = GPIO_Mode_AF_PP;	// TX
	gpio.GPIO_Pin = GPIO_Pin_2;
	GPIO_Init(GPIOA, &gpio);

	gpio.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	gpio.GPIO_Pin = GPIO_Pin_3;		// RX
	GPIO_Init(GPIOA, &gpio);

	USART_InitTypeDef uart;
	USART_StructInit(&uart);
	uart.USART_BaudRate = 9600;
	USART_Init(USART2, &uart);
	USART_Cmd(USART2, ENABLE);

	unsigned char buf[3] = {0}; const char buf2[3] = "on1";
	unsigned char bufIndex = 0;
	unsigned char znak;
	int ilosc_znakow = 0;

	int TakieSame(unsigned char tab1[3], unsigned char tab2[3])
	{
		for(int i = 0; i < 3; i++)
		{
			if(tab1[i] != tab2[i]) return 0;
		}

		return 1;
	}


	while(1)
	{
			if(USART_GetFlagStatus(USART2, USART_FLAG_RXNE))
			{
				znak = USART_ReceiveData(USART2);

				if(znak != '\r')
				{
					if((bufIndex < 3) && (znak != '\n'))
					{
						buf[bufIndex] = znak;
						bufIndex++;
					}
				}
				else
				{
					printf("Jako string: %s \n\r", buf);
					if(TakieSame(buf, buf2) == 1) GPIO_SetBits(GPIOB, GPIO_Pin_5);
					if(TakieSame(buf, "of1")) GPIO_ResetBits(GPIOB, GPIO_Pin_5);
					bufIndex = 0;
					znak = 0;
				}

			}










		/*if(USART_GetFlagStatus(USART2, USART_FLAG_RXNE))
		{
			char c = USART_ReceiveData(USART2);

			switch(c)
			{
				case 'A':
					GPIO_SetBits(GPIOB, GPIO_Pin_6);
					break;
				case 'a':
					GPIO_ResetBits(GPIOB, GPIO_Pin_6);
					break;
			}
		}	*/
	}

}
