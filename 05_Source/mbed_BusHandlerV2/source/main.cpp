#include "mbed.h"
#include "rtos.h"

#include "BusHandler.h"

//#define LOGGER
#define SENSOR




Serial pcSerial(USBTX, USBRX);


extern "C" void HardFault_Handler(){
    error("hardfault\n");
}

extern "C" void BusFault_Handler(){
    error("busfault\n");
}

extern "C" void UsageFault_Handler(){
    error("usagefault\n");
}

extern "C" void MemManage_Handler(){
    error("mmufault\n");
};



 
int main() {
	pcSerial.baud(115200);
	start_CAN_Bus();
	pcSerial.printf("start\n");
	#ifdef SENSOR
			Thread thread(CAN_thread);
	#endif
	#ifdef LOGGER
			Thread threadRec(CAN_REC_thread,NULL,osPriorityNormal);
			Thread threadPrn(CAN_PRINT_thread,NULL,osPriorityNormal);
	#endif
	while (1);
}
