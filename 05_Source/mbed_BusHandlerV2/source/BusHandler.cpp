#include "BusHandler.h"


//									P	Type	Target Addr	Source Addr	Mess-ID							
//Logger Filter			1	1xxx	xxxx	xxxx	xxxx	xxxx	xxxx	xxxx
//									18			00					00					00							
#define CAN_FILTER_LOGGER								0x18000000

//Sensor BC-Filter	0	0000	1111	1111	0000	0001	0000	0001
//									00			ff					01					01							
#define CAN_FILTER_SENSOR_ID_REQ_BC			0x00ff0101
//Sensor BC-Filter	0	0001	1111	1111	0000	0001	0000	0001
//									01			ff					01					01							
#define CAN_FILTER_SENSOR_ID_GET_BC			0x01ff0101
//Sensor BC-Filter	0	0010	1111	1111	0000	0001	0000	0001
//									02			ff					01					01							
#define CAN_FILTER_SENSOR_TIME_SYNC_BC	0x02ff0101
//Sensor BC-Filter	0	0011	1111	1111	0000	0001	0000	0001
//									03			ff					01					01							
#define CAN_FILTER_SENSOR_START_REC_BC	0x03ff0101
//Sensor BC-Filter	0	0011	1111	1111	0000	0001	0000	0001
//									03			ff					01					01							
#define CAN_FILTER_SENSOR_START_REC_BC	0x03ff0101


//Sensor Filter			0	0100	____	____	0000	0001	0000	0001
//									04			<SensorID>	01					01							
#define CAN_FILTER_SENSOR_TOKEN_SNGL		0x04000101
//Sensor Filter			0	0101	____	____	0000	0001	0000	0001
//									05			<SensorID>	01					01							
#define CAN_FILTER_SENSOR_CONF_SNGL		  0x05000101
//Sensor Filter			0	0110	____	____	0000	0001	0000	0001
//									06			<SensorID>	01					01							
#define CAN_FILTER_SENSOR_OFF_SNGL		  0x06000101


//Sensor BC-Filter	0	0111	1111	1111	0000	0001	0000	0001
//									07			ff					01					01							
#define CAN_FILTER_SENSOR_OFF_BC				0x07ff0101

//Sensor Filter			0	1000	____	____	0000	0001	0000	0001
//									08			<SensorID>	01					01							
#define CAN_FILTER_SENSOR_OP_MD_SNGL		0x08000101


//Sensor Filter			0	1001	1111	1111	0000	0001	0000	0001
//									08			ff					01					01							
#define CAN_FILTER_SENSOR_OP_MD_BC  		0x08ff0101


extern Serial pcSerial;

extern Queue <CANmessage_t, 50> outQueue;
extern MemoryPool<CANmessage_t, 50> mpoolOutQueue;

Queue <CANmessage_t, 50> inQueue;
MemoryPool<CANmessage_t, 50> mpoolInQueue;


int start_CAN_Bus(deviceType_t device){
	CAN_init();	
	if (device == LOGGER){
		setExtGrpCANFilter(CAN_FILTER_LOGGER);
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
	//	24 - 17	: Msg id, number of package in descending order (1 as lowest value)
	//	16 - 9	: Source address
	//	8	 - 0	: Target address
	msgId = ((pMsgType <<24) | (pMsgId<<16) |	(pSender << 8) | pReceiver) & 0x1fffffff;
	pcSerial.printf("message id: %0x\n",msgId);
	return msgId;
}
