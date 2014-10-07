#include "ADC_4088.h"

uint32_t clkdiv;

static inline int div_round_up(int x, int y) {
  return (x + (y - 1)) / y;
};

int register_ADC_interrupt(PinName pin, uint32_t ADC_IRQHandler, uint32_t time){
	
	pinmap_peripheral(pin, PinMap_ADC);
	int adc = (ADCName)pinmap_peripheral(pin, PinMap_ADC);
	MBED_ASSERT(adc != (ADCName)NC);

	// PowerOn
	LPC_SC->PCONP |= (1<<12);

	// calculate minimum clock divider
	//  clkdiv = divider - 1
	//uint32_t MAX_ADC_CLK = 310000;
	uint32_t MAX_ADC_CLK = 30420;
	
	
	uint32_t PCLK = PeripheralClock;
	// PCLK 60'000'000 
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
	
	
	return 0;
	
	
};

int reset_ADC_interrupt()
{
	
	LPC_ADC->CR = (0 << 0)        // SEL: 0 = no channels selected
                  | (clkdiv << 8) // CLKDIV:
                  | (0 << 16)     // BURST: 0 = software control
                  | (1 << 21)     // PDN: 1 = operational
                  | (1 << 24)     // START: 1 = start conversion now
                  | (0 << 27);    // EDGE: not applicable
	return 0;
	
}
