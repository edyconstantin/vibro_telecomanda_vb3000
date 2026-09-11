#ifndef __SEL_TIMER__
#define __SEL_TIMER__

//
// HEADRERS
//
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "inc/hw_ints.h"
#include "inc/hw_gpio.h"
#include "inc/hw_timer.h"
#include "driverlib/interrupt.h"
#include "driverlib/sysctl.h"
#include "driverlib/timer.h"
#include "driverlib/gpio.h"


//
// DEFINITIONS
//
#define TIMER_0_LOAD_INTERVAL					(SysCtlClockGet() / 20)

//#define TIMER_1A_PRESCALLER_VALUE				49   //has to be max 255 (0xff)
#define TIMER_1A_PRESCALLER_VALUE				255   //has to be max 255 (0xff)


//#define TIMER_1_LOAD_INTERVAL					(SysCtlClockGet() / 5000)
//#define TIMER_1_LOAD_INTERVAL					(SysCtlClockGet() / 1000)
#define TIMER_1A_DESIRED_INITIAL_FRECQUENCY		100

// Timer 1 B - ATENTIE: cu prescaler 255 si load 1000 perioada reala e
// 50MHz/(256*1000) = 5.12 ms, NU 1 ms (comentariul original era gresit).
// Bit1ms nu e folosit ca flag de 1 ms in codul actual, deci nu am schimbat
// timpii hardware. Pentru 1 ms ar fi: prescaler 255, load 195.
#define TIMER_1B_PRESCALLER_VALUE				255   //has to be max 255 (0xff)
#define TIMER_1B_LOAD_INTERVAL					1000

//
// VARIABLES
//
//extern unsigned long SelTimer1AInterruptFLAG;


//
// FUNCTIONS
//
extern void SelTimer0AIntHandler(void);
extern void SelTimer0Init(void);
extern void SelTimer0IntEnable(void);

//extern void SelTimer1AIntHandler(void);
extern void SelTimer1Init(void);
extern void SelTimer1IntEnable(void);

#endif
