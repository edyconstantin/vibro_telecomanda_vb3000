// uart0.h - USART1 (RS-485) spre vibsist, ATmega64A
// (numele de functii/variabile sunt pastrati din original: SelUART1*)
#ifndef __UART0_H__
#define __UART0_H__

#include <avr/io.h>

#define UART_BAUDRATE            115200
// 16 MHz, 16x oversampling: UBRR = 16e6/(16*115200)-1 = 7.68 -> 8
// (baud real 111111, eroare -3.55% - acceptabil la 8N1)
#define UART_UBRR_VALUE          8

#define UART_MAX_RX_BUFFER_SIZE  64   // mesajul cel mai lung e ~24 byte
#define UART_MAX_TX_BUFFER_SIZE  64

#define UART_MESSAGE_END_CHARACTER '\r'   // 0x0D
#define UART_RX_START_CHARACTER    '#'    // 0x23

#define FLAG_TRUE  1UL
#define FLAG_FALSE 0UL

extern unsigned char UART1RxBuffer [UART_MAX_RX_BUFFER_SIZE];
extern unsigned char UART1RxMessage[UART_MAX_RX_BUFFER_SIZE];
extern unsigned char UART1TxBuffer [UART_MAX_TX_BUFFER_SIZE];
extern unsigned char UART1RxData;
extern unsigned long UART1FLAG_Recieved_Message;
extern unsigned long UART1IndexRx;
extern unsigned long UART1FLAG_TxEmpty;
extern unsigned long U1_TX_POINTER_R;

void SelUART1Init(void);
void SelUART1SendTxBuffer(unsigned char *text);          // text din RAM
void SelUART1SendTxBufferP(const char *text);   // text din FLASH
void SelUART1TransmitEnable(void);
void SelUART1TransmitDisable(void);

#endif
