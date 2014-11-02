#include "impact_event.h"

/* ------------------------------------------------------------------
 * -- Externals 
 * --------------------------------------------------------------- */ 
 
extern Serial pcSerial;
extern unsigned int timestamp;


/* ------------------------------------------------------------------
 * -- Global Variables
 * --------------------------------------------------------------- */

static Input_ringbuf input_queue;
static unsigned int baseline = BASELINE;     // zero position of sensor signal
static unsigned int threshold = THRESHOLD;   // event detection threshold
static unsigned int peak_noise_threshold;    // threshold on peak plateau
static unsigned int samples_timeout = SAMPLES_UNTIL_TIMEOUT;  // how long must signal remain
                                             // below threshold for impact to end
static unsigned int timeout_counter;
static unsigned char timeout_active;         // is timeout counter active (1) or not(0)


/* ------------------------------------------------------------------
 * -- Functions
 * --------------------------------------------------------------- */ 
 
	/*
	 * See header file
	 */
	void isr_nextMeasurement(){
		pcSerial.printf("ISR ADC Event Recognition called.\n");
		// XXX retrieve value, enqueue
	}

	/*
	 * See header file
	 */
	void event_detection(unsigned int input_value)
	{
		//XXX get difference of abs(value - baseline) and threshold,
		//    if positive we have an E_INPUT_HIGH, otherwise it's E_INPUT_LOW
		signed int value; 
		
		EventID new_event_id = E_NO_EVENT;
		Event new_event;
		
		// decrease the timeout_timer. If zero, event is TIMEOUT
		if(timeout_timer >0){
			timeout_timer--;
			if(timeout_timer == 0){
				new_event_id = E_TIMEOUT;
			}
		}
		
		value = abs(input_value - baseline);
		if(value >= threshold){
			new_event_id = E_INPUT_HIGH;
		} else {
			new_event_id = E_INPUT_LOW;
		}
		
		if(new_event_id != E_NO_EVENT)
			new_event.id = new_event_id;
			new_event.timestamp = timestamp;
			new_event.value = input_value;
			enqueue_event(new_event);
		
		
		
	}
	
	/*
	 * See header file
	 */
	void init_event_handler(void)
	{
		// neue queue erstellen, initialisieren
		init_input_queue();
		// XXXsetze nullwert, threshold, peak-erkennungshöhe, timeout
	}

	/*
	 * See header file
	 */
	void init_input_queue(void)
	{
		unsigned char i;
		
		for(i = 0; i < INPUTQUEUE_LEN; i++){
			event_queue.queue[i].id = E_NO_EVENT;
			event_queue.queue[i].timestamp = 0;
			event_queue.queue[i].value = 0;
		}
		
		event_queue.read_pos = 0;
		event_queue.write_pos = 0;
	}

	/*
	 * See header file
	 */
	void enqueue_input(Event event)
	{
		// XXX critical, disable INT
		
	}

	/*
	 * See header file
	 */
	Event dequeue_input(void)
	{
		// XXX critical, disable INT
		
	}

	/*
	 * See header file
	 */
	unsigned char has_input(void)
	{
		// XXX critical, disable INT
		
	}