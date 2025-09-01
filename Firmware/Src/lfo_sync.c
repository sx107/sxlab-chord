/*
 * lfo_sync.c
 *
 *  Created on: 27 сент. 2023 г.
 *      Author: Sx107
 */


#include <lfo.h>

volatile uint8_t _lfo_sync_running = 0;
volatile uint16_t _lfo_sync_qticks = 0;
volatile uint16_t _lfo_sync_mticks = 0;
volatile uint32_t _lfo_sync_subticks = 0;
volatile uint16_t _lfo_sync_measspd = 0;
volatile uint16_t _lfo_sync_last_spd = 0;

void lfo_sync_configure() {

	RCC->APB1ENR |= RCC_APB1ENR_TIM14EN;
	RCC->APB2ENR |= RCC_APB2ENR_TIM16EN;

	// Tim16
	TIM16->PSC = 127;
	TIM16->ARR = 65535;
	//TIM16->DIER |= TIM_DIER_UIE;
	TIM16->CR1 |= TIM_CR1_ARPE | TIM_CR1_OPM;
	__DSB();
	TIM16->CR1 |= TIM_CR1_CEN;

	// Tim14
	NVIC_SetPriority(TIM14_IRQn, 1);
	NVIC_EnableIRQ(TIM14_IRQn);

	TIM14->PSC = 15;
	TIM14->ARR = 100;
	TIM14->DIER |= TIM_DIER_UIE;
	TIM14->CR1 |= TIM_CR1_ARPE;
	__DSB();
	//TIM14->CR1 |= TIM_CR1_CEN;
}

void lfo_sync_start() {
	_lfo_sync_running = 1;
	_lfo_sync_mticks = 0;
	_lfo_sync_qticks = 0;
	_lfo_sync_subticks = 0;
	_lfo_sync_measspd = _lfo_sync_last_spd;
	TIM14->ARR = _lfo_sync_measspd;
	TIM14->EGR |= TIM_EGR_UG;
	//TIM14->CR1 |= TIM_CR1_CEN;
}
void lfo_sync_stop() {
	_lfo_sync_running = 0;
	TIM16->CR1 &= ~TIM_CR1_CEN;
	TIM14->CR1 &= ~TIM_CR1_CEN;
	TIM16->CNT = 0;
	TIM16->EGR |= TIM_EGR_UG;
}
void lfo_sync_continue() {
	_lfo_sync_running = 1;

}
void lfo_sync_tick() {
		volatile uint16_t spd = TIM16->CNT;
		__DSB();
		TIM16->EGR |= TIM_EGR_UG;
		TIM16->CNT = 0;
		TIM16->CR1 |= TIM_CR1_CEN;
		if(_lfo_sync_running && spd != 0 && spd != 65535) {
			TIM14->ARR = _lfo_sync_measspd;
			_lfo_sync_measspd = spd;
			_lfo_sync_last_spd = spd;
			TIM14->CR1 |= TIM_CR1_CEN;
		}

		_lfo_sync_qticks += 1;
		if(_lfo_sync_qticks >= 24) {_lfo_sync_qticks = 0; _lfo_sync_mticks += 1; }
		if(_lfo_sync_mticks >= 16) {_lfo_sync_mticks = 0;}
		_lfo_sync_subticks = _lfo_sync_qticks * 5461 + (_lfo_sync_mticks << 17);
		TIM14->EGR |= TIM_EGR_UG;

}

uint16_t lfo_sync_value(uint8_t lfo_spd) {
	return (_lfo_sync_subticks >> lfo_spd) & 0x1FFF;
	// 64th note = 0
	// 32nd note = 1
	// 16th note =   2
	// 8th note = 3
	// 1/4 note = 16 - 12 = 4
	// 1/2 note = 5
	// 1 measure = 6
	// 2 measures = 7
	// 4 measures = 8
}

void lfo_sync_process() {

}

void TIM14_IRQHandler() {
	_lfo_sync_subticks += 682;
	TIM14->SR &= ~TIM_SR_UIF;
}
