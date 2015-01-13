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
		
	typedef struct logger_config{
		// Flags
		uint8_t sd_present;		// is logger aware of SD card?
		uint8_t config_modified;// has config modified without being saved to file?
		uint8_t started;		// is logging started or stopped at the moment?
		
		uint8_t nr_of_registered_sensors;
		uint32_t seconds_at_timestamp_reset; // when was timestamp reset?
	} LoggerConfig;
	
	typedef struct sensor_config{
	  // identification
		uint32_t serialID;     // received from sensor
		uint8_t sensor_ID;     // ID number for this sensor in logger
		uint8_t is_registered; // do we know the sensor is online?
		
		// operating parameters
		uint16_t fs;           // sampling frequency in 100 Hz 
		uint16_t threshold;    // input level above or below baseline
		uint16_t baseline;     // baseline level to compensate for 
		                       // orientational deviations
		uint16_t timeout;      // how many samples below threshold will
		                       // terminate an impact
		detail_mode_t detail_level;  // operation mode: 'raw','detailed'..'off'
		uint8_t started;			 // is the sensor recording? 1 = yes, 0 = no
		
		// file name and pointer
		char filename[25];
		FILE *pf_sensor_data;  // data file pointer
		
	} SensorConfig;

typedef struct {
    uint8_t *data;        // pointer to array of data
    // data array is expected to contain all relevant information
    // depending on detail_mode:
    
    // M_SPARSE:
    // uint32_t timestamp of start
    // uint8_t  max_peak
    // uint8_t  nrOfPeaks
    // uint16_t duration of impact (nr of samples)
    
    // M_PEAKS:
    // uint32_t timestamp of start
    // uint8_t  max_peak
    // uint8_t  nrOfPeaks
    // uint16_t duration of impact (nr of samples)
    // uint8_t  *data (containing pairs of uint8_t offset, uint8_t value according to nrOfPeaks)
    
    // M_DETAILED & M_RAW:
    // uint32_t timestamp of start
    // uint32_t nrOfSamples
    // uint8_t  *data (uint8_t sample[nrOfSamples])
} ImpactData_t;



/* ------------------------------------------------------------------
 * -- Prototypes
 * --------------------------------------------------------------- */

	/**
 * initialize sensor_config array
 *   @param   SensorConfig *sc : pointer to the SensorConfig array to init
 *   @retval  none
 */
void init_sensor_config_array(SensorConfig *sc);

 /**
 * initialize new sensor_config
 *   @param   SensorConfig *sc : pointer to the SensorConfig to init
 *   @param   uint8_t id       : id of that sensor.
 *   @retval  none
 */
void sensor_config_init(SensorConfig *sc, uint8_t id);

/**
 * set new sensor_config to default, in case there is no valid config file.
 *   @param   SensorConfig *sc : pointer to the SensorConfig to init
 *   @param   uint8_t id       : id of that sensor.
 *   @retval  none
 */
void sensor_config_default(SensorConfig *sc, uint8_t id);
	
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
 * 
 *   @param   FILE *input      : pointer to input file
 *   @param   SensorConfig *sc : pointer to the sensorConfig for storing
 *   @retval  uint8_t          : 0 when successful, 1 otherwise
 */

uint8_t sensor_config_read_file(FILE *input, SensorConfig *sc);
 /**
 * Read one sensor config from config file.
 *   @param   FILE *input      : pointer to input file
 *   @param   SensorConfig *sc : pointer to the sensorConfig for storing
 *   @retval  uint8_t          : 0 when successful, 1 otherwise
 * 
 * This function will read from a string the configuration data for one sensor.
 * The configuration data will be stored at the SensorConfig pointer given.
 * The sensor_data file pointer will be set to NULL because it is not stored in
 * the config file.
 */
uint8_t sensor_config_from_file(FILE *input, SensorConfig *sc);

 /**
 * Write sensor config array to config file.
 *   @param   FILE *fp         : FILE pointer to config file
 *   @param   SensorConfig *sc : pointer to the sensorConfig Array
 *   @retval  uint8_t          : 0 when successful, 1 otherwise
 * 
 * This function will attempt to store the config data array to
 * a config file. If there is an error, return value will be non-zero.
 */
uint8_t sensor_config_to_file(FILE *fp, SensorConfig *sc);

/**
 * set new logger_config to default.
 *   @param   LoggerConfig logger : pointer to the LoggerConfig to init
 *   @retval  none
 */
void init_logger_config(LoggerConfig logger);

/**
 * register found sensor.
 *   @param   uint64_t serial  : serial number of registered sensor
 *   @param   SensorConfig *sc : pointer to the sensorConfig Array
 *   @retval  uint8_t			     : CAN-ID for registered sensor
 */ 
uint8_t register_sensor(uint32_t serialID, SensorConfig *sc);

/**
 * store impact data
 *   @param   uint8_t sensorId: Index of Sensor in SensorConfig array
 *   @param   detail_mode_t detail_mode: detail_mode of data
 *   @param   uint32_t dataLength: length of received data
 *   @param   ImpactData_t: Impact data according to detail_mode
 *   @retval  uint8_t: 0 on success, 1 on failure
 */
uint8_t store_impact_data(uint8_t sensorId, detail_mode_t detail_mode, uint32_t dataLength, ImpactData_t *data);

#endif
