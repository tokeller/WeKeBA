#include "impact_event.h"

/* ------------------------------------------------------------------
 * -- Externals 
 * --------------------------------------------------------------- */ 
 
extern Serial pcSerial;
extern unsigned int timestamp;


/* ------------------------------------------------------------------
 * -- Global Variables
 * --------------------------------------------------------------- */

static Event_ringbuf event_queue;
static unsigned int baseline = BASELINE;
static unsigned int threshold = THRESHOLD;
static unsigned int samples_timeout = SAMPLES_UNTIL_TIMEOUT;
static unsigned int max_event_length = MAX_EVENT_LENGTH;
static unsigned int timeout_counter;


/* ------------------------------------------------------------------
 * -- Functions
 * --------------------------------------------------------------- */ 
 
	/*
	 * See header file
	 */
	void isr_nextMeasurement(){
		pcSerial.printf("ISR ADC Event Recognition called.\n");
		// XXX retrieve value, hand it over to event_detection
	}

	/*
	 * See header file
	 */
	void event_detection(unsigned int input_value)
	{
		//XXX get difference of abs(Value - baseline) and Threshold, 
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
		init_event_queue();
		// XXXsetze nullwert, threshold, peak-erkennungshöhe, timeout
	}

	/*
	 * See header file
	 */
	void init_event_queue(void)
	{
		unsigned char i;
		
		for(i = 0; i < EVENTQUEUE_LEN; i++){
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
	void enqueue_event(Event event)
	{
		// XXX critical, disable INT
		
	}

	/*
	 * See header file
	 */
	Event dequeue_event(void)
	{
		// XXX critical, disable INT
		
	}

	/*
	 * See header file
	 */
	unsigned char has_event(void)
	{
		// XXX critical, disable INT
		
	}