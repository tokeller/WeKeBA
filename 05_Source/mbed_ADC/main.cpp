#include "mbed.h"
 
AnalogIn ain1(p19);
AnalogIn ain2(p20);
DigitalOut led(LED1);
Serial pcSerial(USBTX, USBRX);
 
int main() {
    pcSerial.baud(9600);
    pcSerial.printf("main()\n");
    float in1 = 0.0f;
    float in2 = 0.0f;
    float oin1 = 0.0f;
    float oin2 = 0.0f;
    
    while (1){
        in1 = ain1.read();
        in2 = ain2.read();
        
        pcSerial.printf("ain1: %f ain2: %f\n",in1, in2);
        
        /*if (in1 != oin1){
            pcSerial.printf("ain1: %f\n",in1);
            oin1 = in1;
        }
        if (in2 != oin2){
            pcSerial.printf("ain2: %f\n",in2);
            oin2 = in2;
        }*/
        
    }
}