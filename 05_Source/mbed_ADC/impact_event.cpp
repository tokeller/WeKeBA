#include "impact_event.h"
#include "impact_fsm.h"

/* TO DO
 * - set up impact recognition as task that runs in fixed intervals
 * - in initialisation of impact rec set baseline, threshold etc 
 * - 
 * 
 */

/* ------------------------------------------------------------------
 * -- Externals 
 * --------------------------------------------------------------- */ 
 
extern Serial pcSerial;
// DEBUG
#ifdef DEBUG_IMPACT
extern AnalogOut pinser18;
#endif
// DEBUG extern DigitalOut pinser2;


/* ------------------------------------------------------------------
 * -- Global Variables
 * --------------------------------------------------------------- */
 
static Input_ringbuf *input_queue = NULL;
uint32_t baseline = BASELINE;     // zero position of sensor signal
static int16_t threshold = THRESHOLD;   // event detection threshold
static uint16_t input_queue_length = INPUTQUEUE_LEN;
uint16_t maximum_impact_length = MAX_IMPACT_LENGTH;
uint32_t samples_timeout = SAMPLES_UNTIL_TIMEOUT;  // how long must signal remain
                                                   // below threshold for impact to end
uint32_t timeout_counter;
uint8_t timeout_active;         // is timeout counter active (1) or not(0)

static uint32_t timestamp = 0;           // timestamp for samples
static uint32_t value = 0;               // sampled value


/* ------------------------------------------------------------------
 * -- Functions
 * --------------------------------------------------------------- */ 
 
	/*
	 * See header file
	 */
	void isr_nextMeasurement(){
		//pcSerial.printf("ISR ADC Event Recognition called.\n");
		// read ADC measurement from Register, automatically resets IRQ
		value = LPC_ADC->GDR;
		value = (value >> 4) & 0xFFF;
		timestamp++;
		
		enqueue_impact_input(timestamp, value);
		
	}

	/*
	 * See header file
	 */
	void impact_event_detection()
	{
		int16_t value; 
		
		EventID new_event_id;
		Input_t input;
		
		// DEBUG
		// output of buffer level to analog out to watch externally for buffer overflow.
		#ifdef DEBUG_IMPACT
		pinser18 = (double)input_queue->count/INPUTQUEUE_LEN;
		#endif
		
		// DEBUG pcSerial.printf("f %d", input_queue.count);
		// TODO add while(1) loop, os_delay etc so it can run as a task.

		if(input_queue->count > 0){
			
			new_event_id = E_NO_EVENT;
			
			/* if there is a measurement value waiting, we have to determine whether
			 * it is above threshold value and generate the according event.
			 * Input above threshold (E_INPUT_HIGH_POS or E_INPUT_HIGH_NEG) will 
			 * stop the timeout counter.
			 */
			input = dequeue_impact_input();
			value = input.value - baseline;
			if(value >= threshold || value <= 0 - threshold){
				if(value >= threshold){
					new_event_id = E_INPUT_HIGH_POS;
				} else {
					new_event_id = E_INPUT_HIGH_NEG;
				}
			}	else if(timeout_active == 1){
				// decrease the timeout counter. If zero, event is E_TIMEOUT
				timeout_counter--;
				if(timeout_counter == 0){
					new_event_id = E_TIMEOUT;
				} else{
					new_event_id = E_INPUT_LOW;
				}
			} else {
				new_event_id = E_INPUT_LOW;
			}
			
			input.value = value;
			impact_fsm(new_event_id, input);

		}
	}
	
	/*
	 * See header file
	 */
	void init_impact_event_handler(void)
	{
		// initialize new input queue and timeout counter
		
		init_impact_input_queue();
		init_impact_action_handler();
		timeout_active = 0;
		timeout_counter = 0;
		
		
		
		init_impact_fsm();
		// TODO set baseline, threshold, peak amplitude bandwidth and timeout sample count
		
		// TODO add event_detection as task.
	}

	/*
	 * See header file
	 */
	void init_impact_input_queue(void)
	{
		unsigned short i;
		if(input_queue == NULL){
			input_queue = (Input_ringbuf *) malloc(sizeof(Input_ringbuf));
			if(input_queue == NULL){
				printf("FATAL: could not allocate memory for input_queue.\n");
				exit(1);
			}
		}
		input_queue->queue = NULL;
		input_queue->queue = (Input_t *) malloc(input_queue_length * sizeof(Input_t));
		if(input_queue->queue == NULL){
			printf("FATAL: could not allocate memory for input_queue values.\n");
			exit(1);
		}
		
		for(i = 0; i < input_queue_length; i++){
			input_queue->queue[i].timestamp = 0;
			input_queue->queue[i].value = 0;
		}
		input_queue->read_pos = 0;
		input_queue->write_pos = 0;
		input_queue->count = 0;
	}
	
	/*
	 * See header file
	 */
	void free_impact_input_queue(void)
	{
		free(input_queue->queue);
		free(input_queue);
		input_queue = NULL;
	}

	/*
	 * See header file
	 */
	void enqueue_impact_input(uint32_t timestamp, uint32_t value)
	{
		// critical section, disable ADC_IRQ
		NVIC_DisableIRQ(ADC_IRQn);
		
		if(input_queue->count < INPUTQUEUE_LEN){
			/* Insert event in queue */
			input_queue->queue[input_queue->write_pos].timestamp = timestamp;
			input_queue->queue[input_queue->write_pos].value = value;
	
			/* Update write position */
			input_queue->write_pos++;
			input_queue->count++;
			if (input_queue->write_pos >= INPUTQUEUE_LEN) {
					input_queue->write_pos = 0;
			}
		} else {
			//pcSerial.printf("\n\n==================================\nFATAL ERROR: Input Queue Overflow.\n==================================\n\n");
			
		}
		
    
		// enable ADC_IRQ
		NVIC_EnableIRQ(ADC_IRQn);
		
	}

	/*
	 * See header file
	 */
	Input_t dequeue_impact_input(void)
	{
		Input_t the_input;
		// critical section, disable ADC_IRQ
		
		NVIC_DisableIRQ(ADC_IRQn);
		if(input_queue->count > 0){
			the_input.timestamp = input_queue->queue[input_queue->read_pos].timestamp;
			the_input.value = input_queue->queue[input_queue->read_pos].value;
			input_queue->read_pos++;
			input_queue->count--;
			if(input_queue->read_pos >= INPUTQUEUE_LEN){
				input_queue->read_pos = 0;
			}
		} else {
			//pcSerial.printf("\n\n==================================\nFATAL ERROR: Input Queue Underflow.\n==================================\n\n");
			the_input.timestamp = 0;
			the_input.value = 0;
		}
		
		// enable ADC_IRQ
		NVIC_EnableIRQ(ADC_IRQn);
		
		return the_input;
	}

	/*
	 * See header file
	 */
/*	unsigned char has_input(void)
	{
		// critical, disable INT
		return
		// enable INT 
		
	}
*/
	
	/*
	 * See header file
	 */
/*	unsigned char has_room(void)
	{
		// critical, disable INT
		
		input_queue.queue[input_queue.write_pos].timestamp = timestamp;
		// enable INT
	}
	*/
	
	/*
	 * See header file
	 */
	void reset_timestamp(void)
	{
		// disable ADC_IRQ
		NVIC_DisableIRQ(ADC_IRQn);
		
		timestamp = 0;
		
		// enable ADC_IRQ
		NVIC_EnableIRQ(ADC_IRQn);
	}
	
	/*
	 * See header file
	 */
	void set_baseline(uint16_t base_in)
	{
		baseline = base_in;
		init_impact_fsm();
	}
	
	/*
	 * See header file
	 */
	void set_threshold(uint16_t threshold_in)
	{
		threshold = threshold_in;
		init_impact_fsm();
	}
	
	/*
	 * See header file
	 */
	void set_max_impact_length(uint16_t max_impact_len)
	{
		
		free_impact();
		maximum_impact_length = max_impact_len;
		init_impact();
		
		init_impact_fsm(); 
	}
	
	/*
	 * See header file
	 */
	void set_input_queue_length(uint16_t queue_len)
	{
		free_impact_input_queue();		
		input_queue_length = queue_len;
		init_impact_input_queue();
		init_impact_fsm();
		
	}
	
	/*
	 * See header file
	 */
	void set_samples_until_timeout(uint16_t samples)
	{
		samples_timeout = samples;
	}
