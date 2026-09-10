#ifndef __TIMERPROG_H__
#define __TIMERPROG_H__
#include "globals.h"
struct Timp
{
   unsigned int msec;
   unsigned char s,m,h;
};
extern struct Timp Tp;
extern unsigned char Bit1ms,Bit10ms;


void SelTimer1Init(void);
void SelTimer1IntEnable(void);
void SprintTimp(char *t,struct Timp *p);
void IncrementTimp(struct Timp *p);
void SelTimer1AIntHandler(void);
void SelTimer1BIntHandler(void);



#endif
