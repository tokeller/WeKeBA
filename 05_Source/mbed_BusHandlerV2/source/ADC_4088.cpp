#include "ADC_4088.h"

Ticker ticker;
uint32_t clkdiv;
uint32_t data_o;

static inline int div_round_up(int x, int y) {
  return (x + (y - 1)) / y;
};

void start_ADC_Conversion(){
		// start conversion, when the ticker fires
		LPC_ADC->CR |=(1 << 24);

}

int register_ADC_interrupt(analogin_s *obj, PinName pin, uint32_t ADC_IRQHandler, uint32_t interval){
	
	
	obj->adc = (ADCName) pinmap_peripheral(pin, PinMap_ADC);
	if (obj->adc == (ADCName)NC){
		return 1;
	}

	// PowerOn
	LPC_SC->PCONP |= (1<<12);

	// calculate minimum clock divider
	//  clkdiv = divider - 1
	//uint32_t MAX_ADC_CLK = 310000;
	// ca 10khz
	uint32_t MAX_ADC_CLK = 12400000;
	//uint32_t MAX_ADC_CLK = 31000;
	
	
	uint32_t PCLK = PeripheralClock;
	// PCLK 60'000'000 
	clkdiv = div_round_up(PCLK, MAX_ADC_CLK) - 1;
	// must enable analog mode (ADMODE = 0)
	__IO uint32_t *reg = (__IO uint32_t*) (LPC_IOCON_BASE + 4 * pin);
	*reg &= ~(1 << 7);


	// Set the generic software-controlled ADC settings
	//LPC_ADC->CR = (0 << 0)        // SEL: 0 = no channels selected
	LPC_ADC->CR = (1 << (int)obj->adc)        // SEL: 0 = no channels selected
								| (clkdiv << 8) // CLKDIV:
								| (0 << 16)     // BURST: 0 = software control
								| (1 << 21)     // PDN: 1 = operational
								| (0 << 24)     // START: 1 = start conversion now
								| (0 << 27);    // EDGE: not applicable


	pinmap_pinout(pin, PinMap_ADC);

								
	NVIC_SetVector(ADC_IRQn, (uint32_t)ADC_IRQHandler);
	NVIC_SetPriority(ADC_IRQn, 0);       //highest priority
	//enable ADC interrupt
	NVIC_EnableIRQ(ADC_IRQn);

	ticker.attach_us(&start_ADC_Conversion, interval);
	
	return clkdiv;
	
	
};

void stop_ADC_Conversion()
{
	  // stop conversion
		LPC_ADC->CR &=0xFEFFFFFF;
};

int get_ADC_result(analogin_s *obj){
  data_o = LPC_ADC->GDR;
	return (data_o >> 4) & 0xFFF; // 12 bit range
	
};


