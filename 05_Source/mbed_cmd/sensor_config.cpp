#include "stdint.h"
#include "MCIFileSystem.h"
#include <cstdio>
#include <stdio.h>
#include "sensor_config.h"

/* ------------------------------------------------------------------
 * -- Constants
 * --------------------------------------------------------------- */

/* ------------------------------------------------------------------
 * -- Functions
 * --------------------------------------------------------------- */
 
/*
* See header file
*/
void init_sensor_config_array(SensorConfig *sc)
{
	uint8_t i;
	for (i = 0; i < MAX_SENSORS; i++){
		// sensor_IDs start at 2, because ID 0 or 1 is reserved for logger.
		sensor_config_init(sc++, i+2);
	}
}
 
/*
* See header file
*/
void sensor_config_init(SensorConfig *sc, uint8_t id)
{
	sc->serialID = 0;      // received from sensor
	sc->sensor_ID = id;      // ID number for this sensor in logger
	
	// operating parameters
	sc->fs = 100;            // sampling frequency in 100 Hz 
	sc->threshold = 200;    // input level above or below baseline
	sc->baseline = 2047;     // baseline level to compensate for 
												 // orientational deviations
	sc->timeout = 30;      // how many samples below threshold will
												 // terminate an impact
	sc->detail_level = M_PEAKS;  // operation mode: 'raw','detailed'..'off'
	sc->started = 0;			 // is the sensor recording? 1 = yes, 0 = no
	
	// file name and pointer
	sc->filename = "";
	sc->pf_sensor_data = NULL;  // data file pointer
}

/*
* See header file
*/
void sensor_config_default(SensorConfig *sc, uint8_t id)
{
	sensor_config_init(sc, id);
	sc->started = 1;
}
 
/*
 * See header file
 */
uint8_t sensor_config_to_str(SensorConfig *sc, char *buffer)
{
	int result;
  // Schreib den string
  result = sprintf(buffer, "{%hhu, %x, %hu, %hu, %hu, %hu, %hu, %hhu}\n", 
			sc->sensor_ID, sc->serialID, sc->fs, sc->threshold, sc->baseline,
			sc->timeout, (uint16_t)sc->detail_level, sc->started);
	
	return result;
}

/*
 * See header file
 */
uint8_t sensor_config_from_file(FILE *input, SensorConfig *sc)
{
	int result;
	uint32_t serialID = 0; 
	uint8_t sensor_ID = 255;
	uint16_t fs = 2001;
	uint16_t threshold = 4097;
	uint16_t baseline = 4097;
	uint16_t timeout = 50000;
	uint16_t detail_level = 20;
	uint8_t success = 1;
	uint8_t started = 2;
	
  result = fscanf(input, "{%hhu, %x, %hu, %hu, %hu, %hu, %hu, %hhu}", 
			&(sc->sensor_ID), &(sc->serialID), &(sc->fs), 
			&(sc->threshold), &(sc->baseline), &(sc->timeout), 
			&detail_level, &started);
  // prüfen...
  if (result == EOF){
		//war das file zu früh fertig => failed, Fehlermeldung raus und defaults einlesen
	}
  if (result == 8){
    //validiere die Werte, speichere sie im sensors array
		if(sensor_ID < MAX_SENSORS){
			sc->sensor_ID = sensor_ID;
		} else {
			success = 0;
		}
		sc->serialID = serialID;
		if(fs > 0 && fs < 2000){
			sc->fs = fs;
		} else {
			success = 0;
		}
		if(threshold < 2047 && (threshold + baseline) < 4096 && baseline - threshold > 0){
			sc->threshold = threshold;
		} else {
			success = 0;
		}
		if(baseline < 4096){
			sc->baseline = baseline;
		} else {
			success = 0;
		}
		// TODO maximum impact length muss > timeout sein
		if(timeout < MAX_INPUT_LENGTH){
			sc->timeout = timeout;
		} else {
			success = 0;
		}
		if(detail_level < 5){
			sc->detail_level = (detail_mode_t)detail_level;
		} else{
			success = 0;
		}
		if(started < 2){
			sc->started = started;
		} else{
			success = 0;
		}
		
		sc->pf_sensor_data = NULL;
	} 
	if (success == 0 || result != 8){
		
		printf("error reading config file");
		return 1;
	}
	// all went fine
	return 0;
}

/*
 * See header file
 */
uint8_t sensor_config_to_file(FILE *fp, SensorConfig *sc)
{
	uint8_t error = 0;
	int i = 0;
	char buffer[80];
	// TODO write to file the header of config file: {nr of sensors to be stored,
	
	for(i = 0; i < MAX_SENSORS; i++){
		/* TODO: check if sensor[i] is registered (serialID valid)
		write config of that sensor to buffer
		store the buffer to the file.
		after all sensors config have been written, return the error value.
		result = sensor_config_to_str(&sc, buffer);
		if(result == 8){}
	

		fprintf(fp, "%s", buffer);*/
	}
	
	
}

/*
 * See header file
 */
void init_logger_config(LoggerConfig logger)
{
	logger.sd_present = 0;
	logger.config_modified = 0;
	logger.started = 0;
}
