#include "mbed.h"
#include "ADC_4088.h"

Serial pcSerial(USBTX, USBRX);
DigitalOut myled(LED1);
DigitalOut pinser(p19);
char ctr = 0;
analogin_s adc;


extern "C" void ADC_IRQHandler()
{
	reset_ADC_interrupt(&adc);
	uint32_t data = 0;
	data = get_ADC_result(&adc);
	ctr++;
  if (ctr%2==0){
		pcSerial.printf("off data: %d\n", data);
		myled = 0;
		pinser = 1;
	}else{
		pcSerial.printf("on data:  %d\n", data);
		myled = 1;
		pinser = 0;
	}
} 



int main() {
	pcSerial.baud(9600);
	pcSerial.printf("start\n");
	PinName pin = p20;
	uint32_t CCLK = SystemCoreClock;
	pcSerial.printf("cclock: %d\n", CCLK);
	
	register_ADC_interrupt(&adc, pin,(uint32_t) ADC_IRQHandler,1000);


	while(1) {
		pcSerial.printf("huhu\n");
		wait(5);
		/*myled = 1;
		wait(0.2);
		myled = 0;
		wait(0.2);*/
	}
}
