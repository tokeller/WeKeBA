#include "mbed.h"
#include "rtos.h"
#include "BusHandler.h"
#include "SerialID.h"


// Logger: 150576ea
#define LOG

// Sensor: 61bfdf6
//#define SEN

Queue <CANmessage_t, 50> outQueue;
MemoryPool<CANmessage_t, 50> mpoolOutQueue;


DigitalOut led2(LED2);

uint32_t serialNr = 0;

uint32_t  canId = 0;


volatile uint64_t timeStamp = 0;

char settingsReceived = 0;

char timeSet = 0;

Serial pcSerial(USBTX, USBRX);

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
 */

void sensor_loop(void const *args){
	// Initialize CAN bus
	start_CAN_Bus(SENSOR);
	serialNr = getSerialNumber();
	Thread thread(CAN_COM_thread);
	pcSerial.printf("Serial: %x\n",serialNr);
	osEvent evt = outQueue.get(0);
	// since the acceptance filter for the sensor was initialized to only accept the serial request
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
	RtosTimer timeMs (time,osTimerPeriodic);
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
				
				printf("Time data: ");
				for (int i = 0; i< message->dataLength; i++){
					printf("%c", message->payload[i]);
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
		
		
		osEvent evt = outQueue.get(0);
		if (evt.status == osEventMessage) {
			CANmessage_t *message = (CANmessage_t*)evt.value.p;
			printf("Out data: ");
			for (int i = 0; i< message->dataLength; i++){
				printf("%c", message->payload[i]);
			}
			printf("\nOut id  : %0x \n\r", message->msgId);
			printf("Out len : %d \n\r", message->dataLength);
			mpoolOutQueue.free(message);
		}
		led2 = 1;
		osDelay(500);
		led2 = 0;
		osDelay(500);
	};
}

void logger_loop(void const *args){
	Thread threadRec(CAN_COM_thread,NULL,osPriorityNormal);
	Thread threadPrn(CAN_PRINT_thread,NULL,osPriorityNormal);
	uint32_t sensor1 = 0x61bfdf6;
	while(1){
		
		
	}
	
}

void CAN_PRINT_thread(void const *args) {
	serialNr = getSerialNumber();
	printf("serialNr: %x\n", serialNr);
	while (true) {
		osEvent evt = outQueue.get(0);
		if (evt.status == osEventMessage) {
			CANmessage_t *message = (CANmessage_t*)evt.value.p;
			printf("Out data: ");
			for (int i = 0; i< message->dataLength; i++){
				printf("%d", message->payload[i]);
			}
			printf("\nOut id  : %0x \n\r", message->msgId);
			printf("Out len : %d \n\r", message->dataLength);
			mpoolOutQueue.free(message);
		}
	}
}

 
int main() {
	pcSerial.baud(115200);
	#ifdef SEN
		//start_CAN_Bus(SENSOR);
		pcSerial.printf("start\n");
		serialNr = getSerialNumber();
		printf("serialNr: %x\n", serialNr);
		//Thread threadRec(CAN_COM_thread,NULL,osPriorityNormal);
		sensor_loop(0);
	#endif
	#ifdef LOG
		start_CAN_Bus(LOGGER);
		pcSerial.printf("start\n");
		Thread threadRec(CAN_COM_thread,NULL,osPriorityNormal);
		//Thread threadPrn(CAN_PRINT_thread,NULL,osPriorityNormal);
		osDelay(2500);
		// send serial request broadcast
		enqueueMessage(0,0,0xff,0x01,GET_SENSOR_SERIAL_BC);
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
		
		osDelay(1000);
		/*char data[5] = {'h','a','l','l','o'};
		enqueueMessage(5,data,0x04,0x01,SEND_TOKEN_SINGLE);
		*/osDelay(1000);
		char data1[5] = {'h','u','h','u','u'};
		enqueueMessage(5,data1,0x04,0x01,SENSOR_CONFIG_SINGLE);
		/*osDelay(1000);
		char data2[5] = {'h','a','h','a','h'};
		enqueueMessage(5,data2,0x04,0x01,SENSOR_OFF_SINGLE);
		osDelay(1000);
		char data3[5] = {'h','i','h','i','i'};
		enqueueMessage(5,data3,0x04,0x01,OP_MODE_SINGLE);
		osDelay(1000);
		
		char data8[5] = {'f','a','a','i','l'};
		enqueueMessage(5,data8,0x05,0x01,OP_MODE_SINGLE);
		osDelay(1000);
		*/
		char data4[5] = {'t','r','o','o','o'};
		enqueueMessage(5,data4,0xff,0x01,TIME_SYNC_BC);
		osDelay(1000);
		/*
		char data5[5] = {'t','r','a','a','a'};
		enqueueMessage(5,data5,0xff,0x01,START_REC_BC);
		osDelay(1000);
		
		char data6[5] = {'b','l','u','b','b'};
		enqueueMessage(5,data6,0xff,0x01,SENSOR_OFF_BC);
		osDelay(1000);
		
		char data7[5] = {'b','l','a','a','h'};
		enqueueMessage(5,data7,0xff,0x01,OP_MODE_BC);
		osDelay(1000);*/
		while (1){
			led2 = 1;
			osDelay(500);
			led2 = 0;
			osDelay(500);
		}
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
