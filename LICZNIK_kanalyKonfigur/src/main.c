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
			
void TIM2_IRQHandler()
{
	if(TIM_GetITStatus(TIM2, TIM_IT_Update) == SET)
	{
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
		GPIO_SetBits(GPIOB, GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8);
	}
	if(TIM_GetITStatus(TIM2, TIM_IT_CC1) == SET)
	{
		TIM_ClearITPendingBit(TIM2, TIM_IT_CC1);
		GPIO_ResetBits(GPIOB, GPIO_Pin_8);
	}
	if(TIM_GetITStatus(TIM2, TIM_IT_CC2) == SET)
	{
		TIM_ClearITPendingBit(TIM2, TIM_IT_CC2);
		GPIO_ResetBits(GPIOB, GPIO_Pin_5);
	}
	if(TIM_GetITStatus(TIM2, TIM_IT_CC3) == SET)
	{
		TIM_ClearITPendingBit(TIM2, TIM_IT_CC3);
		GPIO_ResetBits(GPIOB, GPIO_Pin_6);
	}
	if(TIM_GetITStatus(TIM2, TIM_IT_CC4) == SET)
	{
		TIM_ClearITPendingBit(TIM2, TIM_IT_CC4);
		GPIO_ResetBits(GPIOB, GPIO_Pin_7);
	}
}

int main(void)
{

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

	GPIO_InitTypeDef gpio;
	GPIO_StructInit(&gpio);
	gpio.GPIO_Mode = GPIO_Mode_Out_PP;
	gpio.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8;
	GPIO_Init(GPIOB, &gpio);

	TIM_TimeBaseInitTypeDef timer;
	TIM_TimeBaseStructInit(&timer);
	timer.TIM_CounterMode = TIM_CounterMode_Up;
	timer.TIM_Prescaler = 64000 - 1;
	timer.TIM_Period = 1000 - 1;
	TIM_TimeBaseInit(TIM2, &timer);

	TIM_ITConfig(TIM2, TIM_IT_Update | TIM_IT_CC1 | TIM_IT_CC2 | TIM_IT_CC3 | TIM_IT_CC4, ENABLE);
	TIM_Cmd(TIM2, ENABLE);

	TIM_OCInitTypeDef chanel;
	TIM_OCStructInit (&chanel);
	chanel.TIM_OCMode = TIM_OCMode_Timing;
	chanel.TIM_Pulse = 100;
	TIM_OC1Init(TIM2, &chanel);
	chanel.TIM_Pulse = 200;
	TIM_OC2Init(TIM2, &chanel);
	chanel.TIM_Pulse = 400;
	TIM_OC3Init(TIM2, &chanel);
	chanel.TIM_Pulse = 900;
	TIM_OC4Init(TIM2, &chanel);

	NVIC_InitTypeDef przerwanie;
	przerwanie.NVIC_IRQChannel = TIM2_IRQn;
	przerwanie.NVIC_IRQChannelPreemptionPriority = 0;
	przerwanie.NVIC_IRQChannelSubPriority = 0;
	przerwanie.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&przerwanie);


	while(1);
}
