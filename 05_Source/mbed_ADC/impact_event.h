
#ifndef IMPACT_EVENT
#define IMPACT_EVENT

#include "mbed.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ------------------------------------------------------------------
 * -- Constants, macros
 * --------------------------------------------------------------- */

#define INPUTQUEUE_LEN  512       // how many input values can be cached in the ring buffer
#define MAX_EVENT_LENGTH 512    // XXX this will later be made configurable
#define SAMPLES_UNTIL_TIMEOUT 15 // how many samples need to pass with values below threshold for the impact to end.
#define THRESHOLD 200           // measurement threshold
#define BASELINE 2047           // baseline of the signal
	
/* ------------------------------------------------------------------
 * -- Constants, macros
 * --------------------------------------------------------------- */
	
    
    /**
     * EventID 
     * Tells the FSM what happened
     */
	typedef enum {
		E_RESET = 0,
		E_INPUT_HIGH,
		E_INPUT_LOW,
		E_TIMEOUT,
		E_NO_EVENT
	} EventID;
	
    /**
     * Input_t
     * struct that takes up the timestamp and value of a measurement
     */
	typedef struct {
		uint32_t timestamp;
		int32_t value;
    } Input_t;
	
    /**
     * Input_ringbuf
     * Ringbuffer to hold the input measurements until the event detection 
     * can process them.
     * Has an Input_t array and the reading and writing indices
     */
	typedef struct {
		Input_t queue[INPUTQUEUE_LEN];
		uint16_t read_pos;
		uint16_t write_pos;
		uint16_t count;
	} Input_ringbuf;
	
    /**
     * Impact_t
     * Struct to hold all the data for one impact
     * Start time, duration, peak count, peak maximums and impact maximum
     */
	typedef struct {
		uint32_t starttime;
    uint16_t sample_count;
    uint16_t peak_count;
		uint16_t samples[MAX_EVENT_LENGTH];
		uint16_t peaks[MAX_EVENT_LENGTH]; //XXX allenfalls kürzer, wie viel?
		uint16_t max_amplitude;
	} Impact_t;
	
	/* ------------------------------------------------------------------
	 * -- Function prototypes
	 * --------------------------------------------------------------- */
	
	/** 
     * Interrupt Service Routine to read the newest measurement value from the ADC
	 */
	void isr_nextMeasurement(void);
	
	/** 
   * Function to translate input into events
	 * Detect if there is an Event concerning the FSM and call the FSM accordingly
   *
   * This will run as a separate task
	 */
	void event_detection();
	
	/**
	 * Initialize the module event_handler
	 */
	void init_event_handler(void);

	/**
	 * Initialize the input queue
	 */
	void init_input_queue(void);

	/**
	 * Add an input to the queue
	 * @param   timestamp: timestamp of this measurement
	 * @param   value    : measurement value
	 */
	void enqueue_input(uint32_t timestamp, uint32_t value);

	/**
	 * Retrieve next input from the queue
	 * @retval  next input from queue or NULL pointer
	 */
	Input_t dequeue_input(void);

	/**
	 * Test the queue for available inputs
	 * @retval  0 if no inputs in queue
	 *          >= 1 if inputs available in queue
	 */
//	unsigned char has_input(void);
    
  /**
   * Test if the queue has room for more inputs
   * @retval  0 if queue is full
   *          1 if queue has room for more inputs
   */
//  unsigned char has_room(void);

	
#ifdef __cplusplus
};
#endif

/* ------------------------------------------------------------------
 * -- Header file end
 * --------------------------------------------------------------- */
#endif
