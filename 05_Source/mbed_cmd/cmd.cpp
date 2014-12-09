#include <RTL.h>
#include <stdio.h>

#ifndef MBED_H
#include "mbed.h"
#endif

#include "sensor_config.h"
#include "cmd_action.h"
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
uint8_t menu_fsm_current_sensor;

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


void init_menu_fsm(void)
{
	menu_fsm_state = S_BASEMENU;
	menu_fsm_current_sensor = 0;
	
	#ifdef DEBUG_MENU
	printf("init menu fsm done\n");
	#endif
	
	cmd_enter_basemenu();
	
}

void menu_fsm(uint32_t input)
{
	printf("\nyour entry was: %d\n", input);
	switch(menu_fsm_state){
		
		case(S_BASEMENU):
			switch(input){
				case(0):
					cmd_enter_basemenu();
				break;
				case(1):
					// list files
					printf("list files\n");
					menu_fsm_state = S_LIST_FILES;
					cmd_enter_list_files();
				break;
				
				case(2):
					// format sd card
					printf("sd format\n");
					menu_fsm_state = S_FORMAT_SD;
					cmd_enter_format_sd();
				break;
				
				case(3):
					// mount sd card
					printf("sd mount\n");
					cmd_mount_sd();
					cmd_enter_basemenu();
				break;
				
				case(4):
					// unmount sd
					printf("sd unmount\n");
					cmd_unmount_sd();
					cmd_enter_basemenu();
				break;
				
				case(5):
					// logger status
					printf("logger status\n");
					cmd_print_logger_status();
					cmd_enter_basemenu();
				break;
				
				case(6):
					// start/stop logging
					printf("start/stop logging\n");
					menu_fsm_state = S_LOGGER_START_STOP;
					cmd_enter_logger_start();
				break;
				
				case(7):
					// sensor parameters
					printf("sensor parameters\n");
					menu_fsm_state = S_SENSOR_PARAMS_GET_SENSOR_NR;
					cmd_enter_sensor_params_get_nr();
				break;
				
				case(8):
					// sensor state
					printf("sensor state\n");
					menu_fsm_state = S_SENSOR_STATE;
					cmd_enter_sensor_state();
				break;
				
				case(9):
					// reset timestamp
					printf("reset timestamp\n");
					menu_fsm_state = S_RESET_TIMESTAMP;
					cmd_enter_reset_timestamp();
				break;
				
				case(10):
					// internal clock
					printf("internal clock\n");
					menu_fsm_state = S_INTERNAL_CLOCK;
					cmd_enter_internal_clock();
				break;
				
				case(11):
					// config file
					printf("config file\n");
					menu_fsm_state = S_CONFIG_FILE;
					cmd_enter_config_file();
				break;
				
				default:
					// TODO invalid input
					printf("invalid input. Enter choice (0 to print menu): \n");
				break;
			}
			break;
			
		case(S_LIST_FILES):
			switch(input){
				case(0):
					// exit
					menu_fsm_state = S_BASEMENU;
					cmd_enter_basemenu();
				break;
				
				default:
					// TODO user wants to delete a file, check file exists, print the name and ask for confirmation
					menu_fsm_state = S_DELETE_FILE;
				break;
			}
			break;
			
		case(S_DELETE_FILE):
			switch(input){
				case(0):
					// exit
					menu_fsm_state = S_BASEMENU;
                    cmd_enter_basemenu();
				break;
				
				case(1):
					// TODO delete confirmed, execute
					menu_fsm_state = S_BASEMENU;
					cmd_enter_basemenu();
				break;
				
				default:
					// TODO invalid input
					printf("invalid input. Enter choice (0 to exit):\n");
				break;
			}
			break;
			
		case(S_FORMAT_SD):
			switch(input){
				case(0):
					// exit
					menu_fsm_state = S_BASEMENU;
					cmd_enter_basemenu();
				break;
				
				case(1):
					// TODO execute format sd
					menu_fsm_state = S_BASEMENU;
					cmd_enter_basemenu();
				break;
				
				default:
					// TODO invalid input
					printf("invalid input. Enter choice (0 to exit): \n");
				break;
			}
			break;
			
		case(S_LOGGER_START_STOP):
			switch(input){
				case(0):
					// exit
					menu_fsm_state = S_BASEMENU;
					cmd_enter_basemenu();
				break;
				
				case(1):
					// TODO start/stop the logger
					menu_fsm_state = S_BASEMENU;
					cmd_enter_basemenu();
				break;
				
				default:
					// TODO invalid input
					printf("invalid input. Enter choice (0 to exit): \n");
				break;
			}
			break;
            
		case(S_SENSOR_PARAMS_GET_SENSOR_NR):
			switch(input){
				case(0):
					// TODO exit
					menu_fsm_state = S_BASEMENU;
					cmd_enter_basemenu();
				break;
                    
				default:
					// TODO user gave a number, check if valid, select that sensor
					if(cmd_sensor_id_is_valid(input)){ // TODO check if valid
						menu_fsm_current_sensor = input;
						menu_fsm_state = S_SENSOR_PARAMETERS;
						cmd_enter_sensor_params();
					} else {
						printf("%d is not a valid sensor id.\n", input);
						menu_fsm_state = S_BASEMENU;
						cmd_enter_basemenu();
					}
				break;
			}
			break;
			
		case(S_SENSOR_PARAMETERS):
			switch(input){
				case(0):
					// exit
					menu_fsm_state = S_BASEMENU;
					cmd_enter_basemenu();
				break;
				
				case(1):
					// set sampling rate
					menu_fsm_state = S_SENSOR_PARAMS_FS;
					cmd_enter_sensor_params_fs();
				break;
				
				case(2):
					// set threshold value
					menu_fsm_state = S_SENSOR_PARAMS_THRES;
					cmd_enter_sensor_params_thres();
				break;
				
				case(3):
					// set baseline value
					menu_fsm_state = S_SENSOR_PARAMS_BASELINE;
					cmd_enter_sensor_params_baseline();
				break;
				
				case(4):
					// timeout value
					menu_fsm_state = S_SENSOR_PARAMS_TIMEOUT;
					cmd_enter_sensor_params_timeout();
				break;
				
				case(5):
					// detail level
					menu_fsm_state = S_SENSOR_PARAMS_DETAIL;
					cmd_enter_sensor_params_detail();
				break;
				
				default:
					// TODO invalid input
					printf("invalid input. Enter choice (0 to exit): \n");
				break;
			}
			break;
			
		case(S_SENSOR_PARAMS_FS):
			switch(input){
				case(0):
					// exit
					menu_fsm_state = S_SENSOR_PARAMETERS;
					cmd_enter_sensor_params();
				break;
				
				default:
					// TODO user gave a number, check if valid and set f_s
					
				break;
			}
			break;
			
		case(S_SENSOR_PARAMS_THRES):
			switch(input){
				case(0):
					// exit
					menu_fsm_state = S_SENSOR_PARAMETERS;
					cmd_enter_sensor_params();
				break;
				
				default:
					// TODO invalid input
				break;
			}
			break;
			
		case(S_SENSOR_PARAMS_BASELINE):
			switch(input){
				case(0):
					// exit
					menu_fsm_state = S_SENSOR_PARAMETERS;
					cmd_enter_sensor_params();
				break;
				
				default:
					// TODO user gave a number, check if valid, set baseline
				break;
			}
			break;
			
		case(S_SENSOR_PARAMS_TIMEOUT):
			switch(input){
				case(0):
					// exit
					menu_fsm_state = S_SENSOR_PARAMETERS;
					cmd_enter_sensor_params();
				break;
				
				default:
					// TODO user gave a number, check if valid, set timeout
				break;
			}
			break;
			
		case(S_SENSOR_PARAMS_DETAIL):
			switch(input){
				case(0):
					// exit
					menu_fsm_state = S_SENSOR_PARAMETERS;
					cmd_enter_sensor_params();
				break;
				
				default:
					if(input <= 5){
						// TODO guter input, modus setzen
					} else {
						cmd_enter_sensor_params_detail();
						printf("invalid input. Enter choice (0 to exit): \n");
					}
				break;
			}
			break;
			
		case(S_SENSOR_STATE):
			switch(input){
				case(0):
					// exit
					menu_fsm_state = S_BASEMENU;
					cmd_enter_basemenu();
				break;
				
				default:
					// invalid input
					printf("invalid input. Enter choice (0 to exit): \n");
				break;
			}
			break;
			
		case(S_RESET_TIMESTAMP):
			switch(input){
				case(0):
					// exit
					menu_fsm_state = S_BASEMENU;
					cmd_enter_basemenu();
				break;
				
				case(1):
					menu_fsm_state = S_BASEMENU;
					cmd_reset_timestamp();
				break;
				
				default:
					// TODO invalid input
				break;
			}
			break;
			
		case(S_INTERNAL_CLOCK):
			switch(input){
				case(0):
					// exit
					menu_fsm_state = S_BASEMENU;
          cmd_enter_basemenu();
				break;
				
				case(1):
					// set the date
					menu_fsm_state = S_INTERNAL_CLOCK_SET_DATE;
					cmd_enter_internal_clock_set_date();
				break;
				
				case(2):
					// set the time
					menu_fsm_state = S_INTERNAL_CLOCK_SET_TIME;
					cmd_enter_internal_clock_set_time();
				break;
				
				default:
					// TODO invalid input
				break;
			}
			break;
			
		case(S_INTERNAL_CLOCK_SET_DATE):
			switch(input){
				case(0):
					// exit
					menu_fsm_state = S_INTERNAL_CLOCK;
					cmd_enter_internal_clock();
				break;
				
				case(1):
					// inc yr
					cmd_internal_clock_inc_yr();
					cmd_enter_internal_clock_set_date();
				break;
				
				case(2):
					// dec yr
					cmd_internal_clock_dec_yr();
					cmd_enter_internal_clock_set_date();
				break;
				
				case(3):
					// inc mnt
					cmd_internal_clock_inc_mnt();
					cmd_enter_internal_clock_set_date();
				break;
				
				case(4):
					// dec mnt
					cmd_internal_clock_dec_mnt();
					cmd_enter_internal_clock_set_date();
				break;
				
				case(5):
					// inc day
					cmd_internal_clock_inc_10day();
					cmd_enter_internal_clock_set_date();
				break;
				
				case(6):
					// dec day
					cmd_internal_clock_dec_10day();
					cmd_enter_internal_clock_set_date();
				break;
				
				case(7):
					// inc day
					cmd_internal_clock_inc_day();
					cmd_enter_internal_clock_set_date();
				break;
				
				case(8):
					// dec day
					cmd_internal_clock_dec_day();
					cmd_enter_internal_clock_set_date();
				break;
				
				default:
					// TODO check valid input, store, proceed
				break;
			}
			break;
			
		case(S_INTERNAL_CLOCK_SET_TIME):
			switch(input){
				case(0):
					// exit
					menu_fsm_state = S_INTERNAL_CLOCK;
					cmd_enter_internal_clock();
				break;
				
				case(1):
					// inc hr
					cmd_internal_clock_inc_hr();
					cmd_enter_internal_clock_set_time();
				break;
				
				case(2):
					// dec hr
					cmd_internal_clock_dec_hr();
					cmd_enter_internal_clock_set_time();
				break;
				
				case(3):
					// inc min
					cmd_internal_clock_inc_10min();
					cmd_enter_internal_clock_set_time();
				break;
				
				case(4):
					// dec min
					cmd_internal_clock_dec_10min();
					cmd_enter_internal_clock_set_time();
				break;
				
				case(5):
					// inc min
					cmd_internal_clock_inc_min();
					cmd_enter_internal_clock_set_time();
				break;
				
				case(6):
					// dec min
					cmd_internal_clock_dec_min();
					cmd_enter_internal_clock_set_time();
				break;
				
				case(7):
					// inc sec
					cmd_internal_clock_inc_sec();
					cmd_enter_internal_clock_set_time();
				break;
				
				case(8):
					// dec sec
					cmd_internal_clock_dec_sec();
					cmd_enter_internal_clock_set_time();
				break;
				
				default:
					// TODO check valid input, store, proceed
				break;
			}
			break;
			
		case(S_CONFIG_FILE):
			switch(input){
				case(0):
					// exit
					menu_fsm_state = S_BASEMENU;
          cmd_enter_basemenu();
				break;
				
				case(1):
					// read config file
					menu_fsm_state = S_BASEMENU;
					cmd_read_config_file();
				break;
				
				case(2):
					// store config file
					menu_fsm_state = S_BASEMENU;
					cmd_store_config_file();
				break;
				
				default:
					// TODO invalid input
				break;
			}
			break;
		
		default:
			menu_fsm_state = S_BASEMENU;
            cmd_enter_basemenu();
		break;
	}
}


