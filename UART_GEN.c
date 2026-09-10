//UART_GEN.c
#include "sel_uart.h"
#include "uart.h"

#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_sysctl.h"
#include "inc/hw_types.h"
#include "inc/hw_uart.h"
#include "inc/hw_uart.h"
#include "driverlib/debug.h"
#include "driverlib/interrupt.h"
#include "driverlib/gpio.h"
#include "driverlib/uart.h"

// UART1 VARIABLES
unsigned char UART1RxBuffer[UART_MAX_RX_BUFFER_SIZE];
unsigned char UART1RxMessage[UART_MAX_RX_BUFFER_SIZE];
unsigned char UART1TxBuffer[UART_MAX_TX_BUFFER_SIZE];
unsigned char UART1TxMessage[UART_MAX_TX_BUFFER_SIZE];

unsigned char UART1RxData = 0;
unsigned long UART1FLAG_Recieved_Message = FLAG_FALSE;
unsigned long UART1IndexRx = 0;
unsigned long UART1FLAG_TxEmpty = FLAG_TRUE;
unsigned long UART1IndexTx = 0;
unsigned long UART1TxSize = 0;

#define u32 			unsigned long
#define u8				unsigned char

#define LOW 0
#define HIGH 1
#define TRUE 1
#define FALSE 0

#define U1_TX_BUFFER_SIZE 2048

/////////////// UART1 FIFO MODE VARS ///////////////////
u32 U1_TX_POINTER_R = 0;

// FUNCTIONS
//
// UART1 Interrupt handler for FIFO Mode
//
void UART1FIFOIntHandler(void)
{
	unsigned long ulUART1IntStatus = 0;
	unsigned int i;
	unsigned char c;
	//unsigned long UART1TxEnablePinStatus = 0;
    ulUART1IntStatus = UARTIntStatus(UART1_BASE, true);
	UARTIntClear(UART1_BASE, ulUART1IntStatus); // Clear the asserted interrupts.
	// Rx and Recieve Timeout Interrupt
	if ((ulUART1IntStatus & UART_INT_RX)|| (ulUART1IntStatus & UART_INT_RT))// RX set
	    {
		while(!(HWREG(UART1_BASE + UART_O_FR) & UART_FR_RXFE))
    	    {	
			UART1RxData = HWREG(UART1_BASE + UART_O_DR);	
	    	// Read the recieved character
			switch(UART1RxData)
	        	{
	        	case UART_MESSAGE_END_CHARACTER:  /// copiem mesajul
	            	if(UART1IndexRx)
					   {
					   for(i=0;i<UART1IndexRx;i++) UART1RxMessage[i]=UART1RxBuffer[i];
					   UART1RxMessage[i]=0;
					   UART1FLAG_Recieved_Message = FLAG_TRUE; 
					   UART1IndexRx = 0;
					   }
	        	break;
	        	case UART_RX_START_CHARACTER:
					// Reset buffer index
					UART1IndexRx = 0;
	        	default:
					UART1RxBuffer[UART1IndexRx] = UART1RxData;
	            	UART1IndexRx ++;
					UART1RxBuffer[UART1IndexRx] = 0;
	        		if (UART1IndexRx >= UART_MAX_RX_BUFFER_SIZE)  // clear buffer
	            		UART1IndexRx = 0;
						
				}//end switch (UART1RxData)
			} // end while read from FIFO
       	 }// end if ((ulUART1IntStatus & UART_INT_RX)|| (ulUART1IntStatus & UART_INT_RT))// RX set
	// Tx Interrupt 
	if (ulUART1IntStatus & UART_INT_TX) // TX interrupt
		{
		// try to send more chars to the U1_TX_FIFO if possible
			c = UART1TxBuffer[U1_TX_POINTER_R ++];
			if(c)
			   {
			   HWREG(UART1_BASE + UART_O_DR) = c;	 // send chars until TX_FIFO is full
			   }
			   else
			   {
			   SelUART1TransmitDisable();
			   UART1FLAG_TxEmpty=1;
			   }
		};// endif (ulUART1IntStatus & UART_INT_TX) // TX interrupt	
}

void SelUART1TransmitEnable(void)
{
	int i = 1000;
    HWREG(UART1_GPIO_DIRECTION_PINS_BASE + (GPIO_O_DATA + (UART1_GPIO_PIN_TX_ENABLE << 2))) = UART1_GPIO_PIN_TX_ENABLE;   // pin in 1
	while(i--);
}

void SelUART1TransmitDisable(void)
	{
    HWREG(UART1_GPIO_DIRECTION_PINS_BASE + (GPIO_O_DATA + (UART1_GPIO_PIN_TX_ENABLE << 2))) = 0;   // pin in 1
	}

//
// UART1 Initialisation function
//
void SelUART1Init(void)
	{
	// enable UART1 peripheral and corespondig GPIO peripheral
	SysCtlPeripheralEnable(SYSCTL_PERIPH_UART1);
	SysCtlPeripheralEnable(UART1_SYSCTL_GPIO_PERIPH);
	SysCtlPeripheralEnable(UART1_SYSCTL_GPIO_DIRECTION_PINS_PERIPH);
	// Set GPIO RX and TX pins as UART1 pins and direction pins as outputs
	GPIOPinTypeUART(UART1_GPIO_PINS_BASE, UART1_GPIO_RX_TX_PINS);
	GPIOPinTypeGPIOOutput(UART1_GPIO_DIRECTION_PINS_BASE, UART1_GPIO_PIN_TX_ENABLE);
	// Initial write ot data direction pins
	GPIOPinWrite(UART1_GPIO_DIRECTION_PINS_BASE, UART1_GPIO_PIN_TX_ENABLE, 0);
    // Configure the UART1 baudrate and other parameters
    UARTConfigSetExpClk(UART1_BASE, SysCtlClockGet(), UART1_BAUDRATE, (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));
	UARTFIFODisable(UART1_BASE);
	// Enable processor interrupts if they are not already enabled
    IntMasterEnable();
    // Enable the UART1 interrupt.
    IntEnable(INT_UART1);
	UARTIntEnable(UART1_BASE, UART_INT_RX | UART_INT_RT | UART_INT_TX);
	}


void SelUART1SendTxBuffer(unsigned char *text)
	{
	unsigned int i;
	unsigned char c;
    if(text[0] == 0) return;
	while(UART1FLAG_TxEmpty == 0); // astept terminarea transmisiei anterioare
	for(i=0;i< UART_MAX_TX_BUFFER_SIZE- 4;i++)
	   {
	   c=text[i] ;
	   if(c == 0) break;
	   UART1TxBuffer[i] =c;
	   };
	UART1TxBuffer[i++] =0Xff;
	UART1TxBuffer[i] =0;
	// Disable UART1 Tx interrupt
	UARTIntDisable(UART1_BASE, UART_INT_TX);
	SelUART1TransmitEnable();

	UART1FLAG_TxEmpty=0;
	HWREG(UART1_BASE + UART_O_DR) = UART1TxBuffer[0];	 // send chars until TX_FIFO is full
	U1_TX_POINTER_R = 1;

	UARTIntEnable(UART1_BASE, UART_INT_TX);
	}

