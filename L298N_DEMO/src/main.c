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

	/*   GLOBAL VARIABLES  */
#define ENA	GPIO_Pin_0						//	ENABLE MOTOR_A (LEFT)	| PWM -> SPEED
#define ENB	GPIO_Pin_6						//	ENABLE MOTOR_B (RIGHT)	| PWM -> SPEED
#define IN1	GPIO_Pin_1						//	DIRECTION MOTOR_A
#define IN2	GPIO_Pin_2						//
#define IN3	GPIO_Pin_7						//	DIRECTION MOTOR_B
#define IN4	GPIO_Pin_8						//
#define PORTA GPIOA		//	direction signals PORT
#define PORTB GPIOB

	/*  MOTOR FUNCTIONS   */
void LeftMotor_ON(void);
void LeftMotor_OFF(void);
void LeftMotor_Forward(void);
void LeftMotor_Backward(void);
void LeftMotor_Brake(void);
void LeftMotor_Coast(void);
void RightMotor_ON(void);
void RightMotor_OFF(void);
void RightMotor_Forward(void);
void RightMotor_Backward(void);
void RightMotor_Brake(void);
void RightMotor_Coast(void);
void Forward(void);
void Backward(void);
void Left(int time_ms);
void Right(int time_ms);
void Brake(void);
void Coast(void);

/*	PERIPHERIALS CONFIGURATIONS	*/
void RCC_Config(void);
void GPIO_Config(void);
void delay(int tim_ms);

int tim = 0;
/* - - - -  	MAIN	- - - - 	*/
int main(void)
{
	RCC_Config();
	GPIO_Config();
	LeftMotor_ON();
	RightMotor_ON();

	while(1)
	{
		Forward(); delay(1000);
		Backward(); delay(2000);
		Left(1000);
		Right(2000)
	}
}

/*		MOTOR FUNCTIONS		*/
void LeftMotor_ON(void)
{
	GPIO_SetBits(PORTA, ENA);
}

void LeftMotor_OFF(void)
{
	GPIO_ResetBits(PORTA, ENA);
}

void LeftMotor_Forward(void)
{
	GPIO_SetBits(PORTA, IN1);
	GPIO_ResetBits(PORTA, IN2);
}

void LeftMotor_Backward(void)
{
	GPIO_ResetBits(PORTA, IN1);
	GPIO_SetBits(PORTA, IN2);
}

void LeftMotor_Brake(void)
{
	GPIO_SetBits(PORTA, IN1);
	GPIO_SetBits(PORTA, IN2);
}

void LeftMotor_Coast(void)
{
	GPIO_ResetBits(PORTA, IN1);
	GPIO_ResetBits(PORTA, IN2);
}

void RightMotor_ON(void)
{
	GPIO_SetBits(PORTB, ENB);
}

void RightMotor_OFF(void)
{
	GPIO_ResetBits(PORTB, ENB);
}

void RightMotor_Forward(void)
{
	GPIO_SetBits(PORTB, IN3);
	GPIO_ResetBits(PORTB, IN4);
}

void RightMotor_Backward(void)
{
	GPIO_ResetBits(PORTB, IN3);
	GPIO_SetBits(PORTB, IN4);
}

void RightMotor_Brake(void)
{
	GPIO_SetBits(PORTB, IN3);
	GPIO_SetBits(PORTB, IN4);
}

void RightMotor_Coast(void)
{
	GPIO_ResetBits(PORTB, IN3);
	GPIO_ResetBits(PORTB, IN4);
}

void Forward(void)
{
	RightMotor_Forward();
	LeftMotor_Forward();
}

void Backward(void)
{
	RightMotor_Backward();
	RightMotor_Backward();
}

void Left(int time_ms)
{
	LeftMotor_Forward();
	RightMotor_Backward();
	delay(time_ms);
}

void Right(int time_ms)
{
	LeftMotor_Backward();
	RightMotor_Forward();
	delay(time_ms);
}

void Brake(void)
{
	LeftMotor_Brake();
	RightMotor_Brake();
}

void Coast(void)
{
	LeftMotor_Coast();
	RightMotor_Coast();
}


/*		PERIPHERIALS CONFIGURATION		*/
void RCC_Config(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB, ENABLE);
	SysTick_Config(SystemCoreClock / 1000);
}

void GPIO_Config(void)
{
	GPIO_InitTypeDef gpio;
	GPIO_StructInit(&gpio);
	gpio.GPIO_Mode = GPIO_Mode_Out_PP;
	gpio.GPIO_Pin = ENA | IN1 | IN2;
	GPIO_Init(PORTA, &gpio);

	gpio.GPIO_Pin = ENB | IN3 | IN4;
	GPIO_Init(PORTB, &gpio);
}

void SysTick_Handler()
{
	if(tim) --tim;
}

void delay(int time_ms)
{
	tim = time_ms;
	while(tim);
}
