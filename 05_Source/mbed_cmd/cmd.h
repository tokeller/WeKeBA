#ifndef CMD_H
#define CMD_H

#include "stdint.h"


/* ------------------------------------------------------------------
 * -- Type definitions
 * --------------------------------------------------------------- */
	typedef enum state{
		S_BASEMENU,
		S_LIST_FILES,
		S_DELETE_FILE,
		S_FORMAT_SD,
		S_LOGGER_START_STOP,
		S_SENSOR_PARAMETERS,
    S_SENSOR_PARAMS_GET_SENSOR_NR,
		S_SENSOR_PARAMS_FS,
		S_SENSOR_PARAMS_THRES,
		S_SENSOR_PARAMS_BASELINE,
		S_SENSOR_PARAMS_TIMEOUT,
		S_SENSOR_PARAMS_DETAIL,
		S_SENSOR_PARAMS_DETAIL_RAW_DURATION, // TODO
		S_SENSOR_STATE,
		S_RESET_TIMESTAMP,
		S_INTERNAL_CLOCK,
		S_INTERNAL_CLOCK_SET_DATE,
		S_INTERNAL_CLOCK_SET_TIME,
		S_CONFIG_FILE
	} MenuState;


/* ------------------------------------------------------------------
 * -- Structures
 * --------------------------------------------------------------- */

/* ------------------------------------------------------------------
 * -- Prototypes
 * --------------------------------------------------------------- */
void init_menu_fsm(void);
void menu_fsm(uint32_t input);
void get_cmd_event_thread(void const *args);
void write_file (void);

#endif
