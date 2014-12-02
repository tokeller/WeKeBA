#ifndef LPC4088_CAN_HANDLER
#define LPC4088_CAN_HANDLER

#include "BusProtocol.h"

typedef struct {
		uint16_t threshold;			// 10 bit threshold
		uint16_t zeroLevel; 		// 10 bit zero level
		uint16_t samplingRate; 	// 12 bit sampling rate (100 Hz steps)
		uint16_t timeoutRange;	// 16 bit timeout range
} SensorConfigMsg_t;

int start_CAN_Bus(void);

void CAN_REC_thread(void const *args);

void CAN_PRINT_thread(void const *args);

void CAN_thread(void const *args);

#endif
