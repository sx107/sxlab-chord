/*
 * synth.h
 *
 *  Created on: Jan 12, 2022
 *      Author: Sx107
 */

#ifndef SYNTH_H_
#define SYNTH_H_

#include <main.h>

extern volatile uint8_t _synth_drone;
extern volatile uint8_t _synth_midisync;
extern volatile uint8_t _synth_knob_active[9];

void synth_process();
void synth_update();

void synth_load_preset(uint8_t preset);
void synth_save_preset(uint8_t preset);

void synth_set_pb(uint16_t pb);
void synth_set_mod(uint8_t mod);
void synth_set_cc(uint8_t id, uint8_t cc);
void synth_reset_midi();

#define SYNTH_CC_COARSE 0
#define SYNTH_CC_FINE 1
#define SYNTH_CC_UNISON 2
#define SYNTH_CC_SCALE 3
#define SYNTH_CC_SPREAD 4


#endif /* SYNTH_H_ */
