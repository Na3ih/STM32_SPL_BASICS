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
#include "./../FATFs/ff.h"
#include "./../FATFs/diskio.h"

void GPIO_Config(void);
void RCC_Config(void);
void NVIC_Config(void);
void SPI_Config(void);
void UART_Config(void);

static FATFS g_sFatFs;
//static DIR g_sDirObject;
//static FILINFO g_sFileInfo;
//static FIL g_sFileObject;

#define PATH_BUF_SIZE   80
//static char g_cCwdBuf[PATH_BUF_SIZE] = "/";

int main(void)
{
   FRESULT fresult;
   FIL plik;
   FILINFO plikInfo ;
   DIR Dir;
   FILINFO *p;

   WORD zapisanych_bajtow;


     RCC_Conf(); SysTick_Conf();


    fresult = f_mount(0, &g_sFatFs);

   // Tworzenie pliku
   fresult = f_open (&plik,"plik.txt", FA_CREATE_ALWAYS | FA_WRITE);
   fresult = f_write(&plik, "zawartosc pliku", 15, &zapisanych_bajtow);
   fresult = f_close (&plik);


     while (1);
}
