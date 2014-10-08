#include "mbed.h"

Serial pcSerial(USBTX, USBRX);
Ticker ticker;
DigitalOut led1(LED1);
DigitalOut led2(LED2);
CAN can1(p9, p10);
CAN can2(p34, p33);
AnalogIn ain(p20);
char counter = 0;
float ainO = 0.0;

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
    pcSerial.baud(9600);
    pcSerial.printf("main()\n");
    //ticker.attach(&send, 1);
    CANMessage msg;
    while(1) {
        //if (pcSerial.readable()) {
					//	ainO = ain.read();
						//printf("analog read: %f\n", ainO);
        //    printf("loop()\n");
          	if(can1.read(msg)) {
                pcSerial.printf("Message received: %d\n", msg.data[0]);
            //    pcSerial.printf("Message received: %d\n", 1);
                led2 = !led2;
            } 
            //wait(0.2);
        //}
    }
}
