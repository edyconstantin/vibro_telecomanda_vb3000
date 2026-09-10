
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_gpio.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "driverlib/watchdog.h"

#include "globals.h"
#include "oled.h"
#include "draws.h"
#include "sel_uart.h"
#include "message.h"
#include "timer.h"
#include "TimerProg.h"
#include <stdio.h>
#include "kbd.h"

//#define OLED_CONTRAST 250


//*****************************************************************************
//
// The error routine that is called if the driver library encounters an error.
//
//*****************************************************************************
#ifdef DEBUG
void __error__(char *pcFilename, unsigned long ulLine)
{
}
#endif


void WatchDogIntHandler()
{
	WatchdogIntClear(WATCHDOG0_BASE);
}



void watch_dog_start()
{
	SysCtlPeripheralEnable(SYSCTL_PERIPH_WDOG0);
	WatchdogEnable(WATCHDOG0_BASE);
	IntEnable(INT_WATCHDOG);
	WatchdogStallEnable(WATCHDOG0_BASE);
	WatchdogReloadSet(WATCHDOG0_BASE,SysCtlClockGet()/4);
	WatchdogResetEnable(WATCHDOG0_BASE);
	WatchdogLock(WATCHDOG0_BASE);
}



//*****************************************************************************
//
//
//*****************************************************************************
int main(void)
{
int k;
//int i,j,k=0,t=0;
//unsigned char m=0;
//float a=3;
//float b=7;
//char text[32];
    // ************************************************************
    // Set Core voltage and CLK
	
	SysCtlLDOSet(SYSCTL_LDO_2_75V);
	SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_8MHZ);
	SystemClock = SysCtlClockGet();
    // ************************************************************ 
    // Enable the peripherals 
    //
	
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART1);  // folosit pt comunicatia seriala



    SysCtlPeripheralEnable(KBD_INPORT_PORT1);  // tastatura
	GPIOPinTypeGPIOInput(KBD_INPORT_BASE1,KBD_K1  | KBD_K2  | KBD_K3  | KBD_K4);
    SysCtlPeripheralEnable(KBD_INPORT_PORT2);  // tastatura
	GPIOPinTypeGPIOInput(KBD_INPORT_BASE2,KBD_K5  | KBD_K6  | KBD_K7  | KBD_K8);
    SysCtlPeripheralEnable(KBD_INPORT_PORT3);  // tastatura
	GPIOPinTypeGPIOInput(KBD_INPORT_BASE3,KBD_K9  | KBD_K10);


	SysCtlDelay(5);


    // Set OLED control pins as outputs

    SysCtlPeripheralEnable(LCD_CONTROL_PORT1);  // comenzi la afisor
	GPIOPinTypeGPIOOutput(LCD_CONTROL_BASE1 ,LCD_CS1_PIN | LCD_RESET_PIN | LCD_CS2_PIN );
	HWREG(LCD_CONTROL_BASE1 + (GPIO_O_DATA + (LCD_CS1_PIN << 2))) = LCD_CS1_PIN;   // pin in 1
	HWREG(LCD_CONTROL_BASE1 + (GPIO_O_DATA + (LCD_RESET_PIN << 2))) = LCD_RESET_PIN;   // pin in 1
	HWREG(LCD_CONTROL_BASE1 + (GPIO_O_DATA + (LCD_CS2_PIN << 2))) = LCD_CS2_PIN;   // pin in 1
	


    SysCtlPeripheralEnable(LCD_CONTROL_PORT);  // comenzi la afisor
	GPIOPinTypeGPIOOutput(LCD_CONTROL_BASE, LCD_ENABLE_PIN | LCD_DCMD_PIN | LCD_RDWR_PIN);
	HWREG(LCD_CONTROL_BASE + (GPIO_O_DATA + (LCD_ENABLE_PIN << 2))) = LCD_ENABLE_PIN;   // pin in 1
	HWREG(LCD_CONTROL_BASE + (GPIO_O_DATA + (LCD_RDWR_PIN << 2))) = LCD_RDWR_PIN;   // pin in 1
	HWREG(LCD_CONTROL_BASE + (GPIO_O_DATA + (LCD_DCMD_PIN << 2))) = LCD_DCMD_PIN;   // pin in 1

	


    // Set COM1 control pins as outputs
	//GPIOPinTypeGPIOOutput(COM1_BASE ,COM1_TXENA_PIN);

	  
    // Set OLED data pins as outputs

    SysCtlPeripheralEnable(LCD_DATA_PORT);  // mag de date cu afisorul
	GPIOPinTypeGPIOOutput(LCD_DATA_BASE,0XFF);
	HWREG(LCD_DATA_BASE + (GPIO_O_DATA + (0XFF << 2))) = 0XFF;   // pin in 1
	


    // ************************************************************
    // Enable processor interrupts.
    //
   // IntMasterEnable();

    //	************************************************************
    // Set GPIO A0 and A1 as UART pins.
    //
    //GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    // ************************************************************
    // Configure the UART for 115,200, 8-N-1 operation.
    //
    //UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), 115200, (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));
    //IntEnable(INT_UART0);
    //UARTIntEnable(UART0_BASE, UART_INT_RX | UART_INT_RT);
    //
    // Prompt for text to be entered.
    //
    //UARTSend((unsigned char *)"Enter text: ", 12);
    //
    // Loop forever echoing data through the UART.
    //
		
		
// 	DisplaySendCommand(0xAF);
// 	DisplaySendCommand(0xC8);
// 	//DisplaySendCommand(0x2E);  // scrool
//	
//	DisplaySendCommand(0x81);//set contrast control
//  DisplaySendCommand(OLED_CONTRAST);
DisplayReset();
//HWREG(LCD_CONTROL_BASE + (GPIO_O_DATA + (LCD_RESET_PIN << 2))) = 0;   // pin in 0
//		
	SelTimer1Init();
	SelTimer1IntEnable();
	
	//SelUART1PrmReset();
    SelUART1Init();
		
while(0){  ///test LCD
	for( k=0;k<64;k++){
		PutPixel(k,k);
		PutPixel(100,k);
		SysCtlDelay(5000000); // delay_ms(100);
		RefreshPage();
	}
	ClearPage(0);
}

	//watch_dog_start();
while(1) {
	//WatchdogUnlock(WATCHDOG0_BASE);
	//WatchdogReloadSet(WATCHDOG0_BASE,SysCtlClockGet()/4);
	//WatchdogLock(WATCHDOG0_BASE);
	if(Bit1ms)
	  {
	  Bit1ms=0;
	  };
	if(UART1FLAG_Recieved_Message)
	   {
	   //	UART1RxMessage[i]=0;
	   // Decodificam mesajul
	   DecodeMsg(UART1RxMessage);
	   UART1FLAG_Recieved_Message=0;
	   } 


	if(Bit10ms) { // actiuni la 10 ms
	  // SelUART1SendTxBuffer("....");
	   //ReadKeys();
	   ExecuteKeys();
	   switch(MainStatusMachine) {
		    case MSM_START:  // display SELETRON, wait for answ
	          ClearPage(0);
	          PutImage(siglaSeletron);
						TimeStatusMachine=40; 
						MainStatusMachine=MSM_REST;
				break;
				case MSM_REST:
					if(TimeStatusMachine == 0)
						 { // nu a reusit comunicatia in 0.5 secunde
						 TimeStatusMachine=100; 
						 MainStatusMachine=MSM_GET_PAR;
						 }
				break;
				case MSM_GET_PAR:
					if(VibsistNumber == MY_V_NUMBER)  MainStatusMachine=MSM_GO_SET0;	
					if(TimeStatusMachine == 0)
						 { 
						 TimeStatusMachine=100; // 
						 MainStatusMachine=MSM_ERR_COM;
						 }
				break;
				case MSM_ERR_COM: // eroare de comunicatie - reincercam dupa 1 secunda
					if(TimeStatusMachine == 0)
						 {
						 SelUART1SendTxBuffer(">START\r");  // 
						 MainStatusMachine=MSM_START;
						 }
				break;
				case MSM_GO_SET0:   	
					 selected_Parameter=SELECTED_NONE;
					 MainStatusMachine=MSM_SET0;   
				break;
				case MSM_GO_SLIDE:      MainStatusMachine=MSM_SLIDE;  break;
				case MSM_GO_SWEEP:	    MainStatusMachine=MSM_SWEEP;  break;
				case MSM_GO_RUN:        MainStatusMachine=MSM_RUN;    break;
				//case MSM_GO_STOP_SWEEP:	MainStatusMachine=MSM_REST;   break;

				//default: 	MainStatusMachine=MSM_START;  // eroare -> reset
			 };
            //extern int TimeStatusMachine;
	   DrawScreen();
	   Bit10ms=0;
	 };
   
 }
}
