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
			
void RCC_Config(void);
void GPIO_Config(void);
void TIM2_Config(void);
	void PWM_Config(void);	// w TIM2_Config()
	void ECHO_Config(void);	// w TIM2_Config()
void NVIC_Config(void);

volatile int START, STOP;		// time of HIGH at ECHO
int GetECHO(void);
float GetDistance();

#include <stdio.h>

void send_char(char c)
{
 while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
 USART_SendData(USART1, c);
}

int __io_putchar(int c)
{
    send_char(c);
    return c;
}
// - - - - - - - - - - - - - - - - - -
int main(void)
{
	RCC_Config();
	GPIO_Config();
	TIM2_Config();
	NVIC_Config();
	USART1_Config();
	send_char('c');
	while(1)
	{
		long dist = GetDistance();
		printf("START: %i   STOP: %i   -- %ul \n", START, STOP, dist);
		if(dist > 10.0) GPIO_ResetBits(GPIOC, GPIO_Pin_13);
		else GPIO_SetBits(GPIOC, GPIO_Pin_13);
	}

}

// - - - - - - - - - - - - - - - - - -

void RCC_Config(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO | RCC_APB2Periph_USART1, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,  ENABLE);
}

void GPIO_Config(void)
{
	GPIO_InitTypeDef gpio;
	GPIO_StructInit(&gpio);
	gpio.GPIO_Mode = GPIO_Mode_AF_PP;
	gpio.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_9;			// TIM2CH2, USART1 TX
	GPIO_Init(GPIOA, &gpio);

	gpio.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	gpio.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;		// TIM2 Ch3, Ch4
	GPIO_Init(GPIOA, &gpio);

	gpio.GPIO_Mode = GPIO_Mode_Out_PP;
	gpio.GPIO_Pin = GPIO_Pin_13;				// LED
	GPIO_Init(GPIOC, &gpio);
}

void USART1_Config(void)
{
	USART_InitTypeDef uart;
	USART_StructInit(&uart);
	uart.USART_BaudRate = 115200;
	USART_Init(USART1, &uart);
	USART_Cmd(USART1, ENABLE);
}

void TIM2_Config(void)
{
	TIM_TimeBaseInitTypeDef timer;
	TIM_TimeBaseStructInit(&timer);
	timer.TIM_ClockDivision = TIM_CKD_DIV1;
	timer.TIM_CounterMode = TIM_CounterMode_Up;
	timer.TIM_Prescaler = 72 - 1;					// 72MHz / 72 = 1MHz
	timer.TIM_Period = 50000;						// 50ms
	TIM_TimeBaseInit(TIM2, &timer);

	PWM_Config();
	ECHO_Config();

	TIM_Cmd(TIM2, ENABLE);
}

void PWM_Config(void)
{
	TIM_OCInitTypeDef pwm;
	TIM_OCStructInit(&pwm);
	pwm.TIM_OCMode = TIM_OCMode_PWM1;
	pwm.TIM_OutputState = TIM_OutputState_Enable;
	pwm.TIM_OCPolarity = TIM_OCPolarity_High;
	pwm.TIM_Pulse = 10;								// 10us
	TIM_OC2Init(TIM2, &pwm);

	TIM_OC1PreloadConfig(TIM2, TIM_OCPreload_Enable);
	TIM_ARRPreloadConfig(TIM2, ENABLE);
}

void ECHO_Config(void)
{
	TIM_ICInitTypeDef ch1;
	TIM_ICStructInit(&ch1);
	ch1.TIM_Channel = TIM_Channel_3;
	ch1.TIM_ICFilter = 0;									// wy³¹czony
	ch1.TIM_ICPolarity = TIM_ICPolarity_Rising;				// narastaj¹ce zbocze na ECHO
	ch1.TIM_ICSelection = TIM_ICSelection_DirectTI;
	ch1.TIM_ICPrescaler = TIM_ICPSC_DIV1;
	TIM_ICInit(TIM2, &ch1);

	TIM_ICInitTypeDef ch2;
	TIM_ICStructInit(&ch2);
	ch2.TIM_Channel = TIM_Channel_4;
	ch2.TIM_ICFilter = 0;
	ch2.TIM_ICPolarity = TIM_ICPolarity_Falling;			//opadaj¹ce zbocze na ECHO
	ch2.TIM_ICSelection = TIM_ICSelection_DirectTI;
	ch2.TIM_ICPrescaler = TIM_ICPSC_DIV1;
	TIM_ICInit(TIM2, &ch2);

	TIM_ITConfig(TIM2, TIM_IT_CC3, ENABLE);					// W£¥CZENIE PRZERWAÑ OD KANALU 3 I 4
	TIM_ITConfig(TIM2, TIM_IT_CC4, ENABLE);					// TIMERA 2
}

void NVIC_Config(void)
{
	NVIC_InitTypeDef interrupts;
	interrupts.NVIC_IRQChannel = TIM2_IRQn;
	interrupts.NVIC_IRQChannelCmd = ENABLE;
	interrupts.NVIC_IRQChannelPreemptionPriority = 0;
	interrupts.NVIC_IRQChannelSubPriority = 0;
	NVIC_Init(&interrupts);
}

void TIM2_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM2, TIM_IT_CC3) != RESET)
	{
		TIM_ClearITPendingBit(TIM2, TIM_IT_CC3);
		START = TIM_GetCapture3(TIM2);
	}

	else if(TIM_GetITStatus(TIM2, TIM_IT_CC4) != RESET)
	{
		TIM_ClearITPendingBit(TIM2, TIM_IT_CC4);
		STOP = TIM_GetCapture4(TIM2);
	}
}

int GetECHO(void)
{
	return STOP - START;
}

float GetDistance()
{
	return (STOP - START) / 58.0;
}
