
#ifndef IMPACT_EVENT
#define IMPACT_EVENT

#include "mbed.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ------------------------------------------------------------------
 * -- Constants, macros
 * --------------------------------------------------------------- */

#define INPUTQUEUE_LEN  1024       // how many input values can be cached in the ring buffer
#define MAX_IMPACT_LENGTH 1024    // XXX this will later be made configurable
#define SAMPLES_UNTIL_TIMEOUT 30 // how many samples need to pass with values below threshold for the impact to end.
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
		E_INPUT_HIGH_POS,
		E_INPUT_HIGH_NEG,
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
		int16_t value;
    } Input_t;
	
    /**
     * Input_ringbuf
     * Ringbuffer to hold the input measurements until the event detection 
     * can process them.
     * Has an Input_t array and the reading and writing indices
     */
	typedef struct {
		// old: Input_t queue[INPUTQUEUE_LEN];
		Input_t *queue;
		uint16_t read_pos;
		uint16_t write_pos;
		uint16_t count;
	} Input_ringbuf;
	
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
	void impact_event_detection(void);
	
	/**
	 * Initialize the module event_handler
	 */
	void init_impact_event_handler(void);

	/**
	 * Initialize the input queue
	 */
	void init_impact_input_queue(void);

	/**
	 * Free the input queue
	 */
	void free_impact_input_queue(void);
		
	/**
	 * Add an input to the queue
	 * @param   timestamp: timestamp of this measurement
	 * @param   value    : measurement value
	 */
	void enqueue_impact_input(uint32_t timestamp, uint32_t value);

	/**
	 * Retrieve next input from the queue
	 * @retval  next input from queue or NULL pointer
	 */
	Input_t dequeue_impact_input(void);

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

	/**
   * Reset the timestamp counter to zero
   * @retval  none
   */
	void reset_timestamp(void);
	
	/**
	 * Set the baseline value of the sensor input
	 * @param  uint16_t baseline: what value shall be considered zero g
	 */
	void set_baseline(uint16_t base_in);
	 
	/**
	 * Set the threshold value
	 * @param uint_16_t threshold: at what level input is considered 'HIGH'
	 */
	void set_threshold(uint16_t threshold_in);
	
	/**
	 * Set the maximum impact length. How many samples should an impact have at most.
	 * @param uint_16_t max_impact_len: number of samples an impact can have
	 */
	void set_max_impact_length(uint16_t max_impact_len);
	
	/**
	 * Set the impact queue length. How many samples can be cached for evaluation.
	 * @param uint_16_t input_queue_len: number of samples an impact can have
	 */
	void set_input_queue_length(uint16_t queue_len);
	
	/**
	 * Set the samples until timeout. How many samples must input remain below
	 * threshold for the impact to be considered finished.
	 * @param uint_16_t samples_until_timeout
	 */
	void set_samples_until_timeout(uint16_t samples);	
	
	
	
#ifdef __cplusplus
};
#endif

/* ------------------------------------------------------------------
 * -- Header file end
 * --------------------------------------------------------------- */
#endif
