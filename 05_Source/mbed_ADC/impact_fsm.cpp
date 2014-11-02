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
  Event_FSM event;
	event.id= E_RESET;
  event.timestamp = 0;
  event.value = 0;
  impact_fsm(event);
}
 
/*
 * See header file
 */
void impact_fsm(Event_FSM event)
{
	static State state = S_NOT_IN_EVENT;
	
    
	EventID id = event.id;
	
	// TODO in impact_event: decrease timeout counter, if zero, we have event E_TIMEOUT
	switch(state){
		
		/* -- State Not_In_Event
		* ------------------------------------------------------- */
		case S_NOT_IN_EVENT:
			
			switch(id){
				
				case E_RESET:
					break;
				
				case E_INPUT_HIGH:
					// action
				  // new impact
				  // add peak
				  // update_peak_maximum
					state = S_IN_EVENT_PEAK;
					break;
				
				case E_INPUT_LOW:
					break;
				
				case E_TIMEOUT:
					break;
				
				default: ;
			}
			
			break;
		
		/* -- State In_Event_In_Peak
		* ------------------------------------------------------- */			
		case S_IN_EVENT_PEAK:
			
			switch(id){
				
				case E_RESET:
					state = S_NOT_IN_EVENT;
				  break;
				
				case E_INPUT_HIGH:
					// action
				  // update_peak_maximum
					break;
				
				case E_INPUT_LOW:
					// action
				  // start_timer
					state = S_IN_EVENT_NO_PEAK;
					break;
				
				case E_TIMEOUT:
					break;
				
				default: ;
			}
			
			break;
		
		/* -- State In_Event_Not_In_Peak
		* ------------------------------------------------------- */
		case S_IN_EVENT_NO_PEAK:
			
			switch(id){
				
				case E_RESET:
					state = S_NOT_IN_EVENT;
				  break;
				
				case E_INPUT_HIGH:
					// action
				  // add_peak
				  // update_maxima
					state = S_IN_EVENT_PEAK;
					break;
				
				case E_INPUT_LOW:
					break;
				
				case E_TIMEOUT:
					// action
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
