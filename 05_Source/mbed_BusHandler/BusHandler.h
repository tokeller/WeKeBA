/* LPC4088 BusHandler library
 *
 */

#ifndef BUS_HANDLER_4088_H
#define BUS_HANDLER_4088_H

#include "mbed.h"
#include "CAN.h"
#include "BusProtocol.h"
#include "cmsis_os.h"
#include "Queue.h"	
	
typedef struct {
		uint32_t dataLength;
		char *payload;
		char receiver;
		msgType_t msgType;
	} msgData_t;

/**	Function to package the received data and put into sending queue
	* @param	dataLength						Length of data (in bytes)
	* @param  payload								payload to be sent
	* @param  receiver							Device that should receive the message
	* @param 	msgType								Type of the message to be sent
	*	@retval returnCode						returns 0 for success, 1 for failure
	*/
uint32_t sendMessage(uint32_t dataLength, char *payload, char receiver, msgType_t msgType);	
	
	
/**	Bushandler thread for the logger unit
	*	@retval returnCode						returns 0 for success, 1 for failure
	*/
void processMessagesLogger(void const *args);

/**	Bushandler Thread for the sensor unit
	*	@retval returnCode						returns 0 for success, 1 for failure
	*/
void processMessagesSensor(void const *args);

#endif
