/* LPC4088 BusHandler library
 *
 */

#ifndef BUS_HANDLER_4088_H
#define BUS_HANDLER_4088_H

#include "mbed.h"
#include "CAN.h"
#include "BusProtocol.h"
#include "cmsis_os.h"
#include "rtos.h"	
	
#define SENTMSGBUFFER_LEN 512
typedef struct {
		uint32_t dataLength;
		char *payload;
		char sender;
		msgType_t msgType;
	} msgData_t;

	/**
	 * sentMsgBuffer
	 * Ringbuffer to hold the sent messages until the ACK/NACK was received.
	 * Has an msgData_t array and the reading and writing indices
	 */
typedef struct {
		msgData_t queue[SENTMSGBUFFER_LEN];
		uint16_t read_pos;
		uint16_t write_pos;
		uint16_t count;
	} sentMsgBuffer;	
	
/**	Function to package the received data and put into sending queue
	* @param	dataLength						Length of data (in bytes)
	* @param  payload								payload to be sent
	* @param  receiver							Device that should receive the message
	* @param 	msgType								Type of the message to be sent
	*	@retval returnCode						returns 0 for success, 1 for failure
	*/
uint32_t sendMessage(uint32_t dataLength, char *payload, char receiver, msgType_t msgType);	
	
/**	Function to convert the received message
	* @param	msg										message received from another device
	*	@retval msgData 							message data
	*/
msgData_t convertMessage(message_t msg);	

/**	Bushandler thread for the logger unit
	*/
void processMessagesLogger(void const *args);

/**	Bushandler Thread for the sensor unit
	*/
void processMessagesSensor(void const *args);

/**	Store sent messages
	*	@param  sentMsg								processed message to be stored for ACK/NACK processing
	*	@retval returnCode						returns 0 for success, 1 for failure
	*/
uint32_t storeSentMessages(message_t sentMsg);

/**	Check ACK'd messages and depending on status, they'll either be deleted or resent
	*	@param  receivedMsg						received ACK msg to be checked. 
	*	@retval returnCode						returns 0 for success, 1 for failure
	*/
uint32_t checkAckMessages(message_t sentMsg);

msgData_t getInboundMessage(void);

#endif
