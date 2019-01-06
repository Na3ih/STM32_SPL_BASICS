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

int Daj(char color, char bufor[])
{
	int i = 0, mul = 100, val = 0;

	while(bufor[i] != color) i++;

	for(int j = i; j < i+4; j++)
	{
		val = (int)bufor[j] * mul;
		mul = mul / 10;
	}

	return val;
}

int main(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3 | RCC_APB1Periph_TIM4, ENABLE);

	GPIO_InitTypeDef gpio;
	GPIO_StructInit(&gpio);
	gpio.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	gpio.GPIO_Pin = GPIO_Pin_3;
	GPIO_Init(GPIOA, &gpio);

	gpio.GPIO_Mode = GPIO_Mode_AF_PP;
	gpio.GPIO_Pin = GPIO_Pin_2;
	GPIO_Init(GPIOA, &gpio);

	gpio.GPIO_Mode = GPIO_Mode_AF_PP;
	gpio.GPIO_Speed = GPIO_Speed_2MHz;
	gpio.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8;
	GPIO_Init(GPIOB, &gpio);

	TIM_TimeBaseInitTypeDef timer;
	TIM_TimeBaseStructInit(&timer);
	timer.TIM_CounterMode = TIM_CounterMode_Up;
	timer.TIM_Prescaler = 64 - 1;  // 1MHz
	timer.TIM_Period = 1000 - 1;
	TIM_TimeBaseInit(TIM4, &timer);

	TIM_OCInitTypeDef chanel;
	TIM_OCStructInit(&chanel);
	chanel.TIM_OCMode = TIM_OCMode_PWM2;
	chanel.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OC1Init(TIM4, &chanel);
	TIM_OC2Init(TIM4, &chanel);
	TIM_OC3Init(TIM4, &chanel);
	TIM_Cmd(TIM4, ENABLE);

	NVIC_InitTypeDef przerwanie;
	przerwanie.NVIC_IRQChannel = TIM4_IRQn;
	przerwanie.NVIC_IRQChannelCmd = ENABLE;
	przerwanie.NVIC_IRQChannelPreemptionPriority = 0;
	przerwanie.NVIC_IRQChannelSubPriority = 0;
	NVIC_Init(&przerwanie);

	USART_InitTypeDef uart;
	USART_StructInit(&uart);
	uart.USART_BaudRate = 9600;
	USART_Init(USART2, &uart);

	USART_Cmd(USART2, ENABLE);
	//printf("USART OK");

    char buf[] = {0};
	unsigned int bufIndex = 0;

	while(1)
	{
		if(USART_GetFlagStatus(USART2, USART_FLAG_RXNE))
		{
			char znak = USART_ReceiveData(USART2);
			//printf("get \n");
			if(znak != '\r' && znak != '\n')
			{
				buf[bufIndex] = znak;
				bufIndex++;
			}
			else
			{
				bufIndex = 0;

				int red = Daj('r', buf);
				int blue = Daj('b', buf);
				int green = Daj('g', buf);
				printf("RED: %d BLUE: %d, GREEN: %d \n", red, blue, green);

				TIM_SetCompare3(TIM4, blue);
				TIM_SetCompare2(TIM4, green);
				TIM_SetCompare1(TIM4, red);
			}
		}
	}
}
