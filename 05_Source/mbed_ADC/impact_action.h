
#ifndef IMPACT_ACTION
#define IMPACT_ACTION

#include "mbed.h"
#include "impact_event.h"

#ifdef __cplusplus
extern "C" {
#endif	

	/* ------------------------------------------------------------------
	 * -- Function prototypes
	 * --------------------------------------------------------------- */    

	/**
	 * Initializes the module action_handler
	 */
	void init_action_handler(void);

	/**
	 * Reset the variable for impacts and store the first value.
	 * 
	 * @param value		the first measurement value that crossed the threshold
	 */
	void new_impact(Input_t imp_input);

	/**
	 * End impact
	 * The current impact has ended. Trim the trailing values below threshold, then
	 * call function store_impact.
	 */
	void end_impact(Input_t imp_input);
	
	/**
	 * Adds a sample to the current impact
	 */ 
	 void add_sample(Input_t imp_input);
	
	/**
	 * Adds a peak to the current impact
	 * 
	 * @param value		the first measurement value that crossed the threshold
	 */
	void add_peak(Input_t imp_input);
	
	/**
	 * Ends the current peak
	 */
	void end_peak();
	
	/**
	 * Update maximum levels of current peak and current impact
	 * 
	 * @param value		the current measurement value to be compared to the current maxima
	 */
	void update_maxima(Input_t imp_input);
	
	/**
	 * Start the timeout counter of state S_IN_EVENT_NO_PEAK
     * Timer is updated by event_detection
	 * When timeout counter reaches zero, the impact has ended.
	 */
	void start_timer(void);
    
  /**
   * Stop the timeout counter
	 */
	void stop_timer(void); 
	
	/**
	 * Write impact data to file
	 */
	void store_impact(void);
	
#ifdef __cplusplus
};
#endif

#endif
