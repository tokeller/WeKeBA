#ifndef MBED_H
#include "mbed.h"
#endif

#include "Serial.h"
#include "MCIFileSystem.h"
#include "BusHandler.h"
#include "sensor_config.h"
#include "file_ops.h"
#include "cmd_action.h"

extern SensorConfig sensor[MAX_SENSORS];
extern LoggerConfig logger;
extern uint8_t time_updated;
extern uint8_t menu_fsm_current_sensor;
extern MCIFileSystem mcifs;
static uint8_t detail_str[6][11] = 
{
	// strings to describe the detail levels
	"raw",
	"detailed",
	"peaks only",
	"sparse",
	"off",
	""
};

/*
 * See header file
 */
void cmd_enter_basemenu(void)
{
	printf("entering basemenu\n");
	printf(" 1) list files\n");
	printf(" 2) format SD card\n");
	printf(" 3) mount SD card\n");
	printf(" 4) unmount SD card\n");
	printf(" 5) logger status\n");
	printf(" 6) start/stop logging\n");
	printf(" 7) sensor parameters\n");
	printf(" 8) sensor states\n");
	printf(" 9) reset timestamp\n");
	printf("10) internal clock\n");
	printf("11) config file\n");
	printf("\n");
	if(logger.config_modified){
		printf("Config has been modified but not saved to SD card.\n");
	}
	if(!logger.sd_present){
		printf("No SD card found, please insert card and mount it.\n");
	}
}

/*
 * See header file
 */
void cmd_enter_list_files(void)
{
	
	printf("entering list files\n");
	// list files here:
	recursiveList("/mci/");
	// LATER, won't be in first final version
	// printf(" #) select any file to be deleted.\n");
	printf(" 0) exit\n");
}

/*
 * See header file
 */
void cmd_enter_delete_file(void)
{
	printf("entering delete_file\n");
	// LATER, won't be in first final version
	printf(" 1) confirm deletion of file %s\n", "none");
	printf(" 0) cancel\n");
}

/*
 * See header file
 */
void cmd_enter_format_sd(void)
{
	printf("entering format sd\n");
	printf(" 1) confirm formatting of SD card. \n    All data will be erased\n");
	printf(" 0) cancel\n");
}

/*
 * See header file
 */
void cmd_format_sd(void)
{
	printf("formatting SD Card\n");
	mcifs.format();
	if(mcifs.disk_initialize() == 1){
		printf("Formatting SD card FAILED. Please use a Computer to format the card.\n");
	} else {
		printf("Formatting done\n");
	}
	printf("Returning to base menu.\n");
}

/*
 * See header file
 */
void cmd_mount_sd(void)
{
	printf("entering mount SD card\n");
	if(!mcifs.cardInserted()){
		printf("No SD card detected! Please insert card and try again!\n");
	} else {
		logger.sd_present = 1;
	}
	
}

/*
 * See header file
 */
void cmd_enter_unmount_sd(void)
{
	printf("entering unmount sd\n");
	if(logger.config_modified){
		printf("****************************************************************** \n");
		printf("* WARNING: sensor configuration data has not been saved to file! * \n");
		printf("* If you want to save config to file, cancel now.                * \n");
		printf("****************************************************************** \n");
	}
	printf(" 1) unmount SD card\n    This will stop logging and close all data files.");
	printf(" 0) cancel\n");
}
/*
 * See header file
 */
void cmd_unmount_sd(void)
{
	printf("Unmounting SD card: stop logging and close all data files.\n");
	// stop logging. This will also close all the files.
	cmd_stop_logger();
	// TODO send BC to all sensors to go 'offline' but do not set
	// the started flag to 0 in the config. This way, we will restart only
	// those sensors that were running before the stop.
	
	
	printf("Logging has been stopped.\n");
	
	// TODO: if config has been changed but not stored, display a warning.
	
	printf("All files have been closed.\n");
	// set flag so we can display a reminder in the base menu.
	logger.sd_present = 0;
	
	printf("You may now remove the SD card.\n");
}

/*
 * See header file
 */
void cmd_print_logger_status(void)
{
	printf("displaying logger status\n"); // TODO show logger stats
}

/*
 * See header file
 */
void cmd_enter_logger_start(void)
{
	printf("entering logger start/stop\n");
	printf("Logger is %s\n", logger.started ? "running" : "stopped");
	printf(" 1) %s the logging.\n", logger.started ? "stop": "start");
	printf(" 0) cancel\n");
}

/*
 * See header file
 */
void cmd_start_logger(void)
{
	// TODO was braucht es alles, um den logger zu starten?
	uint8_t i;
	uint8_t success = 1;
	// open all data files
	for(i = 0; i < MAX_SENSORS; i++){
		//if(sensor[i].pf_sensor_data != NULL){
		if(sensor[i].pf_sensor_data == NULL){
			if(sensor[i].is_registered == 1){
				printf("Sensorfiles anlegen\n");
				if(!cmd_open_sensor_file(i)){
					printf("Sensorfile failed\n");
					success = 0;
				}
			}
		}
	}
	// TODO was, wenn nicht alle files geöffnet werden konnten?
	// TODO was muss man alles noch tun, um den logger zu starten?
	logger.started = 1;
}

/*
 * See header file
 */
void cmd_stop_logger(void)
{
	uint8_t i;
	// close all data files
	for(i = 0; i < MAX_SENSORS; i++){
		cmd_close_sensor_file(i);
	}
	// TODO was muss man alles noch tun, um den logger zu stoppen?
	logger.started = 0;
}

/*
 * See header file
 */
void cmd_enter_sensor_params(void)
{
	// TODO: stop acquisition before changing fs, then reset timestamp and start again.
	printf("entering sensor params\n");
	
	printf(" 1) set sampling rate ");
	if(menu_fsm_current_sensor !=99){ // retrieve current value unless all sensors selected
		printf("(current: %3.0d00 Hz)", 
		sensor[menu_fsm_current_sensor].fs);
	}
	printf("\n");
	
	printf(" 2) set threshold value ");
	if(menu_fsm_current_sensor !=99){ // retrieve current value unless all sensors selected
		printf("(current: %5.0d)", 
			sensor[menu_fsm_current_sensor].threshold);
	}
	printf("\n");
	
	printf(" 3) set baseline value ");
	if(menu_fsm_current_sensor !=99){ // retrieve current value unless all sensors selected
		printf("(current: %5.0d)", 
			sensor[menu_fsm_current_sensor].baseline);
	}
	printf("\n");
	
	printf(" 4) set timeout ");
	if(menu_fsm_current_sensor !=99){ // retrieve current value unless all sensors selected
		printf("(current: %5.0d)", 
			sensor[menu_fsm_current_sensor].timeout);
	}
	printf("\n");
	
	printf(" 5) set detail level ");
	if(menu_fsm_current_sensor !=99){ // retrieve current value unless all sensors selected
		printf("(current: %s)", 
			detail_str[sensor[menu_fsm_current_sensor].detail_level]);
	}
	printf("\n");
	
	printf(" 6) start or stop recording ");
	if(menu_fsm_current_sensor !=99){ // retrieve current value unless all sensors selected
		printf("(current: %s)",
			(sensor[menu_fsm_current_sensor].started ? "started" : "stopped"));
	}
	printf("\n");
	
	printf(" 0) exit\n");
}

/*
 * See header file
 */
void cmd_enter_sensor_params_get_nr(void)
{
	printf("entering sensor params get nr\n");
	menu_fsm_current_sensor = 0;
	cmd_list_sensor_states();
	printf(" #) Select a sensor from the list.\n");
	printf("99) Select all sensors.\n");
	printf(" 0) cancel\n");
}

/*
 * See header file
 */
uint8_t cmd_sensor_id_is_valid(uint8_t sensor_index)
{
	// sensor_index must be within sensor array and a registered sensor or 99 for ALL_SENSORS
	if(sensor_index == 99 || (sensor_index < MAX_SENSORS && sensor[sensor_index].is_registered != 0) ){
		return 1;
	}
	return 0;
}

/*
 * See header file
 */
void cmd_enter_sensor_params_fs(void)
{
	printf("entering sensor params fs \n");
	printf(" #) Enter sampling rate in Hz. (multiple of 100 Hz in range 100..200'000 Hz)\n");
	printf(" 0) cancel\n");
}

/*
 * See header file
 */
void cmd_set_sampling_freq(uint8_t sensor_index, uint32_t fs)
{
	int i;
	// fs must be between 1 and 2000 (100..200'000 Hz). The CPU can't handle faster sampling.
	
	fs = fs/100; // user enters desired sampling in Hz, we need to divide by 100.
	
	if(fs > 2000){
		printf("Sampling rate %d00 Hz not supported, too high.\n", fs);
	} else if (fs == 0){
		printf("Sampling rate 0 Hz not supported.\n");
		printf("To turn off sensor, set the detail level to 'off'\n");
		printf("or use start/stop menu\n");
	} else if(sensor_index == 99) {
		// set all sensors
		for(i = 0; i < MAX_SENSORS; i++){
			if(sensor[i].serialID != 0){
				sensor[i].fs = fs;
				cmd_send_config_to_sensor(i);
				logger.config_modified = 1;
			} // fi
		} // rof
	} else {
		if(sensor[sensor_index].fs != fs){
			logger.config_modified = 1;
		}
		sensor[sensor_index].fs = fs;
		cmd_send_config_to_sensor(sensor_index);
	} // fi
}

/*
 * See header file
 */
void cmd_enter_sensor_params_thres(void)
{
	printf("entering sensor params thres \n");
	printf(" #) Enter threshold value.\n");

	printf("baseline + threshold must not exceed 4096\nand\n");

	printf("baseline - threshold must not be below 0\n");
	printf(" 0) cancel\n");
}

/*
 * See header file
 */
void cmd_set_threshold(uint8_t sensor_index, uint32_t threshold)
{
	int i;
	// threshold must be smaller than baseline
	// threshold must be smaller than 4096-baseline
	if(sensor_index == 99) {
		// set all sensors
		for(i = 0; i < MAX_SENSORS; i++){
			if(sensor[i].serialID != 0){
				if((threshold + sensor[i].baseline) > 4096 
						&& sensor[i].baseline < threshold){
					printf("Invalid threshold value:\n");
					printf("threshold + baseline must not exceed 4096\nand\n");
					printf("threshold must be smaller than baseline value.\n");
				} else {
					sensor[i].threshold	= threshold;
					cmd_send_config_to_sensor(i);
					logger.config_modified = 1; 
				} // fi
			} // fi
		} // rof
	} else if((threshold + sensor[sensor_index].baseline) > 4096 
							&& sensor[sensor_index].baseline < threshold){
		printf("Invalid threshold value:\n");
		printf("threshold + baseline must not exceed 4096\nand\n");
		printf("threshold must be smaller than baseline value.\n");
	} else {
		if(sensor[sensor_index].threshold != threshold){
			logger.config_modified = 1;
		}
		sensor[sensor_index].threshold = threshold;
		cmd_send_config_to_sensor(sensor_index);
	} // fi
}

/*
 * See header file
 */
void cmd_enter_sensor_params_baseline(void)
{
	printf("entering sensor params baseline \n");
	printf(" #) Enter baseline value (default: 2047).\n");
	printf("baseline + threshold must not exceed 4096\nand\n");

	printf("baseline - threshold must not be below 0\n");
	printf(" 0) cancel\n");
}

/*
 * See header file
 */
void cmd_set_baseline(uint8_t sensor_index, uint32_t baseline)
{
		int i;
	// baseline must be larger than threshold
	// baseline + threshold must be smaller than 4096
	if(sensor_index == 99) {
		// set all sensors
		for(i = 0; i < MAX_SENSORS; i++){
			if(sensor[i].serialID != 0){
				if((baseline + sensor[i].threshold) > 4096 
						&& sensor[i].threshold < baseline){
					printf("Invalid baseline value:\n");
					printf("threshold + baseline must not exceed 4096\nand\n");
					printf("threshold must be smaller than baseline value.\n");
				} else {
					sensor[i].baseline	= baseline;
					cmd_send_config_to_sensor(i);
					logger.config_modified = 1;
				} // fi
			} // fi
		} // rof
	} else if((baseline + sensor[sensor_index].threshold) > 4096 
							&& sensor[sensor_index].threshold < baseline){
		printf("Invalid baseline value:\n");
		printf("threshold + baseline must not exceed 4096\nand\n");
		printf("threshold must be smaller than baseline value.\n");
	} else {
		if(sensor[sensor_index].baseline != baseline){
			logger.config_modified = 1;
		}
		sensor[sensor_index].baseline = baseline;
		cmd_send_config_to_sensor(sensor_index);
	} // fi
}

/*
 * See header file
 */
void cmd_enter_sensor_params_timeout(void)
{
	printf("entering sensor params timeout \n");
	printf(" #) Enter timeout in samples.\n");
	printf(" 0) cancel\n");
}

/*
 * See header file
 */
void cmd_set_timeout(uint8_t sensor_index, uint32_t timeout)
{
	int i;
	// timeout must be less than 256.
	if(timeout > 255){
		printf("Timeout too long, can not exceed %d.\n", 255);
	} else {
		// warning for short timeout
		if (timeout == 0){
			printf("Timeout 0 will end impact after each peak.\n");
			printf("Timeout 0 in effect.\n");
		} 
		if(sensor_index == 99) {
			// set all sensors
			for(i = 0; i < MAX_SENSORS; i++){
				if(sensor[i].serialID != 0){
					sensor[i].timeout = timeout;
					cmd_send_config_to_sensor(i);
					logger.config_modified = 1;
				} // fi
			} // rof
		} else {
			if(sensor[sensor_index].timeout != timeout){
				logger.config_modified = 1;
			}
			sensor[sensor_index].timeout = timeout;
			cmd_send_config_to_sensor(sensor_index);
		} // fi
	} // fi
}

/*
 * See header file
 */
void cmd_enter_sensor_params_detail(void)
{
	printf("entering sensor params detail level \n");
	printf(" 1) off\n");
	printf(" 2) sparse (only start time, duration, nr of peaks, max peak)\n");
	printf(" 3) peaks only (start time, nr of peaks, peaks\n");
	printf(" 4) detailed (start time, all samples of impact)\n");
	printf(" 5) raw (continuous data)\n");
	printf(" 0) cancel\n");
}

/*
 * See header file
 */
cmd_enter_sensor_params_raw()
{
    printf("please enter duration of raw recording in seconds (max. 3600, 0 to cancel): \n");
}

/*
 * See header file
 */
void cmd_set_detail_mode(uint8_t sensor_index, uint8_t mode)
{
	int i;
	if(sensor_index == 99) {
		// set all sensors
		for(i = 0; i < MAX_SENSORS; i++){
			if(sensor[i].serialID != 0){
				sensor[i].detail_level = (detail_mode_t) mode;
				cmd_send_config_to_sensor(i);
				logger.config_modified = 1;
			} // fi
		} // rof
	} else {
		if(sensor[sensor_index].detail_level != mode){
			logger.config_modified = 1;
		}
		sensor[sensor_index].detail_level = (detail_mode_t) mode;
		cmd_send_config_to_sensor(sensor_index);
	} // fi
}

/*
 * See header file
 */
cmd_set_detail_mode_raw(uint8_t sensor_index, uint32_t sec)
{
    if(sensor[sensor_index].detail_level != M_RAW){
        logger.config_modified = 1;
    }
    sensor[sensor_index].detail_level = (detail_mode_t) mode;
    // TODO: bushandler muss das entgegennehmen
    start_sensor_raw(sensor_index, sec * sensor[sensor_index].fs);
}

/*
 * See header file
 */
void cmd_enter_sensor_start_stop(void)
{
	int i;
	int active = 0;
	int inactive = 0;
	printf("entering sensor params start/stop \n");
	if(menu_fsm_current_sensor == 99){
		for(i = 0; i < MAX_SENSORS; i++){
			if(sensor[i].started){
				active++;
			} else {
				inactive++;
			}//fi
		} // rof
		printf("Started sensors: %d\n", active);
		printf("Stopped sensors: %d\n", inactive);
	} else {
		printf("Selected sensor is currently %s.\n",
		(sensor[menu_fsm_current_sensor].started ? "started" : "stopped"));
	} // fi
	printf(" 1) start\n");
	printf(" 2) stop\n");
	printf(" 0) cancel\n");
}

/*
 * See header file
 */
void cmd_sensor_start(uint8_t sensor_index)
{
	int i;
	if(sensor_index == 99) {
		// set all sensors
		for(i = 0; i < MAX_SENSORS; i++){
			if(sensor[i].serialID != 0){
				if(!cmd_open_sensor_file(i)){
					sensor[i].started = 1;
					cmd_send_config_to_sensor(i);
					logger.config_modified = 1;
				} else {
					printf("Could not create or open file. Please check SD card for free space.\n");
				}
			} // fi
		} // rof
	} else {
		// open file, store filepointer
		if(!cmd_open_sensor_file(sensor_index)){
			if(sensor[sensor_index].started == 0){
				logger.config_modified = 1;
			}
			sensor[sensor_index].started = 1;
			cmd_send_config_to_sensor(sensor_index);
		} else {
			printf("Could not create or open file. Please check SD card for free space.\n");
		}
	} // fi
}

/*
 * See header file
 */
void cmd_sensor_stop(uint8_t sensor_index)
{
	int i;
	if(sensor_index == 99) {
		// set all sensors
		for(i = 0; i < MAX_SENSORS; i++){
			if(sensor[i].serialID != 0){
				sensor[i].started = 0;
				cmd_close_sensor_file(i);
				cmd_send_config_to_sensor(i);
				logger.config_modified = 1;
			} // fi
		} // rof
	} else {
		if(sensor[sensor_index].started == 1){
			logger.config_modified = 1;
		}
		sensor[sensor_index].started = 0;
		if(sensor[sensor_index].pf_sensor_data != NULL){
			cmd_close_sensor_file(sensor_index);
		}
		cmd_send_config_to_sensor(sensor_index);
	} // fi
}

/*
 * See header file
 */
void cmd_send_config_to_sensor(uint8_t index)
{
	SensorConfigMsg_t cfg;
	
	cfg.threshold = sensor[index].threshold;
	cfg.baseline = sensor[index].baseline;
	cfg.fs = sensor[index].fs;
	cfg.timeoutRange = sensor[index].timeout;
	uint16_t started = sensor[index].detail_level;
	started = started << 4;
	started |= sensor[index].started & 0x0f;
	cfg.started = started;
	sendSettings(sensor[index].sensor_ID, cfg);
}

/*
 * See header file
 */
void cmd_enter_sensor_state(void)
{
	printf("Listing sensor config\n");
	cmd_list_sensor_states();
	printf("\n 0) continue\n");
}

/*
 * See header file
 */
void cmd_list_sensor_states(void)
{
	uint8_t i;
	SensorConfig *s = sensor;
	printf("Listing sensor config\n");
	// print the list of sensor configurations, but only if there 
	printf("Nr  SID  serial      fs threshold baseline timeout detail\n");
	for(i = 0; i < MAX_SENSORS; i++){
		if(s->serialID != 0){
			printf("%2d) %2d  %08x %5d      %4d     %4d    %4d %s\t%s\n", 
				i+2, s->sensor_ID, s->serialID, s->fs, s->threshold, 
				s->baseline, s->timeout, 
				detail_str[(uint8_t)s->detail_level], s->started ? "started" : "stopped");
		}
		s++;
	} 
}

/*
 * See header file
 */
void cmd_enter_reset_timestamp(void)
{
	printf("entering timestamp reset\n");
	printf(" 1) re-synchronize timestamp\n");
	printf(" 0) cancel\n");
}

void cmd_reset_timestamp(void)
{
	// TODO call function to resync timestamp!
	// TODO reset timestamp in impact_fsm
	// 
	resetTimestamp();
	printf("timestamp has been reset.\n");
}

/*
 * See header file
 */
void cmd_enter_internal_clock(void)
{
	time_t seconds = time(NULL);
	printf("entering internal clock\n");
	printf(" 1) adjust date\n");
	printf(" 2) adjust time\n");
	printf(" 0) exit\n");
	printf("\n current time: %s\n", ctime(&seconds));
}

/*
 * See header file
 */
void cmd_enter_internal_clock_set_date(void)
{
	time_t seconds = time(NULL);
	printf("adjust internal date\n");
	printf(" 1) adjust date +365 days\n");
	printf(" 2) adjust date -365 days\n");
	printf(" 3) adjust date + 30 days\n");
	printf(" 4) adjust date - 30 days\n");
	printf(" 5) adjust date + 10 days\n");
	printf(" 6) adjust date - 10 days\n");
	printf(" 7) adjust date +  1 day\n");
	printf(" 8) adjust date -  1 day\n");
	printf(" 0) exit\n");
	printf("\n current time: %s\n", ctime(&seconds));
}

/*
 * See header file
 */
void cmd_enter_internal_clock_set_time(void)
{
	time_t seconds = time(NULL);
	printf("adjust internal time\n");
	printf(" 1) adjust time +1 hour\n");
	printf(" 2) adjust time -1 hour\n");
	printf(" 3) adjust time +10 minute\n");
	printf(" 4) adjust time -10 minute\n");
	printf(" 5) adjust time +1 minute\n");
	printf(" 6) adjust time -1 minute\n");
	printf(" 7) adjust time +1 second\n");
	printf(" 8) adjust time -1 second\n");
	printf(" 0) exit\n");
	printf("\n current time: %s\n", ctime(&seconds));
}

/*
 * See header file
 */
void cmd_internal_clock_inc_yr(void)
{
	time_t seconds = time(NULL) + 365*24*3600;
	set_time(seconds);
	time_updated = 1;
}

/*
 * See header file
 */
void cmd_internal_clock_dec_yr(void)
{
	time_t seconds = time(NULL) - 365*24*3600;
	set_time(seconds);
	time_updated = 1;
}

/*
 * See header file
 */
void cmd_internal_clock_inc_mnt(void)
{
	time_t seconds = time(NULL) + 30*24*3600;
	set_time(seconds);
	time_updated = 1;
}

/*
 * See header file
 */
void cmd_internal_clock_dec_mnt(void)
{
	time_t seconds = time(NULL) - 30*24*3600;
	set_time(seconds);
	time_updated = 1;
}

/*
 * See header file
 */
void cmd_internal_clock_inc_10day(void)
{
	time_t seconds = time(NULL) + 240*3600;
	set_time(seconds);
	time_updated = 1;
}

/*
 * See header file
 */
void cmd_internal_clock_dec_10day(void)
{
	time_t seconds = time(NULL) - 240*3600;
	set_time(seconds);
	time_updated = 1;
}

/*
 * See header file
 */
void cmd_internal_clock_inc_day(void)
{
	time_t seconds = time(NULL) + 24*3600;
	set_time(seconds);
	time_updated = 1;
}

/*
 * See header file
 */
void cmd_internal_clock_dec_day(void)
{
	time_t seconds = time(NULL) - 24*3600;
	set_time(seconds);
	time_updated = 1;
}

/*
 * See header file
 */
void cmd_internal_clock_inc_hr(void)
{
	time_t seconds = time(NULL) + 3600;
	set_time(seconds);
	time_updated = 1;
}

/*
 * See header file
 */
void cmd_internal_clock_dec_hr(void)
{
	time_t seconds = time(NULL) - 3600;
	set_time(seconds);
	time_updated = 1;
}

/*
 * See header file
 */
void cmd_internal_clock_inc_10min(void)
{
	time_t seconds = time(NULL) + 600;
	set_time(seconds);
	time_updated = 1;
}

/*
 * See header file
 */
void cmd_internal_clock_dec_10min(void)
{
	time_t seconds = time(NULL) - 600;
	set_time(seconds);
	time_updated = 1;
}

/*
 * See header file
 */
void cmd_internal_clock_inc_min(void)
{
	time_t seconds = time(NULL) + 60;
	set_time(seconds);
	time_updated = 1;
}

/*
 * See header file
 */
void cmd_internal_clock_dec_min(void)
{
	time_t seconds = time(NULL) - 60;
	set_time(seconds);
	time_updated = 1;
}

/*
 * See header file
 */
void cmd_internal_clock_inc_sec(void)
{
	time_t seconds = time(NULL) + 1;
	set_time(seconds);
	time_updated = 1;
}

/*
 * See header file
 */
void cmd_internal_clock_dec_sec(void)
{
	time_t seconds = time(NULL) - 1;
	set_time(seconds);
	time_updated = 1;
}

/*
 * See header file
 */
void cmd_enter_config_file(void)
{
	printf("entering config menu\n");
	printf(" 1) read configuration from file and set up all sensors accordingly.\n");
	printf(" 2) store current configuration in file. Old config file will be overwritten.\n");
	printf(" 0) cancel\n");
}

/*
 * See header file
 */
void cmd_store_config_file(void)
{
	uint8_t error = 0;
	FILE *fp = NULL;
	
	// open config file (overwrite)
	fp = fopen("/mci/config.txt", "w");
	if(fp != NULL){
		// store configuration in file
		error = sensor_config_to_file(fp, sensor);
	}
	if(!error){
		logger.config_modified = 0;
	} else {
		printf("The config file could not be written. Please check the SD card in a computer.\n");
		printf("The configuration data will remain stored in the logger unless you turn off power.\n");
	}
}

/*
 * See header file
 */
void cmd_read_config_file(void)
{
	FILE *fp = NULL;
	
	// open config file (overwrite)
	fp = fopen("/mci/config.txt", "r");
	if(fp != NULL){
		sensor_config_read_file(fp, sensor);
	}
	// TODO set all sensors according config file
	// for every entry in config file, look for the corresponding sensor in array 'foundsensors'
	
	// if there remain sensors in foundsensors, look for free slots in config array and put them there
	// with default configs
	fclose(fp);
}

/*
 * See header file
 */
uint8_t cmd_open_sensor_file(uint8_t sensor_index)
{
	char fname[13];
  char datetime[5];
	const char format[10] = "%m%d_%H%M";
	time_t currTime;
	struct tm * timeinfo;
	
	// prepare time and format for creating timestamped filename
	fname[0] = NULL;
	datetime[0] = NULL;
	time(&currTime);
	timeinfo = localtime (&currTime);
	
	// open file, store filepointer, store filename
	// TODO check if this works
	strftime(datetime, 4, format, timeinfo);
	sprintf(fname, "/mci/s%02d_%s", sensor[sensor_index].sensor_ID, datetime);
	printf("created filename %s\n",fname);
	sensor[sensor_index].pf_sensor_data = fopen(fname, "a");

	if(sensor[sensor_index].pf_sensor_data != NULL){
		char writtenBytes = fprintf(sensor[sensor_index].pf_sensor_data,"%s\n",datetime);
		if(writtenBytes<0){
			printf("failed first write: %x to %s done\n",writtenBytes,fname);
		} else {
			printf("first write success: %x to %s done\n",writtenBytes,fname);
		}
		fflush(sensor[sensor_index].pf_sensor_data);
		/*
		fclose(sensor[sensor_index].pf_sensor_data);
		sensor[sensor_index].pf_sensor_data = fopen(fname, "a");*/
		strncpy(sensor[sensor_index].filename, fname, 25);
		return 1;
	} else {
		return 0;
	}
}


/*
 * See header file
 */
void cmd_close_sensor_file(uint8_t sensor_index)
{
	// close file, set filepointer to NULL, set filename to ""
	if(sensor[sensor_index].pf_sensor_data != NULL){
		fclose(sensor[sensor_index].pf_sensor_data);
		sensor[sensor_index].pf_sensor_data = NULL;
		strncpy(sensor[sensor_index].filename, "", 1);
	}
}

