 
 
#define   UART_RTOS_Interrupt_buffer_SIZE   32    // <---- set to desired size
 
void Standard_UART_Interrupt(void);
void Setup_CHOSEN_UART(int baud_rate);
char UART_RTOS_getc(void);
int UART_RTOS_putc(char c);
int UART_RTOS_printf(char* input_string);
