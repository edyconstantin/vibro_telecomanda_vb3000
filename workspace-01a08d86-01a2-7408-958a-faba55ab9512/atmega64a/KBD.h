// KBD.h - tastatura 10 taste, ATmega64A
#ifndef __KBD_H__
#define __KBD_H__

#include "pins.h"

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

#define KEY_KB1  1
#define KEY_KB2  2
#define KEY_KB3  4
#define KEY_KB4  8
#define KEY_KB5  16
#define KEY_KB6  32
#define KEY_KB7  64
#define KEY_KB8  128
#define KEY_KB9  256
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

extern unsigned int Key1,Key2,Key3,Key4,Key5,Key6,Key7,Key8,Key9,Key10;
extern int KeyPlusStatus,KeyMinusStatus,KeySelectStatus;
extern int KeySetStatus,KeySlideStatus,KeySweepStatus;
extern int KeyPrevStatus,KeyNextStatus,KeyRunStatus,KeyStopStatus;
extern int KeyPlusTime,KeyMinusTime,KeySelectTime;
extern int KeySetTime,KeySlideTime,KeySweepTime;
extern int KeyPrevTime,KeyNextTime,KeyRunTime,KeyStopTime;
extern int KeyNone;
extern int LastKey;

void ExecuteKeys(void);

#endif
