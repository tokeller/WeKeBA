#include "mbed.h"
#include "UART_RTOS_Interrupt.h"

char UART_RTOS_Interrupt_buffer[UART_RTOS_Interrupt_buffer_SIZE] = "";
int UART_RTOS_Interrupt_buffer_position = 0;
 
Serial CHOSEN_UART(USBTX, USBRX);   // <----- tx,rx, UART2
 
void Standard_UART_Interrupt(void) {
    char c;
    
    if(CHOSEN_UART.readable()) {
            //c = CHOSEN_UART.getc();
            c = (char)LPC_UART0->RBR;     // <----- IMPORTANT: LPC_UARTx must be set to correct UART (UART0 = USB, UART1 = p13/14, UART2 = p27/28, UART3 = p9/10).
            if(UART_RTOS_Interrupt_buffer_position < UART_RTOS_Interrupt_buffer_SIZE - 1) {
                UART_RTOS_Interrupt_buffer[UART_RTOS_Interrupt_buffer_position] = c;
                UART_RTOS_Interrupt_buffer_position++;
            }
    }
}
 
void Setup_CHOSEN_UART(int baud_rate) {
    CHOSEN_UART.baud(baud_rate);
    UART_RTOS_Interrupt_buffer_position = 0;
    CHOSEN_UART.attach(&Standard_UART_Interrupt, CHOSEN_UART.RxIrq);    // Recv interrupt handler
    printf("UART RTOS Interrupt Buffer Size: %d\n", UART_RTOS_Interrupt_buffer_SIZE);     // <----- May wish to disable
}
 
char UART_RTOS_getc(void) {
    int a;
    char c;
    
    while(!UART_RTOS_Interrupt_buffer_position);    //blocking just like getc
    
    CHOSEN_UART.attach(NULL, CHOSEN_UART.RxIrq);    // Recv interrupt handler  --  Have to stop interrupt from firing during this manipulation
    
    c = UART_RTOS_Interrupt_buffer[0];
    for(a = 1; a <= UART_RTOS_Interrupt_buffer_position - 1; a++) {
        UART_RTOS_Interrupt_buffer[a - 1] = UART_RTOS_Interrupt_buffer[a];
    }
    UART_RTOS_Interrupt_buffer[UART_RTOS_Interrupt_buffer_position - 1] = '\0';
    UART_RTOS_Interrupt_buffer_position--;
    
    CHOSEN_UART.attach(&Standard_UART_Interrupt, CHOSEN_UART.RxIrq);    // Recv interrupt handler
     
    return c;
}
 
int UART_RTOS_putc(char c) {
    CHOSEN_UART.putc(c);
    return 1;
}
 
int UART_RTOS_printf(char* input_string) {
    CHOSEN_UART.printf(input_string);
    return 1;
}
