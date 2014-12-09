#ifndef LPC4088_CANBUS
#define LPC4088_CANBUS

#include "mbed.h"
#include "rtos.h"

#define BUFFER_SIZE 512

/* 	Type of the sending device
	*/
typedef enum {LOGGER
					  , SENSOR
} deviceType_t;

/**	Message types
	* Logger messages:
	* 	GET_SENSOR_SERIAL_BC					BC to all sensors, requesting their serial number
	* 	SET_SENSOR_ID_SINGLE 					Targeted message, setting the sensors CAN-ID
	* 	TIME_SYNC_BC									BC to all sensors, resetting the timer count for all sensors
	*		START_REC_BC
	* 	SEND_TOKEN_SINGLE							Targeted message, allowing one sensor to send data (a specified number of messages)
	* 	SENSOR_CONFIG_SINGLE					Targeted message, setting the config of a single sensor
	* 	SENSOR_OFF_SINGLE
	* 	SENSOR_OFF_BC
	* 	OP_MODE_SINGLE								Targeted message, setting the operation mode of a single sensor (RAW, EXT, STD)
	* 	OP_MODE_BC										BC to all sensors, setting the operation mode of all sensors (only EXT and STD allowed!)
	*
	* Sensor messages:
	* 	SERIAL_SINGLE									Targeted message, sending the sensors serial number to the logger
	* 	RAW_DATA_SINGLE								Targeted message, sending raw data to the logger
	* 	ACK_TOKEN_SINGLE							Targeted message, ACK to the logger for a received command
	* 	IMPACT_EXT_SINGLE							Targeted message, sending extended data of one impact to the logger
	* 	IMPACT_STD_SINGLE							Targeted message, sending standard data of one impact to the logger
	*		DATA_OVERFLOW_SINGLE					Too many impacts on sensor, overflow
	*		NULL_MESSAGE									no message present
	*/
typedef enum {
	GET_SENSOR_SERIAL_BC	= 0x00,      //00000
	SET_SENSOR_ID_SINGLE	= 0x01,      //00001
	TIME_SYNC_BC					= 0x02,      //00010
	START_REC_BC					= 0x03,      //00011
	SEND_TOKEN_SINGLE			= 0x04,      //00100
	SENSOR_CONFIG_SINGLE	= 0x05,      //00101
	SENSOR_OFF_SINGLE			= 0x06,      //00110
	SENSOR_OFF_BC					= 0x07,      //00111
	OP_MODE_SINGLE				= 0x08,      //01000
	OP_MODE_BC						= 0x09,      //01001
	SERIAL_SINGLE					= 0x18,      //11000
	RAW_DATA_SINGLE				= 0x1A,      //11010
	ACK_TOKEN_SINGLE			= 0x19,      //11001
	IMPACT_EXT_SINGLE			= 0x1B,      //11011
	IMPACT_STD_SINGLE			= 0x1C,      //11100
	DATA_OVERFLOW_SINGLE	= 0x1D,      //11101
	NULL_MESSAGE					=	0xFF
} msgType_t;

typedef struct {
		CANMessage queue[BUFFER_SIZE];
		uint16_t read_pos;
		uint16_t write_pos;
		uint16_t count;
} outputRingbuf;


int CAN_init(void);

uint32_t storeRecMessages(CANMessage sentMsg);

CANMessage dequeueOutput (void);

int sendMessage(CANMessage msg);

void CANIRQHandler(void);

void setExtGrpCANFilter (uint32_t id);

#endif
