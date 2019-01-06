/**
  ******************************************************************************
  * @file    main.c
  * @author  Ac6
  * @version V1.0
  * @date    01-December-2013
  * @brief   Default main function.
  ******************************************************************************
*/

#include <math.h>
#include "stm32f10x.h"
#include "stm32f1xx_nucleo.h"

volatile uint32_t tim;
void SysTick_Handler()
{
	if(tim) --tim;
}

float calc_pwm(float val)
{
 const float k = 0.1f;
 const float x0 = 60.0f;
 return 300.0f / (1.0f + exp(-k * (val - x0)));
}

int main(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
	SysTick_Config(SystemCoreClock / 1000);

	GPIO_InitTypeDef gpio;
	GPIO_StructInit(&gpio);
	gpio.GPIO_Mode = GPIO_Mode_AF_PP;
	gpio.GPIO_Speed = GPIO_Speed_2MHz;
	gpio.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8;
	GPIO_Init(GPIOB, &gpio);

	TIM_TimeBaseInitTypeDef timer;
	TIM_TimeBaseStructInit(&timer);
	timer.TIM_CounterMode = TIM_CounterMode_Up;
	timer.TIM_Prescaler = 64 - 1;
	timer.TIM_Period = 1000 - 1;
	TIM_TimeBaseInit(TIM4, &timer);

	TIM_OCInitTypeDef chanel;
	TIM_OCStructInit(&chanel);
	chanel.TIM_OCMode = TIM_OCMode_PWM2; 	// 0 -> 1
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

	int counter = 0;

	while(1)
	{
		 float r = 50 * (1.0f + sin(counter / 100.0f));
		 float g = 50 * (1.0f + sin(1.5f * counter / 100.0f));
		 float b = 50 * (1.0f + sin(2.0f * counter / 100.0f));
		 TIM_SetCompare1(TIM4, calc_pwm(b));
		 TIM_SetCompare2(TIM4, calc_pwm(g));
		 TIM_SetCompare3(TIM4, calc_pwm(r));
		 tim = 500; while(tim);
		 counter++;
	}
}
