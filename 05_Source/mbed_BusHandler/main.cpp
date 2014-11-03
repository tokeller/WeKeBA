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
    }
}
