#include "mbed.h"
#include "rtos.h"

#include "BusHandler.h"

#include "SerialID.h"

Queue <CANmessage_t, 50> outQueue;
MemoryPool<CANmessage_t, 50> mpoolOutQueue;


DigitalOut led2(LED2);



#define LOG
//#define SEN




Serial pcSerial(USBTX, USBRX);


void sensor_loop(void const *args){
	start_CAN_Bus(SENSOR);
	uint32_t serialNr = 0;
	serialNr = getSerialNumber();
	Thread thread(CAN_COM_thread);
	pcSerial.printf("Serial: %d\n",serialNr);
	while(1){
	
	}
}

void logger_loop(void const *args){
	Thread threadRec(CAN_COM_thread,NULL,osPriorityNormal);
	Thread threadPrn(CAN_PRINT_thread,NULL,osPriorityNormal);
	
	while(1){
		
		
	}
	
}

void CAN_PRINT_thread(void const *args) {
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
		start_CAN_Bus(SENSOR);
		pcSerial.printf("start\n");
		Thread threadRec(CAN_COM_thread,NULL,osPriorityNormal);
	#endif
	#ifdef LOG
		start_CAN_Bus(LOGGER);
		pcSerial.printf("start\n");
		Thread threadRec(CAN_COM_thread,NULL,osPriorityNormal);
		Thread threadPrn(CAN_PRINT_thread,NULL,osPriorityNormal);
	#endif
	while (1){
		#ifdef SEN
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
		}
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
