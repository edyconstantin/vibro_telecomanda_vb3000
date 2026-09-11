// main.c - telecomanda vibratii VB3000 pe ATmega64A
//
// Port al main.c din proiectul Keil (LM3S2965). Fluxul e identic:
//   init periferice -> DisplayReset -> Timere -> UART -> bucla cu
//   masina de stari la 10 ms (bit B10ms din Timer1).
//
// F_CPU = 16 MHz (cristal extern pe PB5/PB6). Vezi pins.h.

#include <avr/io.h>
#include <util/delay.h>

#include "pins.h"
#include "globals.h"
#include "OLED.h"
#include "Draws.h"
#include "uart0.h"
#include "Message.h"
#include "TimerProg.h"
#include "KBD.h"

// ---------------------------------------------------------------------------
// (In originalul Keil, watch_dog_start() era DEFINIT dar comentat/NESOSIT,
//  deci nu il portez. Watchdog-ul ramane oprit, ca la original.)
// ---------------------------------------------------------------------------
static void lcd_gpio_init(void)
{
    // magistrala de date: DB0..DB3 = PD4..PD7, DB4..DB7 = PE0..PE3
    DDRD |= LCD_DB_LOW_PINS;
    DDRE |= LCD_DB_HIGH_PINS;

    // pini de control: EN(PD3), RW(PD2), DI(PE4), CS1(PE5), CS2(PE6), RES(PA4)
    DDRD  |= (1<<LCD_EN_PIN) | (1<<LCD_RW_PIN);
    DDRE  |= (1<<LCD_DI_PIN) | (1<<LCD_CS1_PIN) | (1<<LCD_CS2_PIN);
    DDRA  |= (1<<LCD_RES_PIN);

    // starea initiala, ca in original: tot 1, date 0xFF
    LCD_EN_PORT  |= (1<<LCD_EN_PIN);
    LCD_RW_PORT  |= (1<<LCD_RW_PIN);
    LCD_DI_PORT  |= (1<<LCD_DI_PIN);
    LCD_CS1_PORT |= (1<<LCD_CS1_PIN);
    LCD_CS2_PORT |= (1<<LCD_CS2_PIN);
    LCD_RES_PORT |= (1<<LCD_RES_PIN);
    LCD_DB_LOW_PORT  |= LCD_DB_LOW_PINS;
    LCD_DB_HIGH_PORT |= LCD_DB_HIGH_PINS;
}

int main(void)
{
    // *** GPIO afisor ***
    lcd_gpio_init();

    // *** GPIO tastatura (input) ***
    DDRB &= ~((1<<K1_PIN) | (1<<K2_PIN) | (1<<K3_PIN) | (1<<K4_PIN));
    DDRC &= ~((1<<K5_PIN) | (1<<K6_PIN) | (1<<K7_PIN) | (1<<K8_PIN) |
              (1<<K9_PIN) | (1<<K10_PIN));
    // daca tastele nu au pull extern, activeaza pull-up-urile:
    // PORTB |= (1<<K1_PIN)|(1<<K2_PIN)|(1<<K3_PIN)|(1<<K4_PIN);
    // PORTC |= (1<<K5_PIN)|(1<<K6_PIN)|(1<<K7_PIN)|(1<<K8_PIN)|(1<<K9_PIN)|(1<<K10_PIN);

    _delay_ms(1);

    // *** afisor: reset + init (o singura data) ***
    DisplayReset();

    // *** timere: 10 ms + 1 ms ***
    SelTimer1Init();
    SelTimer1IntEnable();

    // *** UART RS-485 ***
    SelUART1Init();

    SystemClock = F_CPU;

    while (1) {
        if (Bit1ms) {
            Bit1ms = 0;
        }

        if (UART1FLAG_Recieved_Message) {
            // decodificam mesajul
            DecodeMsg(UART1RxMessage);
            UART1FLAG_Recieved_Message = 0;
        }

        if (Bit10ms) {  // actiuni la 10 ms
            ExecuteKeys();
            switch (MainStatusMachine) {
                case MSM_START:  // display SELETRON, wait for answer
                    ClearPage(0);
                    PutImage(siglaSeletron);
                    TimeStatusMachine = 40;
                    MainStatusMachine = MSM_REST;
                    break;
                case MSM_REST:
                    if (TimeStatusMachine == 0) {
                        // nu a reusit comunicatia in 0.5 secunde
                        TimeStatusMachine = 100;
                        MainStatusMachine = MSM_GET_PAR;
                    }
                    break;
                case MSM_GET_PAR:
                    if (VibsistNumber == MY_V_NUMBER) MainStatusMachine = MSM_GO_SET0;
                    if (TimeStatusMachine == 0) {
                        TimeStatusMachine = 100;
                        MainStatusMachine = MSM_ERR_COM;
                    }
                    break;
                case MSM_ERR_COM: // eroare de comunicatie - reincercam dupa 1 s
                    if (TimeStatusMachine == 0) {
                        SelUART1SendTxBufferP(">START\r");
                        MainStatusMachine = MSM_START;
                    }
                    break;
                case MSM_GO_SET0:
                    selected_Parameter = SELECTED_NONE;
                    MainStatusMachine = MSM_SET0;
                    break;
                case MSM_GO_SLIDE:  MainStatusMachine = MSM_SLIDE;  break;
                case MSM_GO_SWEEP:  MainStatusMachine = MSM_SWEEP;  break;
                case MSM_GO_RUN:    MainStatusMachine = MSM_RUN;    break;
            }

            DrawScreen();
            Bit10ms = 0;
        }
    }
}
