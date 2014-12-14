#include "mbed.h"
#include "rtos.h"
#include "BusHandler.h"
#include "SerialID.h"


#include "ADC_4088.h"
#include "impact_fsm.h"
// Logger: 150576ea
#define LOG

// Sensor: 61bfdf6
//#define SEN

//#define DEBUG_IMPACT

#ifdef DEBUG_IMPACT
Input_t deb_data[1000] = 
#include "golfb10k.h"
;
#endif

Queue <CANmessage_t, 50> outQueue;
MemoryPool<CANmessage_t, 50> mpoolOutQueue;


AnalogIn pinser2(p17);
AnalogIn pinser3(p16);

DigitalOut led2(LED2);

uint32_t serialNr = 0;
uint32_t  canId = 0;


volatile uint64_t timeStamp = 0;

/*
 *  Flow control flags
 *
 */
char settingsReceived = 0;
char timeSet = 0;
char startRecording = 0;

/*
 * ADC variables
 *
 */

analogin_s adc;
uint32_t data = 0;
uint32_t old_data = 0;

Serial pcSerial(USBTX, USBRX);

void time(void const *n) {
    timeStamp++;
}

extern "C" void ADC_IRQHandler()
{
	data = get_ADC_result(&adc);

	if(data > 2100 || data < 1900){
	  pcSerial.printf("get: %d\n", data);
	}
	stop_ADC_Conversion();
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
		uint32_t comparer = 0x01ff0101;	
		evt = outQueue.get(0);
		if (evt.status == osEventMessage) {
			CANmessage_t *message = (CANmessage_t*)evt.value.p;
			printf("received msg: ");
			for (int i = 0; i< message->dataLength; i++){
				printf("%x", message->payload[i]);
			}
			printf("\nreceived id  : %0x \n\r", message->msgId);
			printf("received len : %d \n\r", message->dataLength);
			if (message->msgId == comparer){
				printf("\ngot CAN-ID: %x\n",message->payload[0]);
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
	
	enableSensorFilter(canId);
	
	
	timeStamp = 0;
	// set the timer to an interval of 100 ms
	timeMs.start(100);
	// try for 5 s to get a settings message
	while((timeStamp < 50) & (settingsReceived == 0)) {
		osEvent evt = outQueue.get(0);
		if (evt.status == osEventMessage) {
			CANmessage_t *message = (CANmessage_t*)evt.value.p;
			// check, if the settings message arrived
			if (message->msgId == (0x05000101 | (canId << 16))){
				// set the settings flag to 1
				settingsReceived = 1;
				/*
				 * TODO set the settings
				 *
				 */
				/*set_baseline(0x00);
				set_threshold(0x00);
				set_max_impact_length(0x00);
				set_samples_until_timeout(0x00);
				*/
				printf("settings data: ");
				for (int i = 0; i< message->dataLength; i++){
					printf("%x", message->payload[i]);
				}
				printf("\nTime id  : %0x \n\r", message->msgId);
				printf("Time len : %d \n\r", message->dataLength);
			}
			mpoolOutQueue.free(message);
		}
	}
	timeMs.stop();
	//no settings received, use the default ones
	if (settingsReceived == 0){
		/*
		 * TODO set default settings
		 *
		 */
	}
	
	// wait for the timestamp reset message
	while (timeSet == 0){
		osEvent evt = outQueue.get(0);
		if (evt.status == osEventMessage) {
			CANmessage_t *message = (CANmessage_t*)evt.value.p;
			// check, if the timebroadcast arrived
			if (message->msgId == 0x02ff0101){
				// reset the timer
				timeStamp = 0;
				
				timeMs.start(1);
				
				timeSet = 1;
				printf("Timestamp data: ");
				for (int i = 0; i< message->dataLength; i++){
					printf("%c", message->payload[i]);
				}
				printf("\nTimestamp id  : %0x \n\r", message->msgId);
				printf("Timestamp len : %d \n\r", message->dataLength);
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
					if (message->msgId == 0x03ff0101){
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
			while(1) {
				impact_event_detection();
				wait_us(50);
			}
		}
		/*osEvent evt = outQueue.get(0);
		if (evt.status == osEventMessage) {
			CANmessage_t *message = (CANmessage_t*)evt.value.p;
			printf("Out data: ");
			for (int i = 0; i< message->dataLength; i++){
				printf("%c", message->payload[i]);
			}
			printf("\nOut id  : %0x \n\r", message->msgId);
			printf("Out len : %d \n\r", message->dataLength);
			mpoolOutQueue.free(message);
		}*/
		led2 = 1;
		osDelay(500);
		led2 = 0;
		osDelay(500);
		printf("the time is: %llu\n",timeStamp);
	};
}

/*
 *	Logger thread
 *		- TODO: SD-Card prep
 *		- Initialize CAN bus
 *		- start the communication thread
 *		- TODO: get Config from SD card or wait for console input
 *		- send serialID request broadcast
 *		- store the CAN-ID and set the filter for the directed messages
 *		- enter a 5s wait loop to receive the settings, if they don't arrive, use the default settings
 *		- await the timesync from the logger
 *    - enter the operation loop:
 *			- wait for the start recording message
 * 			- start the event detection
 */


void logger_loop (void const *args){
	start_CAN_Bus(LOGGER);
	Thread threadRec(CAN_COM_thread,NULL,osPriorityNormal);
	osDelay(1000);
	// send serial request broadcast
	enqueueMessage(0,0,0xff,0x01,GET_SENSOR_SERIAL_BC);
	
	// loop through all sensors or timeout
		osEvent evt = outQueue.get(0);
		while(evt.status != osEventMessage){
			evt = outQueue.get(0);
		}
		printf("\n\ngot response\n\n");
		// received a response, check if it was the serial one
		
		uint32_t sensor1 = 0x61bfdf6;
		if (evt.status == osEventMessage) {
			CANmessage_t *message = (CANmessage_t*)evt.value.p;
			printf("Response data: ");
			for (int i = 0; i< message->dataLength; i++){
				printf("%x", message->payload[i]);
			}
			printf("\nResponse id  : %0x \n\r", message->msgId);
			printf("Response len : %d \n\r", message->dataLength);
			if (message->msgId == 0x18010001){
				if ((message->payload[0] == ((sensor1 >> 24) & 0xff)) &&
						(message->payload[1] == ((sensor1 >> 16) & 0xff)) &&
						(message->payload[2] == ((sensor1 >> 8) & 0xff)) &&
						(message->payload[3] == ((sensor1) & 0xff))){
					printf("sensor 1 registered\n");
					char serial[5];
					/*
							* TODO: call register_sensor to get the CANIf	
					 *
					 */
					serial[0] = 0x04;
					serial[1] = message->payload[0];
					serial[2] = message->payload[1];
					serial[3] = message->payload[2];
					serial[4] = message->payload[3];
					enqueueMessage(5,serial,0xff,0x01,SET_SENSOR_ID_SINGLE);				
				}
				
			}
			mpoolOutQueue.free(message);
		}		
	// End: loop through all sensors or timeout	
	
	osDelay(1000);
	
	// loop sending the configs to all sensors
		SensorConfigMsg_t cfg;
		cfg.threshold = 1023;
		cfg.baseline = 4095;
		cfg.fs = 1023;
		cfg.timeoutRange = 4095 * 2;
		sendSettings(0x04,cfg);
	//end: loop sending the configs to all sensors
	
	// send time sync BC
	enqueueMessage(0,0,0xff,0x01,TIME_SYNC_BC);
	osDelay(1000);
	
	// set all sensors to start recording
	enqueueMessage(0,0,0xff,0x01,START_REC_BC);
		
	// send the token out
	char data = MAX_NR_OF_MESSAGES;
	enqueueMessage(1,&data,0x04,0x01,SEND_TOKEN_SINGLE);
	while (1){
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
		}
		led2 = 1;
		osDelay(500);
		led2 = 0;
		osDelay(500);
	}	
}
 
int main() {
	pcSerial.baud(115200);
	#ifdef SEN
		pcSerial.printf("start\n");
		sensor_loop(0);
	#endif
	#ifdef LOG
		pcSerial.printf("start\n");
		logger_loop(0);
	#endif
	while (1){
	}
}

extern "C" void HardFault_Handler(){
    error("hardfault\n");
}

extern "C" void BusFault_Handler(){
    error("busfault\n");
}

extern "C" void UsageFault_Handler(){
    error("usagefault\n");
}

extern "C" void MemManage_Handler(){
    error("mmufault\n");
};

	/*char data[5] = {'h','a','l','l','o'};
	  enqueueMessage(5,data,0x04,0x01,SEND_TOKEN_SINGLE);
	  char data1[5] = {'h','u','h','u','u'};
	  enqueueMessage(5,data1,0x04,0x01,SENSOR_CONFIG_SINGLE);
		osDelay(1000);
		char data2[5] = {'h','a','h','a','h'};
		enqueueMessage(5,data2,0x04,0x01,SENSOR_OFF_SINGLE);
		osDelay(1000);
		char data3[5] = {'h','i','h','i','i'};
		enqueueMessage(5,data3,0x04,0x01,OP_MODE_SINGLE);
		osDelay(1000);
		
		char data8[5] = {'f','a','a','i','l'};
		enqueueMessage(5,data8,0x05,0x01,OP_MODE_SINGLE);
		osDelay(1000);

		char data5[5] = {'t','r','a','a','a'};
		enqueueMessage(5,data5,0xff,0x01,START_REC_BC);
		osDelay(1000);
		
		char data6[5] = {'b','l','u','b','b'};
		enqueueMessage(5,data6,0xff,0x01,SENSOR_OFF_BC);
		osDelay(1000);
		
		char data7[5] = {'b','l','a','a','h'};
		enqueueMessage(5,data7,0xff,0x01,OP_MODE_BC);
		osDelay(1000);		
		*/
