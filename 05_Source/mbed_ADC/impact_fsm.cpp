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
void impact_fsm(EventID event, Input_t fsm_input)
{
	static State state = S_NOT_IN_IMPACT;
	
	switch(state){
		
		/* -- State Not_In_Impact
		* ------------------------------------------------------- */
		case S_NOT_IN_IMPACT:
			
			switch(event){
				
				case E_RESET:
				  stop_timer();
					break;
				
				case E_INPUT_HIGH_POS:
				  new_impact(fsm_input);
					add_sample(fsm_input);
					update_maxima_pos(fsm_input);
				  stop_timer();
					state = S_IN_IMPACT_PEAK_POS;
					break;
				
				case E_INPUT_HIGH_NEG:
					new_impact(fsm_input);
					add_sample(fsm_input);
					update_maxima_neg(fsm_input);
					stop_timer();
					state = S_IN_IMPACT_PEAK_NEG;
					break;
				
				case E_INPUT_LOW:
					break;
				
				case E_TIMEOUT:
				  stop_timer();
					break;
				
				default: 
					break;
			}
			
			break;
		
		/* -- State S_IN_IMPACT_PEAK_POS
		* ------------------------------------------------------- */			
		case S_IN_IMPACT_PEAK_POS:
			
			switch(event){
				
				case E_RESET:
				  stop_timer();
					state = S_NOT_IN_IMPACT;
				  break;
				
				case E_INPUT_HIGH_POS:
				  add_sample(fsm_input);
					update_maxima_pos(fsm_input);
					break;
				
				case E_INPUT_HIGH_NEG:
					add_sample(fsm_input);
					end_peak();
					update_maxima_neg(fsm_input);
					state = S_IN_IMPACT_PEAK_NEG;
					break;
				
				case E_INPUT_LOW:
				  add_sample(fsm_input);
				  end_peak();
				  start_timer();
					state = S_IN_IMPACT_NO_PEAK;
					break;
				
				case E_TIMEOUT:
				  stop_timer();
					break;
				
				default: 
					break;
			}
			
			break;
		
		/* -- State S_IN_IMPACT_PEAK_NEG
		* ------------------------------------------------------- */			
		case S_IN_IMPACT_PEAK_NEG:
			
			switch(event){
				
				case E_RESET:
				  stop_timer();
					state = S_NOT_IN_IMPACT;
				  break;
				
				case E_INPUT_HIGH_POS:
				  add_sample(fsm_input);
					end_peak();
					update_maxima_pos(fsm_input);
					state = S_IN_IMPACT_PEAK_POS;
					break;
				
				case E_INPUT_HIGH_NEG:
					add_sample(fsm_input);
					update_maxima_neg(fsm_input);
					break;
				
				case E_INPUT_LOW:
				  add_sample(fsm_input);
				  end_peak();
				  start_timer();
					state = S_IN_IMPACT_NO_PEAK;
					break;
				
				case E_TIMEOUT:
				  stop_timer();
					break;
				
				default: 
					break;
			}
			
			break;
		
		/* -- State S_IN_IMPACT_NO_PEAK
		* ------------------------------------------------------- */
		case S_IN_IMPACT_NO_PEAK:
			
			switch(event){
				
				case E_RESET:
				  stop_timer();
					state = S_NOT_IN_IMPACT;
				  break;
				
				case E_INPUT_HIGH_POS:
				  add_sample(fsm_input);
					update_maxima_pos(fsm_input);
				  stop_timer();
					state = S_IN_IMPACT_PEAK_POS;
					break;
				
				case E_INPUT_HIGH_NEG:
					add_sample(fsm_input);
					update_maxima_neg(fsm_input);
					stop_timer();
					state = S_IN_IMPACT_PEAK_NEG;
				
				case E_INPUT_LOW:
				  add_sample(fsm_input);
					break;
				
				case E_TIMEOUT:
				  stop_timer();
					// end impact
					store_impact();
					state = S_NOT_IN_IMPACT;
					break;
				
				default: 
					break;
			}
			
			break;
		
		default: 
			break;
	}
	
}
