#include "BusProtocol.h"

// Location for IAP commands (like reading the serial number)
#define IAP_LOCATION 0x1FFF1FF1
typedef void (*IAP)(unsigned long [], unsigned long[] );
IAP iap_entry = (IAP) IAP_LOCATION;
unsigned long command[5] = {0,0,0,0,0};
unsigned long result[5] = {0,0,0,0,0};

#define CAN_HDR_EXT 						0x1fffffff

//									P	Type	Mess-ID			Source Addr	Target Addr							
//Logger Filter			1	1000	0000	0000	0000	0000	0000	0001
//									18			00					00					01							
#define CAN_FILTER_LOGGER						0x18000001
#define CAN_FILTER_LOGGER_TEST						0x1C000001

//Sensor Filter			0	0000	0000	0000	0000	0001	____	____
//									00			00					01					<SensorID>							
#define CAN_FILTER_SENSOR						0x00000100
//Sensor BC-Filter	0	0000	0000	0000	0000	0001	1111	1111
//									00			00					01					ff							
#define CAN_FILTER_SENSOR_BC				0x180001ff

#define CAN_FILTER_SENSOR_TEST			0x18000102


//									0	0000	0000	0000	0000	0000	1111	1111
#define CAN_FILTER_RECEIVER						0x000000ff

//									0	0000	0000	0000	1111	1111	0000	0000	
#define CAN_FILTER_SENDER					0x0000ff00

CAN can(p9, p10);
CANMessage msg;	
CANMessage msgRead;

uint32_t deviceSerial = 0;	// store the devices serialID
uint32_t deviceMask = 0; 		// Mask for the CAN acceptance filter
uint32_t deviceID = 0; 			// only one byte for serial ID available!
deviceType_t deviceType;

volatile char counterInt = 0;

/** Structure of header:
	*	Bits 0-7:		Receiver address
	* Bits 8-15:	Sender address
	* Bits 16-23:	Message ID (either a sequence number for data or the number of the (first) package to ACK)
	* Bits 24-29:	Message type	
	*/
typedef enum {GET_SENSOR_SERIAL_HDR_BC = 0x000001ff, 
							SET_SENSOR_ID_HDR_BC     = 0x010001ff,
							TIME_SYNC_HDR_BC 				 = 0x020001ff,
			//				ACK_LOGGER_HDR_SGL 			 = 0x03000100,  // receiver address must be device ID
							SEND_TOKEN_HDR_SGL 			 = 0x04000100,  // receiver address must be device ID
							SENSOR_CONFIG_HDR_SGL    = 0x05000100,  // receiver address must be device ID	
							SENSOR_CONFIG_HDR_BC     = 0x060001ff,
							OP_MODE_HDR_SGL  				 = 0x07000100,  // receiver address must be device ID 
							OP_MODE_HDR_BC  				 = 0x080001ff,
							SERIAL_HDR_SGL 					 = 0x18000001,  // sender address must be 0 (serial is to long)
							ACK_SENSOR_HDR_SGL 			 = 0x19000001,  // sender address must be deviceID, received from logger
							RAW_DATA_HDR_SGL 				 = 0x1A000001,  // sender address must be deviceID, received from logger
							IMPACT_EXT_HDR_SGL 			 = 0x1B000001,  // sender address must be deviceID, received from logger
							IMPACT_STD_HDR_SGL 			 = 0x1C000001   // sender address must be deviceID, received from logger
							} LEADING_HDR;


static outputRingbuf outputHighPrio;
static outputRingbuf outputNormalPrio;

							
uint32_t queueOutput(CANMessage message){
	//
	if ((message.id & 0x1C000000) == 0x1C000000){
		if (outputNormalPrio.count < OUTPUTQUEUE_LEN){
			outputNormalPrio.queue[outputNormalPrio.write_pos].format = message.format;
			outputNormalPrio.queue[outputNormalPrio.write_pos].id = message.id;
			outputNormalPrio.queue[outputNormalPrio.write_pos].len = message.len;
			outputNormalPrio.queue[outputNormalPrio.write_pos].type = message.type;
			memcpy(outputNormalPrio.queue[outputNormalPrio.write_pos].data,message.data,message.len);
			outputNormalPrio.count++;
			if (outputNormalPrio.write_pos < OUTPUTQUEUE_LEN - 1){
				outputNormalPrio.write_pos++;
			} else {
				outputNormalPrio.write_pos = 0;
			}
		} else {
			//printf("Normal queue overflow\n");
		}
	} else {
		if (outputHighPrio.count < OUTPUTQUEUE_LEN){
			outputHighPrio.queue[outputHighPrio.write_pos].format = message.format;
			outputHighPrio.queue[outputHighPrio.write_pos].id = message.id;
			outputHighPrio.queue[outputHighPrio.write_pos].len = message.len;
			outputHighPrio.queue[outputHighPrio.write_pos].type = message.type;
			memcpy(outputHighPrio.queue[outputHighPrio.write_pos].data,message.data,message.len);
			outputHighPrio.count++;
			if (outputHighPrio.write_pos < OUTPUTQUEUE_LEN - 1){
				outputHighPrio.write_pos++;
			} else {
				outputHighPrio.write_pos = 0;
			}
		} else {
			//printf("High queue overflow\n");
		}
	}
	return 0;
}

CANMessage dequeueOutput (void){
	CANMessage msgOut;
	__disable_irq();
	// Inbound message with high prio received
	if (outputHighPrio.count > 0){
		msgOut.format = outputHighPrio.queue[outputHighPrio.read_pos].format;
		msgOut.id = outputHighPrio.queue[outputHighPrio.read_pos].id;
		msgOut.len = outputHighPrio.queue[outputHighPrio.read_pos].len;
		msgOut.type = outputHighPrio.queue[outputHighPrio.read_pos].type;
		memcpy(msgOut.data,outputHighPrio.queue[outputHighPrio.read_pos].data,outputHighPrio.queue[outputHighPrio.read_pos].len);
		outputHighPrio.count--;
		if (outputHighPrio.read_pos < 511){
			outputHighPrio.read_pos++;
		} else {
			outputHighPrio.read_pos = 0;
		}
	// no more high prio messages pending, check normal prio buffer
	} else if (outputNormalPrio.count > 0){
		msgOut.format = outputNormalPrio.queue[outputNormalPrio.read_pos].format;
		msgOut.id = outputNormalPrio.queue[outputNormalPrio.read_pos].id;
		msgOut.len = outputNormalPrio.queue[outputNormalPrio.read_pos].len;
		msgOut.type = outputNormalPrio.queue[outputNormalPrio.read_pos].type;
		memcpy(msgOut.data,outputNormalPrio.queue[outputNormalPrio.read_pos].data,outputNormalPrio.queue[outputNormalPrio.read_pos].len);
		outputNormalPrio.count--;
		if (outputNormalPrio.read_pos < 511){
			outputNormalPrio.read_pos++;
		} else {
			outputNormalPrio.read_pos = 0;
		}
	// no inbound messages awaiting processing, return a 0 msg
	} else {
		msgOut.id = 0x0000;		
		msgOut.len = 0x00;
	}
	__enable_irq();
	return msgOut;
	
}

/**	Function returning the serial number of a device. 
	* @return 	8 bytes of the full serial that are unique for each device
	*
	*/

uint32_t getSerialNumber(void){
    command[0] = 58;  // read device serial number
    iap_entry(command, result);
    if (result[0] == 0) {
				return (((result[1] & 0x0000ffff)<< 16) | (result[3] & 0x0000ffff));
    } else {
        return 0;
    };
};

	
/**
  * Setup acceptance filter on CAN
  */
void setCANFilter (uint32_t id)  {
  static int CAN_std_cnt = 0;
  static int CAN_ext_cnt = 0;
         uint32_t buf0, buf1;
         int cnt1, cnt2, bound1;

  /* Acceptance Filter Memory full */
  if ((((CAN_std_cnt + 1) >> 1) + CAN_ext_cnt) >= 512)
    return;                                       /* error: objects full */

  /* Setup Acceptance Filter Configuration 
    Acceptance Filter Mode Register = Off  */                                 
  LPC_CANAF->AFMR = 0x00000001;
                                   /* Add mask for extended identifiers */
  id |= (0 << 29);                        /* Add controller number */

  cnt1 = ((CAN_std_cnt + 1) >> 1);
  cnt2 = 0;
  while (cnt2 < CAN_ext_cnt)  {                /* Loop through extended existing masks */
    if (LPC_CANAF_RAM->mask[cnt1] > id)
      break;
    cnt1++;                                    /* cnt1 = U32 where to insert new mask */
    cnt2++;
  }

  buf0 = LPC_CANAF_RAM->mask[cnt1];            /* Remember current entry */
  LPC_CANAF_RAM->mask[cnt1] = id;              /* Insert mask */

  CAN_ext_cnt++;

  bound1 = CAN_ext_cnt - 1;
  /* Move all remaining extended mask entries one place up */
  while (cnt2 < bound1)  {
    cnt1++;
    cnt2++;
    buf1 = LPC_CANAF_RAM->mask[cnt1];
    LPC_CANAF_RAM->mask[cnt1] = buf0;
    buf0 = buf1;
  }        
   
  /* Calculate std ID start address (buf0) and ext ID start address (buf1) */
  buf0 = ((CAN_std_cnt + 1) >> 1) << 2;
  buf1 = buf0 + (CAN_ext_cnt << 2);

  /* Setup acceptance filter pointers */
  LPC_CANAF->SFF_sa     = 0;
  LPC_CANAF->SFF_GRP_sa = buf0;
  LPC_CANAF->EFF_sa     = buf0;
  LPC_CANAF->EFF_GRP_sa = buf1;
  LPC_CANAF->ENDofTable = buf1;

  LPC_CANAF->AFMR = 0x00000000;                  /* Use acceptance filter */
}

/**	Function to store a received message on the output queue
	*	
	*/
void storeReceivedMsg(void){
	__disable_irq();
	counterInt++;
	if (can.read(msg)){
		
		// store message on queue
		queueOutput(msg);
	}
	__enable_irq();
}

/**	Function to initialize the bus handler on CAN-port 1 as a specific device
	*		1. Get the device serial number, if not a logger
	*		2. 
	*	@param  device 							  Device type, either LOGGER or SENSOR
	*	@retval returnCode						returns 0 for sucess, 1 for failure
	*/
uint32_t init(deviceType_t device){
	can.reset();
	can.frequency(1000000);
	can.mode(CAN::Normal);
	can.attach(&storeReceivedMsg,CAN::RxIrq);
	deviceType = device;
	#ifdef FILTERON
		if (device == SENSOR){
			// setup CAN acceptance filter for broadcasts only (until a deviceID was received)
			//setCANFilter(CAN_FILTER_SENSOR_BC);
			setCANFilter(CAN_FILTER_SENSOR_TEST);
		} else {
			deviceSerial = 0x0001;
			// since the deviceID of the logger is always 0x01, we can set the final filter 
			setCANFilter(CAN_FILTER_LOGGER_TEST);
		}
	#endif
	return 0;
}

/**	Function to read a received message
	*	@retval message								returns the received message (or an empty, if none present)
	*/	
message_t readReceivedMsg(void){
	// get message from queue
	//printf("read msg\n");
	msgRead = dequeueOutput();
	message_t output;
	// message isn't empty, meaning a message was present
	if (msgRead.id != 0x0000 && msgRead.len != 0x00){
		printf("got one\n");
		printf("\n\nints called %d\n\n\n",counterInt);
		char len = msgRead.len & 0x0f;
		if (len >= 8){
			output.dataLength = 8;
		} else {
			output.dataLength = len;
		}
		uint32_t identifier = msgRead.id;
		output.msgId = identifier;
		switch (identifier & 0x1F000000){
			case GET_SENSOR_SERIAL_HDR_BC & 0x1F000000:
				output.msgType = GET_SENSOR_SERIAL_BC;
				break;
			case SET_SENSOR_ID_HDR_BC & 0x1F000000:
				output.msgType = SET_SENSOR_ID_BC;
				break;
			case TIME_SYNC_HDR_BC & 0x1F000000:
				output.msgType = TIME_SYNC_BC;
				break;
	/*		case ACK_LOGGER_HDR_SGL & 0x1F000000:
				output.msgType = ACK_LOGGER_SINGLE;
				break;*/
			case SEND_TOKEN_HDR_SGL & 0x1F000000:
				output.msgType = SEND_TOKEN_SINGLE;
				break;
			case SENSOR_CONFIG_HDR_SGL  & 0x1F000000:
				output.msgType = SENSOR_CONFIG_SINGLE;
				break;
			case SENSOR_CONFIG_HDR_BC & 0x1F000000:
				output.msgType = SENSOR_CONFIG_BC;
				break;
			case OP_MODE_HDR_SGL & 0x1F000000:
				output.msgType = OP_MODE_SINGLE;
				break;
			case OP_MODE_HDR_BC & 0x1F000000:
				output.msgType = OP_MODE_BC;
				break;
			case SERIAL_HDR_SGL & 0x1F000000:
				output.msgType = SERIAL_SINGLE;
				break;
			case ACK_SENSOR_HDR_SGL & 0x1F000000:
				output.msgType = ACK_SENSOR_SINGLE;
				break;		
			case RAW_DATA_HDR_SGL & 0x1F000000:
				output.msgType = RAW_DATA_SINGLE;
				break;
			case IMPACT_EXT_HDR_SGL & 0x1F000000:
				output.msgType = IMPACT_EXT_SINGLE;
				break;
			case IMPACT_STD_HDR_SGL & 0x1F000000:
				output.msgType = IMPACT_STD_SINGLE;
				break;
			default:
				break;
			}
			memcpy(output.payload,msgRead.data,msgRead.len);
		// prepare a NULL message
		}	else {
			output.msgType = NULL_MESSAGE;			
		}
	return output;
}


/**	Function to set the a CAN filter for a device
	*	@param  device 							  Device type, only SENSOR allowed (LOGGER will be set when calling init() and shouldn't
	*																have more than one filter)
	* @param 	devID									8 bit deviceID, received from the logger
	*	@retval returnCode						returns 0 for sucess, 1 for failure
	*/
uint32_t setFilterForID(deviceType_t device, uint32_t devID){
	if (device == SENSOR){
		deviceID = devID;
		// mask bits 8 till 31 of the deviceID, add them to the filter mask and 
		// setup the CAN acceptance filter with the proper deviceID
		setCANFilter(CAN_FILTER_SENSOR | (deviceID & CAN_FILTER_RECEIVER));
	} else {
		// the LOGGER already has his fixed deviceID and the filter was set. return an error
		return 1;
	}	
	return 0;
}

/**	Function to send a message
	* @param  message								Message to be sent
	*	@retval returnCode						returns 0 for sucess, 1 for failure
	*/
uint32_t sendMsg(message_t message){
	uint32_t msgIdHeader = 0;
	switch (message.msgType)
	{
		case GET_SENSOR_SERIAL_BC:
			msgIdHeader |= GET_SENSOR_SERIAL_HDR_BC;
			break;
		case SET_SENSOR_ID_BC:
			msgIdHeader |= SET_SENSOR_ID_HDR_BC;
			break;
		case TIME_SYNC_BC:
			msgIdHeader |= TIME_SYNC_HDR_BC;
			break;
		/*case ACK_LOGGER_SINGLE:
			msgIdHeader |= ACK_LOGGER_HDR_SGL;
			break;*/
		case SEND_TOKEN_SINGLE:
			msgIdHeader |= SEND_TOKEN_HDR_SGL;
			break;
		case SENSOR_CONFIG_SINGLE:
			msgIdHeader |= SENSOR_CONFIG_HDR_SGL;
			break;
		case SENSOR_CONFIG_BC:
			msgIdHeader |= SENSOR_CONFIG_HDR_BC;
			break;
		case OP_MODE_SINGLE:
			msgIdHeader |= OP_MODE_HDR_SGL;
			break;
		case OP_MODE_BC:
			msgIdHeader |= OP_MODE_HDR_BC;
			break;
		case SERIAL_SINGLE:
			msgIdHeader |= SERIAL_HDR_SGL;
			break;
		case ACK_SENSOR_SINGLE:
			msgIdHeader |= ACK_SENSOR_HDR_SGL;
			break;
		case RAW_DATA_SINGLE:
			msgIdHeader |= RAW_DATA_HDR_SGL;
			break;
		case IMPACT_EXT_SINGLE:
			msgIdHeader |= IMPACT_EXT_HDR_SGL;
			break;
		case IMPACT_STD_SINGLE:
			msgIdHeader |= IMPACT_STD_HDR_SGL;
			break;
		default:
			return 1;
	}
	msgIdHeader |= ((message.msgId << 16) & (deviceID << 8) & message.receiver);
	CANMessage msg(msgIdHeader, message.payload, message.dataLength ,CANData, CANExtended);
	
	if(can.write(msg)) {
		return 0;
	}else {
		return 1;
	}
}
