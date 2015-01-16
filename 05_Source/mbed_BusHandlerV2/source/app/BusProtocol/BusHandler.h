#ifndef LPC4088_CAN_HANDLER 
#define LPC4088_CAN_HANDLER

#include "BusProtocol.h"

//#define SEN

#define LOG
// left sensor  150576ea
// right sensor 110b7676

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
#define ALL_OFF_MSG			0x07ff0101


#define TOKEN_MSG				0x04000101
#define SETTINGS_MSG  	0x05000101
#define SINGLE_OFF_MSG  0x06000101

#define SERIAL_MSG			0x18010001

#define IMP_SPARSE_MSG		0x1c010000
#define IMP_PEAKS_MSG			0x1b010000
#define IMP_DETAILED_MSG  0x1a010000
#define IMP_RAW_MSG				0x1fffffff


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
	uint16_t started;  			// 10 bit flags, [0] indicating if started. rest reserved bits
	uint16_t fs; 						// 12 bit sampling rate (100 Hz steps)
	uint16_t timeoutRange;	// 16 bit timeout range
	uint16_t baseline; 			// 16 bit zero level
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
	uint8_t	 numberOfPkgs;	// 8 bit, number of packages (including the starter one)
	uint32_t timestamp;			// 32 bit, timestamp of first package. one tick = 100us
	uint8_t  value[4];			// 32 bit, first 4 values of raw data
} ImpRawDataStart_t;

typedef struct{
	uint8_t  value[8];			// 64 bit, values of raw data
} ImpRawData_t;

typedef struct{
	uint8_t  numberOfPkgs;  // 8 bit, number of packages (1 for std, < 255 for extended)
	uint32_t timestamp;			// 32 bit, timestamp of impact. one tick = 100us
	uint8_t	 maxPeaks;			// 8 bit, value of highest peak
	uint8_t  nrOfPeaks;			// 8 bit, number of peaks in impact
	uint16_t duration;			// 16 bit, duration of impact (in 100us steps)
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

/*
 * prepare the CAN bus for operation
 * @params deviceType_t device: type of device initiating the communication
 *
 *	Logger: Set the group filters to allow all messages from the sensors
 *	Sensor: Set the first filter to only allow a serial request from the logger to pass
 */
int start_CAN_Bus(deviceType_t device);


/*
 *	enqueue a standard impact message 
 *	
 *   pack a standard impact event together and put it on the in-queue
 */
int enqueueMessage(uint32_t dataLength, ImpStd_t payload, char receiver, char sender, msgType_t msgType);

/*
 *	enqueue a data message
 *
 *	 take a undefined payload (may exceed the limit of 8 bytes, several messages will be prepared) and 
 *	 prepare it for sending, before putting it on the in-queue
 */

int enqueueMessage(uint32_t dataLength, char *payload, char receiver, char sender, msgType_t msgType);

/*
 *	Communication thread
 *
 *	 Thread handling the communication by reading messages from the in-queue and send it to the bus or by
 *	 processing received messages from the buffer and put them on the out-queue
 */

void CAN_COM_thread(void const *args);

/*
 *	Function to prepare the message ID
 *
 */

uint32_t prepareMsgId(msgType_t inMsgType, char inReceiver, char inSender, uint32_t inMsgId);

/*
 *	send the serial number of a sensor to the logger
 *
 */

void sendSerialResponse(uint32_t serialNr);

/*
 *	set the broadcast filters on the sensor
 *
 */

void enableBroadCastFilter(void);

/*
 *	set the specific filters for a can identifier
 *
 */

void enableSensorFilter(uint8_t canId);

/*
 *	process a sensor config message and send it to the proper sensor
 *
 */

void sendSettings(uint16_t receiver, SensorConfigMsg_t settings);

/*
 *	set if the sensor received a token and store the number of max. allowed messages to be sent
 *
 */

int setTokenStatus(char status, char counter);

/*
 *	stop all sensor units
 *
 */

void stopAllSensors(void);

/*
 *	stop one sensor units
 *
 */

void stopSensor(uint8_t sensorId);

/*
 *	empty the queue of outbound messages
 *
 */

void emptyQueue(void);


/*
 *	reset the timestamp of the sensors
 *
 */

void resetTimestamp(void);


#endif
