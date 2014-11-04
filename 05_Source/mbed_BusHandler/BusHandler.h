/* LPC4088 BusHandler library
 *
 */

#ifndef BUS_HANDLER_4088_H
#define BUS_HANDLER_4088_H

#include "mbed.h"
#include "CAN.h"
#include "BusProtocol.h"

#ifdef __cplusplus
extern "C" {
#endif

/**	Function to package the received data and put into sending queue
	* @param	dataLength						Length of data (in bytes)
	* @param  payload								payload to be sent
	* @param  receiver							Device that should receive the message
	* @param 	msgType								Type of the message to be sent
	*	@retval returnCode						returns 0 for success, 1 for failure
	*/
uint32_t prepareMsg(uint32_t dataLength, char *payload, char receiver, msgType_t msgType);


/**	Function to read data from the receiving queue, if any present
	* @param	dataLength						Length of complete data (in bytes)
	* @param  data									data received
	* @param  receiver							Device that should receive the message
	* @param 	msgType								Type of the message to be sent
	*	@retval returnCode						returns 0 for success, 1 for failure
	*/

	
#ifdef __cplusplus
}
#endif
#endif
