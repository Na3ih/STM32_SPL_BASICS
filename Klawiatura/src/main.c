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
#include <stdio.h>

// - - - - -> PINOUT <- - - - - - - - -
// rows
#define r1_pin GPIO_Pin_11
#define r1_port GPIOB
#define r2_pin GPIO_Pin_10
#define r2_port GPIOB
#define r3_pin GPIO_Pin_1
#define r3_port GPIOB
#define r4_pin GPIO_Pin_0
#define r4_port GPIOB
// columns
#define c1_pin GPIO_Pin_8
#define c1_port GPIOA
#define c2_pin GPIO_Pin_7
#define c2_port GPIOA
#define c3_pin GPIO_Pin_6
#define c3_port GPIOA
#define c4_pin GPIO_Pin_5
#define c4_port GPIOA
// interrupt inputs = columns
#define e1_pin GPIO_PinSource8
#define e1_port GPIO_PortSourceGPIOA
#define e2_pin GPIO_PinSource7
#define e2_port GPIO_PortSourceGPIOA
#define e3_pin GPIO_PinSource6
#define e3_port GPIO_PortSourceGPIOA
#define e4_pin GPIO_PinSource5
#define e4_port GPIO_PortSourceGPIOA

GPIO_TypeDef* row_ports[4] = {r1_port, r2_port, r3_port, r4_port};
uint16_t row_pins[4] = {r1_pin, r2_pin, r3_pin, r4_pin};

GPIO_TypeDef* col_ports[4] = {c1_port, c2_port, c3_port, c4_port};
uint16_t col_pins[4] = {c1_pin, c2_pin, c3_pin, c4_pin };

// - - - - - - -> KEYBOARD MAP <- - - - - - - - -
char but_tab[4][4] = {	{ '1', '2', '3', 'A'},
					 	{ '4', '5', '6', 'B'},
					 	{ '7', '8', '9', 'C'},
					 	{ '*', '0', '#', 'D'}	};

void RCC_Config(void);
void GPIO_Config(void);
void USART_Config(void);
char num_buf[3] = {0};
int ind = 0;
void EXTI_Config(void);
void NVIC_Config(void);

void send_char(char c);
int __io_putchar(int c);
void EXTI_9_5_IRQHandler(void);
int tim = 0;
void SysTick_Handler();
void OutLow(void);
void OutHigh(void);
char GetButton(void);

int main(void)
{
	RCC_Config();
	GPIO_Config();
	USART_Config();
	EXTI_Config();
	NVIC_Config();
	SysTick_Config(SystemCoreClock / 1000);
	OutLow();

	printf("Konfiguracja OK");

	while(1)
	{
		printf("buf: %c %c %c", num_buf[0], num_buf[1], num_buf[2]);
		tim = 1000; while(tim);
		GPIO_ResetBits(GPIOC, GPIO_Pin_13);
	}
}

// - - - - - - - - - - - -> FUNCTIONS <- - - - - - - - - - - - - -
void RCC_Config(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
}

void GPIO_Config(void)
{
	GPIO_InitTypeDef gpio;
	GPIO_StructInit(&gpio);

	gpio.GPIO_Mode = GPIO_Mode_AF_PP;	// TX2
	gpio.GPIO_Pin = GPIO_Pin_2;
	GPIO_Init(GPIOA, &gpio);

	gpio.GPIO_Mode = GPIO_Mode_AF_OD;	// rows, OpenDrain to short circuit protection between rows
	gpio.GPIO_Speed = GPIO_Speed_2MHz;	// minimalize contact vibrations
	for(int i = 0; i < 4; i++)
	{
		gpio.GPIO_Pin = row_pins[i];
		GPIO_Init(row_ports[i], &gpio);
	}

	gpio.GPIO_Mode = GPIO_Mode_IPU;	// cols, INPUT PULLUP
	for(int i = 0; i < 4; i++)
	{
		gpio.GPIO_Pin = col_pins[i];
		GPIO_Init(col_ports[i], &gpio);
	}
}

void USART_Config(void)
{
	USART_InitTypeDef uart;
	USART_StructInit(&uart);
	uart.USART_BaudRate = 115200;
	USART_Init(USART2, &uart);
	USART_Cmd(USART2, ENABLE);
}

void EXTI_Config(void)
{
	GPIO_EXTILineConfig(e1_port, e1_pin);
	GPIO_EXTILineConfig(e2_port, e2_pin);
	GPIO_EXTILineConfig(e3_port, e3_pin);
	GPIO_EXTILineConfig(e4_port, e4_pin);

	EXTI_InitTypeDef ext;
	EXTI_StructInit(&ext);
	ext.EXTI_Mode = EXTI_Mode_Interrupt;
	ext.EXTI_Trigger = EXTI_Trigger_Falling;
	ext.EXTI_Line = e1_pin | e2_pin | e3_pin | e4_pin;
	ext.EXTI_LineCmd = ENABLE;
	EXTI_Init(&ext);
}

void NVIC_Config(void)
{
	NVIC_InitTypeDef nvic;
	nvic.NVIC_IRQChannel = EXTI9_5_IRQn;
	nvic.NVIC_IRQChannelCmd = ENABLE;
	nvic.NVIC_IRQChannelPreemptionPriority = 3;		// least significant because introduce a delay
	nvic.NVIC_IRQChannelSubPriority = 3;			// in Handler()
	NVIC_Init(&nvic);
}

void SysTick_Handler()
{
	if(tim) --tim;
}

void EXTI9_5_IRQHandler(void)
{
	GPIO_SetBits(GPIOC, GPIO_Pin_13);
	printf("PRZERWA");
	char c = GetButton();
	printf("%c\n",c);
	if((c <= 0) && (c >= 9))
		{
			num_buf[ind] = c;
			ind++;
		}
	else if(c == 'A')
		{
			printf("num_buf: %s \n", num_buf);
			num_buf[0] = 0; num_buf[1] = 0; num_buf[2] = 0;
			ind = 0;
		}
	EXTI_ClearITPendingBit(EXTI_Line8 | EXTI_Line7 | EXTI_Line6 | EXTI_Line5);
}

// - - - - - - -> for printf <- - - - - - - - - -
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

void OutLow(void)
{
	GPIO_ResetBits(r1_port, r1_pin);
	GPIO_ResetBits(r2_port, r2_pin);
	GPIO_ResetBits(r3_port, r3_pin);
	GPIO_ResetBits(r4_port, r4_pin);
}

void OutHigh(void)
{
	GPIO_SetBits(r1_port, r1_pin);
	GPIO_SetBits(r2_port, r2_pin);
	GPIO_SetBits(r3_port, r3_pin);
	GPIO_SetBits(r4_port, r4_pin);
}

char GetButton(void)
{
	OutHigh();

	for(int i = 0; i < 4; i++)
	{
		GPIO_ResetBits(row_ports[i], row_pins[i]);
		//tim = 4500; while(tim);

		for(int j = 0; j < 4; j++)
		{
			if(GPIO_ReadInputDataBit(col_ports[j], col_pins[j]) == 0) { OutLow(); return but_tab[i][j]; }
		}

		GPIO_SetBits(row_ports[i], row_pins[i]);
	}

	OutLow();
	return '/';
}
