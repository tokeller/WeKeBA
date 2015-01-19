#include "ProcessingLoops.h"

#ifdef LOG
// create a large output queue (messages from the communication thread) on the logger
Queue <CANmessage_t, 800> outQueue;
MemoryPool<CANmessage_t, 800> mpoolOutQueue;
#endif

#ifdef SEN
// the output message queue on the sensor has to hold only a few messages
Queue <CANmessage_t, 50> outQueue;
MemoryPool<CANmessage_t, 50> mpoolOutQueue;
#endif


uint32_t serialNr = 0;
uint32_t  canId = 0;

analogin_s adc;
uint32_t data = 0;
uint32_t old_data = 0;

extern Serial pcSerial;
extern SensorConfig sensor[MAX_SENSORS];

volatile uint64_t timeStamp = 0;

/*
 *  Flow control flags
 *
 */
char settingsReceived = 0;
char timeSet = 0;
char startRecording = 0;
char sensorOffline = 0;

void time(void const *n) {
    timeStamp++;
}


/*
 *	Sensor thread
 *		- Initialize CAN bus (set the filter so only the SerialRequest will be received)
 *		- get the serial number
 *		- start the communication thread
 *		- process the SerialRequest from the logger by sending the read serial number
 *			and enable all broadcast filters (since no ID was yet received, the sensor can only process
 *			broadcast messages)
 *		- wait till the IDBroadcast with the sensors serial number has arrived and store the
 *			corresponding CAN-ID
 *		- store the CAN-ID and set the filter for the directed messages
 *		- enter a 5s wait loop to receive the settings, if they don't arrive, use the default settings
 *		- await the timesync from the logger
 *    - enter the operation loop:
 *			- wait for the start recording message
 * 			- start the event detection
 */

void sensor_loop(void const *args){
	// Initialize CAN bus
	start_CAN_Bus(SENSOR);
	RtosTimer timeMs (time,osTimerPeriodic);
	serialNr = getSerialNumber();
	// start the CAN communcation thread
	Thread thread(CAN_COM_thread);
	pcSerial.printf("Serial: %x\n",serialNr);
	osEvent evt = outQueue.get(0);
	// since the acceptance filter for the sensor was initialized to only accept the serial request
	// the first message to arrive should be the one
	while(evt.status != osEventMessage){
		evt = outQueue.get(0);
	}
	// received a serial request, send the id out
	if (evt.status == osEventMessage) {
		CANmessage_t *message = (CANmessage_t*)evt.value.p;
		mpoolOutQueue.free(message);
		// enable all broadcast filters
		enableBroadCastFilter();
		// send the serial number as response to the request
		sendSerialResponse(serialNr);
	}
	char canIdReceived = 0;
	
	// read the messages till the proper CAN-ID arrives
	while (canIdReceived == 0){
		evt = outQueue.get(0);
		if (evt.status == osEventMessage) {
			CANmessage_t *message = (CANmessage_t*)evt.value.p;
			printf("received msg: ");
			for (int i = 0; i< message->dataLength; i++){
				printf("%x", message->payload[i]);
			}
			printf("\nreceived id  : %0x \n\r", message->msgId);
			printf("received len : %d \n\r", message->dataLength);
			if (message->msgId == CAN_ID_MSG){
				printf("\ngot CAN-ID: %x\n",message->payload[0]);
				// check the received message for the matching serialNr
				if ((message->payload[1] == ((serialNr >> 24) & 0xff)) &&
					  (message->payload[2] == ((serialNr >> 16) & 0xff)) &&
					  (message->payload[3] == ((serialNr >> 8) & 0xff)) &&
					  (message->payload[4] == ((serialNr) & 0xff))){
					printf("serial valid\n");
					canIdReceived = 1;
					canId = message->payload[0];
				}
			}
			mpoolOutQueue.free(message);
		}
	};
	
	// enable the sensor specific filters
	enableSensorFilter(canId);	

	// wait for the settings message
	while(settingsReceived == 0){
		osEvent evt = outQueue.get(0);
		if (evt.status == osEventMessage) {
			CANmessage_t *message = (CANmessage_t*)evt.value.p;
			// check, if the settings message arrived
			if (message->msgId == (SETTINGS_MSG | (canId << 16))){
				// set the settings flag to 1
				settingsReceived = 1;
				// process the settings message
				processSettings(message);
				
				printf("settings data: ");
				for (int i = 0; i< message->dataLength; i++){
					printf("%x", message->payload[i]);
				}
				printf("\nsettings id  : %0x \n\r", message->msgId);
				printf("settings len : %d \n\r", message->dataLength);
			}
			mpoolOutQueue.free(message);
		}
	}
	
	// wait for the timestamp reset message
	while (timeSet == 0){
		osEvent evt = outQueue.get(0);
		if (evt.status == osEventMessage) {
			CANmessage_t *message = (CANmessage_t*)evt.value.p;
			// check, if the timebroadcast arrived
			if (message->msgId == TIME_SYNC_MSG){
				// reset the timer
				reset_timestamp();
				
				timeSet = 1;
				printf("\nTimestamp id  : %0x \n\r", message->msgId);
			}
			mpoolOutQueue.free(message);
		}
	}
	
	while(1){
		/*
		 * Wait for the first "start recording" message
		 */
		while (startRecording == 0){
			osEvent evt = outQueue.get(0);
			if (evt.status == osEventMessage) {
				CANmessage_t *message = (CANmessage_t*)evt.value.p;
					if (message->msgId == START_REC_MSG){
						startRecording = 1;
						//Thread impThread(impact_thread,NULL,osPriorityNormal);
					}
				mpoolOutQueue.free(message);
			}
		}
		
		if (startRecording == 1){
			#ifdef DEBUG_IMPACT
				int deb_i;
			#endif
			PinName pin = p15;
			#ifndef DEBUG_IMPACT
				// when Debugging, don't run ADC interruptS
				// fine tuned impact recognition and processing
				uint32_t clkd = register_ADC_interrupt(&adc, pin,(uint32_t) isr_nextMeasurement,100);
			#endif
			init_impact_event_handler();
			#ifdef DEBUG_IMPACT
			// load debug data
			for (deb_i = 0; deb_i < 1000; deb_i++){
				enqueue_impact_input(deb_data[deb_i].timestamp, deb_data[deb_i].value); 
			}
			pcSerial.printf("DEBUGGING MODE IMPACT RECOGNITION\n\ndebug data loaded, begin analysis\n");
			#endif
			//setTokenStatus(1,255);
			while(1) {
				// check the incoming messages
				osEvent evt = outQueue.get(0);
				if (evt.status == osEventMessage) {
					CANmessage_t *message = (CANmessage_t*)evt.value.p;
						switch (message->msgId){
							case START_REC_MSG:
								startRecording = 1;
								sensorOffline = 0;
								break;
							case ALL_OFF_MSG:
								init_impact_event_handler();
								sensorOffline = 1;
								break;
							case TIME_SYNC_MSG:
								// reset the timer
								reset_timestamp();
								break;
							default:
								if (message->msgId == (SETTINGS_MSG | (canId << 16))){
									processSettings(message);
								} else if (message->msgId == (TOKEN_MSG | (canId << 16))){
									uint64_t nrOfSamples = 0;
									if (message->dataLength > 1){
										nrOfSamples = message->payload[1];
										nrOfSamples = nrOfSamples << 8;
										nrOfSamples |= message->payload[2];
										nrOfSamples = nrOfSamples << 8;
										nrOfSamples |= message->payload[3];
										nrOfSamples = nrOfSamples << 8;
										nrOfSamples |= message->payload[4];
										nrOfSamples = nrOfSamples << 8;
										nrOfSamples |= message->payload[5];
										nrOfSamples = nrOfSamples << 8;
										nrOfSamples |= message->payload[6];
										nrOfSamples = nrOfSamples << 8;
										nrOfSamples |= message->payload[7];
									}
									setTokenStatus(1,message->payload[0],nrOfSamples);
									sensorOffline = 0;
									pcSerial.printf("Token received, start sending %d msgs\n",message->payload[0]);
								} else if (message->msgId == (SINGLE_OFF_MSG| (canId << 16))){
									init_impact_event_handler();
									sensorOffline = 1;
								}
								break;
						}
					mpoolOutQueue.free(message);
				}
				// process detected events
				if (sensorOffline == 0){
					impact_event_detection();
				} else {
					// if the sensor was set offline, the message queue will be emptied
					emptyQueue();
				}
				osDelay(1);
			}
		}
	};
}

/*
 *	Logger thread
 *		- TODO: SD-Card prep
 *		- Initialize CAN bus
 *		- start the communication thread
 *		- get Config from SD card or wait for console input
 *		- send serialID request broadcast
 *		- process received serial-IDs and send the corresponding CAN-Ids as broadcast
 *		- send the configs of all registered sensors
 *	  - send the timesync to all sensors
 *		- start the sensors recording mode
 *		- enter the processing loop:
 *			- commence sending the token to the sensor
 *			- receive and store the messages
 */


void logger_loop (void const *args){
	char allSensorsReceived = 0;
	char headerReceived = 0;
	uint16_t cachePointer = 0;
	uint8_t *dataCache;
	
	start_CAN_Bus(LOGGER);
	RtosTimer timeMs (time,osTimerPeriodic);
	
	cmd_mount_sd();
  cmd_read_config_file();
	
	Thread threadRec(CAN_COM_thread,NULL,osPriorityNormal);
	osDelay(1000);
	// send serial request broadcast
	enqueueMessage(0,0,0xff,0x01,GET_SENSOR_SERIAL_BC);
	//char sensorCounter = 0;
	// start a timer to count to 5s for the response timeout
	timeStamp = 0;
	timeMs.start(100);
	osEvent evt = outQueue.get(0);
	while(evt.status != osEventMessage){
		evt = outQueue.get(0);
	}
			
	uint16_t nrOfRegSensors = 0;
	// loop through all sensors or till the timeout occurs
	//while (timeStamp < 50 || allSensorsReceived == 0){
	while (timeStamp < 50){
		// received a response, check if it was a serial one
		if (evt.status == osEventMessage) {
			printf("\n\ngot response\n\n");
			CANmessage_t *message = (CANmessage_t*)evt.value.p;
			printf("Response data: ");
			for (int i = 0; i< message->dataLength; i++){
				printf("%x", message->payload[i]);
			}
			printf("\nResponse id  : %0x \n\r", message->msgId);
			printf("Response len : %d \n\r", message->dataLength);
			if (message->msgId == SERIAL_MSG){		
					
					uint32_t serialID = message->payload[0];
					serialID = serialID << 8;
					serialID |= message->payload[1];
					serialID = serialID << 8;
					serialID |= message->payload[2];
					serialID = serialID << 8;
					serialID |= message->payload[3];
				
					uint8_t canIdentifier = 0;
					// register the sensor and get its CAN identifier
					canIdentifier = register_sensor(serialID, sensor);
					char serial[5];
					serial[0] = canIdentifier;
					nrOfRegSensors++;
					serial[1] = message->payload[0];
					serial[2] = message->payload[1];
					serial[3] = message->payload[2];
					serial[4] = message->payload[3];
					// send the sensors CAN identifier as broadcast
					enqueueMessage(5,serial,0xff,0x01,SET_SENSOR_ID_SINGLE);				
				
				
			}
			mpoolOutQueue.free(message);
		}
		evt = outQueue.get(0);		
	// End: loop through all sensors or timeout	
	}
	timeMs.stop();
	osDelay(1000);
	for(int i = 0; i < nrOfRegSensors;i++){
	// loop sending the configs to all sensors
		SensorConfigMsg_t cfg;
		cfg.threshold = sensor[i].threshold;
		cfg.baseline = sensor[i].baseline;
		cfg.fs = sensor[i].fs;
		cfg.timeoutRange = sensor[i].timeout;
		cfg.started = (sensor[i].detail_level<<4);
		sendSettings(i+2,cfg);
	//end: loop sending the configs to all sensors
	}
	osDelay(1000);
	// send time sync BC
	resetTimestamp();
	osDelay(1000);
	
	// set all sensors to start recording
	enqueueMessage(0,0,0xff,0x01,START_REC_BC);
		
	// send the token out
	char MaxOfMsg = MAX_NR_OF_MESSAGES;
	uint64_t nrOfMsg = 10;
	char sensorId = 0x02;
	enqueueMessage(1,&MaxOfMsg,sensorId,0x01,SEND_TOKEN_SINGLE);
	pcSerial.printf("nr of reg sensors: %d\n",nrOfRegSensors);
	while (1){
		// the logger will always have the send token, must be reset for each cycle
		setTokenStatus(1,255,0);
		// receive the data from the sensors
		osEvent evt = outQueue.get(0);
		if (evt.status == osEventMessage) {
			CANmessage_t *message = (CANmessage_t*)evt.value.p;
			printf("Sensor data: ");
			for (int i = 0; i< message->dataLength; i++){
				printf("%x", message->payload[i]);
			}
			uint8_t sensId = (message->msgId >> 8) & 0x0f;
			uint8_t msgNr = (message->msgId & 0xff);
			printf("\nSensor msg id  : %0x \n\r", message->msgId);
			printf("\nSensor id      : %0x \n\r", sensId);
			printf("Sensor len       : %d \n\r", message->dataLength);
			ImpactData_t *iData  = (ImpactData_t*) malloc(sizeof(ImpactData_t));
			
			// received a sparse message with just one part, can be stored right away
			if ((message->msgId & 0x1fff0000) == IMP_SPARSE_MSG & msgNr == 1){	
				uint8_t array[8] = {0};
				array[0] = (uint8_t) message->payload[0];
				array[1] = (uint8_t) message->payload[1];
				array[2] = (uint8_t) message->payload[2];
				array[3] = (uint8_t) message->payload[3];
				array[4] = (uint8_t) message->payload[4];
				array[5] = (uint8_t) message->payload[5];
				array[6] = (uint8_t) message->payload[6];
				array[7] = (uint8_t) message->payload[7];
				
				//memcpy(iData->data,message->payload,message->dataLength);
				iData->data = &array[0];
				sensId -= 2;
				uint8_t res = store_impact_data(sensId,sensor[sensId].detail_level,message->dataLength,iData);
				if(res == 0){
					printf("written\n");
				}else {
					printf("error\n");
				}
				     // received a peak message, either the start (SPARSE with more than 1 pkg) or a sole date package
			} else if (((message->msgId & 0x1fff0000) == IMP_SPARSE_MSG & msgNr > 1) || ((message->msgId & 0x1fff0000) == IMP_PEAKS_MSG)){
				
				if (headerReceived == 0){
					// get the message number to allocate enough memory
					uint16_t cacheSize = (message->msgId & 0xff);
					// allocate the data cache for 8 uint8_t per expected message
					dataCache = (uint8_t *) malloc (sizeof(uint8_t) * 8 * cacheSize);
					headerReceived = 1;
					cachePointer = 0;
				}
				// store the received message data
				for (int i = 0; i<message->dataLength; i++){
					dataCache[cachePointer + i] = (uint8_t) message->payload[i];
				}
				
				// if the last message was received (id = 1), write it to the file
				if ((message->msgId & 0xff) == 1){
					cachePointer += message->dataLength + 1;
					iData->data = dataCache;
					sensId -= 2;
					uint8_t res = store_impact_data(sensId,sensor[sensId].detail_level, cachePointer,iData);
					if(res == 0){
						printf("written\n");
					}else {
						printf("error\n");
					}				
					// initialize the pointer and flag for the next message
					cachePointer = 0;
					headerReceived = 0;
				} else {					
					// set the cachPointer ready for the next message
					cachePointer += 8;
				}
				
			// received a detailed impact message
			}else if ((message->msgId & 0x1fff0000) == IMP_DETAILED_MSG){
				if (headerReceived == 0){
					// get the message number to allocate enough memory
					uint16_t cacheSize = (message->msgId & 0xff);
					// allocate the data cache for 8 uint8_t per expected message
					dataCache = (uint8_t *) malloc (sizeof(uint8_t) * 8 * cacheSize);
					headerReceived = 1;
					cachePointer = 0;
				}
				// store the received message data
				for (int i = 0; i<message->dataLength; i++){
					dataCache[cachePointer + i] = (uint8_t) message->payload[i];
				}
				
				// if the last message was received (id = 1), write it to the file
				if ((message->msgId & 0xff) == 1){
					cachePointer += message->dataLength + 1;
					iData->data = dataCache;
					sensId -= 2;
					uint8_t res = store_impact_data(sensId,sensor[sensId].detail_level, cachePointer,iData);
					if(res == 0){
						printf("written\n");
					}else {
						printf("error\n");
					}				
					// initialize the pointer and flag for the next message
					cachePointer = 0;
					headerReceived = 0;
				} else {					
					// set the cachPointer ready for the next message
					cachePointer += 8;
				}
				
			}
			// free the impacat data struct
			free(iData);
			mpoolOutQueue.free(message);
			nrOfMsg--;
		}
		if (nrOfMsg <= 0){
			nrOfMsg = 10;
			if (sensorId < nrOfRegSensors + 1){
				sensorId++;
			} else {
				sensorId = 0x02;
			}
			pcSerial.printf("give token to Sensor %d of %d\n",sensorId,nrOfRegSensors);
			enqueueMessage(1,&MaxOfMsg,sensorId,0x01,SEND_TOKEN_SINGLE);
		}
	}	
}

// command line input reader
void get_cmd_event_thread(void const *args)
{
	Setup_CHOSEN_UART(115200);
	uint32_t event;
	char input[20];
	init_menu_fsm();
	while(1){
		pcSerial.printf(" >");
		int i = 0;
		while(1){
			input[i] = UART_RTOS_getc();
			if (input[i] == '\r'){
				break;
			}
			i++;
		}
		input[i+1] = '\0';
		event = atoi(input);
		input[0] = 0;
		menu_fsm(event);
		osDelay(100);
	}
}


/*
 *	ProcessSettings
 *	@params		CANmessage_t *message: incoming settings message
 *
 * 	This function processes the received settings and sets them accordingly
 */

void processSettings(CANmessage_t *message){
	// 200
	uint16_t threshold = message->payload[0];
	threshold = threshold << 2;
	threshold |= message->payload[1] >> 6;
	printf("Threshold %x\n",threshold);
	set_threshold(threshold);
	
	// started and CTRL-Flags
	//setTokenStatus((message->payload[2]>>4)&0x01,0,0);
	
	// set detail mode
	char detailMode = message->payload[1]&0x0f;
	printf("Detail mode: %x\n",detailMode);
	setDetailMode((detail_mode_t) detailMode);
	
	// 10000 Khz: (sampling rate (in 100 Hz steps) * 100) / 100 = micro sec
	uint16_t fsampling = message->payload[2] & 0x0f;
	fsampling = fsampling<<8;
	fsampling |= message->payload[3];
	printf("fsampling %x\n",fsampling);
	set_ADC_frequency(fsampling);
	
	// 30
	uint16_t timeout = message->payload[4];
	timeout = timeout<<8;
	timeout |= message->payload[5];
	printf("timeout %x\n",timeout);
	set_samples_until_timeout(timeout);
	
	// 2047
	uint16_t baseline = message->payload[6] & 0x3f;
	baseline = baseline << 8;
	baseline |= message->payload[7] & 0xff;
	printf("baseline %x\n",baseline);
	set_baseline(baseline);	
}
