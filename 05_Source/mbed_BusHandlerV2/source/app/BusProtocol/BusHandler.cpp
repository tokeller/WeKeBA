#include "BusHandler.h"

extern Serial pcSerial;

#ifdef LOG
// the logger requires a large receiving buffer, but just only a small transmitting buffer
extern Queue <CANmessage_t, 800> outQueue;
extern MemoryPool<CANmessage_t, 800> mpoolOutQueue;
Queue <CANmessage_t, 50> inQueue;
MemoryPool<CANmessage_t, 50> mpoolInQueue;
#endif

#ifdef SEN
// the sensors will require a larger transmitting buffer, but they'll only receive few and small messages
extern Queue <CANmessage_t, 50> outQueue;
extern MemoryPool<CANmessage_t, 50> mpoolOutQueue;
Queue <CANmessage_t, 800> inQueue;
MemoryPool<CANmessage_t, 800> mpoolInQueue;
#endif


char sentData = 0;
// the token must be initialized with 1 for all sensors during the configuration phase. After the timesync,
// each sensor will revoke its token and wait till it gets one from the logger
char tokenReceived = 1;
// the messageCounter indicates, how many messages the logger will expect
uint64_t messageCounter = 0;


/*
 *	see header
 */
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

/*
 *	see header
 */
void CAN_COM_thread(void const *args) {
	CANmessage_t *outOnQueue;
	CANMessage sending;
	
	while (1) {
		if (tokenReceived == 1){
			osEvent evt = inQueue.get(0);
			if (evt.status == osEventMessage) {
				//pcSerial.printf("inbound\n");
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
				messageCounter--;
				// if all messages were sent, revoke the token
				if (messageCounter <= 0){
					setTokenStatus(0,0,0);
					pcSerial.printf("Token revoked\n");
				}
				mpoolInQueue.free(message);
			}
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

/*
 *	see header
 */

int enqueueMessage(uint32_t dataLength, ImpStd_t payload, char receiver, char sender, msgType_t msgType){
	char data[8];
	//printf("\n\ntimestamp %x\n",payload.timestamp);
	data[0] = ((payload.timestamp) >> 24) & 0xff;
	data[1] = ((payload.timestamp) >> 16) & 0xff;;
	data[2] = ((payload.timestamp) >> 8) & 0xff;;
	data[3] = (payload.timestamp) & 0xff;;
	//printf("packed timestamp %x%x%x%x\n",data[0],data[2],data[2],data[3]);
	data[4] = payload.maxPeaks;
	//printf("maxPeak %x\n",payload.maxPeaks);
	//printf("packed maxPeak %x\n",data[4]);
	data[5] = payload.nrOfPeaks;
	//printf("nrOfPeaks %x\n",payload.nrOfPeaks);
	//printf("packed nrPeak %x\n",data[5]);
	data[6] = (payload.duration>>8) & 0xff;
	data[7] =  payload.duration & 0xff;
	//printf("duration %x\n",payload.duration);
	//printf("packed duration %x\n",data[6]);
	sentData = 1;
	return enqueueMessage(8,data,receiver,sender,msgType);
}

/*
 *	see header
 */

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
		if(sentData == 1){
			printf("sent payload %x%x%x%x%x%x%x%x\n",inOnQueue->payload[0],
																						 inOnQueue->payload[1],
																						 inOnQueue->payload[2],
																						 inOnQueue->payload[3],
																						 inOnQueue->payload[4],
																						 inOnQueue->payload[5],
																						 inOnQueue->payload[6],
																						 inOnQueue->payload[7]);
			sentData = 0;
		}
		stat = inQueue.put(inOnQueue);
	} else {
		uint8_t numberOfMessages = dataLength/ 8;
		uint8_t restSize = dataLength%8;
		if (restSize > 0){
			numberOfMessages +=1;
		};
		for (int i = 0; i<numberOfMessages; i++){
			inOnQueue = mpoolInQueue.alloc();
			if(msgType == IMPACT_EXT_SINGLE && i == 0){
				inOnQueue->msgType = IMPACT_STD_SINGLE;
			} else {
				inOnQueue->msgType = msgType;
			}
			inOnQueue->receiver = receiver;
			inOnQueue->sender = sender;
			inOnQueue->msgId = numberOfMessages - i;
			if ((i == (numberOfMessages - 1)) && (restSize > 0)){
				inOnQueue->dataLength = restSize;
				memcpy(inOnQueue->payload,payload+(i*8),restSize);
				printf("payload multi part %s\n",inOnQueue->payload+0x00);
			}else{
				inOnQueue->dataLength =8;
				memcpy(inOnQueue->payload,payload+(i*8),8);
				printf("payload multi end %s\n",inOnQueue->payload+0x00);
			}
			stat = inQueue.put(inOnQueue);
		};
		
	};
	return 0;
}

/*
 *	see header
 */

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

/*
 *	see header
 */

void sendSerialResponse(uint32_t serialNr){
	char data[4];
	data[0] = (serialNr>> 24) & 0xff;
	data[1] = (serialNr>> 16) & 0xff;
	data[2] = (serialNr>> 8) & 0xff;
	data[3] = serialNr & 0xff;	
	enqueueMessage(4, data, 0x01, 0x00, SERIAL_SINGLE);
}

/*
 *	see header
 */

void enableBroadCastFilter(){
	setExtGrpCANFilter(CAN_FILTER_SENSOR_ID_GET_BC,CAN_FILTER_SENSOR_ID_GET_BC);
	setExtGrpCANFilter(CAN_FILTER_SENSOR_TIME_SYNC_BC,CAN_FILTER_SENSOR_TIME_SYNC_BC);
	setExtGrpCANFilter(CAN_FILTER_SENSOR_START_REC_BC,CAN_FILTER_SENSOR_START_REC_BC);
	setExtGrpCANFilter(CAN_FILTER_SENSOR_OFF_BC,CAN_FILTER_SENSOR_OFF_BC);
	setExtGrpCANFilter(CAN_FILTER_SENSOR_OP_MD_BC,CAN_FILTER_SENSOR_OP_MD_BC);
}

/*
 *	see header
 */

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

/*
 *	see header
 */

void sendSettings(uint16_t receiver, SensorConfigMsg_t settings){
	uint64_t data = settings.threshold;
	data = data<<10;
	data |= settings.started;
	data = data<<12;
	data |= settings.fs;
	data = data<<16;
	data |= settings.timeoutRange;
	data = data<<16;
	data |= settings.baseline;
	printf("\n\nbaseline:%x\n",settings.baseline);
	char part[8];	
	part[0] = (data >> 56) & 0xff;
	part[1] = (data >> 48) & 0xff;
	part[2] = (data >> 40) & 0xff;
	part[3] = (data >> 32) & 0xff;
	part[4] = (data >> 24) & 0xff;
	part[5] = (data >> 16) & 0xff;
	part[6] = (data >> 8)  & 0xff;
	part[7] =  data 			 & 0xff;
	enqueueMessage(8,part,receiver & 0xff,0x01,SENSOR_CONFIG_SINGLE);
}

/*
 *	see header
 */

int setTokenStatus(char status, char counter, uint64_t rawNrSamples){
	/*if (rawNrSamples > 0){
		messageCounter = rawNrSamples;
	} else {*/
		messageCounter = counter;
	//}
	tokenReceived = status;
	return 1;
}

/*
 *	see header
 */

void stopAllSensors(void){
	enqueueMessage(0,0,0xff,0x01,SENSOR_OFF_BC);
}

/*
 *	see header
 */

void stopSensor(uint8_t sensorId){
	enqueueMessage(0,0,sensorId,0x01,SENSOR_OFF_SINGLE);	
}

/*
 *	see header
 */

void emptyQueue(void){
	char queueNotEmptied = 1;
	while(queueNotEmptied){
		osEvent evt = inQueue.get(0);
		if (evt.status != osEventMessage){
			queueNotEmptied = 0;
		} else {
			CANmessage_t *message = (CANmessage_t*)evt.value.p;
			mpoolInQueue.free(message);
		}
	}
}

void resetTimestamp(void){
	enqueueMessage(0,0,0xff,0x01,TIME_SYNC_BC);
}


void start_sensor_raw(uint64_t nrOfSamples, SensorConfig sensor){
	// stop all sensors
	stopAllSensors();

	// send the changed config
	SensorConfigMsg_t cfg;	
	cfg.threshold = sensor.threshold;
	cfg.baseline = sensor.baseline;
	cfg.fs = sensor.fs;
	cfg.timeoutRange = sensor.timeout;
	uint16_t started = sensor.detail_level;
	started = started << 4;
	started |= 0x01 & 0x0f;
	cfg.started = started;
	sendSettings(sensor.sensor_ID, cfg);
	
	// send the token to the sensor.
	char payload[8];
	// set the number of packages to 255
	payload[0]= 0xff;
	// store the number of samples
	for (int i = 7; i > 2; i--){
		payload[i] = nrOfSamples & 0xff;
		nrOfSamples = nrOfSamples >> 8;
	}
	// take the number of samples and send them to the sensor
	enqueueMessage(8,payload,sensor.sensor_ID,0x01,SEND_TOKEN_SINGLE);
	// overwrite the nrOfMsgs on the logger
	
}