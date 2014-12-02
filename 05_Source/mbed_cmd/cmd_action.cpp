#ifndef MBED_H
#include "mbed.h"
#endif

#include "Serial.h"
#include "cmd_action.h"


static char detail_str[6][11] = 
{
	// strings to describe the detail levels
	"raw",
	"detailed",
	"peaks only",
	"sparse",
	"off",
	""
};

static char cmd_config_modified = 0;

/*
 * See header file
 */
void cmd_enter_basemenu(void)
{
	printf("entering basemenu\n");
	printf(" 1) list files\n");
	printf(" 2) format sd card\n");
	printf(" 3) mount sd card\n");
	printf(" 4) unmount sd card\n");
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
	printf(" #) select any file to be deleted.\n");
	printf(" 0) exit\n");
}

/*
 * See header file
 */
void cmd_enter_delete_file(void)
{
	printf("entering delete_file\n");
	printf(" 1) confirm deletion of file %s\n", "TODO");
	printf(" 0) cancel\n");
}

/*
 * See header file
 */
void cmd_enter_format_sd(void)
{
	printf("entering format sd\n");
	printf(" 1) confirm formatting of SD card. \n    All data will be erased\n"); // TODO
	printf(" 0) cancel\n");
}

/*
 * See header file
 */
void cmd_mount_sd(void)
{
	printf("entering mount sd\n");
	printf(" 1) \n"); // TODO open file system
	printf(" 0) cancel\n");
}

/*
 * See header file
 */
void cmd_unmount_sd(void)
{
	printf("entering unmount sd\n");
	printf(" 1) \n"); // TODO check for open files, close them, close file system
	printf(" 0) cancel\n");
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
	printf("entering logger start / stop\n");
	printf(" 1) start or stop the logging.\n"); // TODO
	printf(" 0) cancel\n");
}

/*
 * See header file
 */
void cmd_enter_sensor_params(void)
{
	printf("entering sensor params\n");
	printf(" 1) set sampling rate (current: %5.0d Hz)\n", 10999); // TODO retrieve current value
	printf(" 2) set threshold value (current: %d5.0)\n", 200); // TODO
	printf(" 3) set baseline value (current: %d5.0)\n", 2999); // TODO
	printf(" 4) set timeout (current: %5.0d)\n", 444); // TODO
	printf(" 5) set detail level (current: %s)\n", detail_str[5-1]); // TODO
	printf(" 0) exit\n");
}

/*
 * See header file
 */
void cmd_enter_sensor_params_get_nr(void)
{
	printf("entering sensor params get nr\n");
	printf(" #) Select a sensor from the list.\n");
	printf(" 0) cancel\n");
}

/*
 * See header file
 */
char cmd_sensor_id_is_valid(char sensor_id)
{
	return 0; // TODO
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
void cmd_enter_sensor_params_thres(void)
{
	printf("entering sensor params thres \n");
	printf(" #) Enter threshold value.\n");
	printf(" 0) cancel\n");
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
void cmd_enter_sensor_params_timeout(void)
{
	printf("entering sensor params timeout \n");
	printf(" #) Enter timeout in samples.\n");
	printf(" 0) cancel\n");
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
void cmd_enter_sensor_state(void)
{
	printf("entering sensor state\n");
	printf(" 0) exit\n");
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
	
}

/*
 * See header file
 */
void cmd_read_config_file(void)
{
	// TODO read configuration from file
	// TODO set all sensors according config file
	
}
