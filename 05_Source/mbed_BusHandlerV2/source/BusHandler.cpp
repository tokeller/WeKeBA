#include "BusHandler.h"

extern Serial pcSerial;

Queue <CANMessage, 50> inQueue;
Queue <CANMessage, 50> outQueue;
MemoryPool<CANMessage, 50> mpoolOutQueue;
MemoryPool<CANMessage, 50> mpoolInQueue;

DigitalOut led1(LED1);
DigitalOut led2(LED2);
char counter = 0;

int start_CAN_Bus(void){
	CAN_init();	
}

void CAN_REC_thread(void const *args) {
	CANMessage *outOnQueue;
	while (1) {
		CANMessage data = dequeueOutput();
		if(data.id != 0){
			outOnQueue = mpoolOutQueue.alloc();
			memcpy(outOnQueue->data,data.data,data.len);
			outOnQueue->format = data.format;
			outOnQueue->id = data.id;
			outOnQueue->len = data.len;
			outQueue.put(outOnQueue);
		}
  }
}

void CAN_PRINT_thread(void const *args) {
	while (true) {
		osEvent evt = outQueue.get();
		if (evt.status == osEventMessage) {
				CANMessage *message = (CANMessage*)evt.value.p;
				printf("Out data: %d%d%d%d%d%d%d%d \n\r", message->data[0]
																								, message->data[1]
																								, message->data[2]
																								, message->data[3]
																								, message->data[4]
																								, message->data[5]
																								, message->data[6]
																								, message->data[7]);
				printf("Out id  : %d \n\r", message->id);
				printf("Out len : %d \n\r", message->len);
				mpoolOutQueue.free(message);
		}
	}
}


void CAN_thread(void const *args) {
		int id = 1337;
		CANMessage msg;
		while (true) {
			for (int i = 0; i <10; i++){
        led2 = !led2;
				msg.data[0] = 0xff;
				msg.data[1] = 0;
				msg.data[2] = 0xff;
				msg.data[3] = 0;
				msg.data[4] = 0xff;
				msg.data[5] = 0;
				msg.data[6] = 0xff;
				msg.data[7] = counter&0x7ff;
				msg.len = 8;
				msg.id = id;
				msg.format = CANStandard;
				msg.type = CANData;
        if(sendMessage(msg)) {
					printf("package\n\n\n\n");
					counter++;
					pcSerial.printf("Data sent: %d", msg.data[0]);
					pcSerial.printf("%d", msg.data[1]);
					pcSerial.printf("%d", msg.data[2]);
					pcSerial.printf("%d", msg.data[3]);
					pcSerial.printf("%d", msg.data[4]);
					pcSerial.printf("%d", msg.data[5]);
					pcSerial.printf("%d", msg.data[6]);
					pcSerial.printf("%d\n", msg.data[7]);
					pcSerial.printf("Id sent: %d\n", msg.id);
					pcSerial.printf("len sent: %d\n", msg.len);
					pcSerial.printf("format sent: %d\n", msg.format);
					pcSerial.printf("type sent: %d\n", msg.type);
					id++;
        } 
        osDelay(2000);
			}
    }
}
