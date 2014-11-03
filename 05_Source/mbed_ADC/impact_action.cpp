#include "impact_event.h"
#include "impact_action.h"

/* ------------------------------------------------------------------
 * -- Externals 
 * --------------------------------------------------------------- */ 
 
extern Serial pcSerial;
extern uint32_t timeout_counter;
extern uint8_t timeout_active;
extern uint32_t samples_timeout;

/* ------------------------------------------------------------------
 * -- Global variables
 * --------------------------------------------------------------- */
 
static Impact_t impact;

/* ------------------------------------------------------------------
 * -- Functions
 * --------------------------------------------------------------- */ 
 
	/*
	 * See header file
	 */
	void init_action_handler(void)
	{
		
	}

	/*
	 * See header file
	 */
	void new_impact(uint32_t value)
	{
		uint16_t i;
		
		impact.starttime = 0;
		impact.sample_count = 0;
		impact.peak_count = 0;
		impact.max_amplitude = 0;
		// OPTION: don't reset arrays to zero.
		for ( i = 0; i < MAX_EVENT_LENGTH; i++){
			impact.samples[i] = 0;
			impact.peaks[i] = 0;
		}
		
		add_sample(value);
	}
	
	/*
	 * See header file
	 */
	void end_impact(uint32_t value)
	{
		
	}
	
	void add_sample(uint32_t value)
	{
		impact.samples[impact.sample_count] = value;
		impact.sample_count++;
		// TODO watch out, if impact is too long => seg fault!
		
		// update current peak and impact maximum
		if(value > impact.peaks[impact.peak_count]){
			impact.peaks[impact.peak_count] = value;
			if(value > impact.max_amplitude)
				impact.max_amplitude = value;
		}
		
		
	}
	
	/*
	 * See header file
	 */
	void add_peak(uint32_t value)
	{
		// probably not needed, we only increase peak counter when peak is finished.
	}
	
	/*
	 * See header file
	 */
	void end_peak()
	{
		impact.peak_count++;
		// TODO watch out for overflow!
	}
	 
	
	/*
	 * See header file
	 */
	void start_timer(void)
	{
		timeout_counter = samples_timeout;
		timeout_active = 1;
	}
	
	/*
	 * See header file
	 */
	void stop_timer(void)
	{
		timeout_active = 0;
	}
	/*
	 * See header file
	 */
	void store_impact(void)
	{
		pcSerial.printf("***********\n");
		pcSerial.printf("Impact complete:\n");
		pcSerial.printf("Starttime: %d\n", impact.starttime);
		pcSerial.printf("Samples: %d\n", impact.samples);
		pcSerial.printf("Peaks: %d\n", impact.peak_count);
		pcSerial.printf("Maximum: %d\n", impact.max_amplitude);
		pcSerial.printf("***********\n");
	}
