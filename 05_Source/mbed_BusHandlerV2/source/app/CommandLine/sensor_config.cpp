#include "stdint.h"
#include "MCIFileSystem.h"
#include <cstdio>
#include <string.h>
#include <stdio.h>
#include "sensor_config.h"


extern SensorConfig sensor[MAX_SENSORS];
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
	sc->is_registered = 0;   // do we know the sensor is online?
	
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
	sc->filename[0] = 0;
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
  result = sprintf(buffer, "{%hhu, %x, %hu, %hu, %hu, %hu, %hu, %hhu},\n",
			sc->sensor_ID, sc->serialID, sc->fs, sc->threshold, sc->baseline,
			sc->timeout, (uint16_t)sc->detail_level, sc->started);
	
	return result;
}

/*
 * See header file
 */
uint8_t sensor_config_read_file(FILE *input, SensorConfig *sc)
{
	int i;
	uint8_t result;
	uint8_t entries = 0;
	SensorConfig temp_config;


//result = fscanf(input, "{%hhu, %x, %hu, %hu, %hu, %hu, %hu, %hhu},\n", 
	char fileHdr[5];
	if(fgets(fileHdr,5,input) != NULL){
		printf("read Hdr OK\n");
	};
	int pos = ftell(input);
	printf("position: %x\n",pos);
	//result = sscanf(fileHdr, "{%hhu,%[^\n]\n", &entries);	
	result = sscanf(fileHdr, "{%hhu,", &entries);	
	
	// position the file on the first data record
	if(fgets(fileHdr,5,input) != NULL){
		printf("preread Hdr OK\n");
	};
	printf("entries in file: %d, result: %d\n",entries, result);
	// read the number of config entries
	if(result == 1 && entries > 0 && entries <= MAX_SENSORS){
		printf("read configs\n");
		// read the configs and store them in the sensor config array
		for(i = 0; i < entries; i++){
			result = sensor_config_from_file(input, &temp_config);
			if(result == 0){
				printf("read sensor\n");
				//sc[temp_config.sensor_ID] = temp_config;
				// copy from temp into sensor array at id-2 because sensor[0] has id 2
				//memcpy((void*)(& sc[temp_config.sensor_ID - 2]), &temp_config, sizeof(SensorConfig));
				memcpy(&sc[temp_config.sensor_ID - 2], &temp_config, sizeof(SensorConfig));
			}
			
		}
	}
	return result;
	// TODO after this function, call the function to all sensors to send their serialID and register them, then send them their configuration and start capturing data.
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
	
  //result = fscanf(input, "{%hhu, %x, %hu, %hu, %hu, %hu, %hu, %hhu},\n", 
	
	char string[45];
	if(fgets(string,45,input) != NULL){
		printf("read OK\n");
	};
	result = sscanf(string, "{%hhu, %x, %hu, %hu, %hu, %hu, %hu, %hhu},%[^\n]\n", 
			&sensor_ID, &serialID, &fs, 
			&threshold, &baseline, &timeout, 
			&detail_level, &started);
  // prüfen...
  if (result == EOF){
		//war das file zu früh fertig => failed, Fehlermeldung raus und defaults einlesen
	}
  if (result == 9){
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
			printf("threshold %d\n",threshold);
		} else {
			success = 0;
		}
		if(baseline < 4096){
			sc->baseline = baseline;
			printf("baseline %d\n",baseline);
		} else {
			success = 0;
		}
		// timeout can be max 256. reason: only 8 bit for distance between peaks in can message
		if(timeout < 256){
			sc->timeout = timeout;
			printf("timeout %d\n",timeout);
		} else {
			success = 0;
		}
		if(detail_level < 5){
			sc->detail_level = (detail_mode_t)detail_level;
			printf("detail_level %d\n",detail_level);
		} else{
			success = 0;
		}
		if(started < 2){
			sc->started = started;
		} else{
			success = 0;
		}
		
		sc->is_registered = 0;
		sc->filename[0] = 0;
		sc->pf_sensor_data = NULL;
	} 
	if (success == 0 || result != 9){
		
		printf("error reading config file, success %d, result %d \n",success,result);
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
	uint8_t result = 0;
	int i = 0;
	char buffer[80];
	// write to file the header of config file: {nr of sensors to be stored,
	fprintf(fp, "{ %d,\n", MAX_SENSORS);
	
	for(i = 0; i < MAX_SENSORS; i++){
		result = sensor_config_to_str(sc, buffer);
		sc++;
		if(result == 8){
			// all parameters written to string
		} else {
			error = 1;
			return error;
		}
		fprintf(fp, "%s", buffer);
	}
	fprintf(fp, "}");
	return error;
}

/*
 * See header file
 */
void init_logger_config(LoggerConfig logger)
{
	logger.sd_present = 0;
	logger.config_modified = 0;
	logger.started = 0;
	logger.nr_of_registered_sensors = 0;
	logger.seconds_at_timestamp_reset = 0;
}

/*
 * See header file
 */
uint8_t register_sensor(uint32_t serialID, SensorConfig *sc){
	for(int i = 0; i < MAX_SENSORS; i++){
		printf("file: %x\n",sc->serialID);
		printf("sensor: %x\n",serialID);
		if (sc->serialID == serialID){
			sc->is_registered = 1;
			return sc->sensor_ID;
		}
		sc++;
	}
	return 0;
}

/*
 * See header file
 */
uint8_t store_impact_data(uint8_t id, detail_mode_t detail_mode, uint32_t dataLength, ImpactData_t *data){
    
    char buffer[10300], smallbuf[10];
    int i;
    uint32_t timest;
/*  Buffer init necessary?
    buffer = "";
    smallbuf = "";
  */  
    // check if file open
    if(sensor[id].pf_sensor_data == NULL){
			  printf("sensor data of %d is NULL\n", id);
			  printf("sensor fname is %s\n", sensor[id].filename);
        if(strcmp(sensor[id].filename," ")== 0){
						printf("file not created\n");
            return 1; // file was not created on start of logging
        } else {
            // try to open file
            sensor[id].pf_sensor_data = fopen(sensor[id].filename, "a");
						printf("trying file open\n");
            if(sensor[id].pf_sensor_data == NULL){
                // give up
								printf("giving up\n");
                return 1;
            }
        }
    }
    
    // TODO prepare the data in a buffer string
    switch(detail_mode){
        case M_RAW:
				{
            // TODO
            timest  = data->data[0];
						timest  = timest << 8;
            timest |= data->data[1];
						timest  = timest << 8;
            timest |= data->data[2];
						timest  = timest << 8;
            timest |= data->data[3];
            sprintf(buffer, "start=%u,samples=%u;",timest, dataLength - 4);
            for(i = 4; i < dataLength; i++){
                sprintf(smallbuf, "%hhd,", data->data[i]);
                strcat(buffer, smallbuf);
            }
            strcat(buffer,";\n");
            break;
					}
        case M_DETAILED:
				{
            timest  = data->data[0];
						timest  = timest << 8;
            timest |= data->data[1];
						timest  = timest << 8;
            timest |= data->data[2];
						timest  = timest << 8;
            timest |= data->data[3];
            
            // TODO add safety: count space used in buffer!
            sprintf(buffer, "start=%u,samples=%u;",timest, dataLength - 4);
            for(i = 4; i < dataLength; i++){
                sprintf(smallbuf, "%hhd,", data->data[i]);
                strcat(buffer, smallbuf);
            }
            strcat(buffer,";\n");
            break;
				} 
        case M_PEAKS:
				{
            timest  = data->data[0];
						timest  = timest << 8;
            timest |= data->data[1];
						timest  = timest << 8;
            timest |= data->data[2];
						timest  = timest << 8;
            timest |= data->data[3];
            
            // TODO add safety: count space used in buffer!
						uint16_t samples = data->data[6];
						samples = samples <<8;
						samples |= data->data[7];
            sprintf(buffer, "start=%u,samples=%hu,nrpeaks=%hhu;", timest, samples, data->data[5]);
            for(i = 8; i < dataLength; i+=2){
                sprintf(smallbuf, "%hhu %hhd,", data->data[i], data->data[i+1]);
                strcat(buffer, smallbuf);
            }
            strcat(buffer,";\n");
            break;
				}
        case M_SPARSE:
				{
            timest  = data->data[0];
						timest  = timest << 8;
            timest |= data->data[1];
						timest  = timest << 8;
            timest |= data->data[2];
						timest  = timest << 8;
            timest |= data->data[3];
            
            // TODO add safety: count space used in buffer!
						uint16_t samplesSpar = data->data[6];
						samplesSpar = samplesSpar <<8;
						samplesSpar |= data->data[7];


						sprintf(buffer, "start=%u,samples=%hu,nrpeaks=%hhu,max=%hhd;\n", timest, samplesSpar,
                    data->data[5], data->data[4]);
            break;
				}
        default:
            break;
    }
    
    
    // append buffer string to data file of sensor
    fprintf(sensor[id].pf_sensor_data, "%s", buffer);
    
    // return 0 if successful, 1 if failure
    return 0;
}

