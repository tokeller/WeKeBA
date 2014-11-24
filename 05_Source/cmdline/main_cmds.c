
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
    { "help",    Cmd_help, "               : Display list of commands. help cmd will display detailed help for cmd.",
        "help - lists all available commands with a short help text.\n\n\
        Calling help with a command name as argument will display a \n\
        detailed help text for this command." },
    { "h",    Cmd_help, "                  : alias for help",
        "alias for help" },
    { "?",    Cmd_help, "                  : alias for help",
        "alias for help" },
    { "ls",    Cmd_dummy, "                : list files.",
        "Displays a list of files with their name and file size.\n\n\
        At the end of the list, remaining space on card is displayed." },
    { "rm",    Cmd_dummy, "                : remove a file.",
        "rm 'filename' deletes the file named 'filename' and frees the space." },
    { "sd_format",    Cmd_dummy, "         : format SD card.",
        "Formats the SD card to prepare it for use in the logger.\n\n\
        This command will erase all files stored on the SD card." },
    { "sd_mount",    Cmd_dummy, "          : try to mount newly inserted card.",
        "Tries to mount the SD card to be used for storing files.\n\n\
        If no valid file system can be found, you will need to\n\
        format the card." },
    { "sd_unmount",    Cmd_dummy, "        : prepare SD card for removal.",
        "Prepares the SD card to be removed from the logger.\n\n\
        This command will try to close all open files. If \n\
        unsuccessful, stop the logging operation and then\n\
        try to unmount the card again." },
    { "logger_state",    Cmd_dummy, "      : display state of logging unit.",
        "Displays the state of the logging unit.\n\n\
        sampling rate\n\
        operation mode\n\
        
        space used/left on SD card\n\
        list of sensors" },
    { "logging",    Cmd_dummy, "           : start or stop logging.",
        "Start or stop logging of data into files. When stopped, the \n\
        logger will close all open files on the SD card. This will allow\n\
        you to unmount the card prior to removal.\n\
        Starting the logging without a mounted SD card will not succeed.\n\
        The logger will try to store in internal storage all received events\n\
        during stopped logging. The internal storage is limited." },
    { "mode",    Cmd_dummy, "              : set mode of operation.",
        "Set operation mode of the system.\n\n\
        Available modes:\n\
        1:   raw data. Will record continuous data from a sensor unit.\n\
             You must provide a duration for recording TODO\n\
        2:   detailed impact data. Will record all samples from beginning\n\
             of an impact to its end.\n\
        3:   sparse impact data. Will record beginning and duration of an \n\
             impact along with all peak maxima.\n\
        4:   minimal data. Will record beginning, duration, number of peaks\n\
             and the maximum amplitude of the impact." },
    { "sensor_parameters",    Cmd_dummy, " : set measurement parameters.",
        "Set sensor parameters. You must provide at least one parameter\n\
        and the number of a sensor.\n\n\
        sensor_parameters -option value sensor_id\n\
        Arguments:\n\
        -t:        threshold\n\
        -T:        timeout\n\
        -b:        baseline (usually 2047)\n\
        -s:        sampling rate in 100 Hz (for 10 kHz enter 100)\n\
        sensor_id: " },
    { "sensor_priority",    Cmd_dummy, "   : set priority of sensors.",
        "Displays a list of sensors. You can sort the sensors into the\n\
        desired transmission priority using an interactive menu." },
    { "sensor_state",    Cmd_dummy, "      : display status of sensor(s).",
        "Displays the state of a sensor unit. Values displayed are:\n\
        cache usage\n\
        bus errors TODO \n\
        baseline value\n\
        threshold value\n\
        timeout\n\
        sampling rate\n\
        operation mode\n\n\
        Possible arguments:\n\
        -a:   lists state of all registered sensors in short format." },
    { "timestamp",    Cmd_dummy, "         : display or set timestamp.",
        "Displays the timestamp of the logging unit and how much time is\n\
        left until roll-over.\n\n\
        Possible arguments:\n\
        -c:   list timestamp value of all sensor units. The logger will \n\
              call all sensor units to provide their timestamp values.\n\
              Timestamps displayed will have some inaccuracy due to the\n\
              bus delay.\n\
        -s:   synchronize timestamp. The logger will order all sensor units\n\
              to reset timestamp to zero and refer all future impacts and \n\
              events to the new timestamp." },
    { "time",    Cmd_dummy, "              : display or set internal clock.",
        "Displays internal clock.\n\n\
        Possible arguments:\n\
        -s:   set the internal clock.\n\
              Enter time and date in the following format:\n\
              year month day hour min sec, that is\n\
              in numbers separated by spaces, using 24 hour format." },
    { 0, 0, 0, 0 }
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

//*****************************************************************************
//
// This function implements the "dummy" command.  It does nothing.
//
//*****************************************************************************
int Cmd_dummy(int argc, char *argv[])
{
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
