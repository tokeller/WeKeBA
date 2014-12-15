#ifndef LPC4088_CAN_HANDLER
#define LPC4088_CAN_HANDLER

#include "BusProtocol.h"

/*									P	Type	Target Addr	Source Addr	Mess-ID							
 *Logger Filter			1	1xxx	xxxx	xxxx	xxxx	xxxx	xxxx	xxxx
 * 									18			00					00					00							
 */
#define CAN_FILTER_LOGGER_LOW  					0x18000000
/*									P	Type	Target Addr	Source Addr	Mess-ID							
 *Logger Filter			1	1101	1111	1111	1111	1111	1111	1111
 * 									1D			ff					ff					ff							
 */
#define CAN_FILTER_LOGGER_UPPER 				0x1dffffff




/*
 *Sensor BC-Filter	0	0000	1111	1111	0000	0001	0000	0001
 *									00			ff					01					01							
 */
#define CAN_FILTER_SENSOR_ID_REQ_BC			0x00ff0101
/*
 *Sensor BC-Filter	0	0001	1111	1111	0000	0001	0000	0001
 *									01			ff					01					01							
 */
#define CAN_FILTER_SENSOR_ID_GET_BC			0x01ff0101
/*
 *Sensor BC-Filter	0	0010	1111	1111	0000	0001	0000	0001
 *									02			ff					01					01							
 */
#define CAN_FILTER_SENSOR_TIME_SYNC_BC	0x02ff0101
/*
 *Sensor BC-Filter	0	0011	1111	1111	0000	0001	0000	0001
 *									03			ff					01					01							
 */
#define CAN_FILTER_SENSOR_START_REC_BC	0x03ff0101
/*
 *Sensor BC-Filter	0	0111	1111	1111	0000	0001	0000	0001
 *									07			ff					01					01							
 */
#define CAN_FILTER_SENSOR_OFF_BC				0x07ff0101
/*
 *Sensor Filter			0	1001	1111	1111	0000	0001	0000	0001
 *									09			ff					01					01							
 */
#define CAN_FILTER_SENSOR_OP_MD_BC  		0x09ff0101


//Sensor Filter			0	0100	____	____	0000	0001	0000	0001
//									04			<SensorID>	01					01							
#define CAN_FILTER_SENSOR_TOKEN_SNGL		0x04000101
//Sensor Filter			0	0101	____	____	0000	0001	0000	0001
//									05			<SensorID>	01					01							
#define CAN_FILTER_SENSOR_CONF_SNGL		  0x05000101
//Sensor Filter			0	0110	____	____	0000	0001	0000	0001
//									06			<SensorID>	01					01							
#define CAN_FILTER_SENSOR_OFF_SNGL		  0x06000101
//Sensor Filter			0	1000	____	____	0000	0001	0000	0001
//									08			<SensorID>	01					01							
#define CAN_FILTER_SENSOR_OP_MD_SNGL		0x08000101

#define CAN_ID_MSG 			0x01ff0101
#define TIME_SYNC_MSG 	0x02ff0101
#define START_REC_MSG		0x03ff0101
#define SETTINGS_MSG  	0x05000101

#define SERIAL_MSG			0x18010001

#define MAX_NR_OF_MESSAGES							0xff


typedef struct {
	msgType_t msgType;
	uint32_t	msgId;
	uint32_t  sender;
	uint32_t	receiver;
	char			dataLength;
	char		  payload[8];
} CANmessage_t;

typedef struct {
	uint16_t threshold;			// 10 bit threshold
	uint16_t baseline; 			// 10 bit zero level
	uint16_t fs; 						// 12 bit sampling rate (100 Hz steps)
	uint16_t timeoutRange;	// 16 bit timeout range
} SensorConfigMsg_t;

typedef struct{
	uint8_t  canID;					// 8 bit, CAN identifier, send from logger to the sensor
	uint32_t serialID;			// 32 bit, serial ID of the requestor
} CANId_t;

typedef struct{
	uint8_t  nrOfPackages;	// 8 bit, max number of packages that the logger will allow
} Token_t;

typedef struct{
	uint8_t  opMode;				// 8 bit, operation mode (Detailed, Raw, Standard...)
} OpMode_t;

typedef struct{
	uint32_t serialNr;			// 32 bit, unique part of the chips serial number
} Serial_t;

typedef struct{
	uint16_t nrOfRdyPkg;		// 16 bit, number of packages the sensor will send
	uint32_t currentAmtPkg;	// 24 bit, amount of packages the sensor has currently stored
} TokenAck_t;

typedef struct{
	uint32_t timestamp;			// 32 bit, timestamp of first package. one tick = 100us
	uint8_t  value[4];			// 32 bit, first 4 values of raw data
} ImpRawDataStart_t;

typedef struct{
	uint8_t  value[8];			// 64 bit, values of raw data
} ImpRawData_t;

typedef struct{
	uint32_t timestamp;			// 32 bit, timestamp of impact. one tick = 100us
	uint8_t	 maxPeaks;			// 8 bit, value of highest peak
	uint8_t  nrOfPeaks;			// 8 bit, number of peaks in impact
	uint8_t	 duration;			// 8 bit, duration of impact (in 100us steps)
} ImpStd_t;

typedef struct{
	uint8_t  offsetP1;			// 8 bit, time offset of peak 1
	uint8_t  valueP1;				// 8 bit, value of peak 1
	uint8_t  offsetP2;			// 8 bit, time offset of peak 2
	uint8_t  valueP2;				// 8 bit, value of peak 2
	uint8_t  offsetP3;			// 8 bit, time offset of peak 3
	uint8_t  valueP3;				// 8 bit, value of peak 3
	uint8_t  offsetP4;			// 8 bit, time offset of peak 4
	uint8_t  valueP4;				// 8 bit, value of peak 4
} ImpExtData_t;

int start_CAN_Bus(deviceType_t device);

int enqueueMessage(uint32_t dataLength, ImpStd_t payload, char receiver, char sender, msgType_t msgType);

int enqueueMessage(uint32_t dataLength, char *payload, char receiver, char sender, msgType_t msgType);

void CAN_COM_thread(void const *args);

void CAN_PRINT_thread(void const *args);

uint32_t prepareMsgId(msgType_t inMsgType, char inReceiver, char inSender, uint32_t inMsgId);

void sendSerialResponse(uint32_t serialNr);

void enableBroadCastFilter(void);

void enableSensorFilter(uint8_t canId);

void sendSettings(char receiver, SensorConfigMsg_t settings);

#endif
