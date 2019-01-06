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

int tim = 0;
void SysTick_Handler()
{
	if(tim) --tim;
}

int main(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1 | RCC_APB1Periph_USART2, ENABLE);
	SysTick_Config(SystemCoreClock / 1000);

	GPIO_InitTypeDef gpio;
	GPIO_StructInit(&gpio);

	gpio.GPIO_Mode = GPIO_Mode_AF_PP;
	gpio.GPIO_Pin = GPIO_Pin_2; //TX2
	GPIO_Init(GPIOA, &gpio);

	gpio.GPIO_Mode = GPIO_Mode_AF_OD;
	gpio.GPIO_Speed = GPIO_Speed_50MHz;
	gpio.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_Init(GPIOB, &gpio);

	USART_InitTypeDef uart;
	USART_StructInit(&uart);
	uart.USART_BaudRate = 115200;
	USART_Init(USART2, &uart);
	USART_Cmd(USART2, ENABLE);

	I2C_InitTypeDef i2c;
	I2C_StructInit(&i2c);
	i2c.I2C_Ack = I2C_Ack_Enable;
	i2c.I2C_ClockSpeed = 100000;
	i2c.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	i2c.I2C_DutyCycle = I2C_DutyCycle_2;
	i2c.I2C_Mode = I2C_Mode_I2C;
	I2C_Init(I2C1, &i2c);
	I2C_Cmd(I2C1, ENABLE);

	while(1)
	{
		// Ustaw
		I2C_GenerateSTART(I2C1, ENABLE);
		while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT)); printf("START ok  ");

		I2C_Send7bitAddress(I2C1, 0x40, I2C_Direction_Transmitter);
		while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)); printf("TRANSMITER ok  ");

		I2C_SendData(I2C1, 0x0A);
		while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED)); printf("WYSLANO ok  ");

		I2C_AcknowledgeConfig(I2C1, DISABLE);
		I2C_GenerateSTOP(I2C1, ENABLE);	printf("STOP\n");

		tim = 2000; while(tim);

		//Czytaj
		I2C_GenerateSTART(I2C1, ENABLE);
		while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT)) printf("START ok  ");

		I2C_AcknowledgeConfig(I2C1, ENABLE);

		I2C_Send7bitAddress(I2C1, 0x41, I2C_Direction_Receiver);
		while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED)) printf("RECEIVER ok  ");

		I2C_AcknowledgeConfig(I2C1, DISABLE);
		I2C_GenerateSTOP(I2C1, ENABLE);	printf("STOP  ");

		while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED)); printf("ODEBRANO ok  \n");

		uint8_t data = I2C_ReceiveData(I2C1);

		printf("ODEBRANO: %lu \n", data);


	}


}
