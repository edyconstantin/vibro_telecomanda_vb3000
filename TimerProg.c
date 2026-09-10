#include "timer.h"
#include "KBD.h"
#include <stdio.h>
#include "TimerProg.h"

struct Timp Tp;
unsigned char Bit1ms=0,Bit10ms=0;
void SelTimer1Init(void)
	{
    /**** Timer1A -> 10ms  *****/
    /**** Timer1B ->  1ms  *****/


	unsigned long SystemClk = 0;
	unsigned long RealPrescaller = TIMER_1A_PRESCALLER_VALUE + 1;
	unsigned long LoadInterval;
	// Enable TIMER1 peripheral
	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER1);
	TimerConfigure(TIMER1_BASE, TIMER_CFG_16_BIT_PAIR | TIMER_CFG_A_PERIODIC | TIMER_CFG_B_PERIODIC);
	TimerPrescaleSet(TIMER1_BASE, TIMER_A, TIMER_1A_PRESCALLER_VALUE);
	SystemClk = SysCtlClockGet();
	LoadInterval = SystemClk / (RealPrescaller * TIMER_1A_DESIRED_INITIAL_FRECQUENCY);
	TimerLoadSet(TIMER1_BASE, TIMER_A, LoadInterval); // set the timer load value after it reaches 0
	TimerControlStall (TIMER1_BASE, TIMER_A, true); // set the timer to stall during debug
	TimerPrescaleSet(TIMER1_BASE, TIMER_B, TIMER_1B_PRESCALLER_VALUE);
	TimerLoadSet(TIMER1_BASE,TIMER_B,TIMER_1B_LOAD_INTERVAL); // set the timer load value after it reaches 0
	TimerControlStall (TIMER1_BASE, TIMER_B, true); // set the timer to stall during debug
	// Disable the timer A and B triggers for the ADC
    HWREG(TIMER1_BASE + TIMER_O_CTL) = (HWREG(TIMER1_BASE + TIMER_O_CTL) & (~(TIMER_CTL_TBOTE | TIMER_CTL_TBOTE)));
    // Enable the timers.
    TimerEnable(TIMER1_BASE, TIMER_BOTH);
	}
void SelTimer1IntEnable(void)
	{
    IntMasterEnable();
	IntEnable(INT_TIMER1A);
	IntEnable(INT_TIMER1B);
    TimerIntEnable(TIMER1_BASE, TIMER_TIMA_TIMEOUT);
	TimerIntEnable(TIMER1_BASE, TIMER_TIMB_TIMEOUT);

	}




void SprintTimp(char *t,struct Timp *p)
{
   sprintf(t,"%02d:%02d:%02d.%02d",p->h,p->m,p->s,p->msec);
}
void IncrementTimp(struct Timp *p)
{
  p->msec++;
  if(p->msec>99)
     {
	 p->msec=0;
	 p->s++;
	 if(p->s>59)
	   {
	   p->s=0;
	   p->m++;
	   if(p->m>59)
	    {
		p->m=0;
		p->h++;
		if(p->h>23) p->h=0;
		};
	   };
	 };
}
void SelTimer1AIntHandler(void)
{
	// !WARNING! This is the interrupt handler for Timer 1 when
	unsigned long TIMER1AIntStatus = 0;
    TIMER1AIntStatus = TimerIntStatus(TIMER1_BASE, true);
    TimerIntClear(TIMER1_BASE, TIMER1AIntStatus);
	if (TIMER1AIntStatus & TIMER_TIMA_TIMEOUT)
		 {
		 // intrerupere la 10 ms
		if(TimeStatusMachine) TimeStatusMachine--;
		 IncrementTimp(&Tp);
		 Bit10ms=1;
		 // Lucrul cu tastatura

		 };
}

void SelTimer1BIntHandler(void)
	{

	unsigned long TIMER1BIntStatus = 0;
    TIMER1BIntStatus = TimerIntStatus(TIMER1_BASE, true);
    TimerIntClear(TIMER1_BASE, TIMER1BIntStatus);
	if (TIMER1BIntStatus & TIMER_TIMB_TIMEOUT)
		{
		// intrerupere la 1 msec
		Bit1ms=1;



		}
	}

