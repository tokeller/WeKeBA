#ifndef LPC4088_CAN_HANDLER
#define LPC4088_CAN_HANDLER

#include "BusProtocol.h"




typedef struct {
	msgType_t msgType;
	uint32_t	msgId;
	uint32_t  sender;
	uint32_t	receiver;
	char			dataLength;
	char		  payload[8];
} CANmessage_t;

typedef struct {
	uint16_t fs; 						// 12 bit sampling rate (100 Hz steps)
	uint16_t threshold;			// 10 bit threshold
	uint16_t baseline; 			// 10 bit zero level
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

int enqueueMessage(uint32_t dataLength, char *payload, char receiver, char sender, msgType_t msgType);

void CAN_COM_thread(void const *args);

void CAN_PRINT_thread(void const *args);

uint32_t prepareMsgId(msgType_t inMsgType, char inReceiver, char inSender, uint32_t inMsgId);

#endif
