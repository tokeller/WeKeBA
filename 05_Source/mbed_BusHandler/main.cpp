#include "mbed.h"
#include "BusHandler.h"

#define LOGGER_UNIT
//#define SENSOR_UNIT

CAN can1(p9, p10);
Serial pcSerial(USBTX, USBRX);


DigitalOut myled(LED1);

int main() {
		
		//deviceType_t log = LOGGER;
		deviceType_t log = SENSOR;
		init(log);
	
	/*	message_t msg;
		msg.msgType = GET_SENSOR_SERIAL_BC;
		sendMsg(msg);
    while(1) {
		*/		CANMessage msg;
    while(1) {
        //if (pcSerial.readable()) {
					//	ainO = ain.read();
						//printf("analog read: %f\n", ainO);
        //    printf("loop()\n");
          	if(can1.read(msg)) {
                pcSerial.printf("Message received: %d\n", 1);
                myled = !myled;
            } 
		
/*        myled = 1;
        wait(0.2);
        myled = 0;
        wait(0.2);
    */}
}
