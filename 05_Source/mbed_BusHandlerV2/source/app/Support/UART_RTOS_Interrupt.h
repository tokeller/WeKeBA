#include "mbed.h"

// size for console input buffer
#define   UART_RTOS_Interrupt_buffer_SIZE   32

/*
 *	Interrupt processing the input from the console
 *
 */
void Standard_UART_Interrupt(void);

/*
 *	Setup the UART, configuring the baud rate and attaching the interrupt
 *
 */
void Setup_CHOSEN_UART(int baud_rate);

/*
 *	Read the written characters without locking the thread by using the interrupts
 *
 */
char UART_RTOS_getc(void);

/*
 *	enveloping function to put a character
 *
 */
int UART_RTOS_putc(char c);

/*
 *	printf enveloping function
 *
 */
int UART_RTOS_printf(char* input_string);
