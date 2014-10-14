
#ifndef IMPACT_FSM
#define IMPACT_FSM

#include "mbed.h"
#include "impact_event.h"
#include "impact_action.h"

#ifdef __cplusplus
extern "C" {
#endif
	
	typedef enum state{
		S_NOT_IN_EVENT,
		S_IN_EVENT_PEAK,
		S_IN_EVENT_NO_PEAK
	} State;
	

/* ------------------------------------------------------------------
 * -- Function prototypes
 * --------------------------------------------------------------- */

/* 
 * Initialize the state machine
 */
void init_state_machine(void);

/** The Finite State Machine
*   Event handling depending on current state of the FSM
*/
void fsm(Event new_event, signed int value);
	
#ifdef __cplusplus
};
#endif
/* ------------------------------------------------------------------
 * -- Header file end
 * --------------------------------------------------------------- */
#endif
