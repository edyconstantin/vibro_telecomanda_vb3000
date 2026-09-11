// UART0.c - USART1 (RS-485) pe ATmega64A
//
// Port al UART_GEN.c (versiunea compilata in proiectul Keil), cu fix-urile
// din versiunea optimizata:
//  - timeout pe busy-waitul de transmitere
//  - fara scriere out-of-bounds in bufferul RX
//
// USART0 (PD2/PD3) e ocupat de afisor (RW/E) -> folosim USART1:
//  RXD1 = PD0 (D24), TXD1 = PD1 (D25)
// TXEN pune linia pe transmitere doar pe durata mesajului.
//
// AVR: SelUART1SendTxBuffer() primeste text din RAM;
//      SelUART1SendTxBufferP() primeste text din FLASH (literals).

#include "uart0.h"
#include "pins.h"
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <util/delay.h>

unsigned char UART1RxBuffer [UART_MAX_RX_BUFFER_SIZE];
unsigned char UART1RxMessage[UART_MAX_RX_BUFFER_SIZE];
unsigned char UART1TxBuffer [UART_MAX_TX_BUFFER_SIZE];
unsigned char UART1RxData = 0;
unsigned long UART1FLAG_Recieved_Message = FLAG_FALSE;
unsigned long UART1IndexRx = 0;
unsigned long UART1FLAG_TxEmpty = FLAG_TRUE;
unsigned long U1_TX_POINTER_R = 0;

// ---------------------------------------------------------------------------
void SelUART1Init(void)
{
    DDRD  |=  (1<<PD1);                      // TXD1 out
    DDRD  &= ~(1<<PD0);                      // RXD1 in
    UART_TXEN_PORT &= ~(1<<UART_TXEN_PIN);   // TXEN initial 0

    UCSR1B = 0;                              // opreste USART1 in timp ce config
    UBRR1H = 0;
    UBRR1L = UART_UBRR_VALUE;                // 115200 @ 16 MHz (eroare -3.55%)
    UCSR1C = (1<<UCSZ11) | (1<<UCSZ10);      // 8 bit, N, 1 (async = default)
    UCSR1B = (1<<RXEN1) | (1<<TXEN1);        // activeaza RX + TX
}

void SelUART1TransmitEnable(void)
{
    UART_TXEN_PORT |=  (1<<UART_TXEN_PIN);   // TXEN activ (high)
    _delay_us(100);                          // timp de impregnare RS-485
}

void SelUART1TransmitDisable(void)
{
    UART_TXEN_PORT &= ~(1<<UART_TXEN_PIN);   // linia pe receptie
}

// ---------------------------------------------------------------------------
// RX: mesajul incepe cu '#' (reset indice) si se termina cu '\r'
// ---------------------------------------------------------------------------
ISR(USART1_RX_vect)
{
    unsigned long i;

    UART1RxData = UDR1;

    switch (UART1RxData) {
        case UART_MESSAGE_END_CHARACTER:      // '\r' -> copia mesajul
            if (UART1IndexRx) {
                for (i = 0; i < UART1IndexRx; i++)
                    UART1RxMessage[i] = UART1RxBuffer[i];
                UART1RxMessage[i] = 0;
                UART1FLAG_Recieved_Message = FLAG_TRUE;
                UART1IndexRx = 0;
            }
            break;

        case UART_RX_START_CHARACTER:         // '#' -> reset indice
            UART1IndexRx = 0;
            /* fallthrough */

        default:
            // corectat: terminatorul ramane mereu in interiorul bufferului
            if (UART1IndexRx < UART_MAX_RX_BUFFER_SIZE - 1) {
                UART1RxBuffer[UART1IndexRx]     = UART1RxData;
                UART1RxBuffer[UART1IndexRx + 1] = 0;
                UART1IndexRx++;
            } else {
                UART1IndexRx = 0;             // buffer plin -> respinge mesajul
            }
            break;
    }
}

// ---------------------------------------------------------------------------
// TX: un caracter pe fiecare TX complete; ultimul '0' incheie mesajul
// ---------------------------------------------------------------------------
ISR(USART1_TX_vect)
{
    unsigned char c = UART1TxBuffer[U1_TX_POINTER_R++];

    if (c) {
        UDR1 = c;                             // urmatorul caracter pe linie
    } else {
        SelUART1TransmitDisable();
        UART1FLAG_TxEmpty = FLAG_TRUE;
        UCSR1B &= ~(1<<UDRIE1);               // opreste ISR-ul de TX
    }
}

// porneste transmiterea (dupa ce UART1TxBuffer e umplut cu i caractere)
static void uart1_tx_start(unsigned int i)
{
    UCSR1B &= ~(1<<UDRIE1);                   // opreste TX in timp ce config
    SelUART1TransmitEnable();
    UART1FLAG_TxEmpty = FLAG_FALSE;

    UDR1 = UART1TxBuffer[0];                  // primul caracter
    U1_TX_POINTER_R = 1;

    UCSR1B |= (1<<UDRIE1);                    // plimbatul continua din ISR
}

// asteapta transmiterea anterioara (cu timeout + reset stare)
static void uart1_wait_tx_idle(void)
{
    unsigned long timeout = 0;
    // ~60 ms la 16 MHz
    while (UART1FLAG_TxEmpty == 0 && timeout < 300000) timeout++;
    if (UART1FLAG_TxEmpty == 0) {
        UCSR1B &= ~(1<<UDRIE1);
        SelUART1TransmitDisable();
        UART1FLAG_TxEmpty = FLAG_TRUE;
    }
}

void SelUART1SendTxBuffer(unsigned char *text)
{
    unsigned int i;
    unsigned char c;

    if (text[0] == 0) return;

    uart1_wait_tx_idle();

    for (i = 0; i < UART_MAX_TX_BUFFER_SIZE - 4; i++) {
        c = text[i];
        if (c == 0) break;
        UART1TxBuffer[i] = c;
    }
    UART1TxBuffer[i++] = 0xFF;                // separator pe linia RS-485
    UART1TxBuffer[i]   = 0;                   // terminator (opreste TX)

    uart1_tx_start(i - 2);
}

void SelUART1SendTxBufferP(const char *text)
{
    unsigned int i;
    unsigned char c;

    if (pgm_read_byte(text) == 0) return;

    uart1_wait_tx_idle();

    for (i = 0; i < UART_MAX_TX_BUFFER_SIZE - 4; i++) {
        c = pgm_read_byte(text + i);
        if (c == 0) break;
        UART1TxBuffer[i] = c;
    }
    UART1TxBuffer[i++] = 0xFF;
    UART1TxBuffer[i]   = 0;

    uart1_tx_start(i - 2);
}
