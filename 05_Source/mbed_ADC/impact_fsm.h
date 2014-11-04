
#ifndef IMPACT_FSM
#define IMPACT_FSM

#include "mbed.h"
#include "impact_event.h"
#include "impact_action.h"

#ifdef __cplusplus
extern "C" {
#endif
	
	typedef enum state{
		S_NOT_IN_IMPACT,
		S_IN_IMPACT_PEAK,
		S_IN_IMPACT_NO_PEAK
	} State;
	

/* ------------------------------------------------------------------
 * -- Function prototypes
 * --------------------------------------------------------------- */

/**
 * Initialize the state machine
 *   @param   none
 *   @retval  none
 */
void init_impact_fsm(void);

/**
 *   The Finite State Machine
 *   Event handling depending on current state of the FSM
 *   @param   new_event: Event_t defining what happened
 *   @param   input: Input_t with timestamp and value
 *   @retval  none
 */
void impact_fsm(EventID new_event, Input_t input);
	
#ifdef __cplusplus
};
#endif
/* ------------------------------------------------------------------
 * -- Header file end
 * --------------------------------------------------------------- */
#endif
