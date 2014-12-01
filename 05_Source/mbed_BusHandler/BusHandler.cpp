#include "BusHandler.h"

rtos::Queue <msgData_t, 50> inQueue;
rtos::Queue <message_t, 50> outQueue;
sentMsgBuffer sentMsgs;

uint32_t sendMessage(uint32_t dataLength, char *payload, char receiver, msgType_t msgType){
	osStatus stat;
	// if more than 8 bytes should be sent, several messages must be prepared
	if (dataLength <= 8){
		message_t msg;
		msg.receiver = receiver;
		msg.dataLength = dataLength;
		msg.msgType = msgType;
		memcpy(msg.payload, payload, dataLength);
		msg.msgId = 1;
		stat = outQueue.put(&msg);
		printf("payload %s\n",msg.payload+0x00);
	} else {
		uint8_t numberOfMessages = dataLength/ 8;
		uint8_t restSize = dataLength%8;
		if (restSize > 0){
			numberOfMessages +=1;
		};
		for (int i = 0; i<numberOfMessages; i++){
			message_t msg;
			msg.receiver = receiver;
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
		message_t inbMessage = readReceivedMsg();
		msgData_t retMessage;
		// Message from inbound queue received
		if (inbMessage.msgType != NULL_MESSAGE){
			printf("there was something!!!\n");
			// check the message type and process it accordingly
			switch (inbMessage.msgType){
				// the logger received an ACK from the sensor
				case ACK_SENSOR_SINGLE:
					
					break;
				// the logger received a serial number
				case SERIAL_SINGLE:
					
					break;
				
				// the logger received standard impact data (depending on the message id it's either
				// a real single impact or the start of a extended impact)
				case IMPACT_STD_SINGLE:
					printf("received impact std\n");
					// the counter (message identifier bits 16-23) is larger than 1, meaning this standard
					// message is just the start of an extended impact)
					if (((inbMessage.msgId >> 16) & 0xff) > 1){
						
					// just a standard impact, can be sent to the inQueue right away
					} else {
						retMessage = convertMessage(inbMessage);
						inQueue.put(&retMessage);
					}
					break;
				// the logger received an extended impact data (should follow a standard impact)
				case IMPACT_EXT_SINGLE:
					
					break;
				// the logger receives raw data (either continous or from a single impact)
				case RAW_DATA_SINGLE:
					
					break;
				// all other messages from sensors should be passed on via the inQueue
				default:
					
					break;
			}
				
			
			
			// osOK: no messages available on the queue, continue with outbound messages
		} else if (inbMessage.msgType == NULL_MESSAGE){
			// try to get an outbound message (from this unit)
			osEvent ev = outQueue.get(0);
			// Message from outbound queue received
			if (ev.status == osEventMessage){
				message_t outbound = *((message_t *) ev.value.p);
				if (sendMsg(outbound)==1){
					storeSentMessages(outbound);
				} else {
					printf("FAILED!!!!");
				}
				
				// osOK: no messages available on the queue, end this processing cycle
			} else if (ev.status == osOK){
				
				
			}
		}
	}
}

void processMessagesSensor(void const *args){
	deviceType_t dt = SENSOR;
	init(dt);
	setFilterForID(dt,0x02);
	printf("filter set\n");
	
	while (1){
		// try to get an inbound message (to this unit)
		message_t inbMessage = readReceivedMsg();
		msgData_t retMessage;
		// Message from inbound queue received
		if (inbMessage.msgType != NULL_MESSAGE){
			// check the message type and process it accordingly
			switch (inbMessage.msgType){
		/*		// the sensor received an ACK from the logger
				case ACK_LOGGER_SINGLE:*/
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
		} else if (inbMessage.msgType == NULL_MESSAGE){
			// try to get an outbound message (from this unit)
			osEvent ev = outQueue.get(0);
			// Message from outbound queue received
			if (ev.status == osEventMessage){
				message_t outbound = *((message_t *) ev.value.p);
				if (sendMsg(outbound)==0){
					//storeSentMessages(outbound);
					printf("sent from handler\n");
				} else {
					printf("FAILED!!!!");
				}
				
				// osOK: no messages available on the queue, end this processing cycle
			} else if (ev.status == osOK){
				
				
			}
		}
	}	
}

msgData_t getInboundMessage(void){
	msgData_t retVal;
	osEvent ev = inQueue.get(0);
	// Message from outbound queue received
	if (ev.status == osEventMessage){
		retVal = *((msgData_t *) ev.value.p);
	} else {
		retVal.msgType = NULL_MESSAGE;
	}	
	return retVal;
	
}



msgData_t convertMessage(message_t msg){
	msgData_t returnVal;
	returnVal.sender = (msg.msgId >> 8) & 0xff;
	returnVal.dataLength = msg.dataLength;
	returnVal.msgType = msg.msgType;
	for (int i = 0; i < msg.dataLength; i++){
		returnVal.payload[i] = msg.payload[i];
	}		
	return returnVal;	
}

uint32_t storeSentMessages(message_t sentMsg){
	if (sentMsgs.count < OUTPUTQUEUE_LEN){
		sentMsgs.queue[sentMsgs.write_pos].dataLength = sentMsg.dataLength;
		sentMsgs.queue[sentMsgs.write_pos].msgType = sentMsg.msgType;
		sentMsgs.queue[sentMsgs.write_pos].sender = sentMsg.msgId;
		for (int i = 0; i < sentMsg.dataLength;i++){
			sentMsgs.queue[sentMsgs.write_pos].payload[i] = sentMsg.payload[i];
		}
		
		sentMsgs.count++;
		if (sentMsgs.write_pos < SENTMSGBUFFER_LEN) {
			sentMsgs.write_pos++;
		} else {
			sentMsgs.write_pos = 0;
		}
		
		
	} else {
		printf("Sent messages buffer overflow\n");
	}	
}

uint32_t checkAckMessages(message_t sentMsg){
	 
	
}
