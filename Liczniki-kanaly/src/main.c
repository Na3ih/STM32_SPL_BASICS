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
		GPIO_SetBits(GPIOB, GPIO_Pin_8 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7);
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
	gpio.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_Init(GPIOB, &gpio);

	TIM_TimeBaseInitTypeDef timer;
	TIM_TimeBaseStructInit(&timer);
	timer.TIM_CounterMode = TIM_CounterMode_Up;
	timer.TIM_Prescaler = 64000 - 1;
	timer.TIM_Period = 1000 - 1;
	TIM_TimeBaseInit(TIM2, &timer);

	TIM_ITConfig(TIM2, TIM_IT_Update | TIM_IT_CC1 | TIM_IT_CC2 | TIM_IT_CC3 | TIM_IT_CC4, ENABLE);
	TIM_Cmd(TIM2, ENABLE);

	TIM_SetCompare1(TIM2, 100);	// 100ms
	TIM_SetCompare2(TIM2, 200);
	TIM_SetCompare3(TIM2, 500);
	TIM_SetCompare4(TIM2, 900);

	NVIC_InitTypeDef przerwanie;
	przerwanie.NVIC_IRQChannel = TIM2_IRQn;
	przerwanie.NVIC_IRQChannelCmd = ENABLE;
	przerwanie.NVIC_IRQChannelPreemptionPriority = 0x00;
	przerwanie.NVIC_IRQChannelSubPriority = 0x00;
	NVIC_Init(&przerwanie);

	while(1);
}
