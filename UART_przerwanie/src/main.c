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
///////////////////////////////////////////////////////////
int TakieSame(char tab1[3], char tab2[3] )
{
	for(int i = 0; i < 3; i++)
	{
		if(tab1[i] != tab2[i]) return 0;
	}
	return 1;
}
///////////////////////////////////////////////
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

///////////////////////////////////////////////
char buf[3] = {0};
char wlacz[3] = "on1";
char wylacz[3] = "of1";
int bufIndex = 0;
int flaga = 0;

void USART2_IRQHandler()
{
	printf("przerwa");
	if(USART_GetITStatus(USART2, USART_IT_RXNE))
	{
		char znak = USART_ReceiveData(USART2);
		if(znak != '\r')
		{
			if((bufIndex < 3) && (znak != '\n'))
			{
				buf[bufIndex] = znak;
				bufIndex++;
			}
		}
		else flaga = 1;
	USART_ClearITPendingBit(USART2, USART_IT_RXNE);
	}
}
/////////////////////////////////////

int main(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

	GPIO_InitTypeDef gpio;
	GPIO_StructInit(&gpio);
	gpio.GPIO_Mode = GPIO_Mode_Out_PP;
	gpio.GPIO_Pin = GPIO_Pin_6;
	GPIO_Init(GPIOB, &gpio);

	gpio.GPIO_Mode = GPIO_Mode_AF_PP;	// TX
	gpio.GPIO_Pin = GPIO_Pin_2;
	GPIO_Init(GPIOA, &gpio);

	gpio.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	gpio.GPIO_Pin = GPIO_Pin_3;		// RX
	GPIO_Init(GPIOA, &gpio);
	/*
	EXTI_InitTypeDef exti;
	EXTI_StructInit(&exti);
	exti.EXTI_Line = EXTI_Line3;
	exti.EXTI_Mode = EXTI_Mode_USART;
	exti.EXTI_LineCmd = ENABLE;
	exti.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
	EXTI_Init(&exti);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource3);
	*/

	USART_InitTypeDef uart;
	USART_StructInit(&uart);
	uart.USART_BaudRate = 115200;
	USART_Init(USART2, &uart);
	USART_Cmd(USART2, ENABLE);
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);

	NVIC_InitTypeDef nvic;
	nvic.NVIC_IRQChannel = USART2_IRQn;
	nvic.NVIC_IRQChannelCmd = ENABLE;
	nvic.NVIC_IRQChannelPreemptionPriority = 0x00;
	nvic.NVIC_IRQChannelSubPriority = 0x00;
	NVIC_Init(&nvic);


	while(1)
	{
		if(flaga)
		{
			if(TakieSame(buf, wlacz)) GPIO_SetBits(GPIOB,GPIO_Pin_6);
			if(TakieSame(buf, wylacz)) GPIO_ResetBits(GPIOB, GPIO_Pin_6);
			printf("FLAGA");
			printf("Wyswietlam oraz string: %s \r", buf);

			buf[0] = '0'; buf[1] = '0'; buf[2] = '0';
			bufIndex = 0;
			flaga = 0;
		}
	}
}
