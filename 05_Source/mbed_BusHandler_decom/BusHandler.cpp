#include "BusHandler.h"

Queue <msgData_t, 50> inQueue;
Queue <message_t, 50> outQueue;
MemoryPool<message_t, 50> mpool;
MemoryPool<msgData_t, 50> mpoolInQueue;
DigitalOut led1(LED1);
DigitalOut led2(LED2);

sentMsgBuffer sentMsgs;

int counter = 0;


uint32_t sendMessage(uint32_t dataLength, char *payload, char receiver, msgType_t msgType){
	osStatus stat;
	// if more than 8 bytes should be sent, several messages must be prepared
	if (dataLength <= 8){
		message_t *msg = mpool.alloc();
		msg->receiver = receiver;
		msg->dataLength = dataLength;
		msg->msgType = msgType;
		memcpy(msg->payload, payload, dataLength);
		msg->msgId = 1;
		stat = outQueue.put(msg);
		printf("payload single %s\n",msg->payload+0x00);
		printf("id single %d\n",msg->msgId);
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
				printf("payload multi part %s\n",msg.payload+0x00);
			}else{
				msg.dataLength =8;
				memcpy(msg.payload,payload+(i*8),8);
				printf("payload multi end %s\n",msg.payload+0x00);
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
		msgData_t *retMessage;
		// Message from inbound queue received
		if (inbMessage.msgType != NULL_MESSAGE){
			//printf("there was something!!!\n");
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
					//printf("received impact std\n");
					// the counter (message identifier bits 16-23) is larger than 1, meaning this standard
					// message is just the start of an extended impact)
					if (((inbMessage.msgId >> 16) & 0xff) > 1){
						
					// just a standard impact, can be sent to the inQueue right away
					} else {
						retMessage = mpoolInQueue.alloc();
						msgData_t resp = convertMessage(inbMessage);
						retMessage->dataLength = resp.dataLength;
						retMessage->msgType = resp.msgType;
						retMessage->payload = resp.payload;
						retMessage->sender = resp.sender;
						inQueue.put(retMessage);
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
				message_t *outbound = (message_t *) ev.value.p;
				printf("outbound id: %d\n",outbound->msgId);
				printf("outbound len: %d\n",outbound->dataLength);
				printf("outbound rec: %d\n",outbound->receiver);
				if (sendMsg(*outbound)==1){
					storeSentMessages(*outbound);
				} else {
					printf("FAILED!!!!");
				}
				mpool.free(outbound);
				// osOK: no messages available on the queue, end this processing cycle
			} else if (ev.status == osOK){
				
				
			}
		}
		led1 = !led1;
		led2 = !led2;
		
	}
}

void processMessagesSensor(void const *args){
	deviceType_t dt = SENSOR;
	init(dt);
	#ifdef FILTERON
		setFilterForID(dt,0x02);
		printf("filter set\n");
	#endif
	while (1){
		// try to get an inbound message (to this unit)
		message_t inbMessage = readReceivedMsg();
		msgData_t *retMessage;
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
					printf("msg id: %d\n",outbound.msgId);
					printf("data %d%d%d%d%d%d%d%d\n",outbound.payload[0],outbound.payload[1],outbound.payload[2],outbound.payload[3],outbound.payload[4],outbound.payload[5],outbound.payload[6],outbound.payload[7]);
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
	msgData_t *resp;
	msgData_t retVal;
	osEvent ev = inQueue.get(0);
	// Message from outbound queue received
	if (ev.status == osEventMessage){
		counter++;
		//retVal = *((msgData_t *) ev.value.p);
		resp = (msgData_t *) ev.value.p;
		printf("get inbound with queue event called: %d\n",counter);
		printf("retVal sender: %d\n",resp->sender);
		printf("retVal type: %d\n",resp->msgType);
		printf("retVal len: %d\n",resp->dataLength);
		printf("retVal data: %d%d%d%d%d%d%d%d\n",resp->payload[0],
																						 resp->payload[1],
																						 resp->payload[2],
																						 resp->payload[3],
																						 resp->payload[4],
																						 resp->payload[5],
																						 resp->payload[6],
																						 resp->payload[7]);		
		retVal.dataLength = resp->dataLength;
		retVal.msgType = resp->msgType;
		retVal.payload[0] = resp->payload[0];
		retVal.payload[1] = resp->payload[1];
		retVal.payload[2] = resp->payload[2];
		retVal.payload[3] = resp->payload[3];
		retVal.payload[4] = resp->payload[4];
		retVal.payload[5] = resp->payload[5];
		retVal.payload[6] = resp->payload[6];
		retVal.payload[7] = resp->payload[7];
		retVal.sender = resp->sender;
		mpoolInQueue.free(resp);
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
	char buffer[returnVal.dataLength];
	for (int i = 0; i < msg.dataLength; i++){
		 buffer[i] = msg.payload[i];
	}	
	printf("data received: %s\n",buffer+0x00);
	returnVal.payload = buffer;
	return returnVal;	
}

uint32_t storeSentMessages(message_t sentMsg){
	if (sentMsgs.count < SENTMSGBUFFER_LEN){
		sentMsgs.queue[sentMsgs.write_pos].dataLength = sentMsg.dataLength;
		sentMsgs.queue[sentMsgs.write_pos].msgType = sentMsg.msgType;
		sentMsgs.queue[sentMsgs.write_pos].sender = sentMsg.msgId;
		memcpy(sentMsgs.queue[sentMsgs.write_pos].payload,sentMsg.payload,sentMsg.dataLength);
		sentMsgs.count++;
		if (sentMsgs.write_pos < SENTMSGBUFFER_LEN - 1) {
			sentMsgs.write_pos++;
		} else {
			sentMsgs.write_pos = 0;
		}
		return 1;
		
	} else {
		printf("Sent messages buffer overflow\n");
		return 0;
	}	
}

uint32_t checkAckMessages(message_t sentMsg){
	 
		return 1;
}
