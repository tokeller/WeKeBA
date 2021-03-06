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
								break;
							case ALL_OFF_MSG:
								// set offline
								break;
							default:
								if (message->msgId == (SETTINGS_MSG | (canId << 16))){
									processSettings(message);
								} else if (message->msgId == (TOKEN_MSG | (canId << 16))){
									setTokenStatus(1,message->payload[0]);
									pcSerial.printf("Token received, start sending %d msgs\n",message->payload[0]);
								}
								break;
						}
					mpoolOutQueue.free(message);
				}
				// process detected events
				impact_event_detection();
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
	start_CAN_Bus(LOGGER);
	RtosTimer timeMs (time,osTimerPeriodic);
	Thread threadRec(CAN_COM_thread,NULL,osPriorityNormal);
	osDelay(10000);
	// send serial request broadcast
	enqueueMessage(0,0,0xff,0x01,GET_SENSOR_SERIAL_BC);
	char sensorCounter = 0;
	// start a timer to count to 5s for the response timeout
	timeStamp = 0;
	timeMs.start(100);
	osEvent evt = outQueue.get(0);
	while(evt.status != osEventMessage){
		evt = outQueue.get(0);
	}
			
	uint16_t nrOfRegSensors = 0;
	// loop through all sensors or till the timeout occurs
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
		cfg.threshold = 200;
		cfg.baseline = 2047;
		cfg.fs = 100;
		cfg.timeoutRange = 30;
		cfg.started = 0;
		sendSettings(i+2,cfg);
	//end: loop sending the configs to all sensors
	}
	osDelay(1000);
	// send time sync BC
	enqueueMessage(0,0,0xff,0x01,TIME_SYNC_BC);
	osDelay(1000);
	
	// set all sensors to start recording
	enqueueMessage(0,0,0xff,0x01,START_REC_BC);
		
	// send the token out
	//char nrOfMsg = MAX_NR_OF_MESSAGES;
	char nrOfMsg = 10;
	char sensorId = 0x02;
	enqueueMessage(1,&nrOfMsg,sensorId,0x01,SEND_TOKEN_SINGLE);
	pcSerial.printf("nr of reg sensors: %d\n",nrOfRegSensors);
	while (1){
		// the logger will always have the send token, must be reset for each cycle
		setTokenStatus(1,255);
		// receive the data from the sensors
		osEvent evt = outQueue.get(0);
		if (evt.status == osEventMessage) {
			CANmessage_t *message = (CANmessage_t*)evt.value.p;
			printf("Sensor data: ");
			for (int i = 0; i< message->dataLength; i++){
				printf("%x", message->payload[i]);
			}
			printf("\nSensor id  : %0x \n\r", message->msgId);
			printf("Sensor len : %d \n\r", message->dataLength);
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
			enqueueMessage(1,&nrOfMsg,sensorId,0x01,SEND_TOKEN_SINGLE);
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
	setTokenStatus((message->payload[2]>>4)&0x01,0);
	
	
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