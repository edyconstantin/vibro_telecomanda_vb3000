#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_gpio.h"
//#include "driverlib/debug.h"
//#include "driverlib/gpio.h"
//#include "driverlib/interrupt.h"
//#include "driverlib/sysctl.h"


#include "globals.h"
#include "KBD.h"

void ReadKeys(void)
    {
	Key1 =HWREG(KBD_INPORT_BASE1 + (GPIO_O_DATA + (KBD_K1 << 2)));  
	Key2 =HWREG(KBD_INPORT_BASE1 + (GPIO_O_DATA + (KBD_K2 << 2)));  
	Key3 =HWREG(KBD_INPORT_BASE1 + (GPIO_O_DATA + (KBD_K3 << 2)));  
	Key4 =HWREG(KBD_INPORT_BASE1 + (GPIO_O_DATA + (KBD_K4 << 2)));  

	Key5 =HWREG(KBD_INPORT_BASE2 + (GPIO_O_DATA + (KBD_K5 << 2)));  
	Key6 =HWREG(KBD_INPORT_BASE2 + (GPIO_O_DATA + (KBD_K6 << 2)));  
	Key7 =HWREG(KBD_INPORT_BASE2 + (GPIO_O_DATA + (KBD_K7 << 2)));  
	Key8 =HWREG(KBD_INPORT_BASE2 + (GPIO_O_DATA + (KBD_K8 << 2)));  

	Key9 =HWREG(KBD_INPORT_BASE3 + (GPIO_O_DATA + (KBD_K9 << 2)));  
	Key10=HWREG(KBD_INPORT_BASE3 + (GPIO_O_DATA + (KBD_K10<< 2)));  
	}


int KeyPlusStatus=KEY_IDLE,KeyMinusStatus=KEY_IDLE,KeySelectStatus=KEY_IDLE;
int KeySetStatus=KEY_IDLE,KeySlideStatus=KEY_IDLE,KeySweepStatus=KEY_IDLE;
int KeyPrevStatus=KEY_IDLE,KeyNextStatus=KEY_IDLE,KeyRunStatus=KEY_IDLE,KeyStopStatus=KEY_IDLE;
int KeyPlusTime=0,KeyMinusTime=0,KeySelectTime=0;
int KeySetTime=0,KeySlideTime=0,KeySweepTime=0;
int KeyPrevTime=0,KeyNextTime=0,KeyRunTime=0,KeyStopTime;
extern int KeyNone=-1;
int LastKey=0;



void ExecuteKeys(){
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



if(KeyPlus) {
	if(KeyPlusTime <0) {KeyPlusTime=0; KeyPlusStatus = KEY_GO_UP;}   
	if(KeyPlusTime <10000) KeyPlusTime++;
	}
	else {
		if(KeyPlusTime >0) {KeyPlusTime=0; KeyPlusStatus = KEY_GO_DOWN;}  
		if(KeyPlusTime >-10000) KeyPlusTime--;
		}
if(KeyMinus) {
	 if(KeyMinusTime <0) {KeyMinusTime=0; KeyMinusStatus = KEY_GO_UP;}   
	 if(KeyMinusTime <10000) KeyMinusTime++;
	 }
	 else {
		 if(KeyMinusTime >0) {KeyMinusTime=0; KeyMinusStatus = KEY_GO_DOWN;}  
		 if(KeyMinusTime >-10000) KeyMinusTime--;
		 }
if(KeySelect) {
	 if(KeySelectTime <0) {KeySelectTime=0; KeySelectStatus = KEY_GO_UP;}   
	 if(KeySelectTime <10000) KeySelectTime++;
	 }
	 else {
		 if(KeySelectTime >0) {KeySelectTime=0; KeySelectStatus = KEY_GO_DOWN;}  
		 if(KeySelectTime >-10000) KeySelectTime--;
		 }
if(KeySet){
	 if(KeySetTime <0) {KeySetTime=0; KeySetStatus = KEY_GO_UP;}   
	 if(KeySetTime <10000) KeySetTime++;
	 }
	 else {
		 if(KeySetTime >0) {KeySetTime=0; KeySetStatus = KEY_GO_DOWN;}  
		 if(KeySetTime >-10000) KeySetTime--;
		 }
if(KeySlide){
	 if(KeySlideTime <0) {KeySlideTime=0; KeySlideStatus = KEY_GO_UP;}   
	 if(KeySlideTime <10000) KeySlideTime++;
	 }
	else {
		 if(KeySlideTime >0) {KeySlideTime=0; KeySlideStatus = KEY_GO_DOWN;}  
		 if(KeySlideTime >-10000) KeySlideTime--;
		 }
if(KeySlide){
	 if(KeySlideTime <0) {KeySlideTime=0; KeySlideStatus = KEY_GO_UP;}   
	 if(KeySlideTime <10000) KeySlideTime++;
	 }
	else {
		 if(KeySlideTime >0) {KeySlideTime=0; KeySlideStatus = KEY_GO_DOWN;}  
		 if(KeySlideTime >-10000) KeySlideTime--;
		 }
if(KeySweep){
	 if(KeySweepTime <0) {KeySweepTime=0; KeySweepStatus = KEY_GO_UP;}   
	 if(KeySweepTime <10000) KeySweepTime++;
	 }
	else {
		 if(KeySweepTime >0) {KeySweepTime=0; KeySweepStatus = KEY_GO_DOWN;}  
		 if(KeySweepTime >-10000) KeySweepTime--;
		 }
if(KeyPrev) {
	 if(KeyPrevTime <0) {KeyPrevTime=0; KeyPrevStatus = KEY_GO_UP;}   
	 if(KeyPrevTime <10000) KeyPrevTime++;
	 }
	else {
		 if(KeyPrevTime >0) {KeyPrevTime=0; KeyPrevStatus = KEY_GO_DOWN;}  
		 if(KeyPrevTime >-10000) KeyPrevTime--;
		 }
if(KeyNext) {
	 if(KeyNextTime <0) {KeyNextTime=0; KeyNextStatus = KEY_GO_UP;}   
	 if(KeyNextTime <10000) KeyNextTime++;
	 }
	else {
		 if(KeyNextTime >0) {KeyNextTime=0; KeyNextStatus = KEY_GO_DOWN;}  
		 if(KeyNextTime >-10000) KeyNextTime--;
		 }
if(KeyRun) {
	 if(KeyRunTime <0) {KeyRunTime=0; KeyRunStatus = KEY_GO_UP;}   
	 if(KeyRunTime <10000) KeyRunTime++;
	 }
	else {
		 if(KeyRunTime >0) {KeyRunTime=0; KeyRunStatus = KEY_GO_DOWN;}  
		 if(KeyRunTime >-10000) KeyRunTime--;
		 }
if(KeyStop) {
	 if(KeyStopTime <0) {KeyStopTime=0; KeyStopStatus = KEY_GO_UP;}   
	 if(KeyStopTime <10000) KeyStopTime++;
	 }
	else
		 {
		 if(KeyStopTime >0) {KeyStopTime=0; KeyStopStatus = KEY_GO_DOWN;}  
		 if(KeyStopTime >-10000) KeyStopTime--;
		 }



switch(KeyPlusStatus) {
	 case KEY_GO_UP:	
			KeyPlusStatus=KEY_UP; 
			// action on release
	 break;
	 case KEY_GO_DOWN:
			LastKey=KEY_KBPLUS;
			KeyPlusStatus=KEY_DOWN; 
				// action on press
	 break;
	 case KEY_DOWN: 
			// action while pressed
	 break;
		 //case KEY_UP:  break;
	 default: 
			if(KeyPlus) KeyPlusStatus=KEY_UP; else KeyPlusStatus=KEY_DOWN;  break;
	 };

switch(KeyMinusStatus) {
	 case KEY_GO_UP:	
			KeyMinusStatus=KEY_UP; 
			// action on release
	 break;
	 case KEY_GO_DOWN:
			LastKey=KEY_KBMINUS;
			KeyMinusStatus=KEY_DOWN; 
				// action on press
	 break;
	 case KEY_DOWN: 
			// action while pressed
	 break;
		 //case KEY_UP:  break;
	 default: 
				if(KeyMinus) KeyMinusStatus=KEY_UP; else KeyMinusStatus=KEY_DOWN;  break;
	 };

switch(KeySelectStatus) {
	 case KEY_GO_UP:	
			KeySelectStatus=KEY_UP; 
			// action on release
	 break;
	 case KEY_GO_DOWN:
			LastKey=KEY_KBSELECT;
			KeySelectStatus=KEY_DOWN; 
				// action on press
	 break;
	 case KEY_DOWN: 
			// action while pressed
	 break;
		 //case KEY_UP:  break;
	 default: 
				if(KeySelect) KeySelectStatus=KEY_UP; else KeySelectStatus=KEY_DOWN;  break;
	 };

switch(KeySetStatus){
	 case KEY_GO_UP:	
			KeySetStatus=KEY_UP; 
			// action on release
	 break;
	 case KEY_GO_DOWN:
		  LastKey=KEY_KBSET;
			KeySetStatus=KEY_DOWN; 
				// action on press
	 break;
	 case KEY_DOWN: 
			// action while pressed
	 break;
		 //case KEY_UP:  break;
	 default: 
				if(KeySet) KeySetStatus=KEY_UP; else KeySetStatus=KEY_DOWN;  break;
	 };

switch(KeySlideStatus) {
	 case KEY_GO_UP:	
			KeySlideStatus=KEY_UP; 
			// action on release
	 break;
	 case KEY_GO_DOWN:
		  LastKey=KEY_KBSLIDE;
			KeySlideStatus=KEY_DOWN; 
				// action on press
	 break;
	 case KEY_DOWN: 
			// action while pressed
	 break;
		 //case KEY_UP:  break;
	 default: 
				if(KeySlide) KeySlideStatus=KEY_UP; else KeySlideStatus=KEY_DOWN;  break;
	 };

switch(KeySweepStatus) {
	 case KEY_GO_UP:	
			KeySweepStatus=KEY_UP; 
			// action on release
	 break;
	 case KEY_GO_DOWN:
		  LastKey=KEY_KBSWEEP;
			KeySweepStatus=KEY_DOWN; 
				// action on press
	 break;
	 case KEY_DOWN: 
			// action while pressed
	 break;
		 //case KEY_UP:  break;
	 default: 
				if(KeySweep) KeySweepStatus=KEY_UP; else KeySweepStatus=KEY_DOWN;  break;
	 
	 };

switch(KeyPrevStatus) {
	 case KEY_GO_UP:	
			KeyPrevStatus=KEY_UP; 
			// action on release
	 break;
	 case KEY_GO_DOWN:
		  LastKey=KEY_KBPREV;
			KeyPrevStatus=KEY_DOWN; 
				// action on press
	 break;
	 case KEY_DOWN: 
			// action while pressed
	 break;
		 //case KEY_UP:  break;
	 default: 
				if(KeyPrev) KeyPrevStatus=KEY_UP; else KeyPrevStatus=KEY_DOWN;  break;
	 
	 };

switch(KeyNextStatus)	 {
	 case KEY_GO_UP:	
			KeyNextStatus=KEY_UP; 
			// action on release
	 break;
	 case KEY_GO_DOWN:
			LastKey=KEY_KBNEXT;
			KeyNextStatus=KEY_DOWN; 
				// action on press
	 break;
	 case KEY_DOWN: 
			// action while pressed
	 break;
		 //case KEY_UP:  break;
	 default: 
				if(KeyNext) KeyNextStatus=KEY_UP; else KeyNextStatus=KEY_DOWN;  break;
	 
	 };

switch(KeyRunStatus) {
	 case KEY_GO_UP:	
			KeyRunStatus=KEY_UP; 
			// action on release
	 break;
	 case KEY_GO_DOWN:
			LastKey=KEY_KBRUN;
			KeyRunStatus=KEY_DOWN; 
				// action on press
	 break;
	 case KEY_DOWN: 
			// action while pressed
	 break;
		 //case KEY_UP:  break;
	 default: 
				if(KeyRun) KeyRunStatus=KEY_UP; else KeyRunStatus=KEY_DOWN;  break;
	 
	 };

switch(KeyStopStatus)	 {
	 case KEY_GO_UP:	
			KeyStopStatus=KEY_UP; 
			// action on release
	 break;
	 case KEY_GO_DOWN:
			LastKey=KEY_KBSTOP;
			KeyStopStatus=KEY_DOWN; 
				// action on press
	 break;
	 case KEY_DOWN: 
			// action while pressed
	 break;
		 //case KEY_UP:  break;
	 default: 
				if(KeyStop) KeyStopStatus=KEY_UP; else KeyStopStatus=KEY_DOWN;  break;
	 
	 };






}


