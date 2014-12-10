#include "BusHandler.h"

extern Serial pcSerial;

extern Queue <CANmessage_t, 50> outQueue;
extern MemoryPool<CANmessage_t, 50> mpoolOutQueue;

Queue <CANmessage_t, 50> inQueue;
MemoryPool<CANmessage_t, 50> mpoolInQueue;


int start_CAN_Bus(deviceType_t device){
	CAN_init();	
	if (device == LOGGER){
		setExtGrpCANFilter(CAN_FILTER_LOGGER_LOW, CAN_FILTER_LOGGER_UPPER);
	} else {
		// set the filter to only allow the ID request from the logger to pass
		setExtGrpCANFilter(CAN_FILTER_SENSOR_ID_REQ_BC,CAN_FILTER_SENSOR_ID_REQ_BC);
	}
	return 0;
}

void CAN_COM_thread(void const *args) {
	CANmessage_t *outOnQueue;
	CANMessage sending;
	
	while (1) {
		osEvent evt = inQueue.get(0);
		if (evt.status == osEventMessage) {
			pcSerial.printf("inbound\n");
			CANmessage_t *message = (CANmessage_t*)evt.value.p;
			memcpy(sending.data,message->payload,message->dataLength);
			sending.format = CANExtended;
			
			// prepareMsgId(msgType_t inMsgType, char inReceiver, char inSender, uint32_t inMsgId)
			sending.id = prepareMsgId(message->msgType,message->receiver,message->sender,message->msgId);
			sending.len = message->dataLength;
			sending.type = CANData;
			if(sendMessage(sending)){
				pcSerial.printf("OK\n");
			} else{
				pcSerial.printf("NOK\n");
			}
			mpoolInQueue.free(message);
		}		
		CANMessage data = dequeueOutput();
		if(data.id != 0){
			outOnQueue = mpoolOutQueue.alloc();
			memcpy(outOnQueue->payload,data.data,data.len);
			outOnQueue->msgId = data.id;
			outOnQueue->dataLength = data.len;
			outQueue.put(outOnQueue);
		}
  }
}


	//enqueueMessage(8,										data,					0x01,					0x02,  				IMPACT_STD_SINGLE)
int enqueueMessage(uint32_t dataLength, char *payload, char receiver, char sender, msgType_t msgType){
	CANmessage_t *inOnQueue;
	osStatus stat;
	// if more than 8 bytes should be sent, several messages must be prepared
	if (dataLength <= 8){
		inOnQueue = mpoolInQueue.alloc();
		inOnQueue->msgType = msgType;
		inOnQueue->receiver = receiver;
		inOnQueue->sender = sender;
		inOnQueue->msgId = 1;
		inOnQueue->dataLength = dataLength;
		memcpy(inOnQueue->payload,payload,dataLength);
		stat = inQueue.put(inOnQueue);
	} else {
		uint8_t numberOfMessages = dataLength/ 8;
		uint8_t restSize = dataLength%8;
		if (restSize > 0){
			numberOfMessages +=1;
		};
		for (int i = 0; i<numberOfMessages; i++){
			inOnQueue = mpoolInQueue.alloc();
			inOnQueue->msgType = msgType;
			inOnQueue->receiver = receiver;
			inOnQueue->sender = sender;
			inOnQueue->msgId = numberOfMessages - i;
			if ((i == (numberOfMessages - 1)) && (restSize > 0)){
				inOnQueue->dataLength = restSize;
				memcpy(inOnQueue->payload,payload+(i*8),restSize);
				//printf("payload multi part %s\n",inOnQueue->data+0x00);
			}else{
				inOnQueue->dataLength =8;
				memcpy(inOnQueue->payload,payload+(i*8),8);
				//printf("payload multi end %s\n",inOnQueue->payload+0x00);
			}
			stat = inQueue.put(inOnQueue);
		};
		
	};
	return 0;
}

uint32_t prepareMsgId(msgType_t inMsgType, char inReceiver, char inSender, uint32_t inMsgId){
	uint32_t msgId = 0;
	uint32_t pMsgType = inMsgType;
	uint32_t pReceiver = inReceiver;
	uint32_t pSender = inSender;
	uint32_t pMsgId = inMsgId;
	// id format (bits):
	//	29			:	Priority bit, 0 for logger, 1 for sensors		
	//	28 - 25	: Message type
	//	24 - 17	: Target address 
	//	16 - 9	: Source address
	//	8	 - 0	: Msg id, number of package in descending order (1 as lowest value)
	msgId = ((pMsgType <<24) | (pReceiver <<16) |	(pSender << 8) | pMsgId) & 0x1fffffff;
	pcSerial.printf("prepare message id: %0x\n",msgId);
	return msgId;
}

void sendSerialResponse(uint32_t serialNr){
	uint32_t outMsgId = 0;
	char data[4];
	data[0] = (serialNr>> 24) & 0xff;
	data[1] = (serialNr>> 16) & 0xff;
	data[2] = (serialNr>> 8) & 0xff;
	data[3] = serialNr & 0xff;	
	enqueueMessage(4, data, 0x01, 0x00, SERIAL_SINGLE);
}

void enableBroadCastFilter(){
	setExtGrpCANFilter(CAN_FILTER_SENSOR_ID_GET_BC,CAN_FILTER_SENSOR_ID_GET_BC);
	setExtGrpCANFilter(CAN_FILTER_SENSOR_TIME_SYNC_BC,CAN_FILTER_SENSOR_TIME_SYNC_BC);
	setExtGrpCANFilter(CAN_FILTER_SENSOR_START_REC_BC,CAN_FILTER_SENSOR_START_REC_BC);
	setExtGrpCANFilter(CAN_FILTER_SENSOR_OFF_BC,CAN_FILTER_SENSOR_OFF_BC);
	setExtGrpCANFilter(CAN_FILTER_SENSOR_OP_MD_BC,CAN_FILTER_SENSOR_OP_MD_BC);
}

void enableSensorFilter(uint8_t canId){
	uint32_t tempfilter = 0;
	uint32_t tempCanId = canId << 16;
	tempfilter = CAN_FILTER_SENSOR_TOKEN_SNGL | tempCanId;
	setExtGrpCANFilter(tempfilter,tempfilter);;
	
	tempfilter = CAN_FILTER_SENSOR_CONF_SNGL | tempCanId;
	setExtGrpCANFilter(tempfilter,tempfilter);;
	
	
	tempfilter = CAN_FILTER_SENSOR_OFF_SNGL | tempCanId;
	setExtGrpCANFilter(tempfilter,tempfilter);;
	
	
	tempfilter = CAN_FILTER_SENSOR_OP_MD_SNGL | tempCanId;
	setExtGrpCANFilter(tempfilter,tempfilter);;
	
	
};