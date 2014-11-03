#include "mbed.h"
#include "ADC_4088.h"
#include "impact_fsm.h"

Serial pcSerial(USBTX, USBRX);
DigitalOut myled(LED1);
DigitalOut pinser(p14);
AnalogIn pinser1(p18);
AnalogIn pinser2(p17);
AnalogIn pinser3(p16);
/*DigitalOut pinser4(p15);*/
char ctr = 0;
analogin_s adc;
uint32_t data = 0;


extern "C" void ADC_IRQHandler()
{
	data = get_ADC_result(&adc);
	ctr++;
  pinser = ctr%2;
	if(data > 2100 || data < 1900){
	  pcSerial.printf("get: %d\n", data);
	}
	stop_ADC_Conversion();
} 



int main() {
	pcSerial.baud(9600);
	pcSerial.printf("start\n");
	PinName pin = p15;
	uint32_t CCLK = SystemCoreClock;
	pcSerial.printf("cclock: %d\n", CCLK);
	
	uint32_t clkd = register_ADC_interrupt(&adc, pin,(uint32_t) ADC_IRQHandler,100);
	pcSerial.printf("clkdiv: %d\n", clkd);
	while(1) {
		pcSerial.printf("huhu\n");
		wait(5);
		/*myled = 1;
		wait(0.2);
		myled = 0;
		wait(0.2);*/
	}
}
