#ifndef __KBD_H__
#define __KBD_H__
#include "gpio.h"

//void GPIOPinTypeGPIOInput(unsigned long ulPort, unsigned char ucPins)
#define	KBD_INPORT_PORT1     SYSCTL_PERIPH_GPIOE
#define	KBD_INPORT_BASE1      GPIO_PORTE_BASE
#define KBD_K4 		 GPIO_PIN_0
#define KBD_K3 		 GPIO_PIN_1
#define KBD_K2 		 GPIO_PIN_2
#define KBD_K1 		 GPIO_PIN_10

#define	KBD_INPORT_PORT2      SYSCTL_PERIPH_GPIOB
#define	KBD_INPORT_BASE2      GPIO_PORTB_BASE
#define KBD_K8 		 GPIO_PIN_0
#define KBD_K7 		 GPIO_PIN_1
#define KBD_K6 		 GPIO_PIN_2
#define KBD_K5 		 GPIO_PIN_3

#define	KBD_INPORT_PORT3      SYSCTL_PERIPH_GPIOF
#define	KBD_INPORT_BASE3      GPIO_PORTF_BASE
#define KBD_K9 		 GPIO_PIN_1
#define KBD_K10 	 GPIO_PIN_2

#define KeyPlus   Key5
#define KeyMinus  Key4
#define KeySelect Key3
#define KeySweep  Key8
#define KeySlide  Key9
#define KeySet    Key10
#define KeyPrev   Key2
#define KeyNext   Key7
#define KeyRun    Key1
#define KeyStop   Key6


#define KEY_KB1 1
#define KEY_KB2 2
#define KEY_KB3 4
#define KEY_KB4 8
#define KEY_KB5 16
#define KEY_KB6 32
#define KEY_KB7 64
#define KEY_KB8 128
#define KEY_KB9 256
#define KEY_KB10 512

#define KEY_KBPLUS   KEY_KB5
#define KEY_KBMINUS  KEY_KB4
#define KEY_KBSELECT KEY_KB3
#define KEY_KBSWEEP  KEY_KB8
#define KEY_KBSLIDE  KEY_KB9
#define KEY_KBSET    KEY_KB10
#define KEY_KBPREV   KEY_KB2
#define KEY_KBNEXT   KEY_KB7
#define KEY_KBRUN    KEY_KB1
#define KEY_KBSTOP   KEY_KB6



enum {
   
   KEY_IDLE,
   KEY_GO_UP,
   KEY_GO_DOWN,
   KEY_DOWN,
   KEY_UP
};

extern int KeyPlusStatus,KeyMinusStatus,KeySelectStatus;
extern int KeySetStatus,KeySlideStatus,KeySweepStatus;

extern int KeyPlusTime,KeyMinusTime,KeySelectTime;
extern int KeySetTime,KeySlideTime,KeySweepTime;
extern int KeyRunTime;
extern int KeyNone;
extern int LastKey;


void ReadKeys(void);
void ExecuteKeys(void);



#endif

