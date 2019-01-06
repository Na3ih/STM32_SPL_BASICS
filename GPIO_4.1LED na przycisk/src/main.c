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

unsigned int LEDpos = 0x0800;
void EXTI9_5_IRQHandler()
{
	if(EXTI_GetITStatus(EXTI_Line7))
	{
		if(!GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_7)) GPIO_SetBits(GPIOC, GPIO_Pin_13);
		else GPIO_ResetBits(GPIOC, GPIO_Pin_13);
	}
	EXTI_ClearITPendingBit(EXTI_Line7);
}


int main(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOC, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	SysTick_Config(SystemCoreClock/1000);

	GPIO_InitTypeDef gpio;
	GPIO_StructInit(&gpio);
	gpio.GPIO_Mode = GPIO_Mode_Out_PP;
	gpio.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7 |
					GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12;
	GPIO_Init(GPIOB, &gpio);

	gpio.GPIO_Mode = GPIO_Mode_IPU;
	gpio.GPIO_Pin = GPIO_Pin_7;
	GPIO_Init(GPIOA, &gpio);

	gpio.GPIO_Mode = GPIO_Mode_Out_PP;
	gpio.GPIO_Pin = GPIO_Pin_13;
	GPIO_Init(GPIOC, &gpio);

	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource7);
	EXTI_InitTypeDef exti;
	EXTI_StructInit(&exti);
	exti.EXTI_Line = EXTI_Line7;
	exti.EXTI_LineCmd = ENABLE;
	exti.EXTI_Mode = EXTI_Mode_Interrupt;
	exti.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
	EXTI_Init(&exti);

	NVIC_InitTypeDef nvic;
	nvic.NVIC_IRQChannel = EXTI9_5_IRQn;
	nvic.NVIC_IRQChannelCmd = ENABLE;
	nvic.NVIC_IRQChannelPreemptionPriority = 0x00;
	nvic.NVIC_IRQChannelSubPriority = 0x00;
	NVIC_Init(&nvic);

	while(1)
	{
		GPIO_Write(GPIOB, LEDpos);
		tim = 1000; while(tim);
		if(LEDpos > 0x0010) LEDpos = LEDpos >> 1;
		if(LEDpos == 0x0010) LEDpos = 0x0800;
	}
}
