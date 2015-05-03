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
#include "cmd_action.h"

/* ProcessingLoopsStates
 */
typedef enum {
    //Sensor Unit States
    S_S_Started,
    S_S_InitializedCANBus,
    S_S_RetrievedSerialNumber,
    S_S_CommThreadListeningForBroadcastMessages,
    S_S_SerialNumberSent,
    S_S_CANIDReceived,
    S_S_ConfigurationReceived,
    S_S_TimeSyncReceived,
    S_S_OperationLoopStartedNotRecording,
    S_S_OperationLoopStartedNotRecordingAndSending,
    S_S_RecordingRawData,
    S_S_RecordingRawDataAndSending,
    S_S_DetectingEvents,
    S_S_DetectingEventsAndSending,
    
    // Logger States
    S_L_Started,
    S_L_SDCardDetected,
    S_L_WaitForSerialNumbers,
    S_L_SendOutConfigurations,
    S_L_SendOutTimeSyncs,
    S_L_AllSensorsReady,
    S_L_ProcessingHoldingToken,
    S_L_ProcessingWaitingForData
} PLState;


/* ProcessingLoopsEvents
 */
typedef enum{
    // Sensor Unit Events
    E_S_RESET,
    E_S_CANBusInitialized,
    E_S_serialNumberRetrieved,
    E_S_commThreadStarted,
    E_S_receivedSerialRequest,
    E_S_sentSerialNumber,
    E_S_receivedCanID,
    E_S_receivedSettings,
    E_S_receivedTimesync,
    E_S_receivedStartRecordingMessage,
    E_S_receivedToken,
    E_S_sentAllData,
    E_S_tokenTimeout,
    E_S_rawRecordingTimeout,
    E_S_receivedStopRecordingMessage,
    
    // Logger Events
    E_L_RESET,
    E_L_SDCardDetected,
    E_L_configFileRead,
    E_L_configFileNotFound,
    E_L_timeout,
    E_L_receivedSerialNumber,
    E_L_configSentToSensor,
    E_L_timesyncSentToSensor,
    E_L_sentTokenToSensor,
    E_L_receiveEndOfData,
    E_L_tokenTimeout
} PLEvent;

/* ------------------------------------------------------------------
 * -- Function prototypes
 * --------------------------------------------------------------- */

void init_logger_fsm(void);
void logger_fsm(PLEvent new_event);
void init_sensor_fsm(void);
void sensor_fsm(PLEvent new_event);


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
