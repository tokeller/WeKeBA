#include "mbed.h"

#define LPC_CAN1_ERR LPC_CAN1_BASE + 0x08;

Serial pcSerial(USBTX, USBRX);
Ticker ticker;
DigitalOut led1(LED1);
DigitalOut led2(LED2);
CAN can1(p9, p10);
CAN can2(p34, p33);
AnalogIn ain(p20);
char counter = 0;
float ainO = 0.0;
char message[8] = {0,0,0,0,0,0,0,0};

void send() {
    pcSerial.printf("send()\n");
    if(can1.write(CANMessage(1337, &counter, 1))) {
        pcSerial.printf("wloop()\n");
        counter++;
        pcSerial.printf("Message sent: %d\n", counter);
        led1 = !led1;
    } 
}

int main() {
		int err = 0;
    pcSerial.baud(9600);
    pcSerial.printf("main()\n");
    //ticker.attach(&send, 1);
    //CANMessage msg = CANMessage(1337, message, 8, CANData, CANExtended);
		CANMessage msg;
    while(1) {
        //if (pcSerial.readable()) {
					//	ainO = ain.read();
						//printf("analog read: %f\n", ainO);
        //    printf("loop()\n");
          	if(can1.read(msg)) {
            //    pcSerial.printf("Message received: %s\n", msg.data);
								err = LPC_CAN1_ERR;
								pcSerial.printf("error: %d\n",(err));
                pcSerial.printf("Message received: %d\n", 1);
                led2 = !led2;
            } 
            //wait(0.2);
        //}
    }
}
