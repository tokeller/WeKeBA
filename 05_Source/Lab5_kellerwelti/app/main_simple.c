
#include <RTL.h>
#include <stdio.h>




//#include "inc/hw_ints.h"
//#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
//#include "inc/hw_uart.h"
//#include "driverlib/debug.h"
//#include "driverlib/interrupt.h"
//#include "driverlib/rom.h"
//#include "driverlib/rom_map.h"
//#include "driverlib/sysctl.h"
//#include "driverlib/uart.h"
//#include "uartstdio.h"

#include "main.h"
#include "hal_led.h"
#include "hal_system.h"
//from here new for lab
#include "uartstdio.h"
#include "ustdlib.h"
#include "cmdline.h"

int main (void)
{
 if(fformat ("R:")!= 0)			 //Format the Ram
 {
 while(1);
 }
 while(1);							 //Sucsess

}
