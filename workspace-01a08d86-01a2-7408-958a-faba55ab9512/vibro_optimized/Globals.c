#include "globals.h"

unsigned long LastCommand=LC_NONE;  
unsigned long LastCmd=LC_NONE;  


unsigned long SystemClock = 0;
int MainStatusMachine=MSM_START;
int TimeStatusMachine=0;

int VibsistNumber=0;
int Tstart=0,Tstop=0,F1Strike=0;
int KIntegral=20,KDiferential=30,KProportional=40;
int TSweep=0,TDelayOn=0,TDelayOff=0;
int DetectChannel=0,IsRectified0=1,IsReverted0=0,DetectLevel0=700,NSamples0=4,BlindSamples0=100,Amplify0=7;
int IsRectified1=1,IsReverted1=0,DetectLevel1=700,NSamples1=4,BlindSamples1=100,Amplify1=7;


//char selected_Mode=SELECTED_MODE_VERTICAL,last_selected_Mode=SELECTED_MODE_VERTICAL;


char selected_Parameter=SELECTED_NONE;




unsigned char  CONNECTION_STATE= NO_COMMUNICATION;
char FLAG_have_second=0;
char FLAG_scan_keys=0;
char FLAG_inverted_lcd=0;

unsigned int Key1=0,Key2=0,Key3=0,Key4=0,Key5=0,Key6=0,Key7=0,Key8=0,Key9=0,Key10=0;

unsigned char WMessage[64];
unsigned int HaveWMessage=0;

//char selected_Valve=SV_SV1 ; 
unsigned int SV1=0,SV2=0,SV3=0,SV4=0;		
unsigned char VText[24];

// Detect signal
int BlindPeriod=0;
int BlindLevel=0;
int Amplify=0;
int TriggerLevel=0;


