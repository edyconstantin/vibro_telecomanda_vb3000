#ifndef __HF_GLOBALS__
#define __HF_GLOBALS__

#define MY_V_NUMBER 500
extern unsigned long SystemClock;


#define NUMBER_OF_MODES 4 // 3 + NULL_MODE

 // Valori in ms
#define MAX_T_VALUE				  500
#define MIN_T_VALUE				  20
#define MAX_STRIKE_VALUE		999
#define MIN_STRIKE_VALUE		0
#define MAX_SWEEP_VALUE			120
#define MIN_SWEEP_VALUE			5
#define MIN_DELAY_VALUE     120  
#define MAX_K_VALUE         999
#define MIN_K_VALUE         0
//#define DEFAULT_DLYON_VALUE			2
//#define DEFAULT_DLYOFF_VALUE		4


#define TmaxNotOk        1
#define TminNotOk        2
#define F1StrikeNotOk    4



// Last command from KBD
#define LC_NONE        			0x0000
#define LC_H_MODIFIED  			0x0001
#define LC_W_MODIFIED  			0x0002
#define LC_K_MODIFIED  			0x0004
#define LC_D_MODIFIED  			0x0400
#define LC_SET_MODE    			0x0008
#define LC_SLIDE_MODE  			0x0010
#define LC_SLIDE_ACT   			0x0020
#define LC_SWEEP_MODE  			0x0040
#define LC_ACCUMULATOR_LOAD 0x0080
#define LC_RUN_START   			0x0100
#define LC_RUN_STOP    			0x0200


#define TmsecMinim 50
#define TmsecMaxim 400


extern unsigned long LastCommand;  
extern unsigned long LastCmd;  



enum 
{
SELECTED_NONE,
SELECTED_TStart,
SELECTED_T1st,
SELECTED_TStop,
SELECTED_OnDelay,
SELECTED_SWEEP,
SELECTED_OffDelay,
SELECTED_Kr,
SELECTED_Ki,
SELECTED_Kd,
SELECTED_Amplify,
SELECTED_Level,
SELECTED_BlindPeriod,
SELECTED_BlindLevel

//SELECTED_DetectedChannel,
//SELECTED_Rectified,
//SELECTED_Reverted,
//SELECTED_Level,
//SELECTED_NSamples,
//SELECTED_Blind,
//SELECTED_Amplify


};

extern int DetectChannel,IsRectified0,IsReverted0,DetectLevel0,NSamples0,BlindSamples0,Amplify0;
extern int IsRectified1,IsReverted1,DetectLevel1,NSamples1,BlindSamples1,Amplify1;



#define NUMBER_OF_PARAMETERS 7 // 4 + NULL_PARAMETER
struct parameter
  {
  unsigned int Value;
  unsigned int maxValue;
  unsigned int minValue;
  };
extern struct parameter Parameters[NUMBER_OF_PARAMETERS];
extern char selected_Parameter;

extern int VibsistNumber;
extern int Tstart,Tstop,F1Strike;
extern int KIntegral,KDiferential,KProportional;
extern int TSweep,TDelayOn,TDelayOff;



 enum 
{
SELECTED_MODE_VERTICAL, 
SELECTED_MODE_VERTICAL_ADJUST, 
SELECTED_MODE_HORIZONTAL,
SELECTED_MODE_HORIZONTAL_ADJUST
}; 

enum
{
NO_COMMUNICATION,
COMM_OK,
TRANSMISSION_TIMEOUT,
RECIEVED_JUNK,
RECEPTION_TIMEOUT,
INVALID_PARAMETERS,
INVALID_DATA
};

extern char selected_Mode, last_selected_Mode;
extern unsigned char  CONNECTION_STATE;


// FLAGS

extern char FLAG_have_second;
extern char FLAG_scan_keys;
extern char FLAG_inverted_lcd;

// STATE MACHINE
/*
enum{
    S_IDLE,
    S_INIT,
    S_STARTUP,
    S_GET_PAR_INIT,
    S_GET_PAR,
    S_SETTINGS_INIT,
    S_SETTINGS,
    S_RUN_SET_PARAM_INIT,
    S_RUN_SET_PARAM,
    S_RUN_INIT,
    S_RUN
    };
  */
enum {
    MSM_START,
	MSM_REST,
	MSM_GET_PAR,
	MSM_GO_SET0,
	MSM_SET0,
	MSM_SET1,
	MSM_SET2,
	MSM_SET3,
	MSM_SET4,
	MSM_GO_SLIDE,
	MSM_SLIDE,
	MSM_GO_SWEEP,
	MSM_SWEEP,
	MSM_GO_RUN,
	MSM_RUN,
	MSM_GO_STOP_SWEEP,
	MSM_STOP_SWEEP,
	MSM_ERR_COM
};

extern int MainStatusMachine;

extern int TimeStatusMachine;
extern unsigned int Key1,Key2,Key3,Key4,Key5,Key6,Key7,Key8,Key9,Key10;


extern unsigned char WMessage[64];
extern unsigned int HaveWMessage;


enum {SV_SV1, SV_SV2, SV_SV4};
//extern char selected_Valve; 
extern unsigned int SV1,SV2,SV3,SV4;		
extern unsigned char VText[24];

// Detect signal
extern int BlindPeriod;
extern int BlindLevel;
extern int Amplify;
extern int TriggerLevel;


#endif

