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
			
/* - - - - -  GLOBAL VARIABLES  - - - - -  */
#define ENA	GPIO_Pin_4						//	ENABLE MOTOR_A (LEFT)	| PWM -> SPEED
#define ENB	GPIO_Pin_0						//	ENABLE MOTOR_B (RIGHT)	| PWM -> SPEED
#define IN1	GPIO_Pin_6						//	DIRECTION MOTOR_A
#define IN2	GPIO_Pin_7						//
#define IN3	GPIO_Pin_1						//	DIRECTION MOTOR_B
#define IN4	GPIO_Pin_10						//
#define PORTA GPIOA							//	direction signals PORT
#define PORTB GPIOB

/* - - - - -  MOTOR FUNCTIONS - - - - - -   */
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

int tim = 0;
void delay(int tim_ms);

/* - - - - 	SERVO FUNCTIONS	 - - - - - */
void ServoLeft(void);
void ServoRight(void);
void ServoMiddle(void);

/* - - - -SONAR FUNCTIONS  - - - - - -	*/
volatile int START = 0, STOP = 0, MeasuredFlag = 0;		// time of HIGH at ECHO
float GetDistance();

/*  - - - - - - - CONFIGURATION - - - - - - - */
void RCC_Configuration(void);
void GPIO_Configuration(void);
void PWM_Servo_Configuration(void);		// TIM4
void Sonar_Timer_Configuration(void);	// TIM2
	void PWM_TRIG_Configuration(void);
	void ECHO_Configuration(void);
void NVIC_Configuration(void);					// for ECHO


/* - - - - - - - - - - MAIN - - - - - - - - - - */

int main(void)
{
	RCC_Configuration();
	GPIO_Configuration();
	PWM_Servo_Configuration();
	Sonar_Timer_Configuration();
	NVIC_Configuration();

	LeftMotor_ON();
	RightMotor_ON();

	int state = 0;

	while(1)
	{

		if(MeasuredFlag)
		{
			float dist = GetDistance();

					switch(state)
					{
						case 0:						// GO
							GPIO_SetBits(GPIOC, GPIO_Pin_13);
							Forward();
							ServoMiddle();

							if(dist < 10.0) state = 1;				// is space to move?
							break;

						case 1:						// STOP
							GPIO_ResetBits(GPIOC, GPIO_Pin_13);
							Brake();
							ServoRight();
							state = 2;
							break;

						case 2:						// TRY RIGHT
							if(dist > 10.0)
							{
								Right(1000);
								state = 0;
							}
							else
							{
								ServoLeft();
								state = 3;
							}
							break;

						case 3: 					// TRY LEFT
							if(dist > 10.0)
							{
								Left(1000);
								state = 0;
								}
								else
								{
									Left(2000);
									state = 0;
								}
								break;
					}
		}

		MeasuredFlag = 0;
		while(!MeasuredFlag);

	}
}

/* - - - - - - - - - - - - - - - - - - - - - - -  */

// - - - - - - - - PERIPHERIALS CONFIGURATION - - - - - - - */
void PWM_Servo_Configuration(void)
{
	TIM_TimeBaseInitTypeDef timer;
	TIM_TimeBaseStructInit(&timer);
	timer.TIM_ClockDivision = TIM_CKD_DIV1;
	timer.TIM_CounterMode = TIM_CounterMode_Up;
	timer.TIM_Prescaler = 72 - 1;			// 72M / 72 = 1MHz
	timer.TIM_Period = 20000 - 1;		// 1000000 / 20000 = 50Hz -> T = 20ms
	TIM_TimeBaseInit(TIM4, &timer);

	TIM_OCInitTypeDef tim_chanel;
	TIM_OCStructInit(&tim_chanel);
	tim_chanel.TIM_OCMode = TIM_OCMode_PWM1;
	tim_chanel.TIM_OutputState = TIM_OutputState_Enable;
	tim_chanel.TIM_Pulse = 1500;	// 1.5ms
	tim_chanel.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OC1Init(TIM4, &tim_chanel);

	TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Enable);		// buforowanie kana³u 1
	TIM_ARRPreloadConfig(TIM4, ENABLE);						// buforowanie licznika

	TIM_Cmd(TIM4, ENABLE);
}

void PWM_TRIG_Configuration(void)
{
	TIM_OCInitTypeDef pwm;
	TIM_OCStructInit(&pwm);
	pwm.TIM_OCMode = TIM_OCMode_PWM1;
	pwm.TIM_OutputState = TIM_OutputState_Enable;
	pwm.TIM_OCPolarity = TIM_OCPolarity_High;
	pwm.TIM_Pulse = 10;								// 10us
	TIM_OC2Init(TIM2, &pwm);

	TIM_OC1PreloadConfig(TIM2, TIM_OCPreload_Enable);
	TIM_ARRPreloadConfig(TIM2, ENABLE);
}

void ECHO_Configuration(void)
{
	TIM_ICInitTypeDef ch1;
	TIM_ICStructInit(&ch1);
	ch1.TIM_Channel = TIM_Channel_3;
	ch1.TIM_ICFilter = 0;									// wy³¹czony
	ch1.TIM_ICPolarity = TIM_ICPolarity_Rising;				// narastaj¹ce zbocze na ECHO
	ch1.TIM_ICSelection = TIM_ICSelection_DirectTI;
	ch1.TIM_ICPrescaler = TIM_ICPSC_DIV1;
	TIM_ICInit(TIM2, &ch1);

	TIM_ICInitTypeDef ch2;
	TIM_ICStructInit(&ch2);
	ch2.TIM_Channel = TIM_Channel_4;
	ch2.TIM_ICFilter = 0;
	ch2.TIM_ICPolarity = TIM_ICPolarity_Falling;			//opadaj¹ce zbocze na ECHO
	ch2.TIM_ICSelection = TIM_ICSelection_DirectTI;
	ch2.TIM_ICPrescaler = TIM_ICPSC_DIV1;
	TIM_ICInit(TIM2, &ch2);

	TIM_ITConfig(TIM2, TIM_IT_CC3, ENABLE);					// W£¥CZENIE PRZERWAÑ OD KANALU 3 I 4
	TIM_ITConfig(TIM2, TIM_IT_CC4, ENABLE);					// TIMERA 2
}

void Sonar_Timer_Configuration(void)
{
	TIM_TimeBaseInitTypeDef timer;
	TIM_TimeBaseStructInit(&timer);
	timer.TIM_ClockDivision = TIM_CKD_DIV1;
	timer.TIM_CounterMode = TIM_CounterMode_Up;
	timer.TIM_Prescaler = 72 - 1;					// 72MHz / 72 = 1MHz
	timer.TIM_Period = 50000;						// 50ms
	TIM_TimeBaseInit(TIM2, &timer);

	PWM_TRIG_Configuration();
	ECHO_Configuration();

	TIM_Cmd(TIM2, ENABLE);
}

void NVIC_Configuration(void)
{
	NVIC_InitTypeDef interrupts;
	interrupts.NVIC_IRQChannel = TIM2_IRQn;
	interrupts.NVIC_IRQChannelCmd = ENABLE;
	interrupts.NVIC_IRQChannelPreemptionPriority = 0;
	interrupts.NVIC_IRQChannelSubPriority = 0;
	NVIC_Init(&interrupts);
}

void TIM2_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM2, TIM_IT_CC3) != RESET)
	{
		TIM_ClearITPendingBit(TIM2, TIM_IT_CC3);
		START = TIM_GetCapture3(TIM2);
	}

	else if(TIM_GetITStatus(TIM2, TIM_IT_CC4) != RESET)
	{
		TIM_ClearITPendingBit(TIM2, TIM_IT_CC4);
		STOP = TIM_GetCapture4(TIM2);
		MeasuredFlag = 1;
	}
}

void RCC_Configuration(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2 | RCC_APB1Periph_TIM4, ENABLE);
	SysTick_Config(SystemCoreClock / 1000);
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

void GPIO_Configuration(void)
{
	GPIO_InitTypeDef gpio;
	GPIO_StructInit(&gpio);
	gpio.GPIO_Mode = GPIO_Mode_AF_PP;
	gpio.GPIO_Pin = GPIO_Pin_6;			// B6 - PWM to servo
	GPIO_Init(GPIOB, &gpio);

	gpio.GPIO_Pin = GPIO_Pin_1;		// TIM2CH2,
	GPIO_Init(GPIOA, &gpio);

	gpio.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	gpio.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;		// TIM2 Ch3, Ch4
	GPIO_Init(GPIOA, &gpio);

	gpio.GPIO_Mode = GPIO_Mode_Out_PP;
	gpio.GPIO_Pin = GPIO_Pin_13;				// LED
	GPIO_Init(GPIOC, &gpio);

	gpio.GPIO_Mode = GPIO_Mode_Out_PP;
	gpio.GPIO_Pin = ENA | IN1 | IN2;			// MOTOR A
	GPIO_Init(PORTA, &gpio);

	gpio.GPIO_Pin = ENB | IN3 | IN4;			// MOTOR B
	GPIO_Init(PORTB, &gpio);
}


/*- - - - - - MOTOR FUNCTIONS - - - - - - -	*/
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


/* - - - - - - - - - -  SERVO FUNCTIONS - - - - - - - - - */
void ServoLeft(void)
{
	TIM_SetCompare1(TIM4, 2000);
}

void ServoRight(void)
{
	TIM_SetCompare1(TIM4, 600);
}

void ServoMiddle(void)
{
	TIM_SetCompare1(TIM4, 1200);
}

/* - - - -  SONAR FUNCTIONS  - - - - - -	*/

float GetDistance(void)
{
	return (STOP - START) / 58.0;
}
