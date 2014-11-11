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
uint32_t old_data = 0;

Input_t deb_data[980] = 
#include "golfb10k.h"
;


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
	int deb_i;
	
	pcSerial.baud(115200);
	pcSerial.printf("start\n");
	PinName pin = p15;
	uint32_t CCLK = SystemCoreClock;
	pcSerial.printf("cclock: %d\n", CCLK);
	
	// rudimentary ADC IRQ Handler
	//uint32_t clkd = register_ADC_interrupt(&adc, pin,(uint32_t) ADC_IRQHandler,100);
	
	// fine tuned impact recognition and processing
	//uint32_t clkd = register_ADC_interrupt(&adc, pin,(uint32_t) isr_nextMeasurement,100);
	
	// DEBUG: disable ADC for debugging the impact recognition
	//pcSerial.printf("clkdiv: %d\n", clkd);
	init_impact_event_handler();
	for (deb_i = 0; deb_i < 980; deb_i++){
		enqueue_impact_input(deb_data[deb_i].timestamp, deb_data[deb_i].value); 
	}
	
	pcSerial.printf("debug data loaded, begin analysis\n");
	while(1) {
		impact_event_detection();
		wait_us(50);
	}
}
