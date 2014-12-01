#ifndef CMD_H
#define CMD_H

#include "stdint.h"


// Value definitions

//#define UART_CMDBUFFERSIZE_APP          100

//#define UART_STATE_RESTART_APP          0
//#define UART_STATE_PENDING_APP          1
//#define UART_STATE_COMPLETE_APP         2

/* ------------------------------------------------------------------
 * -- Type definitions
 * --------------------------------------------------------------- */
	typedef enum state{
		S_BASEMENU,
		S_LIST_FILES,
		S_DELETE_FILE,
		S_FORMAT_SD,
		S_LOGGER_START_STOP,
		S_DETAIL_LEVEL,
		S_SENSOR_PARAMETERS,
		S_SENSOR_PARAMS_FS,
		S_SENSOR_PARAMS_THRES,
		S_SENSOR_PARAMS_BASELINE,
		S_SENSOR_PARAMS_TIMEOUT,
		S_SENSOR_PARAMS_DETAIL,
		S_SENSOR_STATE,
		S_RESET_TIMESTAMP,
		S_INTERNAL_CLOCK
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
uint32_t Cmd_ls(int argc, char *argv[]);
uint32_t Cmd_rm(int argc, char *argv[]);
uint32_t Cmd_sd_format(int argc, char *argv[]);
uint32_t Cmd_sd_mount(int argc, char *argv[]);
uint32_t Cmd_sd_unmount(int argc, char *argv[]);
uint32_t Cmd_logger_state(int argc, char *argv[]);
uint32_t Cmd_logging(int argc, char *argv[]);
uint32_t Cmd_mode(int argc, char *argv[]);
uint32_t Cmd_sensor_parameters(int argc, char *argv[]);
uint32_t Cmd_sensor_priority(int argc, char *argv[]);
uint32_t Cmd_sensor_state(int argc, char *argv[]);
uint32_t Cmd_timestamp(int argc, char *argv[]);
uint32_t Cmd_time(int argc, char *argv[]);
void write_file (void);

#endif
