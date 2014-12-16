#ifndef PROCLOOPS
#define PROCLOOPS
#include "mbed.h"
#include "BusHandler.h"
#include "SerialID.h"
#include "ADC_4088.h"
#include "impact_fsm.h"
#include "rtos.h"
#include "sensor_config.h"
#include "cmd.h"
#include "file_ops.h"
#include "UART_RTOS_Interrupt.h"


void sensor_loop(void const *args);


void logger_loop(void const *args);

//void get_cmd_event_thread(void const *args);

#endif