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
//static char g_cTmpBuf[PATH_BUF_SIZE];

//*****************************************************************************
//
// The buffer that holds the command line.
//
//*****************************************************************************
//static char g_cCmdBuf[CMD_BUF_SIZE];
static MenuState menu_fsm_state;
uint8_t menu_fsm_current_sensor;

extern Serial pcSerial;
extern LoggerConfig logger;
uint8_t time_updated = 0;


/* ------------------------------------------------------------------------------------ */
/* -- Tasks --------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------------------ */




__task void init(void) {

//  os_tsk_create( led_task_1, 7 );

  os_tsk_delete_self();
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
					menu_fsm_state = S_UNMOUNT_SD;
					cmd_enter_unmount_sd();
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
					// invalid input
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
					// invalid input, return to base menu anyway
					menu_fsm_state = S_BASEMENU;
					cmd_enter_basemenu();
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
					// LATER delete confirmed, execute
					menu_fsm_state = S_BASEMENU;
					cmd_enter_basemenu();
				break;
				
				default:
					// invalid input
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
					// confirm format sd
					cmd_format_sd();
					menu_fsm_state = S_BASEMENU;
					cmd_enter_basemenu();
				break;
				
				default:
					// invalid input
					printf("invalid input. Enter choice (0 to exit): \n");
				break;
			}
			break;
			
		case(S_UNMOUNT_SD):
			switch(input){
				case(0):
					// exit
					menu_fsm_state = S_BASEMENU;
					cmd_enter_basemenu();
				break;
				
				case(1):
					// confirm unmount SD
					cmd_unmount_sd();
					menu_fsm_state = S_BASEMENU;
					cmd_enter_basemenu();
				break;
				
				default:
					// invalid input
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
					// start/stop the logger
					if(logger.started){
						cmd_stop_logger();
					} else {
						printf("logger started\n\n");
						cmd_start_logger();
					}
					menu_fsm_state = S_BASEMENU;
					cmd_enter_basemenu();
				break;
				
				default:
					// invalid input
					printf("invalid input. Enter choice (0 to exit): \n");
				break;
			}
			break;
            
		case(S_SENSOR_PARAMS_GET_SENSOR_NR):
			switch(input){
				case(0):
					// exit
					menu_fsm_state = S_BASEMENU;
					cmd_enter_basemenu();
				break;
                    
				default:
					// user gave a number, check if valid, select that sensor
					// the sensors start with ID 2, but the config array starts with 0 (input - 2 necessary)
					if(input < 99){
						input -= 2;
					}
					if(cmd_sensor_id_is_valid(input)){ // check if valid
						menu_fsm_current_sensor = input;
						menu_fsm_state = S_SENSOR_PARAMETERS;
						cmd_enter_sensor_params();
					} else {
						printf("%d is not a valid sensor id or sensor is not registered.\n", input);
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
				
				case(6):
					// start or stop
					menu_fsm_state = S_SENSOR_PARAMS_STARTSTOP;
					cmd_enter_sensor_start_stop();
				break;
				
				default:
					// invalid input
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
					// user gave a number, check if valid and set f_s
					cmd_set_sampling_freq(menu_fsm_current_sensor, input);
					menu_fsm_state = S_SENSOR_PARAMETERS;
					cmd_enter_sensor_params();
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
					// user gave a number, check if valid and set threshold
					cmd_set_threshold(menu_fsm_current_sensor, input);
					menu_fsm_state = S_SENSOR_PARAMETERS;
					cmd_enter_sensor_params();
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
					// user gave a number, check if valid and set baseline
					cmd_set_baseline(menu_fsm_current_sensor, input);
					menu_fsm_state = S_SENSOR_PARAMETERS;
					cmd_enter_sensor_params();
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
					// user gave a number, check if valid and set timeout
					cmd_set_timeout(menu_fsm_current_sensor, input);
					menu_fsm_state = S_SENSOR_PARAMETERS;
					cmd_enter_sensor_params();
				break;
			}
			break;
			
		case(S_SENSOR_PARAMS_DETAIL):
			switch(input){
				case(9):
					// exit
					menu_fsm_state = S_SENSOR_PARAMETERS;
					cmd_enter_sensor_params();
				break;
				
				default:
					if(input < 4){
						// input valid, set mode of selected sensor.
						cmd_set_detail_mode(menu_fsm_current_sensor, (uint8_t) input);
					} else if(input == 4){
                        if(menu_fsm_current_sensor == 99){
                            // unable to comply
                            printf("raw mode not available for all sensors.\n");
                            menu_fsm_state = S_SENSOR_PARAMETERS;
                            cmd_enter_sensor_params();
                        } else {
                            cmd_enter_sensor_params_raw();
                            menu_fsm_state = S_SENSOR_PARAMS_DETAIL_RAW_DURATION;
                        }
                    } else {
						cmd_enter_sensor_params_detail();
						printf("invalid input. Enter choice (0 to exit): \n");
					}
				break;
			}
			break;
            
        case(S_SENSOR_PARAMS_DETAIL_RAW_DURATION):
            switch(input){
                case(0):
                    // exit
                    menu_fsm_state = S_SENSOR_PARAMETERS;
                    cmd_enter_sensor_params();
                    break;
                default:
                    if(input > 3600){
                        printf("invalid input. Enter duration (0 to cancel): \n");
                    } else {
                        menu_fsm_state = S_SENSOR_PARAMETERS;
                        cmd_set_detail_mode_raw(menu_fsm_current_sensor, input);
                        cmd_enter_sensor_params();
                    }
            }
			
		case(S_SENSOR_PARAMS_STARTSTOP):
			switch(input){
				case(0):
					// exit
					menu_fsm_state = S_SENSOR_PARAMETERS;
					cmd_enter_sensor_params();
				break;
				
				case(1):
					// start
					menu_fsm_state = S_SENSOR_PARAMETERS;
					cmd_sensor_start(menu_fsm_current_sensor);
					cmd_enter_sensor_params();
				break;
				
				case(2):
					// stop
					menu_fsm_state = S_SENSOR_PARAMETERS;
					cmd_sensor_stop(menu_fsm_current_sensor);
					cmd_enter_sensor_params();
				break;
				
				
				
				default:
					// invalid input
					printf("invalid input. Enter choice (0 to exit): \n");
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
					// invalid input
					printf("invalid input. Enter choice (0 to exit): \n");
				break;
			}
			break;
			
		case(S_INTERNAL_CLOCK):
			switch(input){
				case(0):
					// exit
					if(time_updated == 1){
						cmd_reset_timestamp();
						time_updated = 0;
					}
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
					// invalid input
					printf("invalid input. Enter choice (0 to exit): \n");
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
					// invalid input
					printf("invalid input. Enter choice (0 to exit): \n");
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
					// invalid input
					printf("invalid input. Enter choice (0 to exit): \n");
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
					cmd_enter_basemenu();
				break;
				
				case(2):
					// store config file
					menu_fsm_state = S_BASEMENU;
					cmd_store_config_file();
					cmd_enter_basemenu();
				break;
				
				default:
					// invalid input
					printf("invalid input. Enter choice (0 to exit): \n");
				break;
			}
			break;
		
		default:
			menu_fsm_state = S_BASEMENU;
            cmd_enter_basemenu();
		break;
	}
}


