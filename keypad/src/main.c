#include "stm32f10x.h"
#include <stdio.h>

// - - - - - - -> KEYBOARD MAP <- - - - - - - - -
char but_tab[4][4] = {	{ '1', '2', '3', 'A'},
					 	{ '4', '5', '6', 'B'},
					 	{ '7', '8', '9', 'C'},
					 	{ '*', '0', '#', 'D'}	};

void RCC_Config(void);
void GPIO_Config(void);
void USART_Config(void);
void EXTI_Config(void);
void NVIC_Config(void);

void send_char(char c);
int __io_putchar(int c);
void OutLow(void);
void OutHigh(void);
//char GetButton(uint16_t pin, int col);
char GetButton(void);
void EXTI9_5_IRQHandler(void);

int main(void)
{
	RCC_Config();
	GPIO_Config();
	USART_Config();
	EXTI_Config();
	NVIC_Config();

	OutLow();
	while(1)
	{

	}
}

void RCC_Config(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	SysTick_Config(SystemCoreClock / 1000);
}

void GPIO_Config(void)
{
	GPIO_InitTypeDef gpio;
	GPIO_StructInit(&gpio);

	gpio.GPIO_Mode = GPIO_Mode_AF_PP;
	gpio.GPIO_Pin = GPIO_Pin_2;			// TX2
	GPIO_Init(GPIOA, &gpio);

	gpio.GPIO_Mode = GPIO_Mode_Out_PP;
	gpio.GPIO_Pin = GPIO_Pin_13;
	GPIO_Init(GPIOC, &gpio);

	gpio.GPIO_Mode = GPIO_Mode_IPU;		// cols
	gpio.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_8 | GPIO_Pin_7 | GPIO_Pin_6;
	GPIO_Init(GPIOA, &gpio);

	gpio.GPIO_Mode = GPIO_Mode_AF_OD;	// rows
	gpio.GPIO_Speed = GPIO_Speed_2MHz;
	gpio.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_10 | GPIO_Pin_9 | GPIO_Pin_8;
	GPIO_Init(GPIOB, &gpio);
}

void USART_Config(void)
{
	USART_InitTypeDef uart;
	USART_StructInit(&uart);
	uart.USART_BaudRate = 115200;
	USART_Init(USART2, &uart);
	USART_Cmd(USART2, ENABLE);
}

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

void EXTI_Config(void)
{
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource9);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource8);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource7);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource6);

	EXTI_InitTypeDef exti;
	EXTI_StructInit(&exti);
	exti.EXTI_Line = EXTI_Line7 | EXTI_Line6 | EXTI_Line8 | EXTI_Line9;
	exti.EXTI_LineCmd = ENABLE;
	exti.EXTI_Mode = EXTI_Mode_Interrupt;
	exti.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_Init(&exti);
}

void NVIC_Config(void)
{
	NVIC_InitTypeDef nvic;
	nvic.NVIC_IRQChannel = EXTI9_5_IRQn;
	nvic.NVIC_IRQChannelCmd = ENABLE;
	nvic.NVIC_IRQChannelPreemptionPriority = 0x00;
	nvic.NVIC_IRQChannelSubPriority = 0x00;
	NVIC_Init(&nvic);
}

void OutLow(void)
{
	GPIO_ResetBits(GPIOB, GPIO_Pin_11 | GPIO_Pin_10 | GPIO_Pin_9 | GPIO_Pin_8);
}

void OutHigh(void)
{
	GPIO_SetBits(GPIOB, GPIO_Pin_11 | GPIO_Pin_10 | GPIO_Pin_9 | GPIO_Pin_8);
}

uint16_t row_pins[4] = {GPIO_Pin_11, GPIO_Pin_10, GPIO_Pin_9, GPIO_Pin_8 };
uint16_t col_pins[4] = {GPIO_Pin_9, GPIO_Pin_8, GPIO_Pin_7, GPIO_Pin_6 };

int tim = 0;
void SysTick_Handler()
{
	if(tim) --tim;
}

char GetButton(void)
{
	OutHigh();

	for(int i = 0; i < 4; i++)
	{
		GPIO_ResetBits(GPIOB, row_pins[i]);		for(unsigned long int l = 0; l < 2000; l++);//tim = 50; while(tim);

		for(int j = 0; j < 4; j++)
		{
			if(GPIO_ReadInputDataBit(GPIOA, col_pins[j]) == 0) return but_tab[i][j];
		}

		GPIO_SetBits(GPIOB, row_pins[i]);		for(unsigned long int l = 0; l < 2000; l++);//tim = 50; while(tim);
	}

	return 'X';
}

void EXTI9_5_IRQHandler(void)
{
	GPIO_SetBits(GPIOC, GPIO_Pin_13);
	char but = GetButton();
	OutLow();
	__io_putchar(but);

	EXTI_ClearITPendingBit(EXTI_Line9 | EXTI_Line8 | EXTI_Line7 | EXTI_Line6);
}


/*
char GetButton(uint16_t pin, int col_num)
{
	OutHigh();
	char znak = '/';
	for(int i = 0; i < 4; i++)
	{
		GPIO_ResetBits(GPIOB, row_pins[i]);
		//for(unsigned long int l = 0; l < 2000000; l++);

		if(GPIO_ReadInputDataBit(GPIOA, pin) == 0) znak =  but_tab[i][col_num];

		GPIO_SetBits(GPIOB, row_pins[i]);
		//for(unsigned long int l = 0; l < 2000000; l++);
	}

	return znak;
}

void EXTI9_5_IRQHandler(void)
{
	uint16_t pin; int col;
	if(EXTI_GetITStatus(EXTI_Line9)) { pin = GPIO_Pin_9; col = 0; }
	if(EXTI_GetITStatus(EXTI_Line8)) { pin = GPIO_Pin_8; col = 1; }
	if(EXTI_GetITStatus(EXTI_Line7)) { pin = GPIO_Pin_7; col = 2; }
	if(EXTI_GetITStatus(EXTI_Line6)) { pin = GPIO_Pin_6; col = 3; }

	GPIO_SetBits(GPIOC, GPIO_Pin_13);
	char but = GetButton(pin, col);
	OutLow();
	__io_putchar(but);

	EXTI_ClearITPendingBit(EXTI_Line9 | EXTI_Line8 | EXTI_Line7 | EXTI_Line6);
}

*/
