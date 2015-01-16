
#ifndef IMPACT_ACTION
#define IMPACT_ACTION

#include "mbed.h"
#include "impact_event.h"

#ifdef __cplusplus
extern "C" {
#endif	
	
/* ------------------------------------------------------------------
 * -- Constants, macros
 * --------------------------------------------------------------- */
	
	
	/**
	 * Impact_t
	 * Struct to hold all the data for one impact
	 * Start time, duration, peak count, peak maximums and impact maximum
	 */
	typedef struct {
		uint32_t starttime;
		uint16_t baseline;
    uint16_t sample_count;
    uint16_t peak_count;
		int16_t *samples;
		Input_t *peaks;
		int16_t max_amplitude;
		uint32_t max_amplitude_timestamp;
	} Impact_t;
	/* ------------------------------------------------------------------
	 * -- Function prototypes
	 * --------------------------------------------------------------- */    

	/**
	 * Initializes the module action_handler
	 */
	void init_impact_action_handler(void);
	
	/**
	 * Initializes impact struct
	 */
	void init_impact(void);

	/**
	 * Free the impact struct
	 */
	void free_impact(void);
	
	/**
	 * Reset the variable for impacts and store the first value.
	 * 
	 * @param value		the first measurement value that crossed the threshold
	 */
	void new_impact(Input_t imp_input);

	/**
	 * End impact
	 * The current impact has ended. Trim the trailing values below threshold.
	 */
	void end_impact();
	
	/**
	 * Adds a sample to the current impact
	 */ 
	void add_sample(Input_t imp_input);
	
	/**
	 * Updates the current peak and impact maxima for a positive peak
	 */ 
	void update_maxima_pos(Input_t smp_input);
	
	/**
	* Updates the current peak and impact maxima for a negative peak
	 */
	void update_maxima_neg(Input_t smp_input);
	 
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
	 * Send impact data to logger
	 */
	void store_impact(void);
    
    /**
     * Send raw data to logger
     *
     * @param uint32_t starttime: timestamp of first sample
     * @param uint16_t nrOfSamples: length of data array
     * @param uint8_t *samples: pointer to data array
     */
    void store_raw(uint32_t starttime, uint16_t nrOfSamples, uint8_t *samples);
	
#ifdef __cplusplus
};
#endif

#endif
