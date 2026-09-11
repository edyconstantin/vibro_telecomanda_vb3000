#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_gpio.h"

#include "globals.h"
#include "KBD.h"

// Definirea corecta (in original era "extern int KeyNone=-1;" - extern cu
// initializer e invalid in C si da greseli de link pe unele toolchain-uri)
int KeyNone=-1;

int LastKey=0;

int KeyPlusStatus=KEY_IDLE,KeyMinusStatus=KEY_IDLE,KeySelectStatus=KEY_IDLE;
int KeySetStatus=KEY_IDLE,KeySlideStatus=KEY_IDLE,KeySweepStatus=KEY_IDLE;
int KeyPrevStatus=KEY_IDLE,KeyNextStatus=KEY_IDLE,KeyRunStatus=KEY_IDLE,KeyStopStatus=KEY_IDLE;
int KeyPlusTime=0,KeyMinusTime=0,KeySelectTime=0;
int KeySetTime=0,KeySlideTime=0,KeySweepTime=0;
int KeyPrevTime=0,KeyNextTime=0,KeyRunTime=0,KeyStopTime=0;

// ---------------------------------------------------------------------------
// Optimizare: in loc de 10 bucati de cod copiat-lipit (peste 250 de linii,
// dintre care blocul KeySlide era DUBLICAT - probabil gresala de copy-paste,
// ceea ce facea timpii tastei Slide de doua ori mai rapidi ca ai celorlalte),
// totul e acum table-driven. Comportamentul pentru fiecare tasta ramane exact
// același ca in original (exceptie: KeySlide nu mai are dublarea accidentala).
// ---------------------------------------------------------------------------
typedef struct {
    unsigned int *pin;      // valoarea citita (Key1..Key10)
    int *time;              // comparatorul de timp
    int *status;            // starea tastei
    int keyid;              // valoarea pusa in LastKey la eliberare
} KeyDef;

static const KeyDef Keys[10] = {
    { &Key5,   &KeyPlusTime,   &KeyPlusStatus,   KEY_KBPLUS   },
    { &Key4,   &KeyMinusTime,  &KeyMinusStatus,  KEY_KBMINUS  },
    { &Key3,   &KeySelectTime, &KeySelectStatus, KEY_KBSELECT },
    { &Key10,  &KeySetTime,    &KeySetStatus,    KEY_KBSET    },
    { &Key9,   &KeySlideTime,  &KeySlideStatus,  KEY_KBSLIDE  },
    { &Key8,   &KeySweepTime,  &KeySweepStatus,  KEY_KBSWEEP  },
    { &Key2,   &KeyPrevTime,   &KeyPrevStatus,   KEY_KBPREV   },
    { &Key7,   &KeyNextTime,   &KeyNextStatus,   KEY_KBNEXT   },
    { &Key1,   &KeyRunTime,    &KeyRunStatus,    KEY_KBRUN    },
    { &Key6,   &KeyStopTime,   &KeyStopStatus,   KEY_KBSTOP   },
};

void ExecuteKeys(){
    int i;

    // citirea pinilor - exact ca in original (valori brute pe porti)
    Key1 =HWREG(KBD_INPORT_BASE1 + (GPIO_O_DATA + (KBD_K1 << 2)));  
    Key2 =HWREG(KBD_INPORT_BASE1 + (GPIO_O_DATA + (KBD_K2 << 2)));  
    Key3 =HWREG(KBD_INPORT_BASE1 + (GPIO_O_DATA + (KBD_K3 << 2)));  
    Key4 =HWREG(KBD_INPORT_BASE1 + (GPIO_O_DATA + (KBD_K4 << 2)));  

    Key5 =(HWREG(KBD_INPORT_BASE2 + (GPIO_O_DATA + (KBD_K5 << 2))))<<4;  
    Key6 =(HWREG(KBD_INPORT_BASE2 + (GPIO_O_DATA + (KBD_K6 << 2))))<<4;  
    Key7 =(HWREG(KBD_INPORT_BASE2 + (GPIO_O_DATA + (KBD_K7 << 2))))<<4;  
    Key8 =(HWREG(KBD_INPORT_BASE2 + (GPIO_O_DATA + (KBD_K8 << 2))))<<4;  

    Key9 =(HWREG(KBD_INPORT_BASE3 + (GPIO_O_DATA + (KBD_K9 << 2))))<<8;  
    Key10=(HWREG(KBD_INPORT_BASE3 + (GPIO_O_DATA + (KBD_K10<< 2))))<<8;  

    for (i=0;i<10;i++) {
        const KeyDef *k = &Keys[i];

        if (*k->pin) {
            if (*k->time < 0) { *k->time = 0; *k->status = KEY_GO_UP; }
            if (*k->time < 10000) (*k->time)++;
        } else {
            if (*k->time > 0) { *k->time = 0; *k->status = KEY_GO_DOWN; }
            if (*k->time > -10000) (*k->time)--;
        }

        switch (*k->status) {
            case KEY_GO_UP:
                *k->status = KEY_UP;
                // action on release
                break;
            case KEY_GO_DOWN:
                LastKey = k->keyid;
                *k->status = KEY_DOWN;
                // action on press
                break;
            case KEY_DOWN:
                // action while pressed
                break;
            default:
                *k->status = (*k->pin) ? KEY_UP : KEY_DOWN;
                break;
        }
    }
}
