#include "mbed.h"
#include "sdram.h"
#include "ProcessingLoops.h"

// Logger: 150576ea
// Sensor: 61bfdf6


//#define DEBUG_IMPACT

#ifdef DEBUG_IMPACT
Input_t deb_data[1000] = 
#include "golfb10k.h"
;
#endif



AnalogIn pinser2(p17);
AnalogIn pinser3(p16);

DigitalOut led2(LED2);

/*
 * ADC variables
 *
 */



Serial pcSerial(USBTX, USBRX);
 
int main() {
	pcSerial.baud(115200);
	if (sdram_init()) {
		printf("Failed to initialized SDRAM\n");
	}
	#ifdef SEN
		pcSerial.printf("start\n");
		sensor_loop(0);
	#endif
	#ifdef LOG
		pcSerial.printf("start\n");
		logger_loop(0);
	#endif
	while (1){
	}
}

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

	/*char data[5] = {'h','a','l','l','o'};
	  enqueueMessage(5,data,0x04,0x01,SEND_TOKEN_SINGLE);
	  char data1[5] = {'h','u','h','u','u'};
	  enqueueMessage(5,data1,0x04,0x01,SENSOR_CONFIG_SINGLE);
		osDelay(1000);
		char data2[5] = {'h','a','h','a','h'};
		enqueueMessage(5,data2,0x04,0x01,SENSOR_OFF_SINGLE);
		osDelay(1000);
		char data3[5] = {'h','i','h','i','i'};
		enqueueMessage(5,data3,0x04,0x01,OP_MODE_SINGLE);
		osDelay(1000);
		
		char data8[5] = {'f','a','a','i','l'};
		enqueueMessage(5,data8,0x05,0x01,OP_MODE_SINGLE);
		osDelay(1000);

		char data5[5] = {'t','r','a','a','a'};
		enqueueMessage(5,data5,0xff,0x01,START_REC_BC);
		osDelay(1000);
		
		char data6[5] = {'b','l','u','b','b'};
		enqueueMessage(5,data6,0xff,0x01,SENSOR_OFF_BC);
		osDelay(1000);
		
		char data7[5] = {'b','l','a','a','h'};
		enqueueMessage(5,data7,0xff,0x01,OP_MODE_BC);
		osDelay(1000);		
		*/
