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

void EXTI9_5_IRQHandler()
{
	if(EXTI_GetITStatus(EXTI_Line7))	// czy przerwanie z lini 7?
	{
		if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_7) == 0) GPIO_SetBits(GPIOC, GPIO_Pin_13);
		else GPIO_ResetBits(GPIOC, GPIO_Pin_13);
	}

	EXTI_ClearITPendingBit(EXTI_Line7);		// reset flagi przerwania
}

int main(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOC, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);	// taktowanie modu³u funkcji alternatywnych

	GPIO_InitTypeDef(gpio);
	GPIO_StructInit(&gpio);
	gpio.GPIO_Pin = GPIO_Pin_7;
	gpio.GPIO_Mode = GPIO_Mode_IPU;		// input pullup
	GPIO_Init(GPIOA, &gpio);

	gpio.GPIO_Pin = GPIO_Pin_13;
	gpio.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOC, &gpio);

	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource7);

	EXTI_InitTypeDef exti;
	EXTI_StructInit(&exti);
	exti.EXTI_Line = EXTI_Line7;
	exti.EXTI_Mode = EXTI_Mode_Interrupt;
	exti.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
	exti.EXTI_LineCmd = ENABLE;
	EXTI_Init(&exti);

	NVIC_InitTypeDef nvic;
	nvic.NVIC_IRQChannel = EXTI9_5_IRQn;
	nvic.NVIC_IRQChannelPreemptionPriority = 0x00;
	nvic.NVIC_IRQChannelSubPriority = 0x00;
	nvic.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&nvic);

	while(1){}
}
