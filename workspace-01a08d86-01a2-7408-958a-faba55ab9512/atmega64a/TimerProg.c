// TimerProg.c - Timer1 (10 ms) + Timer0 (1 ms) pe ATmega64A @ 16 MHz
//
// Timer1: CTC 16 bit, prescaler 64 -> 250 kHz; OCR1A = 2499 -> exact 10 ms
// Timer0: CTC  8 bit, prescaler 64 -> 250 kHz; OCR0A =  249 -> exact 1 ms
// (fix fata de original: Timer1B "1 ms" era de fapt 5.12 ms;
//  aici Bit1ms e corect 1 ms)

#include "TimerProg.h"
#include <avr/interrupt.h>

struct Timp Tp;
unsigned char Bit1ms = 0, Bit10ms = 0;

void SelTimer1Init(void)
{
    // Timer1: CTC, prescaler 64, OCR1A = 2499
    TCCR1A = 0;
    TCCR1B = (1<<WGM12) | (1<<CS11) | (1<<CS10);
    OCR1A  = 2499;
    TCNT1  = 0;

    // Timer0 (8 bit): CTC, prescaler 64, OCR0 = 249
    // (ATmega64: TCCR0 e un singur registru, fara TCCR0B)
    TCCR0  = (1<<WGM01) | (1<<CS01) | (1<<CS00);
    OCR0   = 249;
    TCNT0  = 0;
}

void SelTimer1IntEnable(void)
{
    // pe ATmega64, Timer0 si Timer1 impart un singur registru TIMSK
    TIMSK = (1<<OCIE0) | (1<<OCIE1A);   // Timer0: 1 ms, Timer1: 10 ms
    sei();
}

// 10 ms: decrementa TimeStatusMachine, avanseaza ceasul, pune Bit10ms
ISR(TIMER1_COMPA_vect)
{
    if (TimeStatusMachine) TimeStatusMachine--;
    IncrementTimp(&Tp);
    Bit10ms = 1;
}

// 1 ms
ISR(TIMER0_COMP_vect)
{
    Bit1ms = 1;
}

// SprintTimp fara stdio (sprintf consuma mult flash pe AVR)
void SprintTimp(char *t, struct Timp *p)
{
    t[0]  = (char)('0' + p->h  / 10);
    t[1]  = (char)('0' + p->h  % 10);
    t[2]  = ':';
    t[3]  = (char)('0' + p->m  / 10);
    t[4]  = (char)('0' + p->m  % 10);
    t[5]  = ':';
    t[6]  = (char)('0' + p->s  / 10);
    t[7]  = (char)('0' + p->s  % 10);
    t[8]  = '.';
    t[9]  = (char)('0' + p->msec / 10);
    t[10] = (char)('0' + p->msec % 10);
    t[11] = 0;
}

void IncrementTimp(struct Timp *p)
{
    p->msec++;
    if (p->msec > 99) {
        p->msec = 0;
        p->s++;
        if (p->s > 59) {
            p->s = 0;
            p->m++;
            if (p->m > 59) {
                p->m = 0;
                p->h++;
                if (p->h > 23) p->h = 0;
            }
        }
    }
}
