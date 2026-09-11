//#include "sel_uart.h"
// UART1 VARIABLES
unsigned char UART1RxBuffer[UART_MAX_RX_BUFFER_SIZE];
unsigned char UART1RxMessage[UART_MAX_RX_BUFFER_SIZE];
char UART1TxBuffer[UART_MAX_TX_BUFFER_SIZE];
unsigned char UART1RxData = 0;
unsigned long UART1FLAG_Recieved_Message = FLAG_FALSE;
unsigned long UART1IndexRx = 0;
unsigned long UART1FLAG_TxEmpty = FLAG_TRUE;
unsigned long UART1IndexTx = 0;
unsigned long UART1TxSize = 0;
unsigned long UART1FLAG_TxEscape = FLAG_FALSE;
unsigned long UART1FLAG_RxEscape = FLAG_FALSE;
unsigned long UART1MessageType = UART_MESSAGE_TYPE_ASCII;

#define u32 			unsigned long
#define u8				unsigned char

#define LOW 0
#define HIGH 1
#define TRUE 1
#define FALSE 0

#define U1_TX_BUFFER_SIZE 2048

/////////////// UART1 FIFO MODE VARS ///////////////////
u32 U1_TX_POINTER_R = 0;
u32 U1_TX_LEN = 0;
// FUNCTIONS
//
// UART1 Interrupt handler for FIFO Mode
//
void UART1FIFOIntHandler(void)
{
	unsigned long ulUART1IntStatus = 0;
	unsigned long SentCharacters = 0;
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
	        	case UART_ASCII_MESSAGE_START_CHARACTER:
					// Reset buffer index and escape flag, read character and change message type to ASCII
					UART1RxBuffer[0] = UART1RxData;
					UART1IndexRx = 1;
					UART1MessageType = UART_MESSAGE_TYPE_ASCII;
					UART1FLAG_RxEscape = FLAG_FALSE;
	        	break;
	
				case UART_BINARY_MESSAGE_START_CHARACTER:
					// Reset buffer index and escape flag, read character and change message type to BINARY
					UART1RxBuffer[0] = UART1RxData;
					UART1IndexRx = 1;
					UART1MessageType = UART_MESSAGE_TYPE_BINARY;
					UART1FLAG_RxEscape = FLAG_FALSE;
				break;
	
				case UART_MESSAGE_ESCAPE_CHARACTER:
					UART1FLAG_RxEscape = FLAG_TRUE;
				break;
	
	        	case UART_MESSAGE_END_CHARACTER:
					// Check if any characters have already been recieved
					// If yes treat normally, else do nothing, probably junk
	        		if (UART1IndexRx)
	            		{
	            		UART1RxBuffer[UART1IndexRx] = UART1RxData;
	            		//SelUART1ClearRxMessage();
	            		SelUART1CopyRxBuffer2RxMessage(UART1IndexRx);
	            		UART1IndexRx = 0;
						UART1FLAG_RxEscape = FLAG_FALSE;
						UART1FLAG_Recieved_Message = FLAG_TRUE;
	            		}
	        	break;
	
	        	default:
					// Check if any characters have already been recieved
					// If yes treat normally, else do nothing, probably junk
	        		if (UART1IndexRx)
	            	    {
					    // Check if an RX Escape sequence is active
					    if (UART1FLAG_RxEscape == FLAG_TRUE)
						   {
						   UART1RxBuffer[UART1IndexRx] = UART1RxData - 1;
						   UART1FLAG_RxEscape = FLAG_FALSE;
						   }
					     else
						   {
						   // Normal sequence, read current character
						   UART1RxBuffer[UART1IndexRx] = UART1RxData;
						   }
						 }
	        		if (UART1IndexRx < UART_MAX_RX_BUFFER_SIZE - 1)
	            		{
	            		UART1IndexRx ++;
	            		}
				}//end switch (UART1RxData)
			} // end while read from FIFO
       	 }// end if ((ulUART1IntStatus & UART_INT_RX)|| (ulUART1IntStatus & UART_INT_RT))// RX set
	// Tx Interrupt 
	if (ulUART1IntStatus & UART_INT_TX) // TX interrupt
		{
		if(U1_TX_LEN == 0)
			{
			SelUART1TransmitDisable();
			// Disable UART1 Tx interrupt, it is no longer needed
			UARTIntDisable(UART1_BASE, UART_INT_TX);
			}
		// try to send more chars to the U1_TX_FIFO if possible
		while(	(!(HWREG(UART1_BASE + UART_O_FR) & UART_FR_TXFF))   &&   (U1_TX_LEN != 0) && (SentCharacters <= 11))
			{
			c = UART1TxBuffer[U1_TX_POINTER_R ++];
			switch(c)
				{
				case UART_ASCII_MESSAGE_START_CHARACTER:	 //'@'		// 0x40
				// no break
				case UART_BINARY_MESSAGE_START_CHARACTER:	 //'#'		// 0x23
				// no break
				case UART_MESSAGE_ESCAPE_CHARACTER:			 //'^'		// 0x5E
				// no break
				   HWREG(UART1_BASE + UART_O_DR) = UART_MESSAGE_ESCAPE_CHARACTER;
				   SentCharacters++;
				   c+=1;
				break;

				case UART_MESSAGE_END_CHARACTER:			 //'\r'	// 0x0D
				   if(U1_TX_LEN > 1)
					{
					HWREG(UART1_BASE + UART_O_DR) = UART_MESSAGE_ESCAPE_CHARACTER;
					SentCharacters++;
					c+=1;
					}
				// no break
				}; // end switch (c)
			// A special character needing escape must be sent an escape character is sent
			// and a flag is set.
			U1_TX_LEN--;
			HWREG(UART1_BASE + UART_O_DR) = c;	 // send chars until TX_FIFO is full
			SentCharacters++;
			}// end while

		if(U1_TX_LEN == 0) // if all chars are sent , clear the TX_FLAG
			{
		//	U1_TX_FLAG = FALSE;
			HWREG(UART1_BASE + UART_O_DR) = 0xFF;
			HWREG(UART1_BASE + UART_O_DR) = 0xFF;
			HWREG(UART1_BASE + UART_O_DR) = 0xFF;
			}
		};// endif (ulUART1IntStatus & UART_INT_TX) // TX interrupt	
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
	GPIOPinTypeGPIOOutput(UART1_GPIO_DIRECTION_PINS_BASE, UART1_GPIO_DIRECTION_PINS);
	// Initial write ot data direction pins
	GPIOPinWrite(UART1_GPIO_DIRECTION_PINS_BASE, UART1_GPIO_DIRECTION_PINS, 0);
    // Configure the UART1 baudrate and other parameters
    UARTConfigSetExpClk(UART1_BASE, SysCtlClockGet(), UART1_BAUDRATE, (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));
	//
	// Enable the UART FIFOs
	UARTFIFOEnable(UART1_BASE);
	// Set the FIFO levels for interrupts
	UARTFIFOLevelSet(UART1_BASE, UART_FIFO_TX1_8, UART_FIFO_RX4_8);
	// Enable processor interrupts if they are not already enabled
    IntMasterEnable();
    // Enable the UART1 interrupt.
    IntEnable(INT_UART1);
	UARTIntEnable(UART1_BASE, UART_INT_RX | UART_INT_RT);
	}

void SelUART1PrmReset(void)
	{
	UART1FLAG_Recieved_Message = FLAG_FALSE;
	UART1IndexRx = 0;
	UART1FLAG_TxEmpty = FLAG_TRUE;
	UART1IndexTx = 0;
	UART1TxSize = 0;
	UART1FLAG_TxEscape = FLAG_FALSE;
	UART1FLAG_RxEscape = FLAG_FALSE;
	UART1MessageType = UART_MESSAGE_TYPE_ASCII;
	}
unsigned long SelUART1SendTxBuffer(unsigned long len)
	{
	unsigned long SentCharacters = 0;
	unsigned char c;
	// Disable UART1 Tx interrupt
	UARTIntDisable(UART1_BASE, UART_INT_TX);
	if(len <= 2) {return 0;}
	U1_TX_POINTER_R = 1;
	SelUART1TransmitEnable();
	U1_TX_LEN = len;
	c = UART1TxBuffer[0];
	U1_TX_LEN--;
	HWREG(UART1_BASE + UART_O_DR) = c;	 // send chars until TX_FIFO is full
	SentCharacters = 1;
	// SHIFT IN THE FIFO AS MANY CHARS AS POSSIBLE FROM THE OLD MSG
	while((!(HWREG(UART1_BASE + UART_O_FR) & UART_FR_TXFF)) && (U1_TX_LEN != 0) && (SentCharacters <= 11))
		{
		c = UART1TxBuffer[U1_TX_POINTER_R++];
		switch(c)
			{
			case UART_ASCII_MESSAGE_START_CHARACTER:	 //'@'		// 0x40
			// no break
			case UART_BINARY_MESSAGE_START_CHARACTER:	 //'#'		// 0x23
			// no break
			case UART_MESSAGE_ESCAPE_CHARACTER:			 //'^'		// 0x5E
			// no break
			HWREG(UART1_BASE + UART_O_DR) = UART_MESSAGE_ESCAPE_CHARACTER;
			SentCharacters++;
			c+=1;
			break;
			case UART_MESSAGE_END_CHARACTER:			 //'\r'	// 0x0D
			if(U1_TX_LEN > 1)
				{
				HWREG(UART1_BASE + UART_O_DR) = UART_MESSAGE_ESCAPE_CHARACTER;
				SentCharacters++;
				c+=1;
				}
			break;
			};
		U1_TX_LEN--;
		HWREG(UART1_BASE + UART_O_DR) = c;	 // send chars until TX_FIFO is full
		SentCharacters++;
		} // end while
	if(U1_TX_LEN == 0) // if all chars are sent , clear the TX_FLAG
		{
		//	U1_TX_FLAG = FALSE;
		HWREG(UART1_BASE + UART_O_DR) = 0xFF;
		HWREG(UART1_BASE + UART_O_DR) = 0xFF;
		HWREG(UART1_BASE + UART_O_DR) = 0xFF;
		}
	// Enable UART1 Tx interrupt
	UARTIntEnable(UART1_BASE, UART_INT_TX);
	return 1;
	}

//
// Function that sets or restets the TXEnable pin
// TxEnableState may be UART1_TX_ENABLE_ACTIVE or UART1_TX_ENABLE_INACTIVE
//
void SelUART1SetTxEnable(unsigned char TxEnableState)
	{
	GPIOPinWrite(UART1_GPIO_DIRECTION_PINS_BASE, UART1_GPIO_PIN_TX_ENABLE, TxEnableState);
	}
// Function that sets or restets the RXEnable pin
// TxEnableState may be UART1_RX_ENABLE_ACTIVE or UART1_RX_ENABLE_INACTIVE
//
void SelUART1SetRxEnable(unsigned char RxEnableState)
	{
	GPIOPinWrite(UART1_GPIO_DIRECTION_PINS_BASE, UART1_GPIO_PIN_RX_ENABLE, RxEnableState);
	}

void SelUART1TransmitEnable(void)
{
	int i = 1000;
	SelUART1SetTxEnable(UART1_TX_ENABLE_ACTIVE);
	SelUART1SetRxEnable(UART1_RX_ENABLE_INACTIVE);
	while(i--);
}

void SelUART1TransmitDisable(void)
	{
	SelUART1SetTxEnable(UART1_TX_ENABLE_INACTIVE);
	SelUART1SetRxEnable(UART1_RX_ENABLE_ACTIVE);
	}
//
// Function that puts a character in the UART transmit data register
// without making any checks.
// WARNING:
// This function should only be called if you are sure by means of software that
// the transmit data regsiter is empty, otherwise the result is unpredictable
//
__inline void SelUARTPutChar(unsigned long ulBase, unsigned char ucData)
	{
    HWREG(ulBase + UART_O_DR) = ucData;
	}
//
// Function that gets a character from the UART recieve data regsiter
// without making any checks.
// WARNING:
// This function should only be called if you are sure by means of software that
// the recieve data register has data, otherwise the result is unpredictable
//
__inline long SelUARTGetChar(unsigned long ulBase)
	{
    return(HWREG(ulBase + UART_O_DR));
	}

// Function that clears the UART1 RxMessage

void SelUART1ClearRxMessage(void)
    {
    unsigned long i;
    for (i = 0; i < UART_MAX_RX_BUFFER_SIZE; i++)   UART1RxMessage[i] = 0;
    };

// Function that copies the contents of the UART Rx Buffer to the Rx Message
void SelUART1CopyRxBuffer2RxMessage(unsigned long MaxIndex)
    {
    unsigned long i;
	if(FLAG_PARSE_MSG == FALSE)
	    {
	    if(MaxIndex >= UART_MAX_RX_BUFFER_SIZE) MaxIndex=UART_MAX_RX_BUFFER_SIZE-1; 
	    for (i = 0; i <= MaxIndex; i++)  UART1RxMessage[i] = UART1RxBuffer[i];
		UART1RxMessage[i] = 0;
	    }
    };


