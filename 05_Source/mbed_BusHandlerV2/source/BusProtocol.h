#ifndef LPC4088_CANBUS
#define LPC4088_CANBUS

#include "mbed.h"
#include "rtos.h"

#define BUFFER_SIZE 512

typedef struct {
		CANMessage queue[BUFFER_SIZE];
		uint16_t read_pos;
		uint16_t write_pos;
		uint16_t count;
} outputRingbuf;


int CAN_init(void);

uint32_t storeRecMessages(CANMessage sentMsg);

CANMessage dequeueOutput (void);

int sendMessage(CANMessage msg);

void CANIRQHandler(void);

#endif
