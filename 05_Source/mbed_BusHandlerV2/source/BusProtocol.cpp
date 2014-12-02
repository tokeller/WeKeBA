#include "BusProtocol.h"

CAN can1(p9, p10);
CANMessage msgIn;	

outputRingbuf recMsgs;

extern Queue <CANMessage, 50> inQueue;
extern Queue <CANMessage, 50> outQueue;
extern MemoryPool<CANMessage, 50> mpoolOutQueue;
extern MemoryPool<CANMessage, 50> mpoolInQueue;

extern Serial pcSerial;


int CAN_init(void){
	can1.reset();
	if(can1.frequency(1000000)){
		pcSerial.printf("Freq 1MHz\n");
	}
	can1.mode(CAN::Normal);
	can1.attach(&CANIRQHandler);
	recMsgs.count = 0;
	recMsgs.read_pos = 0;
	recMsgs.write_pos = 0;
	return 1;	
}

void CANIRQHandler(void){
  __disable_irq;
	if(can1.read(msgIn)){
		storeRecMessages(msgIn);
	}
	__enable_irq;
}

uint32_t storeRecMessages(CANMessage sentMsg){
	if (recMsgs.count < BUFFER_SIZE){
		recMsgs.queue[recMsgs.write_pos].len = sentMsg.len;
		recMsgs.queue[recMsgs.write_pos].format = sentMsg.format;
		recMsgs.queue[recMsgs.write_pos].id = sentMsg.id;
		memcpy(recMsgs.queue[recMsgs.write_pos].data,sentMsg.data,sentMsg.len);
		recMsgs.count++;
		if (recMsgs.write_pos < BUFFER_SIZE - 1) {
			recMsgs.write_pos++;
		} else {
			recMsgs.write_pos = 0;
		}
		
		
	} else {
		//pcSerial.printf("Sent messages buffer overflow\n");
	}	
	return 1;
}

CANMessage dequeueOutput (void){
	CANMessage msg;
	__disable_irq();
	if (recMsgs.count > 0){
		msg.format = recMsgs.queue[recMsgs.read_pos].format;
		msg.id = recMsgs.queue[recMsgs.read_pos].id;
		msg.len = recMsgs.queue[recMsgs.read_pos].len;
		msg.type = recMsgs.queue[recMsgs.read_pos].type;
		memcpy(msg.data,recMsgs.queue[recMsgs.read_pos].data,recMsgs.queue[recMsgs.read_pos].len);
		recMsgs.count--;
		if (recMsgs.read_pos < BUFFER_SIZE - 1){
			recMsgs.read_pos++;
		} else {
			recMsgs.read_pos = 0;
		}
	} else {
		msg.id = 0;
	}
	__enable_irq();
	return msg;
}

int sendMessage(CANMessage msg){
	return (can1.write(msg));
}
