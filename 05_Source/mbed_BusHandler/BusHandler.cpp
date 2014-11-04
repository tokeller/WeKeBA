#include "BusHandler.h"

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

//Sensor Filter			0	0000	0000	0000	0000	0001	____	____
//									00			00					01					<SensorID>							
#define CAN_FILTER_SENSOR						0x00000100
//Sensor BC-Filter	0	0000	0000	0000	0000	0001	1111	1111
//									00			00					01					ff							
#define CAN_FILTER_SENSOR_BC				0x180001ff


//									0	0000	0000	0000	0000	0000	1111	1111
#define CAN_FILTER_RECEIVER						0x000000ff

//									0	0000	0000	0000	1111	1111	0000	0000	
#define CAN_FILTER_SENDER					0x0000ff00


CAN can(p9, p10);
uint32_t deviceSerial = 0;	// store the devices serialID
uint32_t deviceMask = 0; 		// Mask for the CAN acceptance filter
uint32_t deviceID = 0; 			// only one byte for serial ID available!
deviceType_t deviceType;

/** Structure of header:
	*	Bits 0-7:		Receiver address
	* Bits 8-15:	Sender address
	* Bits 16-23:	Message ID (either a sequence number for data or the number of the (first) package to ACK)
	* Bits 24-29:	Message type	
	*/
typedef enum {GET_SENSOR_SERIAL_HDR_BC = 0x000001ff, 
							SET_SENSOR_ID_HDR_BC     = 0x010001ff,
							SENSOR_CONFIG_HDR_SGL    = 0x02000100,  // receiver address must be device ID	
							SENSOR_CONFIG_HDR_BC     = 0x030001ff,
							OP_MODE_HDR_SGL  				 = 0x04000100,  // receiver address must be device ID 
							OP_MODE_HDR_BC  				 = 0x050001ff,
							SEND_TOKEN_HDR_SGL 			 = 0x06000100,  // receiver address must be device ID
							ACK_LOGGER_HDR_SGL 			 = 0x07000100,  // receiver address must be device ID
							TIME_SYNC_HDR_BC 				 = 0x080001ff,
							SERIAL_HDR_SGL 					 = 0x18000001,  // sender address must be 0 (serial is to long)
							RAW_DATA_HDR_SGL 				 = 0x19000001,  // sender address must be deviceID, received from logger
							IMPACT_EXT_HDR_SGL 			 = 0x1A000001,  // sender address must be deviceID, received from logger
							IMPACT_STD_HDR_SGL 			 = 0x1B000001,  // sender address must be deviceID, received from logger
							ACK_SENSOR_HDR_SGL 			 = 0x1D000001   // sender address must be deviceID, received from logger
							} LEADING_HDR;


							
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

/** Function to get the serial number of all attached sensor units
	*
	*
	*/

uint32_t getSerialBroadcast(void){
		
		return (GET_SENSOR_SERIAL_HDR_BC & CAN_HDR_EXT);
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

/**	Function to initialize the bus handler on CAN-port 1 as a specific device
	*		1. Get the device serial number, if not a logger
	*		2. 
	*	@param  device 							  Device type, either LOGGER or SENSOR
	*	@retval returnCode						returns 0 for sucess, 1 for failure
	*/
uint32_t init(deviceType_t device){
	deviceType = device;
	if (device == SENSOR){
		deviceSerial = getSerialNumber();
		// setup CAN acceptance filter for broadcasts only (until a deviceID was received)
		setCANFilter(CAN_FILTER_SENSOR_BC);
	} else {
		deviceSerial = 0x0001;
		// since the deviceID of the logger is always 0x01, we can set the final filter 
		setCANFilter(CAN_FILTER_LOGGER);
	}
	return 0;
}

/**	Function to register a message handler function and a message buffer
	*	@param  processMsg 						Message handling procedure
	* @param  msgBuffer							Buffer for storing received messages
	*	@retval returnCode						returns 0 for sucess, 1 for failure
	*/
uint32_t registerMsgHandler(void (*processMsg)(), message_t *msgBuffer){
	can.attach(processMsg);
	return 0;
}

/**	Function to set the a CAN filter for a device
	*	@param  device 							  Device type, only SENSOR allowed (LOGGER will be set when calling init() and shouldn't
	*																have more than one filter
	* @param 	deviceID							8 bit deviceID, received from the logger
	*	@retval returnCode						returns 0 for sucess, 1 for failure
	*/
uint32_t setFilterForID(deviceType_t device, uint32_t deviceID){
	if (device == SENSOR){
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
	* @param  message								Payload to be sent
	*	@retval returnCode						returns 0 for sucess, 1 for failure
	*/
uint32_t sendMsg(message_t message){
	
	
	char data[8] = {68,65,66,67,68,69,66,0};
	CANMessage msg(CAN_FILTER_SENSOR_BC, data, 8 ,CANData, CANExtended);
	//CANMessage msg(CAN_FILTER_RECEIVER, data, 8 ,CANData, CANExtended);
	
	if(can.write(msg)) {
			printf("Sent!");
	}
	return 0;
}
