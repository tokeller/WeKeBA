#include <RTL.h>
#include <stdio.h>

#ifndef MBED_H
#include "mbed.h"
#endif

#include "cmd.h"

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
static MenuState menu_fsm_state;
static uint8_t menu_fsm_current_sensor;

extern Serial pcSerial;


/* ------------------------------------------------------------------------------------ */
/* -- Tasks --------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------------------ */




__task void init(void) {

//  os_tsk_create( led_task_1, 7 );

  os_tsk_delete_self();
}

//*****************************************************************************
//
// This function implements the "ls" command.  It simply prints the
// current working directory.
//
//*****************************************************************************
int
Cmd_nix(int argc, char *argv[])
{
	FILE *Fptr;						   			//Create File pointer
	FINFO info;	
	
	/*
	if(fcheck("R:") != 0)						//check for a formatted drive
	{
		if(fformat ("R:")!= 0)	  				//Format the drive
		 {
		 while(1);					  				//Handle errors here
		 }
	} */

		// Fptr = fopen("DIRECTORY.LOG","w");												//Create a directory file and print a listing into it
		// info.fileID = 0;
		// while (ffind("*.*",&info) ==0)
		// {
		// //	pcSerial.printf(Fptr,"\n%s %5d bytes ID: %04d",info.name,info.size,info.fileID);	//write the info structure into Directory.log
		// 	pcSerial.printf("\n%s %5d bytes ID: %04d",info.name,info.size,info.fileID);
		// }
		// fclose(Fptr);

	return(0);
}

/*
 * See header file
 */
uint32_t Cmd_ls(int argc, char *argv[])
{
	return 255;
}

/*
 * See header file
 */
uint32_t Cmd_rm(int argc, char *argv[])
{
	return 255;
}

/*
 * See header file
 */
uint32_t Cmd_sd_format(int argc, char *argv[])
{
	return 255;
}

/*
 * See header file
 */
uint32_t Cmd_sd_mount(int argc, char *argv[])
{
	return 255;
}

/*
 * See header file
 */
uint32_t Cmd_sd_unmount(int argc, char *argv[])
{
	return 255;
}

/*
 * See header file
 */
uint32_t Cmd_logger_state(int argc, char *argv[])
{
	return 255;
}

/*
 * See header file
 */
uint32_t Cmd_logging(int argc, char *argv[])
{
	return 255;
}

/*
 * See header file
 */
uint32_t Cmd_mode(int argc, char *argv[])
{
	return 255;
}

/*
 * See header file
 */
uint32_t Cmd_sensor_parameters(int argc, char *argv[])
{
	return 255;
}

/*
 * See header file
 */
uint32_t Cmd_sensor_priority(int argc, char *argv[])
{
	return 255;
}

/*
 * See header file
 */
uint32_t Cmd_sensor_state(int argc, char *argv[])
{
	return 255;
}

/*
 * See header file
 */
uint32_t Cmd_timestamp(int argc, char *argv[])
{
	return 255;
}

/*
 * See header file
 */
uint32_t Cmd_time(int argc, char *argv[])
{
	return 255;
}

/*
 * See header file
 */

//*****************************************************************************
//
// This function implements the "dummy" command.  It does nothing.
//
//*****************************************************************************
uint32_t Cmd_dummy(int argc, char *argv[])
{
    return(0);
}



void init_menu_fsm(void)
{
	menu_fsm_state = S_BASEMENU;
	menu_fsm_current_sensor = 0;
	printf("init menu fsm done\n");
}

void menu_fsm(uint32_t input)
{
	printf("\nyour entry was: %d\n", input);
	switch(menu_fsm_state){
		
		case(S_BASEMENU):
			switch(input){
				case(1):
					// TODO list files
					printf("list files\n");
					menu_fsm_state = S_LIST_FILES;
				break;
				
				case(2):
					// TODO format sd card
					printf("sd format\n");
					menu_fsm_state = S_FORMAT_SD;
				break;
				
				case(3):
					// TODO mount sd card
					printf("sd mount\n");
				break;
				
				case(4):
					// TODO unmount sd
					printf("sd unmount\n");
				break;
				
				case(5):
					// TODO logger status
					printf("logg status\n");
				break;
				
				case(6):
					// TODO start/stop logging
					printf("start/stop logging\n");
					menu_fsm_state = S_LOGGER_START_STOP;
				break;
				
				case(7):
					// TODO detail level
					printf("detail level\n");
					menu_fsm_state = S_DETAIL_LEVEL;
				break;
				
				case(8):
					// TODO sensor parameters
					printf("sensor parameters\n");
					menu_fsm_state = S_SENSOR_PARAMETERS;
				break;
				
				case(9):
					// TODO sensor state
					printf("sensor state\n");
					menu_fsm_state = S_SENSOR_STATE;
				break;
				
				case(10):
					// TODO reset timestamp
					printf("reset timestamp\n");
					menu_fsm_state = S_RESET_TIMESTAMP;
				break;
				
				case(11):
					// TODO internal clock
					printf("internal clock\n");
					menu_fsm_state = S_INTERNAL_CLOCK;
				break;
				
				default:
					// TODO invalid input
				break;
			}
			break;
			
		case(S_LIST_FILES):
			switch(input){
				case(0):
					// TODO list files
				break;
				
				default:
					// TODO invalid input
				break;
			}
			break;
			
		case(S_DELETE_FILE):
			switch(input){
				case(0):
					// exit
					menu_fsm_state = S_BASEMENU;
				break;
				
				case(1):
					// TODO delete confirmed, execute
					menu_fsm_state = S_BASEMENU;
				break;
				
				default:
					// TODO user wants to delete a file, check file exists, print the name and ask for confirmation
				break;
			}
			break;
			
		case(S_FORMAT_SD):
			switch(input){
				case(0):
					// exit
					menu_fsm_state = S_BASEMENU;
				break;
				
				case(1):
					// TODO execute format sd
					menu_fsm_state = S_BASEMENU;
				break;
				
				default:
					// TODO invalid input
				break;
			}
			break;
			
		case(S_LOGGER_START_STOP):
			switch(input){
				case(0):
					// exit
					menu_fsm_state = S_BASEMENU;
				break;
				
				case(1):
					// TODO start/stop the logger
					menu_fsm_state = S_BASEMENU;
				break;
				
				default:
					// TODO invalid input
				break;
			}
			break;
			
		case(S_DETAIL_LEVEL):
			switch(input){
				case(0):
					// TODO handle input
				break;
				
				default:
					// TODO invalid input
				break;
			}
			break;
			
		case(S_SENSOR_PARAMETERS):
			switch(input){
				case(0):
					// TODO handle input
				break;
				
				default:
					// TODO invalid input
				break;
			}
			break;
			
		case(S_SENSOR_PARAMS_FS):
			switch(input){
				case(0):
					// TODO handle input
				break;
				
				default:
					// TODO invalid input
				break;
			}
			break;
			
		case(S_SENSOR_PARAMS_THRES):
			switch(input){
				case(0):
					// TODO handle input
				break;
				
				default:
					// TODO invalid input
				break;
			}
			break;
			
		case(S_SENSOR_PARAMS_BASELINE):
			switch(input){
				case(0):
					// TODO handle input
				break;
				
				default:
					// TODO invalid input
				break;
			}
			break;
			
		case(S_SENSOR_PARAMS_TIMEOUT):
			switch(input){
				case(0):
					// TODO handle input
				break;
				
				default:
					// TODO invalid input
				break;
			}
			break;
			
		case(S_SENSOR_PARAMS_DETAIL):
			switch(input){
				case(0):
					// TODO handle input
				break;
				
				default:
					// TODO invalid input
				break;
			}
			break;
			
		case(S_SENSOR_STATE):
			switch(input){
				case(0):
					// TODO handle input
				break;
				
				default:
					// TODO invalid input
				break;
			}
			break;
			
		case(S_RESET_TIMESTAMP):
			switch(input){
				case(0):
					// TODO handle input
				break;
				
				default:
					// TODO invalid input
				break;
			}
			break;
			
		case(S_INTERNAL_CLOCK):
			switch(input){
				case(0):
					// TODO handle input
				break;
				
				default:
					// TODO invalid input
				break;
			}
			break;
		
		default:
		break;
	}
}

uint32_t bad_command(void)
{
	return 0;
}
