// ============================================================================
// pins.h - maparea de pini ATmega64A pentru VB3000 (telecomanda vibratii)
//
// Conventia de numerotare folosita pe placa (stil Arduino):
//   D0..D7  = PA0..PA7      D8..D15  = PB0..PB7
//   D16..D23= PC0..PC7      D24..D31 = PD0..PD7
//   D32..D39= PE0..PE7      D40..D47 = PF0..PF7
//
// Daca placa ta are alta corespondenta, modifica DOAR acest fisier.
// ============================================================================
#ifndef __PINS_H__
#define __PINS_H__

#include <avr/io.h>

// ----------------------------------------------------------------------------
// AFISOR KS0108 (128x64), interfata paralela 8 bit
// Din libraria ta (modificata pentru mega64):
//   KS0108_GLCD(DI, RW, E, DB0, DB1, DB2, DB3, DB4, DB5, DB6, DB7, CS1, CS2, RES)
//   display = KS0108_GLCD(36, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 37, 38, 4);
//
// Decodat pe pini ATMega64A:
//   DI =D36=PE4   RW =D26=PD2   E  =D27=PD3
//   DB0=D28=PD4   DB1=D29=PD5   DB2=D30=PD6   DB3=D31=PD7
//   DB4=D32=PE0   DB5=D33=PE1   DB6=D34=PE2   DB7=D35=PE3
//   CS1=D37=PE5   CS2=D38=PE6   RES=D4 =PA4
// ----------------------------------------------------------------------------
#define LCD_DI_PORT     PINE
#define LCD_DI_PIN      4          // D36
#define LCD_RW_PORT     PIND
#define LCD_RW_PIN      2          // D26
#define LCD_EN_PORT     PIND
#define LCD_EN_PIN      3          // D27

// magistrala de date e SPLITATA pe doua porturi (PD4..PD7 si PE0..PE3)
#define LCD_DB_LOW_PORT PIND
#define LCD_DB_LOW_PINS ((1<<4)|(1<<5)|(1<<6)|(1<<7))   // DB0..DB3
#define LCD_DB_HIGH_PORT PINE
#define LCD_DB_HIGH_PINS ((1<<0)|(1<<1)|(1<<2)|(1<<3))   // DB4..DB7

#define LCD_CS1_PORT    PINE
#define LCD_CS1_PIN     5          // D37
#define LCD_CS2_PORT    PINE
#define LCD_CS2_PIN     6          // D38
#define LCD_RES_PORT    PORTA
#define LCD_RES_PIN     4          // D4

// ----------------------------------------------------------------------------
// UART RS-485 spre vibsist
// ATENTIE: PD2/RWD0 si PD3/TXD0 (USART0) sunt OCCUPATE de afisor (RW/E),
// deci folosem USART1:  RXD1 = PD0 (D24), TXD1 = PD1 (D25)
// ----------------------------------------------------------------------------
// TODO: verifica pinul TXEN (transceiver RS-485) in schematic!
#define UART_TXEN_PORT  PORTB
#define UART_TXEN_PIN   0          // D8  (presupus)

// ----------------------------------------------------------------------------
// TASTATURA (10 taste) - TODO: verifica pini in schematic! (presupusi)
// ----------------------------------------------------------------------------
// K1..K4 : PB1..PB4 (D9..D12)
#define K1_PORT PORTB
#define K1_PIN  1
#define K2_PORT PORTB
#define K2_PIN  2
#define K3_PORT PORTB
#define K3_PIN  3
#define K4_PORT PORTB
#define K4_PIN  4
// K5..K8 : PC0..PC3 (D16..D19)  (PC0..PC5 impartite cu ADC - ok ca GPIO)
#define K5_PORT PORTC
#define K5_PIN  0
#define K6_PORT PORTC
#define K6_PIN  1
#define K7_PORT PORTC
#define K7_PIN  2
#define K8_PORT PORTC
#define K8_PIN  3
// K9, K10 : PC4, PC5 (D20, D21)
#define K9_PORT PORTC
#define K9_PIN  4
#define K10_PORT PORTC
#define K10_PIN 5

#endif // __PINS_H__
