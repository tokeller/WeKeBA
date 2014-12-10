#ifndef MBED_H
#include "mbed.h"
#endif

#include "Serial.h"
#include "MCIFileSystem.h"
#include "sensor_config.h"
#include "file_ops.h"
#include "cmd_action.h"

extern SensorConfig sensor[MAX_SENSORS];
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

static uint8_t cmd_config_modified = 0;

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
	printf(" 8) sensor state\n");
	printf(" 9) reset timestamp\n");
	printf("10) internal clock\n");
	printf("11) config file\n");
	printf("\n");
	printf("TODO hier noch Meldung einbauen, dass man das config-file speichern sollte\n");
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
		printf("formatting done\n");
	}
	printf("Returning to base menu.\n");
}

/*
 * See header file
 */
void cmd_mount_sd(void)
{
	printf("entering mount SD card\n");
	printf(" 1) \n"); // TODO open file system
	printf(" 0) cancel\n");
}

/*
 * See header file
 */
void cmd_enter_unmount_sd(void)
{
	printf("entering unmount sd\n");
	printf(" 1) unmount SD card\n    This will stop logging and close all data files."); // TODO check for open files, close them, close file system
	printf(" 0) cancel\n");
}

void cmd_unmount_sd(void)
{
	int i;
	printf("Unmounting SD card: stop logging and close all data files.\n");
	// TODO stop logging
	// TODO send BC to all sensors to go 'offline' but do not set
	// the started flag to 0 in the config. This way, we will restart only
	// those sensors that were running before the stop.
	
	printf("Logging has been stopped.\n");
	
	// TODO close all data files
	for(i = 0; i < MAX_SENSORS; i++){
		// TODO close file, set file pointer to zero
	}
	printf("All files have been closed.\n");
	
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
	// TODO: ist logger am laufen oder nicht?
	printf("Logger is running/stopped");
	printf(" 1) stop/start the logging.\n"); // TODO
	printf(" 0) cancel\n");
}

/*
 * See header file
 */
void cmd_enter_sensor_params(void)
{
	printf("entering sensor params\n");
	printf(" 1) set sampling rate (current: %3.0d00 Hz)\n", 
		sensor[menu_fsm_current_sensor].fs); // TODO retrieve current value
	printf(" 2) set threshold value (current: %d5.0)\n", 
		sensor[menu_fsm_current_sensor].threshold); // TODO
	printf(" 3) set baseline value (current: %d5.0)\n", 
		sensor[menu_fsm_current_sensor].baseline); // TODO
	printf(" 4) set timeout (current: %5.0d)\n", 
		sensor[menu_fsm_current_sensor].timeout); // TODO
	printf(" 5) set detail level (current: %s)\n", 
		detail_str[sensor[menu_fsm_current_sensor].detail_level]); // TODO
	printf(" 6) start or stop recording (current: %s)\n",
	(sensor[menu_fsm_current_sensor].started ? "started" : "stopped"));
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
	if(sensor_index == 99 || (sensor_index < MAX_SENSORS && sensor[sensor_index].serialID != 0) ){
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
	printf(" #) Enter sampling rate in Hz. (multiple of 100 Hz)\n");
	printf(" 0) cancel\n");
}

/*
 * See header file
 */
void cmd_set_sampling_freq(uint8_t sensor_index, uint32_t fs)
{
	int i;
	// fs must be between 1 and 2000 (100..200'000 Hz). The CPU can't handle faster sampling.
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
			} // fi
		} // rof
	} else {
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
				} // fi
			} // fi
		} // rof
	} else if((threshold + sensor[sensor_index].baseline) > 4096 
							&& sensor[sensor_index].baseline < threshold){
		printf("Invalid threshold value:\n");
		printf("threshold + baseline must not exceed 4096\nand\n");
		printf("threshold must be smaller than baseline value.\n");
	} else {
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
				} // fi
			} // fi
		} // rof
	} else if((baseline + sensor[sensor_index].threshold) > 4096 
							&& sensor[sensor_index].threshold < baseline){
		printf("Invalid baseline value:\n");
		printf("threshold + baseline must not exceed 4096\nand\n");
		printf("threshold must be smaller than baseline value.\n");
	} else {
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
	// timeout must be shorter than input_queue_length.
	if(timeout > MAX_INPUT_LENGTH){
		printf("Timeout too long, can not exceed %d\n", MAX_INPUT_LENGTH);
	} else if (timeout == 0){
		printf("Timeout 0 will end impact after each peak.\n");
		printf("If you really want this, enter timeout of 1.\n");
	} else if(sensor_index == 99) {
		// set all sensors
		for(i = 0; i < MAX_SENSORS; i++){
			if(sensor[i].serialID != 0){
				sensor[i].timeout = timeout;
				cmd_send_config_to_sensor(i);
			} // fi
		} // rof
	} else {
		sensor[sensor_index].timeout = timeout;
		cmd_send_config_to_sensor(sensor_index);
	} // fi
}

/*
 * See header file
 */
void cmd_enter_sensor_params_detail(void)
{
	printf("entering sensor params detail level \n");
	printf(" 1) raw (continuous data)\n");
	printf(" 2) detailed (start time, all samples of impact)\n");
	printf(" 3) peaks only (start time, nr of peaks, peaks\n");
	printf(" 4) sparse (only start time, duration, nr of peaks, max peak)\n");
	printf(" 5) off\n");
	printf(" 0) cancel\n");
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
			} // fi
		} // rof
	} else {
		sensor[sensor_index].detail_level = (detail_mode_t) mode;
		cmd_send_config_to_sensor(sensor_index);
	} // fi
}

/*
 * See header file
 */
void cmd_enter_sensor_start_stop(void)
{
	printf("entering sensor params start/stop \n");
	printf("selected sensor is currently %s.\n",
		(sensor[menu_fsm_current_sensor].started ? "started" : "stopped"));
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
				// TODO open file,set filepointer
				sensor[i].started = 1;
				cmd_send_config_to_sensor(i);
			} // fi
		} // rof
	} else {
		// TODO open file, set file pointer
		sensor[sensor_index].started = 1;
		cmd_send_config_to_sensor(sensor_index);
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
				// TODO flush all DATA of this sensor to file
				// TODO close file, set file pointer to NULL
				cmd_send_config_to_sensor(i);
			} // fi
		} // rof
	} else {
		sensor[sensor_index].started = 0;
		// TODO flush all DATA of this sensor to file
		// TODO close file, set file pointer to NULL
		cmd_send_config_to_sensor(sensor_index);
	} // fi
}

/*
 * See header file
 */
void cmd_send_config_to_sensor(uint8_t sensor_index)
{
	// TODO send config of sensor[sensor_index] to sensor
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
			printf("%2d) %2d  %08x %5d      %4d     %4d    %4d %s\n", 
				i, s->sensor_ID, s->serialID, s->fs, s->threshold, 
				s->baseline, s->timeout, 
				detail_str[(uint8_t)s->detail_level]);
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
	// TODO print current timestamp
	printf(" 1) re-synchronize timestamp\n");
	printf(" 0) cancel\n");
}

void cmd_reset_timestamp(void)
{
	// TODO call function to resync timestamp!
	// TODO reset timestamp in impact_fsm
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
	printf("\n\ncurrent time: %s\n", ctime(&seconds));
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
	printf("\ncurrent time: %s\n", ctime(&seconds));
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
	printf("\ncurrent time: %s\n", ctime(&seconds));
}

/*
 * See header file
 */
void cmd_internal_clock_inc_yr(void)
{
	time_t seconds = time(NULL) + 365*24*3600;
	set_time(seconds);
}

/*
 * See header file
 */
void cmd_internal_clock_dec_yr(void)
{
	time_t seconds = time(NULL) + 365*24*3600;
	set_time(seconds);
}

/*
 * See header file
 */
void cmd_internal_clock_inc_mnt(void)
{
	time_t seconds = time(NULL) + 30*24*3600;
	set_time(seconds);
}

/*
 * See header file
 */
void cmd_internal_clock_dec_mnt(void)
{
	time_t seconds = time(NULL) - 30*24*3600;
	set_time(seconds);
}

/*
 * See header file
 */
void cmd_internal_clock_inc_10day(void)
{
	time_t seconds = time(NULL) + 240*3600;
	set_time(seconds);
}

/*
 * See header file
 */
void cmd_internal_clock_dec_10day(void)
{
	time_t seconds = time(NULL) - 240*3600;
	set_time(seconds);
}

/*
 * See header file
 */
void cmd_internal_clock_inc_day(void)
{
	time_t seconds = time(NULL) + 24*3600;
	set_time(seconds);
}

/*
 * See header file
 */
void cmd_internal_clock_dec_day(void)
{
	time_t seconds = time(NULL) - 24*3600;
	set_time(seconds);
}

/*
 * See header file
 */
void cmd_internal_clock_inc_hr(void)
{
	time_t seconds = time(NULL) + 3600;
	set_time(seconds);
}

/*
 * See header file
 */
void cmd_internal_clock_dec_hr(void)
{
	time_t seconds = time(NULL) - 3600;
	set_time(seconds);
}

/*
 * See header file
 */
void cmd_internal_clock_inc_10min(void)
{
	time_t seconds = time(NULL) + 600;
	set_time(seconds);
}

/*
 * See header file
 */
void cmd_internal_clock_dec_10min(void)
{
	time_t seconds = time(NULL) - 600;
	set_time(seconds);
}

/*
 * See header file
 */
void cmd_internal_clock_inc_min(void)
{
	time_t seconds = time(NULL) + 60;
	set_time(seconds);
}

/*
 * See header file
 */
void cmd_internal_clock_dec_min(void)
{
	time_t seconds = time(NULL) - 60;
	set_time(seconds);
}

/*
 * See header file
 */
void cmd_internal_clock_inc_sec(void)
{
	time_t seconds = time(NULL) + 1;
	set_time(seconds);
}

/*
 * See header file
 */
void cmd_internal_clock_dec_sec(void)
{
	time_t seconds = time(NULL) - 1;
	set_time(seconds);
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
	// TODO store configuration in file (overwrite)
	uint8_t buffer[80];
	SensorConfig sc;
	int result;
	
	sensor_config_init(&sc, 1);
	
	result = sensor_config_to_str(&sc, buffer);
	
	FILE *fp = fopen("/mci/config.txt", "w+");
	if (fp != NULL) {
		fprintf(fp, "%s", buffer);
		fclose(fp);
	} else {
		printf("file could not be openend\n");
	}
}

/*
 * See header file
 */
void cmd_read_config_file(void)
{
	// TODO read configuration from file
	// TODO set all sensors according config file
	
}
