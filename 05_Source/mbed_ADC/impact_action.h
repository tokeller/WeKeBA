
#ifndef IMPACT_ACTION
#define IMPACT_ACTION

#include "mbed.h"

#ifdef __cplusplus
extern "C" {
#endif	

	/* ------------------------------------------------------------------
	 * -- Function prototypes
	 * --------------------------------------------------------------- */    

	/*
	 * Inititializes the module action_handler
	 */
	void init_action_handler(void);

	/*
	 * Allocate space for new impact
	 * 
	 * @param value		the first measurement value that crossed the threshold
	 */
	void new_impact(unsigned int value);

	/*
	 * End impact
	 * The current impact has ended. Trim the trailing values below threshold, then
	 * call function store_impact.
	 */
	void end_impact(unsigned int value);
	
	/*
	 * Adds a peak to the current impact
	 * 
	 * @param value		the first measurement value that crossed the threshold
	 */
	void add_peak(unsigned int value);
	
	/*
	 * Ends the current peak
	 */
	void end_peak();
	
	/*
	 * Update maximum levels of current peak and current impact
	 * 
	 * @param value		the current measurement value to be compared to the current maxima
	 */
	void update_maxima(unsigned int value);
	
	/*
	 * Start the timeout counter of state S_IN_EVENT_NO_PEAK
	 * When timeout counter reaches zero, the impact has ended.
	 */
	void start_timer(void);
	
	/*
	 * Write impact data to file
	 */
	void store_impact(void);
	
#ifdef __cplusplus
};
#endif

#endif
