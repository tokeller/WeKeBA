
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

//*****************************************************************************
//
// Defines the size of the buffers that hold the path, or temporary
// data from the SD card.  There are two buffers allocated of this size.
// The buffer size must be large enough to hold the longest expected
// full path name, including the file name, and a trailing null character.
//
//*****************************************************************************
#define PATH_BUF_SIZE   80

//*****************************************************************************
//
// Defines the size of the buffer that holds the command line.
//
//*****************************************************************************
#define CMD_BUF_SIZE    64

//*****************************************************************************
//
// This buffer holds the full path to the current working directory.
// Initially it is root ("/").
//
//*****************************************************************************
static char g_cCwdBuf[PATH_BUF_SIZE] = "/";

//*****************************************************************************
//
// A temporary data buffer used when manipulating file paths, or reading data
// from the SD card.
//
//*****************************************************************************
static char g_cTmpBuf[PATH_BUF_SIZE];

//*****************************************************************************
//
// The buffer that holds the command line.
//
//*****************************************************************************
static char g_cCmdBuf[CMD_BUF_SIZE];




/* ------------------------------------------------------------------------------------ */
/* -- Tasks --------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------------------ */

__task void led_task_1(void) {

  os_itv_set(50);
	

 while (1) {
   os_itv_wait();
   ledOn_hal();
 }
}



__task void init(void) {

  os_tsk_create( led_task_1, 7 );

  os_tsk_delete_self();
}

//*****************************************************************************
//
// This function implements the "help" command.  It prints a simple list
// of the available commands with a brief description.
//
//*****************************************************************************
int
Cmd_help(int argc, char *argv[])
{
    tCmdLineEntry *pEntry;

    //
    // Print some header text.
    //
    UARTprintf("\nAvailable commands\n");
    UARTprintf("------------------\n");

    //
    // Point at the beginning of the command table.
    //
    pEntry = &g_sCmdTable[0];

    //
    // Enter a loop to read each entry from the command table.  The
    // end of the table has been reached when the command name is NULL.
    //
    while(pEntry->pcCmd)
    {
        //
        // Print the command name and the brief description.
        //
        UARTprintf("%s%s\n", pEntry->pcCmd, pEntry->pcHelp);

        //
        // Advance to the next entry in the table.
        //
        pEntry++;

        //
        // Wiat for the UART to catch up.
        //
        UARTFlushTx(false);
    }

    //
    // Return success.
    //
    return(0);
}


//*****************************************************************************
//
// This is the table that holds the command names, implementing functions,
// and brief description.
//
//*****************************************************************************
tCmdLineEntry g_sCmdTable[] =
{
    { "help",   Cmd_help,      " : Display list of commands" },
    { "h",      Cmd_help,   "    : alias for help" },
    { "?",      Cmd_help,   "    : alias for help" },
    { "ls",     Cmd_help,      "   : Display list of files" },
    { "chdir",  Cmd_help,         ": Change directory" },
    { "cd",     Cmd_help,      "   : alias for chdir" },
    { "pwd",    Cmd_pwd,      "  : Show current working directory" },
    { "cat",    Cmd_help,      "  : Show contents of a text file" },
    { 0, 0, 0 }
};

//*****************************************************************************
//
// This function implements the "pwd" command.  It simply prints the
// current working directory.
//
//*****************************************************************************
int
Cmd_pwd(int argc, char *argv[])
{
FILE *Fptr;						   			//Create File pointer
FINFO info;	
	
    if(fcheck("R:") != 0)						//check for a formatted drive
{
if(fformat ("R:")!= 0)	  				//Format the drive
 {
 while(1);					  				//Handle errors here
 }
}

// Fptr = fopen("DIRECTORY.LOG","w");												//Create a directory file and print a listing into it
// info.fileID = 0;
// while (ffind("*.*",&info) ==0)
// {
// //	UARTprintf(Fptr,"\n%s %5d bytes ID: %04d",info.name,info.size,info.fileID);	//write the info structure into Directory.log
// 	UARTprintf("\n%s %5d bytes ID: %04d",info.name,info.size,info.fileID);
// }
// fclose(Fptr);




    return(0);

}


int main(void) 
{
	int nStatus;
	
  sysInit_hal();
  ledInit_hal();
  //os_sys_init( init );
	
	UARTprintf("\n\nSD Card Example Program\n");
  UARTprintf("Type \'help\' for help.\n");
	
	//
    // Mount the file system, using logical disk 0.
    //
//     fresult = f_mount(0, &g_sFatFs);
//     if(fresult != FR_OK)
//     {
//         UARTprintf("f_mount error: %s\n", StringFromFresult(fresult));
//         return(1);
//     }


    //
    // Enter an (almost) infinite loop for reading and processing commands from
    //the user.
    //
    while(1)
    {
        //
        // Print a prompt to the console.  Show the CWD.
        //
        UARTprintf("\n%s> ", g_cCwdBuf);

        //
        // Is there a command waiting to be processed?
        //
//         while(UARTPeek('\r') < 0)
//         {
//             //
//             // Process any messages in the widget message queue.
//             //
//             WidgetMessageQueueProcess();
//         }

        //
        // Get a line of text from the user.
        //
        UARTgets(g_cCmdBuf, sizeof(g_cCmdBuf));

        //
        // Pass the line from the user to the command processor.
        // It will be parsed and valid commands executed.
        //
        nStatus = CmdLineProcess(g_cCmdBuf);

        //
        // Handle the case of bad command.
        //
        if(nStatus == CMDLINE_BAD_CMD)
        {
            UARTprintf("Bad command!\n");
        }

        //
        // Handle the case of too many arguments.
        //
        else if(nStatus == CMDLINE_TOO_MANY_ARGS)
        {
            UARTprintf("Too many arguments for command processor!\n");
        }

        //
        // Otherwise the command was executed.  Print the error
        // code if one was returned.
        //
//         else if(nStatus != 0)
//         {
//             UARTprintf("Command returned error code %s\n",
//                         StringFromFresult((FRESULT)nStatus));
//         }
    }
}

//int main (void)
// {
//  if(fformat ("R:")!= 0)			 //Format the Ram
//  {
//  while(1);
//  }
//  while(1);							 //Sucsess

// }
