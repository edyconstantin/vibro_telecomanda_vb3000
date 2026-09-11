// OLED.c - KS0108 paralel 8 bit pe ATmega64A
//
// Port al versiunii LM3S2965 OPTIMIZATE:
//  - randuri "dirty" (se trimite doar ce s-a schimbat)
//  - fara DisplayReset() la fiecare refresh
//  - Bresenham pe intregi
//  - delay-uri echivalente: SysCtlDelay(50) @ 50 MHz = 3 us -> _delay_us(3)
//
// ATENTIE AVR: fonturile si sigla sunt in FLASH (const) si se citesc
// exclusiv prin pgm_read_byte(). DisplaySStringP/DisplaySStringDP citesc
// text din FLASH (literals); DisplaySString/DisplaySStringD citesc din RAM.

#include "OLED.h"
#include "fmt.h"
#include <util/delay.h>
#include <avr/pgmspace.h>

// ---------------------------------------------------------------------------
// Micro-operatiuni de pini
// ---------------------------------------------------------------------------
static inline void lcd_en (unsigned char v) { if (v) LCD_EN_PORT |= (1<<LCD_EN_PIN); else LCD_EN_PORT &= ~(1<<LCD_EN_PIN); }
static inline void lcd_rw (unsigned char v) { if (v) LCD_RW_PORT |= (1<<LCD_RW_PIN); else LCD_RW_PORT &= ~(1<<LCD_RW_PIN); }
static inline void lcd_di (unsigned char v) { if (v) LCD_DI_PORT |= (1<<LCD_DI_PIN); else LCD_DI_PORT &= ~(1<<LCD_DI_PIN); }
static inline void lcd_cs1(unsigned char v) { if (v) LCD_CS1_PORT |= (1<<LCD_CS1_PIN); else LCD_CS1_PORT &= ~(1<<LCD_CS1_PIN); }
static inline void lcd_cs2(unsigned char v) { if (v) LCD_CS2_PORT |= (1<<LCD_CS2_PIN); else LCD_CS2_PORT &= ~(1<<LCD_CS2_PIN); }

// scrie un byte pe DB0..DB7 (split pe PD4..PD7 + PE0..PE3, restul pinilor
// din porturi se conserva - acolo stau si TX/RX-ul USART1 si DI/CS1/CS2)
static inline void lcd_data_out(unsigned char b)
{
    LCD_DB_HIGH_PORT = (LCD_DB_HIGH_PORT & ~LCD_DB_HIGH_PINS) | ((b >> 4) & 0x0F);
    LCD_DB_LOW_PORT  = (LCD_DB_LOW_PORT  & ~LCD_DB_LOW_PINS)  |  (b & 0x0F);
}

// ---------------------------------------------------------------------------
// Dirty mask
// ---------------------------------------------------------------------------
unsigned char LCD_DirtyMask = 0xFF; // la pornire tot afisajul e de trimis

void LCD_MarkRow(unsigned char y)   { if (y < 8) LCD_DirtyMask |= (1 << y); }
void LCD_MarkRows(unsigned char y0, unsigned char y1) { while (y0 <= y1) LCD_MarkRow(y0++); }
void LCD_MarkAll(void)              { LCD_DirtyMask = 0xFF; }
unsigned char LCD_IsDirty(void)     { return LCD_DirtyMask; }

// ---------------------------------------------------------------------------
// Un byte de COMANDA (DI=0, RW=0)
// ---------------------------------------------------------------------------
static void out_lcd(unsigned char byte)
{
    lcd_en(0);
    _delay_us(3);
    lcd_rw(0);
    lcd_di(0);
    lcd_data_out(byte);
    lcd_en(1);          // rising edge -> display isi ia byte-ul
    _delay_us(3);
    lcd_en(0);
    _delay_us(3);
}

// ---------------------------------------------------------------------------
// Un byte de DATE (DI=1, RW=0). RW/DI se seteaza o singura data pe rand
// (vezi RefreshPage), aici e doar pulsul ENA + date.
// ---------------------------------------------------------------------------
static void out_lcd_data(unsigned char byte)
{
    lcd_en(0);
    _delay_us(3);
    lcd_data_out(byte);
    lcd_en(1);
    _delay_us(3);
    lcd_en(0);
    _delay_us(3);
}

// ---------------------------------------------------------------------------
// Reset la pornire (puls RES + initializarea KS0108 pe ambele jumatati)
// ---------------------------------------------------------------------------
void DisplayReset(void)
{
    // puls de reset hardware (pin RES = D4, configurat output in main)
    LCD_RES_PORT &= ~(1<<LCD_RES_PIN);
    _delay_ms(100);
    LCD_RES_PORT |=  (1<<LCD_RES_PIN);
    _delay_ms(10);

    _delay_us(3);
    lcd_en(1);
    lcd_rw(1);
    lcd_di(1);
    lcd_cs1(1);
    lcd_cs2(1);

    lcd_cs1(0);
    out_lcd(0x3F);  // DISPLAY ON
    out_lcd(0x40);  // Y ADDRESS = 0
    out_lcd(0xB8);  // X ADDRESS = 0
    lcd_cs1(1);
    lcd_cs2(0);
    out_lcd(0x3F);
    out_lcd(0x40);
    out_lcd(0xB8);
    lcd_en(1);
    lcd_cs2(1);

    LCD_MarkAll();
}

// ---------------------------------------------------------------------------
// Refresh: doar randurile modificate, in ordinele de CS originale
// ---------------------------------------------------------------------------
void RefreshPage(void)
{
    unsigned char x, y;

    if (!LCD_DirtyMask) return; // nimic de trimis

    // prima jumatate: coloane 64..127 (CS1=0, CS2=1)
    lcd_en(0);
    _delay_us(3);
    lcd_cs1(0);
    lcd_cs2(1);

    for (y = 0; y < 8; y++) {
        if (!(LCD_DirtyMask & (1<<y))) continue;
        out_lcd(0x3F);
        out_lcd(0xB8 | y);
        out_lcd(0x40);
        // comanda ramane cu DI=0; pentru date repunem RW=0, DI=1 o singura data
        lcd_rw(0);
        lcd_di(1);
        for (x = 64; x < 128; x++) {
            out_lcd_data(LCDBuff[y][x]);
        }
    }
    lcd_cs1(1);

    // a doua jumatate: coloane 0..63 (CS1=1, CS2=0)
    lcd_cs2(0);
    for (y = 0; y < 8; y++) {
        if (!(LCD_DirtyMask & (1<<y))) continue;
        out_lcd(0x3F);
        out_lcd(0xB8 | y);
        out_lcd(0x40);
        lcd_rw(0);
        lcd_di(1);
        for (x = 0; x < 64; x++) {
            out_lcd_data(LCDBuff[y][x]);
        }
    }

    lcd_cs1(1);
    lcd_cs2(1);
    lcd_en(1);
    _delay_us(3);

    LCD_DirtyMask = 0; // pagina e la zi
}

// ---------------------------------------------------------------------------
// Primitve de desen (scriu in LCDBuff si marcheaza randuri)
// ---------------------------------------------------------------------------
unsigned char LCDBuff[8][128];

void ClearPage(unsigned char c)
{
    unsigned char m, k;
    for (m = 0; m < 8; m++)
        for (k = 0; k < 128; k++)
            LCDBuff[m][k] = c;
    LCD_MarkAll();
}

void FillRect(int x, int y, int n)
{
    int i;
    y &= 7;
    x &= 127;
    for (i = 0; i < n; i++)
        LCDBuff[y][x+i] = 0xff;
    LCD_MarkRow((unsigned char)y);
}

void PutPixel(unsigned int x, unsigned int y)
{
    unsigned int r, b;
    if (x < 128) {
        if (y < 64) {
            r = y >> 3;
            b = y & 0x07;
            LCDBuff[r][x] |= (unsigned char)(1 << b);
            LCD_MarkRow((unsigned char)r);
        }
    }
}

// imagine 128x8 din FLASH (format: [x + y*128])
void PutImage(const unsigned char *image)
{
    unsigned int x, y;
    for (x = 0; x < 128; x++)
        for (y = 0; y < 8; y++)
            LCDBuff[y][x] = pgm_read_byte(image + x + y*128);
    LCD_MarkAll();
}

// Bresenham pe intregi (fara float)
void PutLine(int x0, int y0, int x1, int y1)
{
    int dx = x1 - x0;
    int dy = y1 - y0;
    int adx = (dx < 0) ? -dx : dx;
    int ady = (dy < 0) ? -dy : dy;
    int sx  = (dx < 0) ? -1 : 1;
    int sy  = (dy < 0) ? -1 : 1;
    int err;

    PutPixel(x0, y0);

    if (adx >= ady) {                      // panta < 1: paseste in x
        err = 2 * ady - adx;
        while (x0 != x1) {
            x0 += sx;
            if (err >= 0) { y0 += sy; err -= 2 * adx; }
            err += 2 * ady;
            PutPixel(x0, y0);
        }
    } else {                               // panta >= 1: paseste in y
        err = 2 * adx - ady;
        while (y0 != y1) {
            y0 += sy;
            if (err >= 0) { x0 += sx; err -= 2 * ady; }
            err += 2 * adx;
            PutPixel(x0, y0);
        }
    }
}

void PutVerticalLine(unsigned char x, unsigned char y0, unsigned char y1)
{
    unsigned char i;
    if (x < 128 && y0 < 64 && y1 < 64) {
        if (y0 < y1) {
            for (i = y0; i <= y1; i++) PutPixel(x, i);
        } else {
            for (i = y1; i <= y0; i++) PutPixel(x, i);
        }
        // (randurile sunt marcate de PutPixel, pixel cu pixel)
    }
}

void PutHorizontalLine(unsigned char x0, unsigned char x1, unsigned char y)
{
    unsigned char i;
    if (x0 < 128 && x1 < 128 && y < 64) {
        if (x0 < x1) {
            for (i = x0; i <= x1; i++) PutPixel(i, y);
        } else {
            for (i = x1; i <= x0; i++) PutPixel(i, y);
        }
    }
}

void Rectangle(int x, int y, int dx, int dy)
{
    int x2 = x + dx, y2 = y + dy;
    PutHorizontalLine(x, x2, y);
    PutHorizontalLine(x, x2, y2);
    PutVerticalLine(x, y, y2);
    PutVerticalLine(x2, y, y2);
}

// ---------------------------------------------------------------------------
// Fonturi
// ---------------------------------------------------------------------------
void DisplayS2Int(int v, unsigned int x, unsigned int y, unsigned char inv)
{
    char text[5];
    fmt_num(text, v, 2);
    DisplaySString((unsigned char *)text, x, y, inv);
}
void DisplayS3Int(int v, unsigned int x, unsigned int y, unsigned char inv)
{
    char text[5];
    fmt_num(text, v, 3);
    DisplaySString((unsigned char *)text, x, y, inv);
}
void DisplayS4Int(int v, unsigned int x, unsigned int y, unsigned char inv)
{
    char text[6];
    fmt_num(text, v, 4);
    DisplaySString((unsigned char *)text, x, y, inv);
}

// text din RAM - font 5x8 (6 col per char, ' ' = blank)
void DisplaySString(unsigned char *text, unsigned int x, unsigned int y, unsigned char inv)
{
    unsigned int var1, var2, var3, a = 0;
    if (y > 7) return;
    LCD_MarkRow((unsigned char)y);
    var1 = x;
    while (*text) {
        var3 = *text;
        var2 = (var3 - ' ') * 6;
        for (a = 0; a < 6; a++) {
            if (var1 >= 128) return;
            LCDBuff[y][var1] = pgm_read_byte(&font[var2]);
            if (inv) LCDBuff[y][var1] ^= 0xff;
            var2++;
            var1++;
        }
        text++;
    }
    if (inv && var1 < 128) LCDBuff[y][var1] = 0xff;
}

// acelasi lucru, text din FLASH (literals)
void DisplaySStringP(const char *text, unsigned int x, unsigned int y, unsigned char inv)
{
    unsigned int var1, var2, var3, a = 0;
    unsigned char c;
    if (y > 7) return;
    LCD_MarkRow((unsigned char)y);
    var1 = x;
    while ((c = pgm_read_byte(text++))) {
        var3 = c;
        var2 = (var3 - ' ') * 6;
        for (a = 0; a < 6; a++) {
            if (var1 >= 128) return;
            LCDBuff[y][var1] = pgm_read_byte(&font[var2]);
            if (inv) LCDBuff[y][var1] ^= 0xff;
            var2++;
            var1++;
        }
    }
    if (inv && var1 < 128) LCDBuff[y][var1] = 0xff;
}

// text din RAM - font 8x16 (9 col per char, 2 randuri, '-' = origin)
void DisplaySStringD(unsigned char *txt, unsigned int x, unsigned int y, unsigned char inv)
{
    unsigned int var1, var2, var3, a = 0;
    if (y > 6) return;
    LCD_MarkRows((unsigned char)y, (unsigned char)(y+1));
    var1 = x;
    while (*txt) {
        var3 = *txt;
        if (var3 == ' ') var3 = '.';
        var2 = (var3 - '-') * 18;
        for (a = 0; a < 9; a++) {
            if (var1 >= 128) return;
            LCDBuff[y][var1]   = pgm_read_byte(&font2[var2+1]);
            if (inv) LCDBuff[y][var1] ^= 0xff;
            LCDBuff[y+1][var1] = pgm_read_byte(&font2[var2]);
            if (inv) LCDBuff[y+1][var1] ^= 0xff;
            var2 += 2;
            var1++;
        }
        txt++;
    }
}

// text din FLASH - font 8x16
void DisplaySStringDP(const char *txt, unsigned int x, unsigned int y, unsigned char inv)
{
    unsigned int var1, var2, var3, a = 0;
    unsigned char c;
    if (y > 6) return;
    LCD_MarkRows((unsigned char)y, (unsigned char)(y+1));
    var1 = x;
    while ((c = pgm_read_byte(txt++))) {
        var3 = c;
        if (var3 == ' ') var3 = '.';
        var2 = (var3 - '-') * 18;
        for (a = 0; a < 9; a++) {
            if (var1 >= 128) return;
            LCDBuff[y][var1]   = pgm_read_byte(&font2[var2+1]);
            if (inv) LCDBuff[y][var1] ^= 0xff;
            LCDBuff[y+1][var1] = pgm_read_byte(&font2[var2]);
            if (inv) LCDBuff[y+1][var1] ^= 0xff;
            var2 += 2;
            var1++;
        }
    }
}

void invert_lcd(void)
{
    unsigned char i, j;
    for (i = 0; i < 8; i++)
        for (j = 0; j < 128; j++)
            LCDBuff[i][j] ^= 0xff;
    LCD_MarkAll();
}
