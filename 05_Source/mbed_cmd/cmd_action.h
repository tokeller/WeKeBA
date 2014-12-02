#include <stdint.h>

/**
 * Print the base menu
 *   @param   none
 *   @retval  none
 */
 void cmd_enter_basemenu(void);

/**
 * Print the list of files
 *   @param   none
 *   @retval  none
 */
void cmd_enter_list_files(void);

/**
 * Print confirmation message
 *   @param   none
 *   @retval  none
 */
void cmd_enter_delete_file(void);

/**
 * Format SD card and print result
 *   @param   none
 *   @retval  none
 */
void cmd_enter_format_sd(void);

/**
 * mount SD card and print result
 *   @param   none
 *   @retval  none
 */
void cmd_mount_sd(void);

/**
 * unmount SD card and print result
 *   @param   none
 *   @retval  none
 */
void cmd_unmount_sd(void);

/**
 * Print logger status
 *   @param   none
 *   @retval  none
 */
void cmd_print_logger_status(void);

/**
 * Print menu for starting or stopping the logger
 *   @param   none
 *   @retval  none
 */
void cmd_enter_logger_start(void);

/**
 * Print menu for setting sensor parameters
 *   @param   none
 *   @retval  none
 */
void cmd_enter_sensor_params(void);

/**
 * Print message for selecting a sensor
 *   @param   none
 *   @retval  none
 */
void cmd_enter_sensor_params_get_nr(void);

/**
 * Print message for setting timeout
 *   @param   sensor_id: the sensor to be configured
 *   @retval  none
 */
char cmd_sensor_id_is_valid(char sensor_id);

/**
 * Print message for setting sampling rate
 *   @param   none
 *   @retval  none
 */
void cmd_enter_sensor_params_fs(void);

/**
 * Print message for setting timeout
 *   @param   sensor_id: the sensor to be configured
 *   @param   fs: sampling rate in Hz, will be cutoff to next lower 100 Hz
 *   @retval  none
 */
void cmd_set_sampling_freq(char sensor_id, uint32_t fs);

/**
 * Print message for setting threshold value
 *   @param   none
 *   @retval  none
 */
void cmd_enter_sensor_params_thres(void);

/**
 * Print message for setting timeout
 *   @param   sensor_id: the sensor to be configured
 *   @param   threshold: how strong must signal be to be a peak
 *   @retval  none
 */
void cmd_set_threshold(char sensor_id, uint32_t baseline);

/**
 * Print message for setting baseline value
 *   @param   none
 *   @retval  none
 */
void cmd_enter_sensor_params_baseline(void);

/**
 * Print message for setting timeout
 *   @param   sensor_id: the sensor to be configured
 *   @param   baseline: the center value for the g sensor
 *   @retval  none
 */
void cmd_set_baseline(char sensor_id, uint32_t baseline);

/**
 * Print message for setting timeout
 *   @param   none
 *   @retval  none
 */
void cmd_enter_sensor_params_timeout(void);

/**
 * Print message for setting timeout
 *   @param   sensor_id: the sensor to be configured
 *   @param   timeout: how many samples it takes until timeout
 *   @retval  none
 */
void cmd_set_timeout(char sensor_id, uint32_t timeout);

/**
 * Print message for setting detail level
 *   @param   none
 *   @retval  none
 */
void cmd_enter_sensor_params_detail(void);

/**
 * Set the requested detailed mode in the selected sensor
 *   @param   sensor_id: the sensor to be configured
 *   @param   mode: the mode to be set
 *   @retval  none
 */
void cmd_set_detail_mode(char sensor_id, char mode);

/**
 * Print list of sensors and their state
 *   @param   none
 *   @retval  none
 */
void cmd_enter_sensor_state(void);

/**
 * Print message for confirmation of timestamp reset
 *   @param   none
 *   @retval  none
 */
void cmd_enter_reset_timestamp(void);

/**
 * Reset the timestamp to zero in all connected devices
 *   @param   none
 *   @retval  none
 */
void cmd_reset_timestamp(void);

/**
 * Print menu for adjusting the internal clock
 *   @param   none
 *   @retval  none
 */
void cmd_enter_internal_clock(void);

/**
 * Print menu for adjusting the date
 *   @param   none
 *   @retval  none
 */
void cmd_enter_internal_clock_set_date(void);

/**
 * Print menu for adjusting the time
 *   @param   none
 *   @retval  none
 */
void cmd_enter_internal_clock_set_time(void);

/**
 * Increase internal clock by 365 days
 *   @param   none
 *   @retval  none
 */
void cmd_internal_clock_inc_yr(void);

/**
 * Decrease internal clock by 365 days
 *   @param   none
 *   @retval  none
 */
void cmd_internal_clock_dec_yr(void);

/**
 * Increase internal clock by 30 days
 *   @param   none
 *   @retval  none
 */
void cmd_internal_clock_inc_mnt(void);

/**
 * Decrease internal clock by 30 days
 *   @param   none
 *   @retval  none
 */
void cmd_internal_clock_dec_mnt(void);

/**
 * Increase internal clock by 10 day
 *   @param   none
 *   @retval  none
 */
void cmd_internal_clock_inc_10day(void);

/**
 * Decrease internal clock by 10 day
 *   @param   none
 *   @retval  none
 */
void cmd_internal_clock_dec_10day(void);

/**
 * Increase internal clock by 1 day
 *   @param   none
 *   @retval  none
 */
void cmd_internal_clock_inc_day(void);

/**
 * Decrease internal clock by 1 day
 *   @param   none
 *   @retval  none
 */
void cmd_internal_clock_dec_day(void);

/**
 * Increase internal clock by 1 hour
 *   @param   none
 *   @retval  none
 */
void cmd_internal_clock_inc_hr(void);

/**
 * Decrease internal clock by 1 hour
 *   @param   none
 *   @retval  none
 */
void cmd_internal_clock_dec_hr(void);

/**
 * Increase internal clock by 10 minutes
 *   @param   none
 *   @retval  none
 */
void cmd_internal_clock_inc_10min(void);

/**
 * Decrease internal clock by 10 minutes
 *   @param   none
 *   @retval  none
 */
void cmd_internal_clock_dec_10min(void);

/**
 * Increase internal clock by 1 minute
 *   @param   none
 *   @retval  none
 */
void cmd_internal_clock_inc_min(void);

/**
 * Decrease internal clock by 1 minute
 *   @param   none
 *   @retval  none
 */
void cmd_internal_clock_dec_min(void);

/**
 * Increase internal clock by 1 sec
 *   @param   none
 *   @retval  none
 */
void cmd_internal_clock_inc_sec(void);

/**
 * Decrease internal clock by 1 second 
 *   @param   none
 *   @retval  none
 */
void cmd_internal_clock_dec_sec(void);

/**
 * Print menu for handling config file
 *   @param   none
 *   @retval  none
 */
void cmd_enter_config_file(void);

/**
 * Store config file to SD card
 *   @param   none
 *   @retval  none
 */
void cmd_store_config_file(void);

/**
 * Read config file from SD card
 *   @param   none
 *   @retval  none
 */
void cmd_read_config_file(void);