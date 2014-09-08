#include "mbed.h"
#include "cmsis_os.h"

Serial pcSerial(USBTX, USBRX);
Ticker ticker;
DigitalOut led1(LED1);
DigitalOut led2(LED2);
CAN can1(p9, p10);
CAN can2(p34, p33);
char counter = 0;

void led2_thread(void const *args) {
    while (true) {
        led2 = !led2;
        osDelay(1000);
    }
}

osThreadDef(led2_thread, osPriorityNormal, DEFAULT_STACK_SIZE);


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
    ticker.attach(&send, 1);
    CANMessage msg;
    
    osThreadCreate(osThread(led2_thread), NULL);    
    
    while(1) {
        if (pcSerial.readable()) {
         
            printf("loop()\n");
            if(can2.read(msg)) {
                pcSerial.printf("Message received: %d\n", msg.data[0]);
            //    pcSerial.printf("Message received: %d\n", 1);
                led2 = !led2;
            } 
            wait(0.2);
        }
        led1 = !led1;
    }
}