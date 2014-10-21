#include "impact_event.h"

/* ------------------------------------------------------------------
 * -- Externals 
 * --------------------------------------------------------------- */ 
 
extern Serial pcSerial;


/* ------------------------------------------------------------------
 * -- Global Variables
 * --------------------------------------------------------------- */

static Event_rb event_queue;

void isr_nextMeasurement(){
	pcSerial.printf("ISR ADC Event Recognition called.\n");
}

	/*
	 * See header file
	 */
	void event_detection();
	
	/*
	 * See header file
	 */
	void init_event_handler(void);

	/*
	 * See header file
	 */
	void init_event_queue(void);

	/*
	 * See header file
	 */
	void enqueue_event(Event event);

	/*
	 * See header file
	 */
	Event dequeue_event(void);

	/*
	 * See header file
	 */
	unsigned char has_event(void);