/* LPC4088 ADC library
 *
 */
 
#ifndef ADC_4088_H
#define ADC_4088_H

#include "mbed.h"
#include "pinmap.h"

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

#ifdef __cplusplus
extern "C" {
#endif

/** Function to register an interrupt routine to an ADC input pin with a defined intervall
 *  @param pin  					PinName of analog input.
 *  @param ADC_IRQHandler Function pointer to callback on interrupt.
 *  @param time 					Time for capture.
 */
int register_ADC_interrupt(PinName pin, uint32_t ADC_IRQHandler, uint32_t time);


/** Function to reset an interrupt
 *  @param none
 */
	
int reset_ADC_interrupt(void);

#ifdef __cplusplus
}
#endif

#endif
