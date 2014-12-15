#ifndef PROCLOOPS
#define PROCLOOPS
#include "mbed.h"
#include "BusHandler.h"
#include "SerialID.h"
#include "ADC_4088.h"
#include "impact_fsm.h"
#include "rtos.h"


void sensor_loop(void const *args);


void logger_loop(void const *args);


#endif