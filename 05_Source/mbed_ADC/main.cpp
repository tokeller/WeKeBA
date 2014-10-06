#include "mbed.h"
#include "pinmap.h"
#include "analogin_api.h"

Serial pcSerial(USBTX, USBRX);
DigitalOut myled(LED1);
DigitalOut pinser(p19);
char ctr = 0;
uint32_t clkdiv = 1;

static const PinMap PinMap_ADC[] = {
    {P0_23, ADC0_0, 0x01},
    {P0_24, ADC0_1, 0x01},
    {P0_25, ADC0_2, 0x01},
    {P0_26, ADC0_3, 0x01},
    {P1_30, ADC0_4, 0x03},
    {P1_31, ADC0_5, 0x03},
    {P0_12, ADC0_6, 0x03},
    {P0_13, ADC0_7, 0x03},
    {NC   , NC    , 0   }
};

extern "C" void ADC_IRQHandler()
{
	LPC_ADC->CR = (0 << 0)        // SEL: 0 = no channels selected
                  | (clkdiv << 8) // CLKDIV:
                  | (0 << 16)     // BURST: 0 = software control
                  | (1 << 21)     // PDN: 1 = operational
                  | (1 << 24)     // START: 1 = start conversion now
                  | (0 << 27);    // EDGE: not applicable
	ctr++;
  if (ctr%2==0){
		//pcSerial.printf("off\n");
		myled = 0;
		pinser = 1;
	}else{
		//pcSerial.printf("on\n");
		myled = 1;
		pinser = 0;
	}
} 

static inline int div_round_up(int x, int y) {
  return (x + (y - 1)) / y;
}


int main() {
    pcSerial.baud(9600);
		pcSerial.printf("start\n");
		PinName pin = p20;
		pinmap_peripheral(pin, PinMap_ADC);
    int adc = (ADCName)pinmap_peripheral(pin, PinMap_ADC);
    MBED_ASSERT(adc != (ADCName)NC);
	
		// PowerOn
		LPC_SC->PCONP |= (1<<12);
		uint32_t PCLK = PeripheralClock;
    // calculate minimum clock divider
    //  clkdiv = divider - 1
    //uint32_t MAX_ADC_CLK = 310000;
		uint32_t MAX_ADC_CLK = 30420;
    clkdiv = div_round_up(PCLK, MAX_ADC_CLK) - 1;
    
    // must enable analog mode (ADMODE = 0)
    __IO uint32_t *reg = (__IO uint32_t*) (LPC_IOCON_BASE + 4 * p20);
    *reg &= ~(1 << 7);
	
	
		// Set the generic software-controlled ADC settings
    LPC_ADC->CR = (0 << 0)        // SEL: 0 = no channels selected
                  | (clkdiv << 8) // CLKDIV:
                  | (0 << 16)     // BURST: 0 = software control
                  | (1 << 21)     // PDN: 1 = operational
                  | (1 << 24)     // START: 1 = start conversion now
                  | (0 << 27);    // EDGE: not applicable
	

		pinmap_pinout(pin, PinMap_ADC);

									
		NVIC_SetVector(ADC_IRQn, (uint32_t)ADC_IRQHandler);
    NVIC_SetPriority(ADC_IRQn, 0);       /* highest priority */
  	//enable ADC interrupt
		NVIC_EnableIRQ(ADC_IRQn);
		
		while(1) {
				pcSerial.printf("huhu\n");
				wait(5);
        /*myled = 1;
        wait(0.2);
        myled = 0;
        wait(0.2);*/
    }
}
