// OLED.h - afisor KS0108 128x64, interfata paralela 8 bit, ATmega64A
#ifndef __OLED_H__
#define __OLED_H__

#include <avr/io.h>
#include "pins.h"

extern const unsigned char font[];        // font 5x8 (6 col/char), in flash
extern const unsigned char font2[];       // font 8x16 (9x2 col/char), in flash
extern const unsigned char siglaSeletron[];

extern unsigned char LCDBuff[8][128];     // 1024 byte SRAM

// --- urmarire randuri modificate (ca in versiunea LM3S2965 optimizata) ---
extern unsigned char LCD_DirtyMask;
void LCD_MarkRow(unsigned char y);
void LCD_MarkRows(unsigned char y0, unsigned char y1);
void LCD_MarkAll(void);
unsigned char LCD_IsDirty(void);

void DisplayReset(void);
void ClearPage(unsigned char c);                  // init complet + puls RES
void RefreshPage(void);                   // trimite doar randurile modificate
void PutPixel(unsigned int x, unsigned int y);
void PutLine(int x0, int y0, int x1, int y1);
void PutVerticalLine(unsigned char x, unsigned char y0, unsigned char y1);
void PutHorizontalLine(unsigned char x0, unsigned char x1, unsigned char y);
void Rectangle(int x, int y, int dx, int dy);
void FillRect(int x, int y, int n);
void PutImage(const unsigned char *image); // imagine din FLASH (128x8)

// text din RAM (buffer-e proprii: WMessage, buffer-e formate etc.)
void DisplaySString (unsigned char *text, unsigned int x, unsigned int y, unsigned char inv);
void DisplaySStringD(unsigned char *txt,  unsigned int x, unsigned int y, unsigned char inv);
// text din FLASH (literals) - echivalentele cu P
void DisplaySStringP(const char *text, unsigned int x, unsigned int y, unsigned char inv);
void DisplaySStringDP(const char *txt,  unsigned int x, unsigned int y, unsigned char inv);

void DisplayS2Int(int v, unsigned int x, unsigned int y, unsigned char inv);
void DisplayS3Int(int v, unsigned int x, unsigned int y, unsigned char inv);
void DisplayS4Int(int v, unsigned int x, unsigned int y, unsigned char inv);
void invert_lcd(void);

#endif
