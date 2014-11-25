#include "impact_fsm.h"

/* TO DO
 * 
 *
 *
 *
 */



/* ------------------------------------------------------------------
 * -- Externals 
 * --------------------------------------------------------------- */ 
 
extern Serial pcSerial;
extern uint32_t timeout_counter;

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
			#ifdef DEBUG_IMPACT_
			printf("NotInImpact, E: %d, T: %d, V: %d, TO: %d\n", event, fsm_input.timestamp, fsm_input.value, timeout_counter);
			#endif
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
			#ifdef DEBUG_IMPACT_
			printf("InImpactPeakPos, E: %d, T: %d, V: %d, TO: %d\n", event, fsm_input.timestamp, fsm_input.value, timeout_counter);
			#endif
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
			#ifdef DEBUG_IMPACT_
			printf("InImpactPeakNeg, E: %d, T: %d, V: %d, TO: %d\n", event, fsm_input.timestamp, fsm_input.value, timeout_counter);
			#endif
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
			#ifdef DEBUG_IMPACT_
			printf("InImpactNoPeak, E: %d, T: %d, V: %d, TO: %d\n", event, fsm_input.timestamp, fsm_input.value, timeout_counter);
			#endif
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
					break;
				
				case E_INPUT_LOW:
				  add_sample(fsm_input);
					break;
				
				case E_TIMEOUT:
				  stop_timer();
					end_impact();
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
