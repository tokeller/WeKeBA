#include "impact_event.h"
#include "impact_action.h"

/* TO DO
 * - check for overflow in peaks, samples
 * - if samples overflow: store current impact, reset sample counter and continue storing at sample index 0, peak-index 0
 * 
 */

/* ------------------------------------------------------------------
 * -- Externals 
 * --------------------------------------------------------------- */ 
 
extern Serial pcSerial;
extern uint32_t timeout_counter;
extern uint8_t timeout_active;
extern uint32_t samples_timeout;
extern uint32_t baseline;
extern uint16_t maximum_impact_length;

/* ------------------------------------------------------------------
 * -- Global variables
 * --------------------------------------------------------------- */
 
static Impact_t *impact = NULL;

/* ------------------------------------------------------------------
 * -- Functions
 * --------------------------------------------------------------- */ 
 
	/*
	 * See header file
	 */
	void init_impact_action_handler(void)
	{
		init_impact();
	}

	/*
	 * See header file
	 */
	void init_impact(void)
	{
		if(impact == NULL)
			impact = (Impact_t *) malloc(sizeof(Impact_t));
		if(impact == NULL){
			printf("FATAL: unable to allocate memory for impact struct\n");
			exit(1);
		}
		impact->peaks   = (Input_t *) malloc(maximum_impact_length * sizeof(Input_t));
		if(impact->peaks == NULL){
			printf("FATAL: unable to allocate memory for impact peaks\n");
			exit(1);
		}
		impact->samples = (int16_t *) malloc(maximum_impact_length * sizeof(int16_t));
		if(impact->samples == NULL){
			printf("FATAL: unable to allocate memory for impact samples\n");
			exit(1);
		}
	}
	
	/*
	 * See header file
	 */
	void free_impact(void)
	{
		
		free(impact->peaks);
		free(impact->samples);
		free(impact);
		impact = NULL;
	}

	/*
	 * See header file
	 */
	void new_impact(Input_t imp_input)
	{
		#ifdef DEBUG_IMPACT
		uint16_t i;
		pcSerial.printf("\t\tnew impact\n");
		#endif
		impact->starttime = imp_input.timestamp;
		impact->baseline = baseline;
		impact->sample_count = 0;
		impact->peak_count = 0;
		impact->max_amplitude = 0;
		// OPTION: don't reset arrays to zero.
		#ifdef DEBUG_IMPACT
		for ( i = 0; i < MAX_IMPACT_LENGTH; i++){
			impact->samples[i] = 0;
			impact->peaks[i].timestamp = 0;
			impact->peaks[i].value = 0;
		}
		#endif
	}
	
	/*
	 * See header file
	 */
	void end_impact()
	{
		// trim trailing samples below threshold, adjust sample count
		impact->sample_count = impact->sample_count - samples_timeout;
	}
	
	/*
	 * See header file
	 */
	void add_sample(Input_t smp_input)
	{
		if(impact->sample_count < maximum_impact_length){
			impact->samples[impact->sample_count] = smp_input.value;
			impact->sample_count++;
		} else{
			printf("BUG: impact longer than defined maximum length\n");
			// TODO solve this: we should just store the data and start a new impact.
			// SOLVE: after storing, set sample_count = 0, set peak_count = 0, maximum = 0
		}

	}
	
	/*
	 * See header file
	 */
	void update_maxima_pos(Input_t smp_input)
	{
		// update current peak maximum
		if(smp_input.value > impact->peaks[impact->peak_count].value){
				impact->peaks[impact->peak_count].value = smp_input.value;
				impact->peaks[impact->peak_count].timestamp = smp_input.timestamp;
			}
		
		// update impact maximum
		if(smp_input.value > impact->max_amplitude){
				impact->max_amplitude = smp_input.value;
			  impact->max_amplitude_timestamp = smp_input.timestamp;
			}
	}
	
	/*
	 * See header file
	 */
	void update_maxima_neg(Input_t smp_input)
	{
		// update current peak minimum
		if(smp_input.value < impact->peaks[impact->peak_count].value){
				impact->peaks[impact->peak_count].value = smp_input.value;
				impact->peaks[impact->peak_count].timestamp = smp_input.timestamp;
			}
		
		// update impact maximum (inverse value!)
		if( 0 - smp_input.value > impact->max_amplitude){
				impact->max_amplitude = 0 - smp_input.value;
			  impact->max_amplitude_timestamp = smp_input.timestamp;
			}
	}
		
	
	/*
	 * See header file
	 */
	/* probably unused
	void add_peak(uint32_t value)
	{
		// probably not needed, we only increase peak counter when peak is finished.
	}
	*/
	
	/*
	 * See header file
	 */
	void end_peak()
	{
		// will not overflow, because sample counter is watched and peak counter cannot be higher than sample counter.
		impact->peak_count++;
		impact->peaks[impact->peak_count].value = 0;
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
		pcSerial.printf("\nImpact complete:\nStarttime: %d\n", impact->starttime);
		pcSerial.printf("Samples: %d\nPeaks: %d\nMaximum: %d\n***********\n", impact->sample_count, impact->peak_count, impact->max_amplitude);
		
		// samples
		for(i = 0; i < impact->sample_count; i++){
			pcSerial.printf("%10u, %5hd; ", impact->starttime + i, impact->samples[i]);
		}
		pcSerial.printf("\n\n");
		
		// peaks
		for(i = 0; i < impact->peak_count + 3; i++){
			pcSerial.printf("%3hu: %10u %5hd\n", i, impact->peaks[i].timestamp, impact->peaks[i].value);
		}
		pcSerial.printf("\n");
	}
