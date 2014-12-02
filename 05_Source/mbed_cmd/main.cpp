#include "mbed.h"
#include "cmsis_os.h"
#include "MCIFileSystem.h"
#include "cmd.h"
 
DigitalOut led1(LED1);
DigitalOut led2(LED2);

Serial pcSerial(USBTX, USBRX);

// storage for the sensor configs and defaults
SensorConfig sensors[MAX_SENSORS];
SensorConfig sensor_defaults;


// just some thread
void led2_thread(void const *args) {
    while (true) {
        led2 = !led2;
        osDelay(1000);
    }
}

// command line input reader
void get_cmd_event_thread(void const *args)
{
	uint32_t event;
	char input[20];
	init_menu_fsm();
	while(1){
		pcSerial.printf(" >");
		pcSerial.scanf("%s", input);
		event = atoi(input);
		pcSerial.printf("%d\n", event);
		menu_fsm(event);
	}
}

osThreadDef(led2_thread, osPriorityNormal, DEFAULT_STACK_SIZE);
osThreadDef(get_cmd_event_thread, osPriorityNormal, DEFAULT_STACK_SIZE);
 
int main() {
	printf("start\n");
	set_time(1417860000); // initially set time to 06.12.2014
	
	osThreadCreate(osThread(led2_thread), NULL);
	osThreadCreate(osThread(get_cmd_event_thread),NULL);

	while (true) {
			led1 = !led1;
			osDelay(500);
	}
}

extern "C" void HardFault_Handler(){
	error("hardfault\n");
}
