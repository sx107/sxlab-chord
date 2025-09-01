/*
 * lfo.h
 *
 *  Created on: Jan 11, 2022
 *      Author: Sx107
 */

#ifndef LFO_H_
#define LFO_H_

#include <main.h>

void lfo_update();
void lfo_process();
void lfo_panic();

void lfo_sync_configure();
void lfo_sync_start();
void lfo_sync_stop();
void lfo_sync_tick();
void lfo_sync_continue();

uint16_t lfo_sync_value(uint8_t lfo);



extern volatile uint8_t _lfo_target;
extern volatile int16_t lfoValue[6];

extern volatile uint16_t _lfo_rate[6];
extern volatile uint16_t _lfo_wf[6];

#define LFO_TGT_COARSE 0
#define LFO_TGT_FINE 1
#define LFO_TGT_UNI 2
#define LFO_TGT_WF 3
#define LFO_TGT_SCALE 4
#define LFO_TGT_SPREAD 5



#endif /* LFO_H_ */
