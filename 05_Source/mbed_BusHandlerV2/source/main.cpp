#include "mbed.h"
#include "rtos.h"

#include "BusHandler.h"

#include "SerialID.h"

Queue <CANmessage_t, 50> outQueue;
MemoryPool<CANmessage_t, 50> mpoolOutQueue;


DigitalOut led2(LED2);

uint32_t serialNr = 0;

uint8_t  canId = 0;

// Logger: 150576ea
#define LOG

// Sensor: 61bfdf6
//#define SEN




Serial pcSerial(USBTX, USBRX);


void sensor_loop(void const *args){
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
		printf("Serial request passed filter\n");
		printf("Serial request id  : %0x \n\r", message->msgId);
		printf("Serial request len : %d \n\r", message->dataLength);
		mpoolOutQueue.free(message);
		// enable all broadcast filters
		enableBroadCastFilter();
	/*	for (int i = 0;i < 12;i++){
			pcSerial.printf("mask %d : %x\n",i,LPC_CANAF_RAM->mask[i]);
		}
		*/// send the serial number as response to the request
		pcSerial.printf("pre-send serial %x\n",serialNr);
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
	for (int i = 0;i < 20;i++){
		pcSerial.printf("mask %d : %x\n",i,LPC_CANAF_RAM->mask[i]);
	}
	while(1){
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
		while (1){
			led2 = 1;
			osDelay(500);
			led2 = 0;
			osDelay(500);
		}
	#endif
	while (1){
		#ifdef SEN/*
		char data[32];
		for (int i = 0; i <10; i++){
			led2 = !led2;
			data[0] = 0xff;
			data[1] = 0;
			data[2] = 0xff;
			data[3] = 0;
			data[4] = 0xff;
			data[5] = 0;
			data[6] = 0xff;
			data[7] = 0;
			data[8] = 0xff;
			data[9] = 0;
			data[10] = 0xff;
			data[11] = 0;
			data[12] = 0xff;
			data[13] = 0;
			data[14] = 0xff;
			data[15] = 0;
			data[16] = 0xff;
			data[17] = 0;
			data[18] = 0xff;
			data[19] = 0;
			data[20] = 0xff;
			data[21] = 0;
			data[22] = 0xff;
			data[23] = 0;
			data[24] = 0xff;
			data[25] = 0;
			data[26] = 0xff;
			data[27] = 0;
			data[28] = 0xff;
			data[29] = 0;
			data[30] = 0xff;
			data[31] = i&0x7ff;
			if(enqueueMessage(32,data,0x01,0x02,IMPACT_STD_SINGLE) == 0){
				printf("package\n\n\n\n");
			} else {
				pcSerial.printf("not enqueued\n");
			}
			osDelay(2000);
		}*/
		#endif
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
