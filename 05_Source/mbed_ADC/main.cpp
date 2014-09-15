#include "mbed.h"
#define USR_UID 0x101
 
AnalogIn ain1(p19);
AnalogIn ain2(p20);
DigitalOut led(LED1);
Serial pcSerial(USBTX, USBRX);
void setAnalogSpeed(int clock);
int semihost_uid(char *uid);


int main() {
    pcSerial.baud(9600);
    pcSerial.printf("main()\n");
    float in1 = 0.0f;
    float in2 = 0.0f;
    float oin1 = 0.0f;
    float oin2 = 0.0f;
		char res[4];
		char* uidOut = res;
		semihost_uid(uidOut);
    //setAnalogSpeed(10000);
    while (1){
     //   in1 = ain1.read();
      //  in2 = ain2.read();
				pcSerial.printf("uid: %c\n",uidOut); 			
        //pcSerial.printf("ain1: %f ain2: %f\n",in1, in2); 			
    }
}

int semihost_uid(char *uid) {
  int32_t args[2];
  args[0] = (uint32_t)uid;
  args[1] = 33;
  return __semihost(USR_UID, &args);
}

void setAnalogSpeed(int clock){
	uint32_t PCLK = PeripheralClock;
	uint32_t MAX_ADC_CLK = clock;
  uint32_t clkdiv = ((PCLK + (MAX_ADC_CLK - 1)) / MAX_ADC_CLK) - 1;

	// Set the generic software-controlled ADC settings
    LPC_ADC->CR = (0 << 0)      // SEL: 0 = no channels selected
                  | (clkdiv << 8) // CLKDIV:
                  | (0 << 16)     // BURST: 0 = software control
                  | (1 << 21)     // PDN: 1 = operational
                  | (0 << 24)     // START: 0 = no start
                  | (0 << 27);    // EDGE: not applicable
	
	
}
