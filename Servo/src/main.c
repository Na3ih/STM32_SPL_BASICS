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
 while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
 USART_SendData(USART1, c);
}

int __io_putchar(int c)
{
    send_char(c);
    return c;
}


void RCC_Configuration(void);
void RCC_Initialize(void);
void GPIO_Initialize(void);
void PWM_Configuration(void);
void PWMTimer_Initialize(void);
void ADC_Initialize(void);
void USART1_Config(void);

int main(void)
{
	RCC_Initialize();
	GPIO_Initialize();
	PWMTimer_Initialize();
	ADC_Initialize();
	USART1_Config();
	printf("OK");
	unsigned int tim = 72000000;

	while(1)
	{
		uint16_t adc = ADC_GetConversionValue(ADC1);
		TIM_SetCompare1(TIM4, adc/2);
		printf("%i\n", adc/2);
	}
}

void RCC_Configuration(void)
{
	/* Ustawienia taktowania peryferiów  */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 | RCC_APB2Periph_USART1, ENABLE);
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);
}

void RCC_Initialize(void)
{
	ErrorStatus HSEStartUpStatus;				// czy ruszyl HSE

	RCC_DeInit(); 								// reset wczeœniejszych ustawieñ taktowania
	RCC_HSEConfig(RCC_HSE_ON);					// w³¹czenie HSE
	HSEStartUpStatus = RCC_WaitForHSEStartUp();	// oczekiwanie na stabilny sygna³

	if(HSEStartUpStatus == SUCCESS)
	{
		FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);
		FLASH_SetLatency(FLASH_Latency_2);		// zw³oka dla FLASH (zale¿ne od taktowania: 0 gdy < 24MHz, 1 dla 24-48MHz, 2 dla > 48MHz)


		RCC_HCLKConfig(RCC_SYSCLK_Div1);			// HCLK = SYSCLK
		RCC_PCLK2Config(RCC_HCLK_Div1);				// PCLK2 = HCLK
		RCC_PCLK1Config(RCC_HCLK_Div2);				// PCLK1 = HCLK / 2

		RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);	// PLLCLK = HSE*9 = 8MHz * 9 = 72MHz
		RCC_PLLCmd(ENABLE);										// uruchom PLL
		while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET);		// czekaj na uruchomienie PLL

		RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);				// ustaw PLL jako zrod³o sygna³u zegarowego
		while(RCC_GetSYSCLKSource() != 0x08);					// czekaj a¿ PLL bedzie sygnalem zegarowym

		/* Ustawienia taktowania peryferiów  */
		RCC_Configuration();
	}
	else {}
}

void GPIO_Initialize(void)
{
	GPIO_InitTypeDef gpio;
	GPIO_StructInit(&gpio);
	gpio.GPIO_Mode = GPIO_Mode_AF_PP;
	gpio.GPIO_Pin = GPIO_Pin_6;			// B6 - PWM to servo
	GPIO_Init(GPIOB, &gpio);

	gpio.GPIO_Mode = GPIO_Mode_AIN;
	gpio.GPIO_Pin = GPIO_Pin_7;			// A7 - analog input
	GPIO_Init(GPIOA, &gpio);

	gpio.GPIO_Mode = GPIO_Mode_AF_PP;	// TX1
	gpio.GPIO_Pin = GPIO_Pin_9;
	GPIO_Init(GPIOA, &gpio);
}

void USART1_Config(void)
{
	USART_InitTypeDef uart;
	USART_StructInit(&uart);
	uart.USART_BaudRate = 115200;
	USART_Init(USART1, &uart);
	USART_Cmd(USART1, ENABLE);
}

void PWM_Configuration(void)
{
	TIM_OCInitTypeDef tim_chanel;
	TIM_OCStructInit(&tim_chanel);
	tim_chanel.TIM_OCMode = TIM_OCMode_PWM1;
	tim_chanel.TIM_OutputState = TIM_OutputState_Enable;
	tim_chanel.TIM_Pulse = 1500;	// 1.5ms
	tim_chanel.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OC1Init(TIM4, &tim_chanel);

	TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Enable);		// buforowanie kana³u 1
	TIM_ARRPreloadConfig(TIM4, ENABLE);						// buforowanie licznika
}

void PWMTimer_Initialize(void)
{
	TIM_TimeBaseInitTypeDef timer;
	TIM_TimeBaseStructInit(&timer);
	timer.TIM_ClockDivision = TIM_CKD_DIV1;
	timer.TIM_CounterMode = TIM_CounterMode_Up;
	timer.TIM_Prescaler = 72 - 1;			// 72M / 72 = 1MHz
	timer.TIM_Period = 20000 - 1;		// 1000000 / 20000 = 50Hz -> T = 20ms
	TIM_TimeBaseInit(TIM4, &timer);

	PWM_Configuration();

	TIM_Cmd(TIM4, ENABLE);
}

void ADC_Initialize(void)
{
	ADC_InitTypeDef adc;
	ADC_StructInit(&adc);
	adc.ADC_ContinuousConvMode = ENABLE;
	adc.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	adc.ADC_NbrOfChannel = 1;
	ADC_Init(ADC1, &adc);

	ADC_RegularChannelConfig(ADC1, ADC_Channel_7, 1, ADC_SampleTime_71Cycles5);
	ADC_Cmd(ADC1, ENABLE);

	ADC_ResetCalibration(ADC1);
	while(ADC_GetResetCalibrationStatus(ADC1));
	ADC_StartCalibration(ADC1);
	while(ADC_GetCalibrationStatus(ADC1));

	ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}
