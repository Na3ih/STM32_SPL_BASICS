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

void RCC_config()
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2 | RCC_APB1Periph_TIM4, ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

	RCC_ADCCLKConfig(RCC_PCLK2_Div6);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
}

void GPIO_config()
{
	GPIO_InitTypeDef gpio;
	GPIO_StructInit(&gpio);
	gpio.GPIO_Mode = GPIO_Mode_AF_PP; 		// TX
	gpio.GPIO_Pin = GPIO_Pin_2;
	GPIO_Init(GPIOA, &gpio);

	gpio.GPIO_Mode = GPIO_Mode_AF_PP;
	gpio.GPIO_Speed = GPIO_Speed_2MHz;
	gpio.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8;
	GPIO_Init(GPIOB, &gpio);
}

void USART_config()
{
	USART_InitTypeDef uart;
	USART_StructInit(&uart);
	uart.USART_BaudRate = 115200;
	USART_Init(USART2, &uart);
	USART_Cmd(USART2, ENABLE);
}

volatile uint16_t adc_values[2];
void DMA_config()
{
	DMA_InitTypeDef dma;
	DMA_StructInit(&dma);
	dma.DMA_BufferSize = 2;
	dma.DMA_DIR = DMA_DIR_PeripheralSRC;
	dma.DMA_MemoryBaseAddr = (uint32_t)adc_values;
	dma.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	dma.DMA_MemoryInc = DMA_MemoryInc_Enable;
	dma.DMA_Mode = DMA_Mode_Circular;
	dma.DMA_PeripheralBaseAddr = (uint32_t)&ADC1->DR;
	dma.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	dma.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_Init(DMA1_Channel1, &dma);
	DMA_Cmd(DMA1_Channel1, ENABLE);
}

void ADC_config()
{
	ADC_InitTypeDef adc;
	ADC_StructInit(&adc);
	adc.ADC_ContinuousConvMode = ENABLE;
	adc.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	adc.ADC_NbrOfChannel = 2;
	adc.ADC_ScanConvMode = ENABLE;
	ADC_Init(ADC1, &adc);

	ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_239Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 2, ADC_SampleTime_239Cycles5);
	ADC_DMACmd(ADC1, ENABLE);
	ADC_Cmd(ADC1, ENABLE);

	ADC_ResetCalibration(ADC1); while(ADC_GetResetCalibrationStatus(ADC1));
	ADC_StartCalibration(ADC1); while(ADC_GetCalibrationStatus(ADC1));

	ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}

void PWM_config()
{
	TIM_TimeBaseInitTypeDef timer;
	TIM_TimeBaseStructInit(&timer);
	timer.TIM_CounterMode = TIM_CounterMode_Up;
	timer.TIM_Prescaler = 64 - 1;
	timer.TIM_Period = 1000 - 1;
	TIM_TimeBaseInit(TIM4, &timer);

	TIM_OCInitTypeDef channels;
	TIM_OCStructInit(&channels);
	channels.TIM_OCMode = TIM_OCMode_PWM1;
	channels.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OC2Init(TIM4, &channels);
	TIM_OC3Init(TIM4, &channels);
	TIM_Cmd(TIM4, ENABLE);

	NVIC_InitTypeDef nvic;
	nvic.NVIC_IRQChannel = TIM4_IRQn;
	nvic.NVIC_IRQChannelCmd = ENABLE;
	nvic.NVIC_IRQChannelPreemptionPriority = 0;
	nvic.NVIC_IRQChannelSubPriority = 0;
	NVIC_Init(&nvic);
}

int tim = 0;
void TIM4_IRQHandler()
{
	tim--;
}


int main(void)
{

	RCC_config();
	GPIO_config();
	USART_config();
	DMA_config();
	ADC_config();
	PWM_config();


	while(1)
	{
		printf("ADC0: %d \nADC1: %d \n", adc_values[0], adc_values[1]);

		if(adc_values[0] > adc_values[1])
		{
			TIM_SetCompare2(TIM4, 900);
			TIM_SetCompare3(TIM4, 100);
		}
		else
		{
			TIM_SetCompare2(TIM4, 100);
			TIM_SetCompare3(TIM4, 900);
		}

		//tim = 10; while(tim);
	}
}
