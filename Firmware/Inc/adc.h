/*
 * adc.h
 *
 *  Created on: Jan 9, 2022
 *      Author: Sx107
 */

#ifndef ADC_H_
#define ADC_H_

#include <main.h>

#define ADC_DATA_SIZE 9
volatile uint16_t adcData[ADC_DATA_SIZE];

void adc_configure();
void ADC1_COMP_IRQHandler(void);

#define ADC_FINE_PITCH	4 // Aka PA6
#define ADC_COAR_PITCH	2 // Aka PA8
#define ADC_UNI			3 // Aka PA7

#define ADC_SCALE		8 // Aka PA5
#define ADC_SPREAD		0 // Aka PA4
#define ADC_WF			7 // Aka PA3

#define ADC_LFO_RATE	5 // Aka PA1
#define ADC_LFO_WF		1 // Aka PA2
#define ADC_LFO_TGT		6 // Aka PA0

#endif /* ADC_H_ */
