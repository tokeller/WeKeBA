#include "impact_fsm.h"


/* ------------------------------------------------------------------
 * -- Externals 
 * --------------------------------------------------------------- */ 
 
extern Serial pcSerial;

/* ------------------------------------------------------------------
 * -- Global variables
 * --------------------------------------------------------------- */
 

/* ------------------------------------------------------------------
 * -- Functions
 * --------------------------------------------------------------- */ 
 
/*
 * See header file
 */
void init_impact_fsm(void)
{
  EventID event;
	Input_t input;
	
	event= E_RESET;
  input.timestamp = 0;
  input.value = 0;
  impact_fsm(event, input);
}
 
/*
 * See header file
 */
void impact_fsm(EventID event, Input_t input)
{
	static State state = S_NOT_IN_EVENT;
	
	// TODO in impact_event: decrease timeout counter, if zero, we have event E_TIMEOUT
	switch(state){
		
		/* -- State Not_In_Event
		* ------------------------------------------------------- */
		case S_NOT_IN_EVENT:
			
			switch(event){
				
				case E_RESET:
					// stop timer
					break;
				
				case E_INPUT_HIGH:
					// action
				  // new impact
				  // add sample
				  // add peak
				  // update_peak_maximum
					state = S_IN_EVENT_PEAK;
					break;
				
				case E_INPUT_LOW:
					break;
				
				case E_TIMEOUT:
					// stop timer
					break;
				
				default: ;
			}
			
			break;
		
		/* -- State In_Event_In_Peak
		* ------------------------------------------------------- */			
		case S_IN_EVENT_PEAK:
			
			switch(event){
				
				case E_RESET:
					// stop timer
					state = S_NOT_IN_EVENT;
				  break;
				
				case E_INPUT_HIGH:
					// action
				  // add sample
				  // update_peak_maximum
					break;
				
				case E_INPUT_LOW:
					// action
				  // add sample
				  // end peak
				  // start_timer
					state = S_IN_EVENT_NO_PEAK;
					break;
				
				case E_TIMEOUT:
					// stop timer
					break;
				
				default: ;
			}
			
			break;
		
		/* -- State In_Event_Not_In_Peak
		* ------------------------------------------------------- */
		case S_IN_EVENT_NO_PEAK:
			
			switch(event){
				
				case E_RESET:
					// stop timer
					state = S_NOT_IN_EVENT;
				  break;
				
				case E_INPUT_HIGH:
					// action
				  // add sample
				  // stop timer
					state = S_IN_EVENT_PEAK;
					break;
				
				case E_INPUT_LOW:
					// action
				  // add sample
					break;
				
				case E_TIMEOUT:
					// action
				  // stop timer
				  // write impact to file
					state = S_NOT_IN_EVENT;
					break;
				
				default: ;
			}
			
			break;
		
		default: ;
			break;
	}
	
}
