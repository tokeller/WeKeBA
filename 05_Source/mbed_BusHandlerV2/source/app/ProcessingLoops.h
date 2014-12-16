#ifndef PROCLOOPS
#define PROCLOOPS
#include "mbed.h"
#include "BusHandler.h"
#include "SerialID.h"
#include "ADC_4088.h"
#include "impact_fsm.h"
#include "rtos.h"
#include "sensor_config.h"
#include "cmd.h"
#include "file_ops.h"
#include "UART_RTOS_Interrupt.h"

/*
 *	Sensor thread
 *		- Initialize CAN bus (set the filter so only the SerialRequest will be received)
 *		- get the serial number
 *		- start the communication thread
 *		- process the SerialRequest from the logger by sending the read serial number
 *			and enable all broadcast filters (since no ID was yet received, the sensor can only process
 *			broadcast messages)
 *		- wait till the IDBroadcast with the sensors serial number has arrived and store the
 *			corresponding CAN-ID
 *		- store the CAN-ID and set the filter for the directed messages
 *		- enter a 5s wait loop to receive the settings, if they don't arrive, use the default settings
 *		- await the timesync from the logger
 *    - enter the operation loop:
 *			- wait for the start recording message
 * 			- start the event detection
 */
void sensor_loop(void const *args);


/*
 *	Logger thread
 *		- TODO: SD-Card prep
 *		- Initialize CAN bus
 *		- start the communication thread
 *		- get Config from SD card or wait for console input
 *		- send serialID request broadcast
 *		- process received serial-IDs and send the corresponding CAN-Ids as broadcast
 *		- send the configs of all registered sensors
 *	    - send the timesync to all sensors
 *		- start the sensors recording mode
 *		- enter the processing loop:
 *			- commence sending the token to the sensor
 *			- receive and store the messages
 */
void logger_loop(void const *args);

/*  Commandline thread
 * 		- read input from the commandline
 *		- trigger the statemachine with the received input
 */
void get_cmd_event_thread(void const *args);

/*
 *	ProcessSettings
 *	@params		CANmessage_t *message: incoming settings message
 *
 * 	This function processes the received settings and sets them accordingly
 */
void processSettings(CANmessage_t *message);

#endif