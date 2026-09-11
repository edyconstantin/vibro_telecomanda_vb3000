#ifndef __OLED_H__
#define __OLED_H__

#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_gpio.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"

#define	LCD_DATA_PORT        SYSCTL_PERIPH_GPIOA
#define	LCD_DATA_BASE        GPIO_PORTA_BASE


//#define	LCD_CONTROL_PORT     SYSCTL_PERIPH_GPIOG
//#define	LCD_CONTROL_BASE     GPIO_PORTG_BASE
//#define LCD_CS1_PIN 	   	     GPIO_PIN_7
//#define LCD_RESET_PIN 	    GPIO_PIN_6
//#define LCD_RDWR_PIN 	    GPIO_PIN_5
//#define LCD_DCMD_PIN 	    GPIO_PIN_4
#define	LCD_CONTROL_PORT1     SYSCTL_PERIPH_GPIOG
#define	LCD_CONTROL_BASE1     GPIO_PORTG_BASE
#define LCD_CS1_PIN 		    GPIO_PIN_7
#define LCD_CS2_PIN			GPIO_PIN_6	
#define LCD_RESET_PIN 	 	 GPIO_PIN_5

 
#define	LCD_CONTROL_PORT     SYSCTL_PERIPH_GPIOC
#define	LCD_CONTROL_BASE    GPIO_PORTC_BASE
#define LCD_ENABLE_PIN 		GPIO_PIN_4
#define LCD_RDWR_PIN 		GPIO_PIN_5
#define LCD_DCMD_PIN 		GPIO_PIN_6


#define	COM1_PORT            SYSCTL_PERIPH_GPIOD
#define	COM1_BASE            GPIO_PORTD_BASE
#define COM1_TXENA_PIN 		GPIO_PIN_1
extern const unsigned char font[];
extern const unsigned char font2[];
extern const unsigned char siglaSeletron[];

extern void DisplayReset(void);

extern unsigned char LCDBuff[8][128];

// --- urmarire randuri modificate (optimizare RefreshPage) ---
extern unsigned char LCD_DirtyMask;
void LCD_MarkRow(unsigned char y);
void LCD_MarkRows(unsigned char y0, unsigned char y1);
void LCD_MarkAll(void);
unsigned char LCD_IsDirty(void);

void ClearPage(unsigned char);
void RefreshPage(void);
void PutPixel(unsigned int x, unsigned int y);
void PutLine(int x0, int y0, int x1, int y1);
void PutVerticalLine(unsigned char x, unsigned char y0, unsigned char y1);
void PutHorizontalLine(unsigned char x0, unsigned char x1, unsigned char y);
void DisplaySString (unsigned char *text,unsigned int x, unsigned int y, unsigned char inv);
void DisplaySStringD (unsigned char *txt,unsigned int x, unsigned int y, unsigned char inv);
void PutImage(const unsigned char *image);
void invert_lcd(void);
void DisplayS2Int(int v,unsigned int x, unsigned int y, unsigned char inv);
void DisplayS3Int(int v,unsigned int x, unsigned int y, unsigned char inv);
void DisplayS4Int(int v,unsigned int x, unsigned int y, unsigned char inv);
void Rectangle(int x, int y, int dx, int dy);
void FillRect(int x,int y,int n);

#endif
