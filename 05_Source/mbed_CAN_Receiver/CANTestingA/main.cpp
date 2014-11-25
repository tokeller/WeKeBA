#include "mbed.h"
#include "CAN.h"

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

CANMessage buffer[10];
int bufferCtr = 0;
int printOut = 0;

void send() {
    pcSerial.printf("send()\n");
    if(can1.write(CANMessage(1337, &counter, 1))) {
        pcSerial.printf("wloop()\n");
        counter++;
        pcSerial.printf("Message sent: %d\n", counter);
        led1 = !led1;
    } 
}

void receiveMsg(void){
	CANMessage msg;
	if(can1.read(msg)){
		memcpy(buffer[bufferCtr].data,msg.data,msg.len);
		buffer[bufferCtr].id = msg.id;
		//pcSerial.printf("interrupt %d\n",bufferCtr);
		bufferCtr++;
		printOut = 1;
	}
}

int main() {
		int err = 0;
    pcSerial.baud(115200);
    pcSerial.printf("main()\n");
    //ticker.attach(&send, 1);
    //CANMessage msg = CANMessage(1337, message, 8, CANData, CANExtended);
		can1.attach(&receiveMsg, CAN::RxIrq);
		can1.frequency(1000);
		can1.mode(CAN::Normal);
    while(1) {
				/*CANMessage msg;
        //if (pcSerial.readable()) {
					//	ainO = ain.read();
						//printf("analog read: %f\n", ainO);
        //    printf("loop()\n");
          	if(can1.read(msg)) {
            //    pcSerial.printf("Message received: %s\n", msg.data);
								//err = LPC_CAN1_ERR;
								//pcSerial.printf("error: %d\n",(err));
                pcSerial.printf("Message received: %c\n", msg.data[0]);
                pcSerial.printf("Message received: %c\n", msg.data[1]);
                pcSerial.printf("Message received: %c\n", msg.data[2]);
                pcSerial.printf("Message received: %c\n", msg.data[3]);
                pcSerial.printf("Message received: %c\n", msg.data[4]);
                pcSerial.printf("Message received: %c\n", msg.data[5]);
                pcSerial.printf("Message received: %c\n", msg.data[6]);
                pcSerial.printf("Message received: %c\n", msg.data[7]);
                led2 = !led2;
							
            } 
						
            wait(0.2);
        //}*/
				if (printOut > 0){
					pcSerial.printf("data %d: %s\n",1,buffer[1].data+0x00);
					pcSerial.printf("id %d: %d\n",1,buffer[1].id);					
					pcSerial.printf("data %d: %s\n",2,buffer[2].data+0x00);
					pcSerial.printf("id %d: %d\n",2,buffer[2].id);
					pcSerial.printf("data %d: %s\n",3,buffer[3].data+0x00);
					pcSerial.printf("id %d: %d\n",3,buffer[3].id);
					pcSerial.printf("data %d: %s\n",4,buffer[4].data+0x00);
					pcSerial.printf("id %d: %d\n",4,buffer[4].id);
					pcSerial.printf("data %d: %s\n",5,buffer[5].data+0x00);
					pcSerial.printf("id %d: %d\n",5,buffer[5].id);
					pcSerial.printf("data %d: %s\n",6,buffer[6].data+0x00);
					pcSerial.printf("id %d: %d\n",6,buffer[6].id);
					pcSerial.printf("data %d: %s\n",7,buffer[7].data+0x00);
					pcSerial.printf("id %d: %d\n",7,buffer[7].id);
					pcSerial.printf("data %d: %s\n",8,buffer[8].data+0x00);
					pcSerial.printf("id %d: %d\n",8,buffer[8].id);
					pcSerial.printf("data %d: %s\n",9,buffer[9].data+0x00);
					pcSerial.printf("id %d: %d\n",9,buffer[9].id);
					printOut = 0;
				}
    }
}
