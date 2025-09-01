/*
 * adc.c
 *
 *  Created on: Jan 9, 2022
 *      Author: Sx107
 */

#include <adc.h>
#include <main.h>
#include <stm32f0xx.h>

void adc_configure() {
	for(uint8_t i = 0; i < ADC_DATA_SIZE; i++) {
		adcData[i] = 0xFFFF;
	}
	// Pins
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
	GPIOA->MODER |= (3 << GPIO_MODER_MODER0_Pos) | (3 << GPIO_MODER_MODER1_Pos);

	// Enable DMA & ADC clock
	RCC->AHBENR |= RCC_AHBENR_DMA1EN;
	RCC->APB2ENR |= RCC_APB2ENR_ADCEN;

	DMA1_Channel1->CPAR = (uint32_t) (&(ADC1->DR));
	DMA1_Channel1->CMAR = (uint32_t)(adcData);
	DMA1_Channel1->CNDTR = ADC_DATA_SIZE;
	DMA1_Channel1->CCR |= DMA_CCR_MINC |
			(1 << DMA_CCR_MSIZE_Pos) |
			(1 << DMA_CCR_PSIZE_Pos) |
			DMA_CCR_CIRC |
			(3 << DMA_CCR_PL_Pos);
	__DMB();

	ADC1->CHSELR |= 	ADC_CHSELR_CHSEL0 | ADC_CHSELR_CHSEL1 | ADC_CHSELR_CHSEL2 | ADC_CHSELR_CHSEL3
			 	 	 | ADC_CHSELR_CHSEL4 | ADC_CHSELR_CHSEL5 | ADC_CHSELR_CHSEL6 | ADC_CHSELR_CHSEL7
					 | ADC_CHSELR_CHSEL8;
	//Async clock, 12 bits, right-align, continuous mode
	ADC1->CFGR1 |= ADC_CFGR1_CONT | ADC_CFGR1_DMACFG;
	ADC1->CFGR1 |= 0 << ADC_CFGR1_RES_Pos; // 12 bits.
	ADC1->SMPR |= 7;

	// Make sure the ADC is disabled.
	if((ADC1->CR & ADC_CR_ADEN) != 0) {
		ADC1->CR |= ADC_CR_ADDIS;
	}
	WAITNBIT(ADC1->CR, ADC_CR_ADEN);
	ADC1->CFGR1 &= ~ADC_CFGR1_DMAEN;

	// Calibrate.
	ADC1->CR |= ADC_CR_ADCAL;
	WAITNBIT(ADC1->CR, ADC_CR_ADCAL);

	// Clear ADC RDY
	if((ADC1->ISR & ADC_ISR_ADRDY) != 0) {
		ADC1->ISR |= ADC_ISR_ADRDY;
	}

	NVIC_EnableIRQ(ADC1_COMP_IRQn);
	NVIC_SetPriority(ADC1_COMP_IRQn,0);
	ADC1->IER |= ADC_IER_OVRIE;

	__DMB();

	//Enable DMA and ADC.
	ADC1->CFGR1 |= ADC_CFGR1_DMAEN;
	DMA1_Channel1->CCR |= DMA_CCR_EN;
	ADC1->CR |= ADC_CR_ADEN;
	WAITBIT(ADC1->ISR, ADC_ISR_ADRDY);
	ADC1->CR |= ADC_CR_ADSTART;

	// Wait while the whole buffer is filled
	while(1) {
		uint8_t allGood = 1;
		for(uint8_t i = 0; i < ADC_DATA_SIZE; i++) {
			if (adcData[i] == 0xFFFF) {allGood = 0;}
		}
		if(allGood == 1) {break;}
	}
}

void ADC1_2_IRQHandler(void) {
	ADC1->ISR |= ADC_ISR_OVR;
	ADC1->CR |= ADC_CR_ADSTP;
	WAITNBIT(ADC1->CR, ADC_CR_ADSTART);
	DMA1_Channel1->CCR &= ~DMA_CCR_EN;
	DMA1_Channel1->CNDTR = ADC_DATA_SIZE;
	DMA1_Channel1->CCR |= DMA_CCR_EN;
	ADC1->CR |= ADC_CR_ADSTART;
}
