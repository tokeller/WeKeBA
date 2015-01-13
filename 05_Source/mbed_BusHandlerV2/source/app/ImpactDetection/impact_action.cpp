#include "impact_event.h"
#include "impact_action.h"
#include "BusHandler.h"
#include "BusProtocol.h"

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
extern uint32_t canId;
extern detail_mode_t detail_mode;

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
		ImpStd_t std;
		ImpExtData_t extData;
		ImpRawDataStart_t rawDataStart;
		ImpRawData_t rawData;
		char *p_data;
		uint32_t prev_timestamp;
		uint32_t dataLength;
		
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
		
		if(detail_mode == M_SPARSE){		
			pcSerial.printf("sparse\n");
			std.numberOfPkgs = 1;
			std.maxPeaks = impact->max_amplitude;
			//printf("amplitude %x\n",impact->max_amplitude);
			std.timestamp = impact->starttime;
			//printf("starttime %x\n",impact->starttime);
			std.nrOfPeaks = impact->peak_count;
			//printf("peak_count %x\n",impact->peak_count);
			std.duration = impact->sample_count;
			//printf("sample_count %x\n",impact->sample_count);
			enqueueMessage(8,std,0x01,canId,IMPACT_STD_SINGLE);
			
		} else if(detail_mode == M_OFF){	
			pcSerial.printf("OFF\n");
			// don't send anything
			
		} else if(detail_mode == M_PEAKS){	
			pcSerial.printf("peaks\n");
			// send peaks only
			// send std, then as many impextdata as necessary
			/* numberOfPkgs = impact->peak_count / 4 + 1;
			if(impact->peak_count%4){
				numberOfPkgs += 1;
			}
			std.numberOfPkgs = numberOfPkgs;
			std.maxPeak = impact->max_amplitude;
			std.timestamp = impact->starttime;
			std.nrOfPeaks = impact->peak_count;
			std.duration = impact->sample_count;
			enqueueMessage(8,std,0x01,canId,IMPACT_STD_SINGLE); */
			
			// prepare data 
			dataLength = 8 + 2 * impact->peak_count;
			p_data = (char *) malloc(dataLength);
			if(p_data != NULL){
				p_data[0] = ((impact->starttime) >> 24) & 0xff;
				p_data[1] = ((impact->starttime) >> 16) & 0xff;
				p_data[2] = ((impact->starttime) >> 8) & 0xff;
				p_data[3] = (impact->starttime) & 0xff;
				p_data[4] = (impact->max_amplitude >> 4) & 0xff;
				p_data[5] = (impact->peak_count) & 0xff;
				p_data[6] = (impact->sample_count >> 8) & 0xff;
				p_data[7] = impact->sample_count & 0xff;
				
				prev_timestamp = impact->starttime;
				for(i = 0; i < impact->peak_count; i++){
					p_data[8+2*i] = (char)(impact->peaks[i].timestamp - prev_timestamp);
					p_data[8+2*i+1] = (char)((impact->peaks[i].value >> 4) & 0xff);
					prev_timestamp = impact->peaks[i].timestamp;
				}
				
				enqueueMessage(dataLength, p_data, 0x01, canId, IMPACT_EXT_SINGLE);
				free(p_data);
				p_data = NULL;
			}
			
			
		} else if(detail_mode == M_DETAILED){	
			pcSerial.printf("detailed\n");
			// send all samples
			// send rawDataStart, then as many rawData as necessary
			dataLength = 4 + impact->sample_count;
			p_data = (char *) malloc(dataLength);
			if(p_data != NULL){
				p_data[0] = ((impact->starttime) >> 24) & 0xff;
				p_data[1] = ((impact->starttime) >> 16) & 0xff;
				p_data[2] = ((impact->starttime) >> 8) & 0xff;
				p_data[3] = impact->starttime & 0xff;
				for(i = 0; i < impact->sample_count; i++){
					p_data[4+i] = (uint8_t)((impact->samples[i] >> 4) & 0xff);
				}
				
				enqueueMessage(dataLength, p_data, 0x01, canId, RAW_DATA_SINGLE);
				free(p_data);
				p_data = NULL;
			}
			
		} else if(detail_mode == M_RAW){	
			pcSerial.printf("RAW\n");
			// TODO: DO NOT HANDLE HERE, RAW MODE SHOULD NOT USE FSM BUT SEND PACKETS DIRECTLY
		}
	}
	
