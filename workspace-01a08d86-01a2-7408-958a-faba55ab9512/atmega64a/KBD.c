// KBD.c - tastatura 10 taste pe ATmega64A
// (logica e identica cu versiunea LM3S2965 optimizata: table-driven,
//  fara dublarea KeySlide care exista in originalul Keil)

#include "globals.h"
#include "KBD.h"

int KeyNone = -1;
int LastKey = 0;

int KeyPlusStatus=KEY_IDLE,KeyMinusStatus=KEY_IDLE,KeySelectStatus=KEY_IDLE;
int KeySetStatus=KEY_IDLE,KeySlideStatus=KEY_IDLE,KeySweepStatus=KEY_IDLE;
int KeyPrevStatus=KEY_IDLE,KeyNextStatus=KEY_IDLE,KeyRunStatus=KEY_IDLE,KeyStopStatus=KEY_IDLE;
int KeyPlusTime=0,KeyMinusTime=0,KeySelectTime=0;
int KeySetTime=0,KeySlideTime=0,KeySweepTime=0;
int KeyPrevTime=0,KeyNextTime=0,KeyRunTime=0,KeyStopTime=0;

typedef struct {
    unsigned int *pin;
    int *time;
    int *status;
    int keyid;
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

// citire pin tasta (pe pin, cu masca - corect fata de originalul STM32
// care citea registru intreg de port)
static unsigned int read_key(unsigned char port, unsigned char pin)
{
    return (port & (1 << pin)) ? 1 : 0;
}

void ExecuteKeys(void)
{
    unsigned char i;

    // PINI - conform pins.h (TODO: verifica in schematic)
    Key1  = read_key(K1_PORT , K1_PIN );
    Key2  = read_key(K2_PORT , K2_PIN );
    Key3  = read_key(K3_PORT , K3_PIN );
    Key4  = read_key(K4_PORT , K4_PIN );
    Key5  = read_key(K5_PORT , K5_PIN );
    Key6  = read_key(K6_PORT , K6_PIN );
    Key7  = read_key(K7_PORT , K7_PIN );
    Key8  = read_key(K8_PORT , K8_PIN );
    Key9  = read_key(K9_PORT , K9_PIN );
    Key10 = read_key(K10_PORT, K10_PIN);

    for (i = 0; i < 10; i++) {
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
                // action la eliberare
                break;
            case KEY_GO_DOWN:
                LastKey = k->keyid;
                *k->status = KEY_DOWN;
                // action la apasare
                break;
            case KEY_DOWN:
                // action timp cat e tasta apasata
                break;
            default:
                *k->status = (*k->pin) ? KEY_UP : KEY_DOWN;
                break;
        }
    }
}
