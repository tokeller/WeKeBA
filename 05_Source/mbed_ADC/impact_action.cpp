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
	void new_impact(Input_t imp_input)
	{
		uint16_t i;
		pcSerial.printf("\t\tnew impact\n");
		impact.starttime = imp_input.timestamp;
		impact.sample_count = 0;
		impact.peak_count = 0;
		impact.max_amplitude = 0;
		// OPTION: don't reset arrays to zero.
		for ( i = 0; i < MAX_IMPACT_LENGTH; i++){
			impact.samples[i] = 0;
			impact.peaks[i].timestamp = 0;
			impact.peaks[i].value = 0;
		}
		
		add_sample(imp_input);
	}
	
	/*
	 * See header file
	 */
	void end_impact(Input_t imp_input)
	{
		// TODO
	}
	
	void add_sample(Input_t smp_input)
	{
		impact.samples[impact.sample_count] = smp_input.value;
		impact.sample_count++;
		// TODO watch out, if impact is too long => seg fault!
		
		// update current peak and impact maximum
		if(smp_input.value > impact.peaks[impact.peak_count].value){
			impact.peaks[impact.peak_count].value = smp_input.value;
			impact.peaks[impact.peak_count].timestamp = smp_input.timestamp;

		}
		if(smp_input.value > impact.max_amplitude){
				impact.max_amplitude = smp_input.value;
			  impact.max_amplitude_timestamp = smp_input.timestamp;
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
		uint16_t i;
		pcSerial.printf("\nImpact complete:\nStarttime: %d\n", impact.starttime);
		pcSerial.printf("Samples: %d\nPeaks: %d\nMaximum: %d\n***********\n", impact.sample_count, impact.peak_count, impact.max_amplitude);
		
		// samples
		for(i = 0; i < impact.sample_count; i++){
			pcSerial.printf("%u, %hu\t", impact.starttime + i, impact.samples[i]);
		}
		pcSerial.printf("\n\n");
		
		// peaks
		for(i = 0; i < impact.peak_count + 3; i++){
			pcSerial.printf("%hu:\t%u\t%hu\n", i, impact.peaks[i].timestamp, impact.peaks[i].value);
		}
		pcSerial.printf("\n");
	}
