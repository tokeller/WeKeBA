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
    Event event = E_RESET;    
    fsm(event, 0);
}
 
/*
 * See header file
 */
void impact_fsm(Event event, signed int value)
{
	static State state = S_NOT_IN_EVENT;
	
	// TODO in impact_event: decrease timeout counter, if zero, we have event E_TIMEOUT
	switch(state){
		
		/* -- State Not_In_Event
		* ------------------------------------------------------- */
		case S_NOT_IN_EVENT:
			
			switch(event){
				
				case E_RESET:
					break;
				
				case E_INPUT_HIGH:
					// action
				  // new impact
				  // add peak
				  // update_peak_maximum
				  // update_impact_maximum
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
			
			switch(event){
				
				case E_RESET:
					state = S_NOT_IN_EVENT;
				  break;
				
				case E_INPUT_HIGH:
					// action
				  // update_peak_maximum
				  // update_impact_maximum
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
			
			switch(event){
				
				case E_RESET:
					state = S_NOT_IN_EVENT;
				  break;
				
				case E_INPUT_HIGH:
					// action
				  // add_peak
				  // update_peak_maximum
				  // update_impact_maximum
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
