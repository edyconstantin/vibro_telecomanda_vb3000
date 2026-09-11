#include "globals.h" 
#include <stdio.h> 
#include "sel_uart.h" 
 
 int IsNum(unsigned char c)
 {
    if(c>='0')
	  if(c<='9')
	     return c-'0';
	return -1;
 
 }
 int IsNumber(unsigned char *text,int m)
 {
    int v1=0,v=0,i;
  	for(i=0;i<m;i++)
	  {
	  v1*=10;
	  v=IsNum(text[i]);
	  if(v<0) return v1;
	  v1+=v;
	  };
	return v1;
 } 
 
void SendHBuff(void)
{
    char text[64];
 	sprintf(text,">SH,%03d,%03d,%03d\r",Tstart,Tstop,F1Strike);
    SelUART1SendTxBuffer((unsigned char *)text);
}
void SendWBuff(void)
{
    char text[64];
	sprintf(text,">SW,%03d,%03d,%03d\r",TSweep, TDelayOn, TDelayOff);
   	SelUART1SendTxBuffer((unsigned char *)text);

}
void SendKBuff(void)
{
    char text[64];
 	sprintf(text,">SK,%03d,%03d,%03d\r",KProportional, KIntegral,KDiferential);
    SelUART1SendTxBuffer((unsigned char *)text);
}
void SendDBuff(void)
{
    char text[64];
	sprintf(text,">SD,%03d,%03d,%03d,%03d\r",Amplify,TriggerLevel,BlindPeriod,BlindLevel);
    SelUART1SendTxBuffer((unsigned char *)text);
}

void DecodeMsg(unsigned char *msg){
 int v1,v2,v3,v4,i;
 switch(msg[0]) {
	  case '#':
	  	switch(msg[1]){
				case 'V':			   
					 v1=IsNumber(&msg[2],3);
					 if(v1<0) v1=0;
					 VibsistNumber=v1;
				break;
				case 'S':  // SET cmd
					 switch(msg[2]) {
					 case 'H': // Hammer: Tstart, Tstop, 1Stroke
						if(msg[3]==',') if(msg[7]==',') if(msg[11]==',') {
							 v1=IsNumber(&msg[4],3);
							 v2=IsNumber(&msg[8],3);
							 v3=IsNumber(&msg[12],3);
							 if(v1>0) if(v2>0) if(v3>0)	{
								Tstart=v1;
								Tstop=v2;
								F1Strike=v3;
								}
							} // if msg[3]==','
					 break;
					 case 'D': // Detect: Amplify  0...18, TriggerLevel  2...100, BlindPeriod 10...60
						if(msg[3]==',') if(msg[7]==',') if(msg[11]==',') if(msg[15]==',') {
							 v1=IsNumber(&msg[4],3);
							 v2=IsNumber(&msg[8],3);
							 v3=IsNumber(&msg[12],3);
							 v4=IsNumber(&msg[16],3);
							 if(v1>=0) if (v1<=18) Amplify=v1;
							 if(v2>=2) if(v2<=100) TriggerLevel=v2;
							 if(v3>=5) if(v3<=98) BlindPeriod=v3;
							 if(v4>=1) if(v4<=50) BlindLevel=v4;
							 }; // if msg[3]==','
					 break;
					 case 'W':  // Sweep: Tsweep, Tdelay ON, TDelay Off
						if(msg[3]==',') if(msg[7]==',') if(msg[11]==',')
							 {
							 v1=IsNumber(&msg[4],3);
							 v2=IsNumber(&msg[8],3);
							 v3=IsNumber(&msg[12],3);
							 if(v1>0) if(v2>0) if(v3>0)	{
									TSweep=v1;
									TDelayOn=v2;
									TDelayOff=v3;
									}

							 }; // if msg[3]==','
					 break;
					 case 'K': // Sweep: Kp, Ki, Kd
						if(msg[3]==',') if(msg[7]==',') if(msg[11]==',') {
							 v1=IsNumber(&msg[4],3);
							 v2=IsNumber(&msg[8],3);
							 v3=IsNumber(&msg[12],3);
							 if(v1>=0) if(v2>=0) if(v3>=0){
								KProportional=v1;
								KIntegral=v2;
								KDiferential=v3;
								}

							 }; // if msg[3]==','
					 break;

				
					 };// end switch(msg[2])
				break;
				case 'G':   // message Get
					 switch(msg[2]) {
						 case 'H': SendHBuff();	break;
						 case 'W': SendWBuff();  break;
						 case 'K': SendKBuff();  break;

						 case 'C': // get last command
							// answer according to:
								if(LastCommand & LC_H_MODIFIED) SendHBuff(), LastCommand ^= LC_H_MODIFIED;
								else if(LastCommand & LC_W_MODIFIED) SendWBuff(), LastCommand ^= LC_W_MODIFIED;
										else if(LastCommand & LC_K_MODIFIED) SendKBuff(), LastCommand ^= LC_K_MODIFIED;
											else if(LastCommand & LC_D_MODIFIED) SendDBuff(), LastCommand ^= LC_D_MODIFIED;
										else
										{
												if(LastCommand & LC_SET_MODE)   SelUART1SendTxBuffer(">MS\r");
												if(LastCommand & LC_SLIDE_MODE) SelUART1SendTxBuffer(">ML\r");
												if(LastCommand & LC_SWEEP_MODE) SelUART1SendTxBuffer(">MW\r");
												if(LastCommand & LC_RUN_START)  SelUART1SendTxBuffer(">MR\r");
												if(LastCommand & LC_RUN_STOP)   SelUART1SendTxBuffer(">MZ\r");
												if(LastCommand & LC_SLIDE_ACT)  SelUART1SendTxBuffer((unsigned char *)VText);
												LastCommand=LC_NONE;
										};
								 
						 break;
						 };//end switch(msg[2])  -> Get message
				break;
				case 'F': // mesaje "FORCE"
					 switch(msg[2])
						 {
					 case 'R': case 'r':  MainStatusMachine = MSM_START;	break;  // RESET
					 case 'S': case 's':  MainStatusMachine = MSM_GO_SET0;	break;	// GoTo SET Mode
					 case 'L': case 'l':  MainStatusMachine = MSM_GO_SLIDE;	break;	
					 case 'W': case 'w':  MainStatusMachine = MSM_GO_SWEEP;	break;	
					 case 'U': case 'u':  MainStatusMachine = MSM_GO_RUN;	break;	
					 case 'Z': case 'z':  MainStatusMachine = MSM_STOP_SWEEP;	break;	

					 };
					 
				break;
			case 'W': case 'w': // RUN time msg
					// corectat: copiere limitata + terminator garantat
					// (in original, un mesaj cu >20 caractere ne-nule lasa
					// WMessage fara '\0' si DisplaySString citea memorie veche)
					i=0;
					while(i<23 && msg[i])
					 {
					 WMessage[i]=msg[i];
					 i++;
					 };
					WMessage[i]=0;
						HaveWMessage=1;
							break;  
				default:   

								;
            };// endswitch(msg[1])	  
	  break;
	  default: // alt caracter - eroare 


		  ;

	  };//endswitch(msg[0])

 }


// BUG corectat: in original, `if(v<vmax) return 1;` facea ca o valoare
// VALIDA (in interval) sa "returneze eroare", iar una invalida (peste vmax)
// sa returneze 0. Acum: -1 = sub minim, 1 = peste maxim, 0 = in interval.
int NotInArea(int v, int vmin, int vmax)
{
   if(v<vmin) return -1;
   if(v>vmax) return 1;
   return 0;
}

int CheckValues()
{
   int ok=0;

   if(NotInArea(Tstart,MIN_T_VALUE,MAX_T_VALUE))               ok += TmaxNotOk;
   if(NotInArea(Tstop,MIN_T_VALUE,MAX_T_VALUE))               ok += TminNotOk;
   if(NotInArea(F1Strike,MIN_STRIKE_VALUE,MAX_STRIKE_VALUE)) ok += F1StrikeNotOk;
     

//#define MAX_SWEEP_VALUE			  120
//#define MIN_SWEEP_VALUE			   10
//#define DEFAULT_DLYON_VALUE			2
//#define DEFAULT_DLYOFF_VALUE		4

	 

   return ok;
}

