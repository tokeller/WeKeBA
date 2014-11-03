#include "impact_event.h"
#include "impact_fsm.h"

/* ------------------------------------------------------------------
 * -- Externals 
 * --------------------------------------------------------------- */ 
 
extern Serial pcSerial;
extern unsigned int timestamp;


/* ------------------------------------------------------------------
 * -- Global Variables
 * --------------------------------------------------------------- */
 
static Input_ringbuf input_queue;
static uint32_t baseline = BASELINE;     // zero position of sensor signal
static uint32_t threshold = THRESHOLD;   // event detection threshold
static uint32_t peak_noise_threshold;    // threshold on peak plateau
uint32_t samples_timeout = SAMPLES_UNTIL_TIMEOUT;  // how long must signal remain
                                                   // below threshold for impact to end
uint32_t timeout_counter;
uint8_t timeout_active;         // is timeout counter active (1) or not(0)



/* ------------------------------------------------------------------
 * -- Functions
 * --------------------------------------------------------------- */ 
 
	/*
	 * See header file
	 */
	void isr_nextMeasurement(){
		uint32_t value;
		uint32_t timestamp;
		pcSerial.printf("ISR ADC Event Recognition called.\n");
		// read ADC measurement from Register, automatically resets IRQ
		value = LPC_ADC->GDR;
		timestamp = 1; // TODO hole Timestamp
		enqueue_input(timestamp, value);
		
	}

	/*
	 * See header file
	 */
	void event_detection()
	{
		//XXX get difference of abs(value - baseline) and threshold,
		//    if positive we have an E_INPUT_HIGH, otherwise it's E_INPUT_LOW
		signed int value; 
		
		EventID new_event_id;
		Input_t input;
		
		// TODO add while(1) loop, os_delay etc so it can run as a task.
		
		if(input_queue.count > 0){
			
			new_event_id = E_NO_EVENT;
			
			// if there is a measurement value waiting, we have to determine whether
			// it is above threshold value and generate the according event.
			// Input above threshold (E_INPUT_HIGH) will stop the timeout counter.
			input = dequeue_input();
			if(input.value > baseline){
				value = input.value - baseline;
			} else {
				value = baseline - input.value;
			}
			
			if(value >= threshold){
				new_event_id = E_INPUT_HIGH;
			} else if(timeout_active == 1){
				// decrease the timeout counter. If zero, event is E_TIMEOUT
				timeout_counter--;
				if(timeout_counter == 0){
					new_event_id = E_TIMEOUT;
				}
			} else {
				new_event_id = E_INPUT_LOW;
			}
			
			// call the FSM with the event, then reset the event.
			if(new_event_id != E_NO_EVENT){
				new_event_id = new_event_id;
				impact_fsm(new_event_id, input);
			}
		}
	}
	
	/*
	 * See header file
	 */
	void init_event_handler(void)
	{
		// initialize new input queue and timeout counter
		init_input_queue();
		timeout_active = 0;
		timeout_counter = 0;
		
		// TODO set baseline, threshold, peak amplitude bandwidth and timeout sample count
		
		// TODO add event_detection as task.
	}

	/*
	 * See header file
	 */
	void init_input_queue(void)
	{
		unsigned short i;
		
		for(i = 0; i < INPUTQUEUE_LEN; i++){
			input_queue.queue[i].timestamp = 0;
			input_queue.queue[i].value = 0;
		}
		
		input_queue.read_pos = 0;
		input_queue.write_pos = 0;
		input_queue.count = 0;
	}

	/*
	 * See header file
	 */
	void enqueue_input(uint32_t timestamp, uint32_t value)
	{
		// XXX critical, disable INT
    
		if(input_queue.count > 0){
			/* Insert event in queue */
			input_queue.queue[input_queue.write_pos].timestamp = timestamp;
			input_queue.queue[input_queue.write_pos].timestamp = value;
	
			/* Update write position */
			input_queue.write_pos++;
			input_queue.count++;
			if (input_queue.write_pos >= INPUTQUEUE_LEN) {
					input_queue.write_pos = 0;
			}
		}
    
		// TODO enable INT
		
	}

	/*
	 * See header file
	 */
	Input_t dequeue_input(void)
	{
		Input_t the_input;
		// TODO critical, disable INT
		if(input_queue.count < INPUTQUEUE_LEN){
			the_input = input_queue.queue[input_queue.read_pos];
			input_queue.read_pos++;
			input_queue.count--;
			if(input_queue.read_pos >= INPUTQUEUE_LEN){
				input_queue.read_pos = 0;
			}
			return the_input;
		} else {
			pcSerial.printf("\n\n==================================\nFATAL ERROR: Input Queue Overflow.\n==================================\n\n");
			the_input.timestamp = 0;
			the_input.value = 0;
			return the_input;
		}
		// TODO enable INT
	}

	/*
	 * See header file
	 */
/*	unsigned char has_input(void)
	{
		// TODO critical, disable INT
		return
		// TODO enable INT 
		
	}
*/
	
	/*
	 * See header file
	 */
/*	unsigned char has_room(void)
	{
		// TODO critical, disable INT
		
        input_queue.queue[input_queue.write_pos].timestamp = timestamp;
		// TODO enable INT
	}
	*/
