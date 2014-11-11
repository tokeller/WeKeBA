#include "mbed.h"
#include "BusHandler.h"
#include "cmsis_os.h"

CAN can1(p9, p10);
Serial pcSerial(USBTX, USBRX);

char array[16]  = {65,65,65,65,65,65,65,65,66,66,66,66,66,66,66,66};

char array1[16] = {67,67,67,67,67,67,67,67,68,68,68,68,68,68,68,68};

char array2[16] = {69,69,69,69,69,69,69,69,70,70,70,70,70,70,70,70};

DigitalOut myled(LED1);

int main() {
		
		pcSerial.printf("starting\n");

		osThreadDef(processMessagesLogger, osPriorityNormal, DEFAULT_STACK_SIZE);
		
		pcSerial.printf("thread creating\n");
    osThreadCreate(osThread(processMessagesLogger), NULL);  
		pcSerial.printf("thread created\n");
		//for(int i = 0; i < 3; i++){
			sendMessage(16, array, 0xff, GET_SENSOR_SERIAL_BC);
			sendMessage(16, array1, 0xff, GET_SENSOR_SERIAL_BC);
			sendMessage(16, array2, 0xff, GET_SENSOR_SERIAL_BC);
		//}
		//deviceType_t log = LOGGER;
		/*deviceType_t log = SENSOR;
		init(log);
	
		message_t msg;
		msg.msgType = GET_SENSOR_SERIAL_BC;
		sendMsg(msg);
		CANMessage mesg;
    while(1) {
				if(can1.read(mesg)) {
						pcSerial.printf("Message received: %s\n", mesg.data);
						myled = !myled;
				} else {
					setFilterForID(log,0x0000);
				}
        myled = 1;
        wait(0.2);
        myled = 0;
        wait(0.2);
    }*/
}
