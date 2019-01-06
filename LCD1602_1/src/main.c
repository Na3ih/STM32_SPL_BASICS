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


int tim = 0;
void SysTick_Handler()
{
	if(tim) --tim;
}


// PO£¥CZENIE
#define LCD_E_PIN GPIO_Pin_12
#define LCD_E_PORT GPIOB

#define LCD_RS_PIN GPIO_Pin_13
#define LCD_RS_PORT GPIOB

#define LCD_D4_PIN GPIO_Pin_4
#define LCD_D4_PORT GPIOB

#define LCD_D5_PIN GPIO_Pin_5
#define LCD_D5_PORT GPIOB

#define LCD_D6_PIN GPIO_Pin_6
#define LCD_D6_PORT GPIOB

#define LCD_D7_PIN GPIO_Pin_7
#define LCD_D7_PORT GPIOB

// USTAWIANIE
#define LCD_E_HIGH GPIO_SetBits(LCD_E_PORT, LCD_E_PIN);
#define LCD_E_LOW GPIO_ResetBits(LCD_E_PORT, LCD_E_PIN);

#define LCD_RS_HIGH GPIO_SetBits(LCD_RS_PORT, LCD_RS_PIN);
#define LCD_RS_LOW 	GPIO_ResetBits(LCD_RS_PORT, LCD_RS_PIN);

// ADRESY FUNKCJI
#define LCD_CLEAR					0x01
#define LCD_HOME					0x02
#define LCDC_ENTRY_MODE				0x04
	#define LCD_EM_SHIFT_CURSOR		    0x00
	#define LCD_EM_SHIFT_DISPLAY	 	0x01
	#define LCD_EM_LEFT		   			0x00
	#define LCD_EM_RIGHT				0x02
#define LCD_ONOFF					0x08
	#define LCD_DISP_ON				    0x04
	#define LCD_CURSOR_ON				0x02
	#define LCDC_CURSOR_OFF				0x00
	#define LCDC_BLINK_ON				0x01
	#define LCDC_BLINK_OFF				0x00
#define LCD_SHIFT					0x10
	#define LCDC_SHIFT_DISP				0x08
	#define LCDC_SHIFT_CURSOR			0x00
	#define LCDC_SHIFT_RIGHT			0x04
	#define LCDC_SHIFT_LEFT				0x00
#define LCD_FUNC					0x20
	#define LCD_8_BIT					0x10
	#define LCD_4_BIT					0x00
	#define LCDC_TWO_LINE				0x08
	#define LCDC_FONT_5x10				0x04
	#define LCDC_FONT_5x7				0x00
#define LCDC_SET_CGRAM				0x40
#define LCDC_SET_DDRAM				0x80

// ADRESY LINII
#define LCD_LINE1 0x00
#define LCD_LINE2 0x40

void LCD_SendHalf(uint8_t data)
{
	LCD_E_HIGH;	// sygna³ strobuj¹cy
	GPIO_WriteBit(LCD_D4_PORT, LCD_D4_PIN, (data & 0x01));
	GPIO_WriteBit(LCD_D5_PORT, LCD_D5_PIN, (data & 0x02));
	GPIO_WriteBit(LCD_D6_PORT, LCD_D6_PIN, (data & 0x04));
	GPIO_WriteBit(LCD_D7_PORT, LCD_D7_PIN, (data & 0x08));
	LCD_E_LOW;
}

void LCD_SendAllByte(uint8_t data)
{
	LCD_SendHalf(data >> 4);
	LCD_SendHalf(data);

	tim = 1; 	// ms opoznienia bo nie sprawdzamy flagi zajetosci
}

void LCD_SendCmd(uint8_t addr)
{
	LCD_RS_LOW;
	LCD_SendAllByte(addr);
}

void LCD_SendChar(uint8_t addr)
{
	LCD_RS_HIGH;
	LCD_SendAllByte(addr);
}

void LCD_SetCursor(uint8_t y, uint8_t x)	// y - wiersz, x - kolumna
{
	switch(y)
	{
		case 0:
			LCD_SendCmd(LCDC_SET_DDRAM);
			LCD_SendCmd(LCD_LINE1 + x);
			break;

		case 1:
			LCD_SendCmd(LCDC_SET_DDRAM);
			LCD_SendCmd(LCD_LINE2 + x);
			break;
	}
}

void LCD_SendString(char* txt)
{
	while(*txt) LCD_SendChar(*txt++);
}

void GPIO_Config()
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC, ENABLE);

	GPIO_InitTypeDef gpio;
	GPIO_StructInit(&gpio);
	gpio.GPIO_Mode = GPIO_Mode_Out_PP;
	gpio.GPIO_Speed = GPIO_Speed_50MHz;

	gpio.GPIO_Pin = LCD_E_PIN; GPIO_Init(LCD_E_PORT, &gpio);
	gpio.GPIO_Pin = LCD_RS_PIN; GPIO_Init(LCD_RS_PORT, &gpio);
	gpio.GPIO_Pin = LCD_D4_PIN; GPIO_Init(LCD_D4_PORT, &gpio);
	gpio.GPIO_Pin = LCD_D5_PIN; GPIO_Init(LCD_D5_PORT, &gpio);
	gpio.GPIO_Pin = LCD_D6_PIN; GPIO_Init(LCD_D6_PORT, &gpio);
	gpio.GPIO_Pin = LCD_D7_PIN; GPIO_Init(LCD_D7_PORT, &gpio);
}

void LCD_Init()
{
	LCD_E_LOW;
	LCD_RS_LOW;

	tim = 15000; while(tim);	// more than 15ms after Vcc rises to 4.5V

	LCD_SendHalf(0x03);
	tim = 4100; while(tim);	// more than 4.1ms

	LCD_SendHalf(0x03);
	tim = 100; while(tim);	// more than 100us

	LCD_SendHalf(0x03);		// 4bit interface
	tim = 100; while(tim);

	LCD_SendHalf(0x02);
	tim = 100; while(tim);

	LCD_SendCmd(LCD_FUNC);
	LCD_SendCmd(LCD_4_BIT);
	LCD_SendCmd(LCDC_TWO_LINE);
	LCD_SendCmd(LCDC_FONT_5x7);
	LCD_SendCmd(LCD_ONOFF);
	LCD_SendCmd(LCD_DISP_ON);
	LCD_SendCmd(LCD_CLEAR);
	tim = 5000; while(tim);

	LCD_SendCmd(LCDC_ENTRY_MODE);
	LCD_SendCmd(LCD_EM_SHIFT_CURSOR);
	LCD_SendCmd(LCD_EM_RIGHT);
}


int main(void)
{
	SysTick_Config(SystemCoreClock / 1000000);

	GPIO_Config(); tim = 15000; while(tim);
	LCD_Init();

	LCD_SetCursor(0,0);
	LCD_SendString("  HAHA :)   ");
	LCD_SetCursor(1,5);
	LCD_SendChar('H');
	while(1);
}
