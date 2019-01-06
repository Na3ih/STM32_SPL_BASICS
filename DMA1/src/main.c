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

#define BUFFER_SIZE 32

volatile uint8_t src_buffer[BUFFER_SIZE];
volatile uint8_t dst_buffer[BUFFER_SIZE];
volatile uint8_t dst2_buffer[BUFFER_SIZE];
void copy_cpu()
{
	for(int i = 0; i < BUFFER_SIZE; i++)
	{
		dst_buffer[i] = src_buffer[i];
	}
}

void copy_dma()
{
	DMA_Cmd(DMA1_Channel1, ENABLE);
	while(DMA_GetFlagStatus(DMA1_FLAG_TC1) == RESET);
}

uint32_t tim = 0;
void SysTick_Handler()
{
	if(tim) --tim;
}


int main(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	SysTick_Config(SystemCoreClock / 1000);

	GPIO_InitTypeDef gpio;
	GPIO_StructInit(&gpio);
	gpio.GPIO_Mode = GPIO_Mode_AF_PP;
	gpio.GPIO_Pin = GPIO_Pin_2;		// TX2
	GPIO_Init(GPIOA, &gpio);

	gpio.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	gpio.GPIO_Pin = GPIO_Pin_3;		// RX2
	GPIO_Init(GPIOA, &gpio);

	USART_InitTypeDef uart;
	USART_StructInit(&uart);
	uart.USART_BaudRate = 9600;
	USART_Init(USART2, &uart);
	USART_Cmd(USART2, ENABLE);

	DMA_InitTypeDef dma;
	DMA_StructInit(&dma);
	dma.DMA_PeripheralBaseAddr = (uint32_t)src_buffer;		// adres zrodlowy (peryferjyjny)
	dma.DMA_PeripheralInc = DMA_PeripheralInc_Enable;
	dma.DMA_MemoryBaseAddr = (uint32_t)dst_buffer;			// adres docelowy (pamieci RAM)
	dma.DMA_MemoryInc = DMA_MemoryInc_Enable;
	dma.DMA_BufferSize = BUFFER_SIZE;
	dma.DMA_M2M = DMA_M2M_Enable;
	DMA_Init(DMA1_Channel1, &dma);					// dma1 kanal1 , mozna 12 na raz

	for(int i = 0; i < BUFFER_SIZE; i++) src_buffer[i] = 100 + i;		// dane przykladowe


	while(1)
	{
		printf("\n\nSOURCE: ");
		for(int i = 0; i < BUFFER_SIZE; i++)
		{
			printf("%lu, ", src_buffer[i]);
		}

		copy_cpu();
		printf(" \n\n CPU: ");
		for(int i = 0; i < BUFFER_SIZE; i++)
		{
			printf("%lu, ", dst_buffer[i]);
		}
															// porownanie czy takie same
		copy_dma();
		printf("\n\n DMA: ");
		for(int i = 0; i < BUFFER_SIZE; i++)
		{
			printf("%lu, ", dst_buffer[i]);
		}
		tim = 10000; while(tim);
	}
}
