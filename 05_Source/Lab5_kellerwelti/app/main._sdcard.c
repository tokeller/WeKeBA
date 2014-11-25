
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
	FILE *Fptr;
  unsigned char count[10],byte;
  char string1[11],string2[6];
	
  sysInit_hal();
  ledInit_hal();
	finit();
  //os_sys_init( init );
	


	 write_file ();
	
//sweet.wav
	//silence.wav
Fptr = fopen("TEST.TXT","r");							//Open the test file
fread( &count[0],1,10,Fptr);							//read the first block of data
byte = fgetc(Fptr);										//read a byte of data
fgets(string1,12,Fptr);									//Read a string of characters
while(1);		//Succsess
}			  							

void write_file (void)
{
unsigned char count[10]= {0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x30};
FILE *Fptr;

if (fcheck ("R:") != 0) { 
 if(fformat ("R:")!= 0)	  				//Format the drive
 {
 while(1);					 
 }
 }

Fptr = fopen("TEST.TXT","w");			 	//create a new fiel

if(Fptr == NULL)
{
while(1);						
}

fwrite(&count,1,10,Fptr);					//Write data to the drive using different formats
fputc('A',Fptr);
fputs("Hello World",Fptr);
fclose(Fptr);
}

	
// {
//  if(fformat ("M:")!= 0)			 //Format the Ram
//  {
//  while(1);
//  }
//  while(1);							 //Sucsess
// }


