#ifndef __SEL_UART__
#define __SEL_UART__

//
// TEST CODE DEFINITIONS
//

//
// HEADERS
//

//#include "inc/hw_types.h"
//#include "inc/hw_ints.h"
//#include "inc/hw_uart.h"
#include "inc/hw_gpio.h"
//#include "inc/hw_memmap.h"
//#include "driverlib/interrupt.h"
//#include "driverlib/gpio.h"
//#include "driverlib/sysctl.h"
//#include "driverlib/uart.h"
//#include "GD_Globals.h"
  
//
// DEFINITIONS
//

// UART1
#define UART1_SYSCTL_GPIO_PERIPH					SYSCTL_PERIPH_GPIOD
#define UART1_GPIO_PINS_BASE						GPIO_PORTD_BASE
#define UART1_GPIO_PIN_RX							GPIO_PIN_2
#define UART1_GPIO_PIN_TX							GPIO_PIN_3
#define UART1_GPIO_RX_TX_PINS						(UART1_GPIO_PIN_RX | UART1_GPIO_PIN_TX)

#define UART1_SYSCTL_GPIO_DIRECTION_PINS_PERIPH		SYSCTL_PERIPH_GPIOD
#define UART1_GPIO_DIRECTION_PINS_BASE				GPIO_PORTD_BASE
#define UART1_GPIO_PIN_TX_ENABLE					GPIO_PIN_1 // active high


//#define UART1_TX_ENABLE_ACTIVE						(unsigned char)UART1_GPIO_PIN_TX_ENABLE

//
// PROTOCOL DEFINITIONS	AND VARIABLES
//
#define __RS_485_MODE__
#define UART1_BAUDRATE							115200
//#define UART1_BAUDRATE							56000

#define UART_MAX_RX_BUFFER_SIZE					256
#define UART_MAX_TX_BUFFER_SIZE					256

#define UART_TX_START_CHARACTER		'>'
#define UART_RX_START_CHARACTER		'#'
//#define UART_BINARY_MESSAGE_START_CHARACTER		'#'		// 0x23
#define UART_MESSAGE_END_CHARACTER				'\r'	// 0x0D
//#define UART_MESSAGE_ESCAPE_CHARACTER			'^'		// 0x5E

#define FLAG_TRUE								(unsigned long)0x00000001
#define FLAG_FALSE								(unsigned long)0x00000000

// UART1 VARIABLES
extern unsigned char UART1RxBuffer [UART_MAX_RX_BUFFER_SIZE];
extern unsigned char UART1RxMessage[UART_MAX_RX_BUFFER_SIZE];
extern unsigned char UART1TxBuffer [UART_MAX_TX_BUFFER_SIZE];
extern unsigned char UART1TxMessage[UART_MAX_TX_BUFFER_SIZE];
extern unsigned char UART1RxData;
extern unsigned long UART1FLAG_Recieved_Message;
extern unsigned long UART1IndexRx;
extern unsigned long UART1FLAG_TxEmpty;
extern unsigned long UART1IndexTx;
extern unsigned long UART1TxSize;

//
// FUNCTIONS
//

// UART1 FUNCTIONS
// Interrupt handlers
extern void SelUART1IntHandler(void);
extern void UART1FIFOIntHandler(void);
extern void SelUART1Init(void);
extern void SelUART1PrmReset(void);
//extern unsigned long SelUART1SendTxBuffer(unsigned long len);
//extern void SelUART1SetTxEnable(unsigned char TxEnableState);
//extern void SelUART1SetRxEnable(unsigned char RxEnableState);
extern void SelUART1TransmitEnable(void);
extern void SelUART1TransmitDisable(void);
//extern void SelUART1ClearRxMessage(void);
//extern void SelUART1CopyRxBuffer2RxMessage(unsigned long MaxIndex);

void SelUART1SendTxBuffer(unsigned char *text);


// UART GENERAL FUNCTIONS
extern __inline void SelUARTPutChar(unsigned long ulBase, unsigned char ucData);
extern __inline long SelUARTGetChar(unsigned long ulBase);

#endif
