#include "BusHandler.h"

rtos::Queue <message_t, 50> inQueue;
rtos::Queue <message_t, 50> outQueue;

uint32_t sendMessage(uint32_t dataLength, char *payload, char receiver, msgType_t msgType){
	osStatus stat;
	// if more than 8 bytes should be sent, several messages must be prepared
	if (dataLength <= 8){
		message_t msg;
		msg.dataLength = dataLength;
		msg.msgType = msgType;
		memcpy(msg.payload, payload, dataLength);
		msg.msgId = 1;
		stat = outQueue.put(&msg);
	} else {
		uint8_t numberOfMessages = dataLength/ 8;
		uint8_t restSize = dataLength%8;
		if (restSize > 0){
			numberOfMessages +=1;
		};
		for (int i = 0; i<numberOfMessages; i++){
			message_t msg;
			msg.msgType = msgType;
			msg.msgId = numberOfMessages - i;
			if ((i == (numberOfMessages - 1)) && (restSize > 0)){
				msg.dataLength =restSize;
				memcpy(msg.payload,payload+(i*8),restSize);
				printf("payload %s\n",msg.payload+0x00);
			}else{
				msg.dataLength =8;
				memcpy(msg.payload,payload+(i*8),8);
				printf("payload %s\n",msg.payload+0x00);
			}
			stat = outQueue.put(&msg);
		};
		
	};
	
	return 0;
}


void processMessagesLogger(void const *args){
	deviceType_t dt = LOGGER;
	init(dt);
	printf("thread gugus\n");
	
	while (1){
		// try to get an inbound message (to this unit)
		osEvent ev = inQueue.get(0);
		// Message from inbound queue received
		if (ev.status == osEventMessage){
			message_t inbound = *((message_t *) ev.value.p);
			
			switch (inbound.msgType){
				// the logger received an ACK from the sensor
				case ACK_SENSOR_SINGLE:
				
					break;
				// all other messages from sensors should be passed on via the inQueue
				default:
					break;
			}
				
			
			
			// osOK: no messages available on the queue, continue with outbound messages
		} else if (ev.status == osOK){
			// try to get an outbound message (from this unit)
			ev = outQueue.get(0);
			// Message from outbound queue received
			if (ev.status == osEventMessage){
				message_t outbound = *((message_t *) ev.value.p);
				sendMsg(outbound);
				
				// osOK: no messages available on the queue, end this processing cycle
			} else if (ev.status == osOK){
				
				
			}
		}
	}
}

void processMessagesSensor(void const *args){
	deviceType_t dt = SENSOR;
	init(dt);
	
	while (1){
		// try to get an inbound message (to this unit)
		osEvent ev = inQueue.get(0);
		// Message from inbound queue received
		if (ev.status == osEventMessage){
			message_t inbound = *((message_t *) ev.value.p);
			switch (inbound.msgType){
				// the sensor received an ACK from the logger
				case ACK_LOGGER_SINGLE:
				// the sensor received a request to send the serial to the logger
				case GET_SENSOR_SERIAL_BC:
				// the sensor received a sensor id from the logger
				case SET_SENSOR_ID_BC:
				// the sensor received the token
				case SEND_TOKEN_SINGLE:
					break;
				// all other messages from sensors should be passed on via the inQueue
				default:
					break;
			}
			
			// osOK: no messages available on the queue, continue with outbound messages
		} else if (ev.status == osOK){
			// try to get an outbound message (from this unit)
			ev = outQueue.get(0);
			// Message from outbound queue received
			if (ev.status == osEventMessage){
			message_t outbound = *((message_t *) ev.value.p);
				
				
				// osOK: no messages available on the queue, end this processing cycle
			} else if (ev.status == osOK){
				
				
			}
		}
	}
}
