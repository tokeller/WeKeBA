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
	
	// file pointer
	sc->pf_sensor_data = NULL;  // data file pointer
}

 
/*
 * See header file
 */
uint8_t sensor_config_to_str(SensorConfig *sc, char *string)
{
	int result;
  // nicht printf sondern halt in den string...
  result = sprintf(string, "{%hhu, %x, %hu, %hu, %hu, %hu, %hu}\n", 
			sc->sensor_ID, sc->serialID, sc->fs, sc->threshold, sc->baseline,
			sc->timeout, (uint16_t)sc->detail_level);
	
  if (result == 0){
		//alles ok
	}
  if (result < 0){
		//nicht ok
	}
	return result;
}

/*
 * See header file
 */
uint8_t sensor_config_from_file(FILE *input, SensorConfig *sc)
{
	int result;
	uint32_t serialID; 
	uint8_t sensor_ID;
	uint16_t fs;
	uint16_t threshold;
	uint16_t baseline;
	uint16_t timeout;
	uint8_t detail_level;
	uint8_t success = 1;
	
  result = fscanf(input, "{%hhu, %x, %hu, %hu, %hu, %hu, %hhu}", 
			&(sc->sensor_ID), &(sc->serialID), &(sc->fs), 
			&(sc->threshold), &(sc->baseline), &(sc->timeout), 
			&detail_level);
  // prüfen...
  if (result == EOF){
		//war das file zu früh fertig => failed, Fehlermeldung raus und defaults einlesen
	}
  if (result == 7){
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
		if(timeout < 512){
			sc->timeout = timeout;
		} else {
			success = 0;
		}
		if(detail_level < 5){
			sc->detail_level = (detail_mode_t)detail_level;
		} else{
			success = 0;
		}
	} 
	if (success == 0 || result != 7){
		
		printf("error reading config file");
		return 1;
	}
	// all went fine
	return 0;
}
