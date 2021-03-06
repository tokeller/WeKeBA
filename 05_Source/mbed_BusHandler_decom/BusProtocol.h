/* LPC4088 CANBusProtocol library
 *
 */

#ifndef BUS_4088_H
#define BUS_4088_H

#define FILTERON

#include "mbed.h"
#include "CAN.h"

#ifdef __cplusplus
extern "C" {
#endif

#define LOGGER_ADR 0x01

#define OUTPUTQUEUE_LEN 512
	
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
	*		NULL_MESSAGE									no message present
	*/
typedef enum {GET_SENSOR_SERIAL_BC		,
							SET_SENSOR_ID_BC				,
							SENSOR_CONFIG_SINGLE		,
							SENSOR_CONFIG_BC				,
							OP_MODE_SINGLE					,
							OP_MODE_BC							,
							SEND_TOKEN_SINGLE				,
							//ACK_LOGGER_SINGLE				,
							TIME_SYNC_BC						,
							SERIAL_SINGLE						,
							RAW_DATA_SINGLE					,
							IMPACT_EXT_SINGLE				,
							IMPACT_STD_SINGLE				,
							ACK_SENSOR_SINGLE				,
							NULL_MESSAGE
							} msgType_t;

/**
	*
	*
	*
	*/

typedef struct {
							msgType_t msgType;
							uint32_t	msgId;
							uint32_t	receiver;
							char			dataLength;
							char		  payload[8];
							} message_t;

							
	/**
	 * Input_ringbuf
	 * Ringbuffer to hold the input measurements until the event detection 
	 * can process them.
	 * Has an Input_t array and the reading and writing indices
	 */
typedef struct {
		CANMessage queue[OUTPUTQUEUE_LEN];
		uint16_t read_pos;
		uint16_t write_pos;
		uint16_t count;
	} outputRingbuf;

							
							
/**	Function to initialize the bus handler on CAN-port 1 as a specific device
	*	@param  device 							  Device type, either LOGGER or SENSOR
	*																Sensors will only be able to receive broadcast messages until 
	*																they've been assigned a deviceID from the logger
	*	@retval returnCode						returns 0 for sucess, 1 for failure
	*/
uint32_t init(deviceType_t device);

							
/**	Function to register a message handler function and a message buffer
	*	@param  processMsg 						Message handling procedure
	*	@retval returnCode						returns 0 for sucess, 1 for failure
	*/
uint32_t registerMsgHandler(void (*processMsg)(void));

/**	Function to read a received message
	*	@retval message								returns the received message (or an empty, if none present)
	*/
message_t readReceivedMsg(void);
							
/**	Function to set the a CAN filter for a device
	*	@param  device 							  Device type, only SENSOR allowed (LOGGER will be set when calling init() and shouldn't
	*																have more than one filter
	* @param 	deviceID							8 bit deviceID, received from the logger
	*	@retval returnCode						returns 0 for sucess, 1 for failure
	*/
uint32_t setFilterForID(deviceType_t device, uint32_t deviceID);
							
							
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
