#include "impact_event.h"

/* ------------------------------------------------------------------
 * -- Externals 
 * --------------------------------------------------------------- */ 
 
extern Serial pcSerial;

void isr_nextMeasurement(){
	pcSerial.printf("ISR ADC Event Recognition called.\n");
}