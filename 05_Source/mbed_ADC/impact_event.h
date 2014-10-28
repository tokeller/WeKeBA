
#ifndef IMPACT_EVENT
#define IMPACT_EVENT

#include "mbed.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ------------------------------------------------------------------
 * -- Constants, macros
 * --------------------------------------------------------------- */

#define EVENTQUEUE_LEN  8       // how many events can be cached in the ring buffer
#define MAX_EVENT_LENGTH 512    // XXX this will later be made configurable
#define SAMPLES_UNTIL_TIMEOUT 15 // how many samples need to pass with values below threshold for the event to end.
#define THRESHOLD 100           // measurement threshold
#define BASELINE 2047           // baseline of the signal
	
/* ------------------------------------------------------------------
 * -- Constants, macros
 * --------------------------------------------------------------- */
	
	typedef enum {
		E_RESET = 0,
		E_INPUT_HIGH,
		E_INPUT_LOW,
		E_TIMEOUT,
		E_NO_EVENT
	} EventID;
	
	typedef struct {
		EventID id;
		unsigned int timestamp;
		signed int value;
	} Event_FSM;
	
	typedef struct {
		Event_FSM queue[EVENTQUEUE_LEN];
		unsigned char read_pos;
		unsigned char write_pos;
	} Event_ringbuf;
	
	typedef struct {
		unsigned int starttime;
		unsigned int duration;
		unsigned short samples[MAX_EVENT_LENGTH];
		unsigned short peaks[MAX_EVENT_LENGTH]; //XXX allenfalls kürzer, wie viel?
		unsigned short max_amplitude;
	} Event_t;
	
	/* ------------------------------------------------------------------
	* -- Function prototypes
	* --------------------------------------------------------------- */
	
	/** Interrupt Service Routine to read the newest measurement value from the ADC
	 */
	void isr_nextMeasurement(void);
	
	/** Function to translate input into events
	*   Detect if there is an Event concerning the FSM
	*/
	void event_detection();
	
	/* 
	 * Initialize the module event_handler
	 */
	void init_event_handler(void);

	/*
	 * Initialize the event queue
	 */
	void init_event_queue(void);

	/*
	 * Add an event to the queue
	 * @param   event: new event
	 */
	void enqueue_event(Event_FSM event);

	/*
	 * Retrieve next event from the queue
	 * @retval  next event in queue or E_NO_EVENT
	 */
	Event_FSM dequeue_event(void);

	/*
	 * Test the queue for available events
	 * @retval  0 if no events in queue
	 *          1 if event available in queue
	 */
	unsigned char has_event(void);

	
#ifdef __cplusplus
};
#endif

/* ------------------------------------------------------------------
 * -- Header file end
 * --------------------------------------------------------------- */
#endif
