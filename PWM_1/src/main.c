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
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
	SysTick_Config(SystemCoreClock / 1000);

	GPIO_InitTypeDef gpio;
	GPIO_StructInit(&gpio);
	gpio.GPIO_Mode = GPIO_Mode_AF_PP;
	gpio.GPIO_Speed = GPIO_Speed_50MHz;
	gpio.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9, ENABLE;
	GPIO_Init(GPIOB, &gpio);

	TIM_TimeBaseInitTypeDef timer;
	TIM_TimeBaseStructInit(&timer);
	timer.TIM_CounterMode = TIM_CounterMode_Up;
	timer.TIM_Prescaler = 64000 - 1;
	timer.TIM_Period = 1000 - 1;
	TIM_TimeBaseInit(TIM4, &timer);

	TIM_OCInitTypeDef chanels;
	TIM_OCStructInit(&chanels);
	chanels.TIM_OCMode = TIM_OCMode_PWM1;	// 1 -> 0
	chanels.TIM_OutputState = TIM_OutputState_Enable;
	chanels.TIM_Pulse = 100;
	TIM_OC1Init(TIM4, &chanels);
	/*chanels.TIM_Pulse = 200;
	TIM_OC2Init(TIM4, &chanels);
	chanels.TIM_Pulse = 500;
	TIM_OC3Init(TIM4, &chanels);
	chanels.TIM_Pulse = 900;
	TIM_OC4Init(TIM4, &chanels);*/

	TIM_Cmd(TIM4, ENABLE);

	while(1)
	{
		for(int i = 0; i < 1000; i=i+100)
		{
			TIM_SetCompare1(TIM4, i);
			tim = 5000; while(tim);
		}

	}

}
