#ifndef SENSOR_CONFIG_H
#define SENSOR_CONFIG_H

#ifndef MBED_H
#include "mbed.h"
#endif
#include "stdint.h"


/* ------------------------------------------------------------------
 * -- Constants
 * --------------------------------------------------------------- */

#define MAX_SENSORS 20

/* ------------------------------------------------------------------
 * -- Type definitions
 * --------------------------------------------------------------- */
 
	typedef enum detail_mode{
		M_OFF = 0,
		M_SPARSE,
		M_PEAKS,
		M_DETAILED,
		M_RAW
	} detail_mode_t;
	
	
	typedef struct sensor_config{
	  // identification
		uint32_t serialID;      // received from sensor
		uint8_t sensor_ID;      // ID number for this sensor in logger
		
		// operating parameters
		uint16_t fs;            // sampling frequency in 100 Hz 
		uint16_t threshold;    // input level above or below baseline
		uint16_t baseline;     // baseline level to compensate for 
		                       // orientational deviations
		uint16_t timeout;      // how many samples below threshold will
		                       // terminate an impact
		detail_mode_t detail_level;  // operation mode: 'raw','detailed'..'off'
		
		// file pointer
		FILE *pf_sensor_data;  // data file pointer
		
	} SensorConfig;
/* ------------------------------------------------------------------
 * -- Prototypes
 * --------------------------------------------------------------- */

 /**
 * initialize new sensor_config
 *   @param   SensorConfig *sc : pointer to the SensorConfig to init
 *   @param   uint8_t id       : id of that sensor.
 *   @retval  none
 */
void sensor_config_init(SensorConfig *sc, uint8_t id);

	
 /**
 * Write sensor config to string for saving config file.
 *   @param   SensorConfig *sc : pointer to the sensorConfig to be saved
 *   @param   char *result     : pointer to string where result should be stored
 *   @retval  uint8_t          : 0 when successful, 1 otherwise
 * 
 * This function will convert the configuration of one sensor to a string. The 
 * string can then be written to a config file.
 * The sensor_data file pointer will not be stored.
 */
uint8_t sensor_config_to_str(SensorConfig *sc, char *result);

 /**
 * Write sensor config to string for saving config file.
 *   @param   char *result     : pointer to input string
 *   @param   SensorConfig *sc : pointer to the sensorConfig for storing
 *   @retval  uint8_t          : 0 when successful, 1 otherwise
 * 
 * This function will read from a string the configuration data for one sensor.
 * The configuration data will be stored at the SensorConfig pointer given.
 * The sensor_data file pointer will be set to NULL because it is not stored in
 * the config file.
 */
uint8_t sensor_config_from_str(char *input, SensorConfig *sc);

#endif
