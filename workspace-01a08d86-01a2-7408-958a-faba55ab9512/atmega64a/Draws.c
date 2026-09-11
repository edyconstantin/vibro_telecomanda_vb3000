 #include "Draws.h"
 #include "KBD.h"
 #include "uart0.h"
 #include "globals.h"
 #include "TimerProg.h"
 #include "Message.h"
 #include "fmt.h"

//-------------------------------------------------------------------------------------------------------
// draw function for S_Setting state
/*
  */



int kbtime;
int LastV2=0;
int InDecrement(int *Val,int Vmin,int Vmax)
   {
   int vmod=0;
   int v= *Val;
 		if(LastKey==KEY_KBPLUS)  v++,kbtime=0,vmod=1;
		if(KeyPlusTime<-50)  
		   {
		   vmod=1;
		   kbtime++;
		   if(KeyPlusTime<-200) kbtime++;
		   if(KeyPlusTime<-300) kbtime+=3;
		   if(KeyPlusTime<-500) kbtime+=5;
		   if(kbtime>=50) v++,kbtime=0;
		   };
		if(v>Vmax) v=Vmax;
   	    if(LastKey==KEY_KBMINUS) v--,kbtime=0,vmod=1;
		if(KeyMinusTime<-50)  
		   {
		   vmod=1;
		   kbtime++;
		   if(KeyMinusTime<-200) kbtime++;
		   if(KeyMinusTime<-300) kbtime+=3;
		   if(KeyMinusTime<-500) kbtime+=5;
		   if(kbtime>=50) v--,kbtime=0;
		   };
		if(v<Vmin) v=Vmin;
	*Val = v;
   return vmod;
   } 		
int InDecPlus(int *Val)
   {
   int vmod=0;
   int v= *Val;
 		if(LastKey==KEY_KBPLUS)  v++,kbtime=0,vmod=1;
		if(KeyPlusTime<-1)  
		   {
		   vmod=1;
		   kbtime++;
		   };
		if(v>10000) v=10000;
   	    if(LastKey==KEY_KBMINUS) v--,kbtime=0,vmod=-11;
		if(KeyMinusTime<-1)  
		   {
		   vmod=-1;
		   kbtime++;
		   };
		if(v<-10000) v=-10000;
	*Val = v;
   return vmod;
   } 		
 void Chenar(void)
{
			PutHorizontalLine(0,127,0);
		    PutHorizontalLine(0,127,55);
		    PutHorizontalLine(0,127,63);
		    PutVerticalLine(0,0,63);
		    PutVerticalLine(127,0,63);
}	
void ChSweep(void)
{
			PutHorizontalLine(40,88,26);
			PutHorizontalLine(40,88,52);
		    PutVerticalLine(40,26,52);
		    PutVerticalLine(88,26,52);
			if(Tstart>Tstop)
		      PutLine(40,26,88,52);
			    else if(Tstop>Tstart) PutLine(40,52,88,26);
				  else PutLine(42,39,86,39);
 }

void DrawScreen_TestKBD(void )
{
	char text[64];
	char KText[64];

	 ClearPage(0);

	 KText[0]='K';KText[1]='=';KText[2]=' ';fmt4(KText+3,LastKey);
	 DisplaySString ((unsigned char*)KText,3,3,0);
	 
	 SprintTimp(text,&Tp);
     DisplaySString((unsigned char*)text,30,7,0);
	 RefreshPage();


}
int CountMsg2=0;
int IndexGrafic=0;
int TabelGrafic[128];
//int ksv=0; 

void DrawScreen(void )
{
	char text[64];
	//char KText[64];
    int v1,v2,i;
    switch(MainStatusMachine)
	   {

	   case MSM_START:
	   break;
	   case MSM_REST:
	   break;
	   
	   case MSM_GET_PAR:
	       ClearPage(0);
	       DisplaySStringP("Get parameters",3,1,0);
	   break;
   /*************************************************************/
   /*************************************************************/
   /*************************************************************/
   /*************************************************************/

	   case MSM_SET0:
	       ClearPage(0);
		   DisplaySStringP("Sweep parameters:", 15, 0,0);

		    DisplaySStringP("DTStart", 0, 1, selected_Parameter == SELECTED_TStart ? 1 : 0);
		    DisplaySStringP("1Strk", 64-15 , 1, selected_Parameter == SELECTED_T1st ? 1 : 0);
		    DisplaySStringP("DTStop",128-38, 1, selected_Parameter == SELECTED_TStop ? 1 : 0);
		    DisplayS3Int(Tstart,12, 2, selected_Parameter == SELECTED_TStart ? 1 : 0);
		    DisplayS3Int(F1Strike,64-10,2, selected_Parameter == SELECTED_T1st ? 1 : 0);
		    DisplayS3Int(Tstop,128-26,2, selected_Parameter == SELECTED_TStop ? 1 : 0);
     		Chenar();
			ChSweep();

			
			switch(selected_Parameter)
			   { 
			   case SELECTED_TStart:  if(InDecrement(&Tstart  ,MIN_T_VALUE,MAX_T_VALUE))           LastCmd |= LC_H_MODIFIED;  break;
			   case SELECTED_TStop:	  if(InDecrement(&Tstop   ,MIN_T_VALUE,MAX_T_VALUE))           LastCmd |= LC_H_MODIFIED; break;
			   case SELECTED_T1st:	  if(InDecrement(&F1Strike,MIN_STRIKE_VALUE,MAX_STRIKE_VALUE)) LastCmd |= LC_H_MODIFIED;   break;
			   };
			switch(LastKey)
			  {
			  case KEY_KBSELECT: selected_Parameter++; if(selected_Parameter > SELECTED_TStop) selected_Parameter= SELECTED_TStart;   break;
			  case KEY_KBNEXT:   selected_Parameter=SELECTED_OnDelay; MainStatusMachine=MSM_SET1;  break;
			  case KEY_KBSLIDE:  LastCommand = LC_SLIDE_MODE  | LastCmd, LastCmd = LC_NONE;     break;
			  case KEY_KBSWEEP:  LastCommand = LC_SWEEP_MODE  | LastCmd, LastCmd = LC_NONE;     break;
              };
			
			LastKey=KeyNone;  
	        DisplaySStringP("Next",102,7,1);
	   break;
   /*************************************************************/
   /*************************************************************/
   /*************************************************************/
   /*************************************************************/


	   case MSM_SET1:			 
	       ClearPage(0);
		   DisplaySStringP("Sweep duration:", 15, 0,0);

		    DisplaySStringP("DlyON", 0, 1, selected_Parameter == SELECTED_OnDelay ? 1 : 0);
		    DisplaySStringP("Duration", 64-24, 1, selected_Parameter == SELECTED_SWEEP ? 1 : 0);
		    DisplaySStringP("DlyOff", 128-38, 1, selected_Parameter == SELECTED_OffDelay ? 1 : 0);
		    DisplayS3Int(TDelayOn,12, 2, selected_Parameter == SELECTED_OnDelay ? 1 : 0);
		    DisplayS3Int(TSweep,64-24,2, selected_Parameter == SELECTED_SWEEP ? 1 : 0);
		    DisplayS3Int(TDelayOff,128-38,2, selected_Parameter == SELECTED_OffDelay ? 1 : 0);
			
  			Chenar();
			ChSweep();

			switch(selected_Parameter)
			   {								  
			   case SELECTED_OnDelay: if(InDecrement(&TDelayOn,0,MIN_DELAY_VALUE))  LastCmd |= LC_W_MODIFIED;   break;
			   case SELECTED_SWEEP:	  if(InDecrement(&TSweep,MIN_SWEEP_VALUE,MAX_SWEEP_VALUE))LastCmd |= LC_W_MODIFIED;   break;
			   case SELECTED_OffDelay:if(InDecrement(&TDelayOff,0,MIN_DELAY_VALUE))LastCmd |= LC_W_MODIFIED;   break;
			   };
			switch(LastKey)
			  {
			  case KEY_KBSELECT:  selected_Parameter++;  if(selected_Parameter > SELECTED_OffDelay) selected_Parameter= SELECTED_OnDelay;  break;
			  case KEY_KBPREV:    selected_Parameter=SELECTED_TStart;  MainStatusMachine=MSM_SET0;	  break;
			  case KEY_KBNEXT:    selected_Parameter=SELECTED_Kr;  MainStatusMachine=MSM_SET2;	  break;
			  //case KEY_KBSLIDE:   MainStatusMachine=MSM_GO_SLIDE;     break;
			  //case KEY_KBSWEEP:   MainStatusMachine=MSM_GO_SWEEP;    break;
			  case KEY_KBSLIDE:  LastCommand = LC_SLIDE_MODE  | LastCmd, LastCmd = LC_NONE;     break;
			  case KEY_KBSWEEP:  LastCommand = LC_SWEEP_MODE  | LastCmd, LastCmd = LC_NONE;     break;
			  };
			LastKey=KeyNone;  

	       DisplaySStringP("Prev",1,7,1);
	       DisplaySStringP("Next",102,7,1);

	   break;
	   case MSM_SET2:
	       ClearPage(0);
		   DisplaySStringP("Feedback:", 15, 0,0);

		    DisplaySStringP("Kr", 0, 1, selected_Parameter == SELECTED_Kr ? 1 : 0);
		    DisplaySStringP("Ki", 64-6, 1, selected_Parameter == SELECTED_Ki ? 1 : 0);
		    DisplaySStringP("Kd", 128-16, 1, selected_Parameter == SELECTED_Kd ? 1 : 0);
		    DisplayS3Int(KProportional,12, 2, selected_Parameter == SELECTED_Kr ? 1 : 0);
		    DisplayS3Int(KIntegral,64-24,2, selected_Parameter == SELECTED_Ki ? 1 : 0);
		    DisplayS3Int(KDiferential,128-38,2, selected_Parameter == SELECTED_Kd ? 1 : 0);
			
  			Chenar();
			ChSweep();

			switch(selected_Parameter)
			   {								  
			   case SELECTED_Kr: if(InDecrement(&KProportional,0,MAX_K_VALUE))  LastCmd |= LC_K_MODIFIED;   break;
			   case SELECTED_Ki: if(InDecrement(&KIntegral    ,0,MAX_K_VALUE))  LastCmd |= LC_K_MODIFIED;   break;
			   case SELECTED_Kd: if(InDecrement(&KDiferential ,0,MAX_K_VALUE))  LastCmd |= LC_K_MODIFIED;   break;
			   };
			switch(LastKey)
			  {
			  case KEY_KBSELECT:  selected_Parameter++;  if(selected_Parameter > SELECTED_Kd) selected_Parameter= SELECTED_Kr;  break;
			  case KEY_KBPREV:    selected_Parameter=SELECTED_OnDelay;  MainStatusMachine=MSM_SET1;	  break;
			  case KEY_KBNEXT:    selected_Parameter=SELECTED_Amplify;  MainStatusMachine=MSM_SET3;	  break;
			  case KEY_KBSLIDE:  LastCommand = LC_SLIDE_MODE  | LastCmd, LastCmd = LC_NONE;     break;
			  case KEY_KBSWEEP:  LastCommand = LC_SWEEP_MODE  | LastCmd, LastCmd = LC_NONE;     break;
			  };
			LastKey=KeyNone;  

	       DisplaySStringP("Prev",1,7,1);
	       DisplaySStringP("Next",102,7,1);


	   break;
	   case MSM_SET3:			  // Display & set   Amplify, TriggerLevel, BlindPeriod
	       ClearPage(0);
 		   Chenar();
		   DisplaySStringP("Trigger detect:", 10, 0,0);

		   PutHorizontalLine(5,24,50);
		   PutHorizontalLine(28,65,12);
		   PutLine(24,50,28,12);
		   PutHorizontalLine(26,62,45);


		   DisplaySStringP("Amp", 2, 1, selected_Parameter == SELECTED_Amplify ? 1 : 0);
		   DisplayS3Int(Amplify, 2, 2, selected_Parameter == SELECTED_Amplify ? 1 : 0);
		   DisplaySStringP("Trg", 2, 4, selected_Parameter == SELECTED_Level ? 1 : 0);
		   DisplayS3Int(TriggerLevel, 2, 5, selected_Parameter == SELECTED_Level ? 1 : 0);
		   DisplaySStringP("Blind", 88, 1, selected_Parameter == SELECTED_BlindPeriod ? 1 : 0);
		   DisplayS3Int(BlindPeriod, 88, 2, selected_Parameter == SELECTED_BlindPeriod ? 1 : 0);
		   DisplaySStringP("Clr", 64, 4, selected_Parameter == SELECTED_BlindLevel ? 1 : 0);
		   DisplayS3Int(BlindLevel, 64, 5, selected_Parameter == SELECTED_BlindLevel ? 1 : 0);


		   //Rectangle(8,18,16,28);
		   //SELECTED_Amplify,
           //SELECTED_Level,
           //SELECTED_BlindPeriod,
           //SELECTED_BlindLevel

		   switch(selected_Parameter)
			   {								  
			   case SELECTED_Amplify:      if(InDecrement(&Amplify,0,18))        LastCmd |= LC_D_MODIFIED;   break;
			   case SELECTED_Level:       if(InDecrement(&TriggerLevel,2,100))  LastCmd |= LC_D_MODIFIED;   break;
			   case SELECTED_BlindPeriod: if(InDecrement(&BlindPeriod,10,60))   LastCmd |= LC_D_MODIFIED;   break;
			   case SELECTED_BlindLevel:  if(InDecrement(&BlindLevel,1,50))     LastCmd |= LC_D_MODIFIED;   break;
			   };



		   switch(LastKey)
			  {
			  case KEY_KBSELECT:  selected_Parameter++;  if(selected_Parameter > SELECTED_BlindLevel) selected_Parameter= SELECTED_Amplify;  break;
			  case KEY_KBPREV:    selected_Parameter=SELECTED_Kr;  MainStatusMachine=MSM_SET2;	  break;
			  //case KEY_KBNEXT:    selected_Parameter=SELECTED_Amplify;  MainStatusMachine=MSM_SET3;	  break;
			  case KEY_KBSLIDE:  LastCommand = LC_SLIDE_MODE  | LastCmd, LastCmd = LC_NONE;     break;
			  case KEY_KBSWEEP:  LastCommand = LC_SWEEP_MODE  | LastCmd, LastCmd = LC_NONE;     break;
			  };
			LastKey=KeyNone;  

	       DisplaySStringP("Prev",1,7,1);
	       //DisplaySStringP("Next",102,7,1);

	   break;
	   case MSM_SET4:
	       // (in original, continutul era tot comentat - cod mort, sters)
	   break;
	   case MSM_SLIDE:
	       ClearPage(0);
 
 		   DisplaySStringP("Slide mode - manual:", 5, 0,0);
		   DisplaySStringP("SV1",7, 1,0);
		   DisplaySStringP("SV2",55,1,0);
		   DisplaySStringP("SV3",55,6,0);
		   DisplaySStringP("SV4",103,1,0);

		   Chenar();
		   PutHorizontalLine(1,127,8);
		   Rectangle(8,18,16,28);
		   Rectangle(56,18,16,28);
		   Rectangle(104,18,16,28);
		   Rectangle(57,19,14,26);
		
       {VText[0]='>';VText[1]='V';VText[2]='V';VText[3]='V';VText[4]=0;VText[5]=0;VText[6]=0;VText[7]=0;VText[8]='\r';VText[9]=0;}
       if(SV1) VText[4]='1',FillRect(11,3,11); else FillRect(11,4,11);
       if(SV2) VText[5]='1',FillRect(59,3,11); else FillRect(59,4,11);
       // TODO: blocul SV3 pare copy-paste gresit (fara else, acopera
       // aceeasi locatie ca SV2) - de verificat in hardware
       if(SV3) VText[6]='1',FillRect(59,4,11); 
		   if(SV4) VText[7]='1',FillRect(107,3,11); else FillRect(107,4,11);
		     //LastCommand |= LC_SLIDE_ACT;
		     //SelUART1SendTxBuffer((unsigned char *)VText);
			 SV1=0; SV2=0; SV3=0; SV4=0; 
			 if(InDecPlus(&v1)>0) SV2=1; LastCommand = LC_SLIDE_ACT; 
			 if(InDecPlus(&v1)<0) SV1=1; LastCommand = LC_SLIDE_ACT;  
			 v1=0;
			 switch(LastKey)
			  {
			  case KEY_KBSET:    LastCommand = LC_SET_MODE ; break;
			  case KEY_KBSWEEP:  LastCommand = LC_SWEEP_MODE ; break;
			  };
			LastKey=KeyNone;  
	   break;
	   case MSM_SWEEP:
 	       ClearPage(0);
		   DisplaySStringP("Sweep mode:", 15, 0,0);
		   Chenar();
		   if(KeyPlusTime<-5) 
			   {
	       SelUART1SendTxBufferP(">MACC\r");
			   LastCommand |= LC_ACCUMULATOR_LOAD;
			   }
		   if(WMessage[2]) CountMsg2=5; else  if(CountMsg2) CountMsg2--;
		   
		   switch(WMessage[2])
		        {
				case '3': case '1':
 			      DisplaySStringP("Discharging", 5, 4,0);
			      DisplaySString(&WMessage[4],0,7,1);
			      WMessage[2]=0;
				break;
				default:
		           ; //DisplaySStringP("Press ^ to discharge",3, 5,0);
			    }

		    DisplaySStringP("Waiting for START",10, 3,0);

			switch(LastKey)
			  {
			  //case KEY_KBSET:     MainStatusMachine=MSM_GO_SET0;    break;
			  //case KEY_KBSLIDE:   MainStatusMachine=MSM_GO_SLIDE;  break;
			  //case KEY_KBSWEEP:   MainStatusMachine=MSM_GO_SWEEP;     break;

			  case KEY_KBSET:    LastCommand = LC_SET_MODE ; break;
			  case KEY_KBSLIDE:  LastCommand = LC_SLIDE_MODE ; break;
			  


              case KEY_KBRUN:     
				 IndexGrafic=2;
			  	 for(i=0;i<128;i++) TabelGrafic[i]=TmsecMinim;
				 LastV2=0; 
			     //MainStatusMachine=MSM_GO_RUN;  
				 LastCommand = LC_RUN_START; 
				 break; 
			  };
			LastKey=KeyNone;  
	   break;
	   case MSM_RUN:
 	       
			switch(LastKey)  {
			  //case KEY_KBSTOP:     MainStatusMachine=MSM_GO_STOP_SWEEP;    break;
			  case KEY_KBSTOP:   LastCommand = LC_RUN_STOP;    break;
			  };
			  LastKey=KeyNone;  
		   
		   if(WMessage[0] == 0) break;
		   ClearPage(0);
		   Chenar();
		   switch(WMessage[2])  {
			  case '0':	// Delay ON
		          DisplaySStringP("Delay ON", 40, 4,0);
				  DisplaySString(&WMessage[4],0,7,1);
			  break;
			  case '1':	 // Sweep
		       DisplaySStringP("Sweep ON", 40, 0,0);
				  //DisplaySString(&WMessage[4],0,7,1);
				  // W1nnnnxxxx
				   v1=IsNumber(&WMessage[3],4);
				   v2=IsNumber(&WMessage[7],3);
				   if(v1<0) v1=0;
				   if(v1>9999) v1=9999;
				   fmt4(text,v1);
		       DisplaySString((unsigned char *)text,0, 7,1);
				   // afisare grafic
				   PutHorizontalLine(0,127,7);
				   ///sprintf(text,"%03d",LastV2);
					 fmt3(text,v2);
		       DisplaySString((unsigned char *)text,109, 7,1);

				   if(v2)
				     {
				     if(v2<TmsecMinim) v2=TmsecMinim;		// v2 e delta T  - 60... 240 ms
				     if(v2>TmsecMaxim) v2=TmsecMaxim;
				     LastV2=v2;
						 if(IndexGrafic<=125)
								{
								TabelGrafic[IndexGrafic]=v2;
								IndexGrafic++;
								}
							else
								{
								for(i=0;i<125;i++) TabelGrafic[i]=TabelGrafic[i+1];
									TabelGrafic[i]=v2;
								}
				   	 };
				   //*******************************************
				   // Optimizare: aritmetica pe intregi (fara float - pe LM3S2965
				   // nu exista FPU, floatul e soft si lent)
				   for(i=2;i<126;i++)
				      {
							v2= TabelGrafic[i]-TmsecMinim;
							if(v2<0) v2=0;
							v1= 55-(v2*47)/(TmsecMaxim-TmsecMinim);
							if(v1<8) v1=8;
							if(v1>55) v1=55;
							PutVerticalLine(i,v1,55);
							}
				   

				   //*******************************************
			  break;
			  case '2':	 // Delay Off
		          DisplaySStringP("Delay OFF", 40, 4,0);
				  DisplaySString(&WMessage[4],0,7,1);
			  break;
              };
		WMessage[0]=0;	  		
	   break;
	   case MSM_STOP_SWEEP:
 	       ClearPage(0);
		   DisplaySStringP("Stop sweep:", 15, 0,0);
		   Chenar();
			switch(LastKey)
			  {
			  case KEY_KBSET:    LastCommand = LC_SET_MODE ; break;
			  case KEY_KBSLIDE:  LastCommand = LC_SLIDE_MODE ; break;

			  //case KEY_KBSET:     MainStatusMachine=MSM_GO_SET0;    break;
			  //case KEY_KBSLIDE:   MainStatusMachine=MSM_GO_SLIDE;  break;
			  case KEY_KBRUN:     
				 IndexGrafic=2;
			  	 for(i=0;i<128;i++) TabelGrafic[i]=TmsecMinim;
						LastV2=0;
			  	 ///LastCommand = LC_RUN_STOP;
					  LastCommand = LC_RUN_START; 
				break; 
				default:
				   // Optimizare: aritmetica pe intregi (fara float)
				   for(i=2;i<126;i++)
				      {
				        v2= TabelGrafic[i]-TmsecMinim;
						if(v2<0) v2=0;
						v1= 55-(v2*47)/(TmsecMaxim-TmsecMinim);
						if(v1<8) v1=8;
						if(v1>55) v1=55;
						PutVerticalLine(i,v1,55);
						}
			  };
			LastKey=KeyNone;  
	   break;
	   case MSM_ERR_COM:
	       ClearPage(0);
	       DisplaySStringDP("ERR COM",15,1,0);
	       DisplaySStringP("Retrying...",3,5,0);
	   break;
		case MSM_GO_SET0:
		case MSM_GO_SLIDE:
		case MSM_GO_SWEEP:
		case MSM_GO_RUN:
		case MSM_GO_STOP_SWEEP: break;
	   default:   
 	       ClearPage(0);
		   DisplaySStringP("Unknown mode:", 15, 0,0);
		   DisplaySStringP("Press one mode key!",3,4,0);
		   Chenar();
			switch(LastKey)
			  {
			  case KEY_KBSET:     LastCommand = LC_SET_MODE ; break;
			  case KEY_KBSWEEP:   LastCommand = LC_SWEEP_MODE ; break;
			  case KEY_KBSLIDE:   LastCommand = LC_SLIDE_MODE ; break;
			  };
			LastKey=KeyNone;  



	   }
     SprintTimp(text,&Tp);
     DisplaySString((unsigned char*)text,30,7,0);
	 //DisplaySString(WMessage, 0, 6,0);

	 // Optimizare: se retrimite pe afisaj DOAR daca s-a modificat ceva
	 // (inainte: pagina intreaga la fiecare 10 ms, ~12 ms CPU de fiecare data)
	 if (LCD_IsDirty()) RefreshPage();

}





