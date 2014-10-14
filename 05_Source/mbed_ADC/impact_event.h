
#ifndef IMPACT_EVENT
#define IMPACT_EVENT

#include "mbed.h"

#ifdef __cplusplus
extern "C" {
#endif
	
	typedef enum event{
		E_RESET = 0,
		E_INPUT_HIGH,
		E_INPUT_LOW,
		E_TIMEOUT
	} Event;
	
	/* ------------------------------------------------------------------
	* -- Function prototypes
	* --------------------------------------------------------------- */
	
	/* 
	 * Initializes the module event_handler
	 */
	void init_event_handler(void);
	
	/** Interrupt Service Routine to read the newest measurement value from the ADC
	 */
	void isr_nextMeasurement(void);
	
	/** Function to translate input into events
	*   Detect if there is an Event concerning the FSM
	*/
	void event_detection();
	

	
#ifdef __cplusplus
};
#endif

/* ------------------------------------------------------------------
 * -- Header file end
 * --------------------------------------------------------------- */

#endif
