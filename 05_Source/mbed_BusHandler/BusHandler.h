/* LPC4088 BusHandler library
 *
 */

#ifndef BUS_4088_H
#define BUS_4088_H

#include "mbed.h"

#ifdef __cplusplus
extern "C" {
#endif

#define LOGGER_ADR 0x01

/* 	Type of the sending device
	*/
typedef enum {LOGGER, SENSOR} deviceType_t;

/**	Message types
	* Logger messages:
	* 	GET_SENSOR_SERIAL_BC					BC to all sensors, requesting their serial number
	* 	SET_SENSOR_ID_SINGLE 					Targeted message, setting the sensors CAN-ID
	* 	SENSOR_CONFIG_SINGLE					Targeted message, setting the config of a single sensor
	* 	SENSOR_CONFIG_BC							BC to all sensors, setting the config of all sensors
	* 	OP_MODE_SINGLE								Targeted message, setting the operation mode of a single sensor (RAW, EXT, STD)
	* 	OP_MODE_BC										BC to all sensors, setting the operation mode of all sensors (only EXT and STD allowed!)
	* 	SEND_TOKEN_SINGLE							Targeted message, allowing one sensor to send data (a specified number of messages)
	* 	ACK_LOGGER_SINGLE							Targeted message, ACK from the logger to a received message
	* 	TIME_SYNC_BC									BC to all sensors, resetting the timer count for all sensors
	*
	* Sensor messages:
	* 	SERIAL_SINGLE									Targeted message, sending the sensors serial number to the logger
	* 	RAW_DATA_SINGLE								Targeted message, sending raw data to the logger
	* 	IMPACT_EXT_SINGLE							Targeted message, sending extended data of one impact to the logger
	* 	IMPACT_STD_SINGLE							Targeted message, sending standard data of one impact to the logger
	* 	ACK_SENSOR_SINGLE							Targeted message, ACK to the logger for a received command
	*/
typedef enum {GET_SENSOR_SERIAL_BC,
							SET_SENSOR_ID_BC,
							SENSOR_CONFIG_SINGLE,
							SENSOR_CONFIG_BC,
							OP_MODE_SINGLE,
							OP_MODE_BC,
							SEND_TOKEN_SINGLE,
							ACK_LOGGER_SINGLE,
							TIME_SYNC_BC,
							SERIAL_SINGLE,
							RAW_DATA_SINGLE,
							IMPACT_EXT_SINGLE,
							IMPACT_STD_SINGLE,
							ACK_SENSOR_SINGLE
							} msgType_t;

/**
	*
	*
	*
	*/

typedef struct {
							msgType_t msgType;
							char			receiver[8];
							char		  payload[64];
							} message_t;

/**	Function to initialize the bus handler on CAN-port 1 as a specific device
	*	@param  device 							  Device type, either LOGGER or SENSOR
	*	@retval returnCode						returns 0 for sucess, 1 for failure
	*/
uint32_t init(deviceType_t device);

/**	Function to register a message handler function and a message buffer
	*	@param  processMsg 						Message handling procedure
	* @param  msgBuffer							Buffer for storing received messages
	*	@retval returnCode						returns 0 for sucess, 1 for failure
	*/
uint32_t registerMsgHandler(uint32_t processMsg, message_t *msgBuffer);

/**	Function to send a message
	* @param  message								Message to be sent
	* @param  receiver							Device that should receive the message
	*	@retval returnCode						returns 0 for sucess, 1 for failure
	*/
uint32_t sendMsg(message_t message);

#ifdef __cplusplus
}
#endif
#endif
