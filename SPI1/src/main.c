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

void Send_String(const char* s)
{
	while(*s)
	{
		send_char(*s++);
	}
}

int tim = 0;
void SysTick_Handler()
{
	if(tim) --tim;
}

int main(void)
{	printf("dupa52");
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2 | RCC_APB1Periph_I2C1, ENABLE);
	SysTick_Config(SystemCoreClock / 1000);

	GPIO_InitTypeDef gpio;
	GPIO_StructInit(&gpio);

	gpio.GPIO_Mode = GPIO_Mode_AF_PP;		// TX2
	gpio.GPIO_Pin = GPIO_Pin_2;
	GPIO_Init(GPIOA, &gpio);

	gpio.GPIO_Mode = GPIO_Mode_AF_OD;
	gpio.GPIO_Speed = GPIO_Speed_50MHz;
	gpio.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;	// SCL | SDA
	GPIO_Init(GPIOB, &gpio);

	USART_InitTypeDef uart;
	USART_StructInit(&uart);
	uart.USART_BaudRate = 9600;
	USART_Init(USART2, &uart);
	USART_Cmd(USART2, ENABLE);

	I2C_InitTypeDef i2c;
	I2C_StructInit(&i2c);
	i2c.I2C_Mode = I2C_Mode_I2C;
	i2c.I2C_Ack = I2C_Ack_Enable;
	i2c.I2C_DutyCycle = I2C_DutyCycle_2;
	i2c.I2C_ClockSpeed = 100000;
	i2c.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	I2C_Init(I2C1, &i2c);
	I2C_Cmd(I2C1, ENABLE);
// /*
	uint8_t data1;
	uint8_t data2;
void Czytaj()
{
	I2C_GenerateSTART(I2C1, ENABLE);
	while (I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT) != SUCCESS); printf("1");

	I2C_Send7bitAddress(I2C1, 0x90, I2C_Direction_Transmitter);
	while (I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED) != SUCCESS);printf("2");

	I2C_SendData(I2C1, 0x00);
	while (I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED) != SUCCESS); printf("3");

	I2C_GenerateSTART(I2C1, ENABLE);
	while (I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT) != SUCCESS );printf("4");

	I2C_AcknowledgeConfig(I2C1, ENABLE);
	I2C_Send7bitAddress(I2C1, 0x90, I2C_Direction_Receiver);
	I2C_AcknowledgeConfig(I2C1, ENABLE);

	while (I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED) != SUCCESS); printf("5");
	data2= I2C_ReceiveData(I2C1);

	while (I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED) != SUCCESS); printf("5");
    I2C_AcknowledgeConfig(I2C1, DISABLE);
    I2C_GenerateSTOP(I2C1, ENABLE);

    while(I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED) != SUCCESS); printf("6");
    data1 = I2C_ReceiveData(I2C1);

    printf("OK");
}
    //*/
	/*
	 //STEROWANIE DAC
	I2C_GenerateSTART(I2C1, ENABLE);
		while (I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT) != SUCCESS);

		I2C_Send7bitAddress(I2C1, 0x90, I2C_Direction_Transmitter);
		while (I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED) != SUCCESS);

		I2C_SendData(I2C1, 0x40);
		while (I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTING) != SUCCESS);

		I2C_GenerateSTART(I2C1, ENABLE);
				while (I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT) != SUCCESS);

				I2C_Send7bitAddress(I2C1, 0x90, I2C_Direction_Transmitter);
				while (I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED) != SUCCESS);
		I2C_SendData(I2C1, 2000);
				while (I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTING) != SUCCESS);

		I2C_AcknowledgeConfig(I2C1, DISABLE);
	    I2C_GenerateSTOP(I2C1, ENABLE);
	    while(I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED) != SUCCESS);
	    uint8_t data2 = I2C_ReceiveData(I2C1);
	*/
	while (1)
	{
	    Czytaj();
		printf("%lu %lu\n",data1, data2);
		printf("while"); tim=1000; while(tim);
	}

}

