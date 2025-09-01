/*
 * synth.c
 *
 *  Created on: Jan 12, 2022
 *      Author: Sx107
 */

#include <main.h>
#include <clock.h>
#include <pwm.h>
#include <synth.h>
#include <adc.h>
#include <lfo.h>
#include <synth_lut.h>
#include <stm32f0xx.h>
#include <eeprom.h>
#include <midi.h>

#define NOSC 6

#define SAWMODE 1

#define OSC_SPREAD_DIV 683
#define OSC_SPREAD_HDIV 341
#define OSC_SPREAD_HYST 200

#define OSC_SCALE_DIV 512
#define OSC_SCALE_HDIV 256
#define OSC_SCALE_HYST 100

#define SYNTH_POT_HYST 150

#define UNISON_NULL 100

#define UNI_PHASE_TRANSITION 256

volatile uint32_t _synth_phinc[NOSC] = {615,617,619,621,623,625};
volatile uint16_t _synth_knob_hlast[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
volatile uint16_t _synth_knob[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
volatile uint8_t _synth_knob_active[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};

volatile uint32_t _synth_phase[NOSC];

volatile uint16_t _synth_spread_raw;
volatile uint16_t _synth_scale_raw;

volatile uint8_t _synth_spread_prev = 0xFF;
volatile uint8_t _synth_scale_prev = 0xFF;

volatile uint8_t _synth_drone = 1;
volatile uint8_t _synth_midisync = 0;

volatile uint8_t first_time = 1;

volatile uint16_t _synth_pb = 8192;
volatile uint8_t _synth_mod = 0;
volatile uint8_t _synth_last_midi = 0xFF;

volatile uint8_t _synth_cc[5];

void synth_load_preset(uint8_t preset) {
	for(uint8_t i = 0; i < 9; i++) {
		_synth_knob_active[i] = 0;
		_synth_knob_hlast[i] = adcData[i];
	}
	uint8_t data[48];
	eeprom_read_buffer(preset * 48, data, 16);
	eeprom_read_buffer(preset * 48+16, data+16, 16);
	eeprom_read_buffer(preset * 48+32, data+32, 16);

	_synth_knob[ADC_FINE_PITCH] = 	*(uint16_t*)(&data[0]);
	_synth_knob[ADC_COAR_PITCH] = 	*(uint16_t*)(&data[2]);
	_synth_knob[ADC_UNI] 		= 	*(uint16_t*)(&data[4]);
	_synth_knob[ADC_WF] 		= 	*(uint16_t*)(&data[6]);
	_synth_knob[ADC_SCALE] 		= 	*(uint16_t*)(&data[8]);
	_synth_knob[ADC_SPREAD] 	= 	*(uint16_t*)(&data[10]);

	lfo_panic();
	for(uint8_t i = 0; i < 6; i++) {
		_lfo_wf[i] = *(uint16_t*)(&data[12+i*4]);
		_lfo_rate[i] = *(uint16_t*)(&data[12+i*4+2]);
	}

	// Other data starts at 6*2 + 6*4 = 36
	_synth_drone = data[36];
	_synth_midisync = data[37];



	uint16_t spread_r = _synth_knob[ADC_SPREAD];
	uint16_t spread_v = spread_r / OSC_SPREAD_DIV;
	_synth_spread_raw = spread_v * OSC_SPREAD_DIV + OSC_SPREAD_HDIV;
	_synth_spread_prev = spread_v;

	uint16_t scale_r = _synth_knob[ADC_SCALE];
	uint16_t scale_v = scale_r / OSC_SCALE_DIV;
	_synth_scale_raw = scale_v * OSC_SCALE_DIV + OSC_SCALE_HDIV;
	_synth_scale_prev = scale_v;
}

void synth_save_preset(uint8_t preset) {
	uint8_t data[48];

	*(uint16_t*)(&data[0]) = _synth_knob[ADC_FINE_PITCH];
	*(uint16_t*)(&data[2]) = _synth_knob[ADC_COAR_PITCH];
	*(uint16_t*)(&data[4]) = _synth_knob[ADC_UNI];
	*(uint16_t*)(&data[6]) = _synth_knob[ADC_WF];
	*(uint16_t*)(&data[8]) = _synth_knob[ADC_SCALE];
	*(uint16_t*)(&data[10]) = _synth_knob[ADC_SPREAD];

	for(uint8_t i = 0; i < 6; i++) {
		*(uint16_t*)(&data[12+i*4]) = _lfo_wf[i];
		*(uint16_t*)(&data[12+i*4+2]) = _lfo_rate[i];
	}

	data[36] = _synth_drone;
	data[37] = _synth_midisync;

	eeprom_write_buffer(preset * 48, data, 16);
	_delayMs(30); // TODO: Fix the eeprom_wait function!
	eeprom_write_buffer(preset * 48+16, data + 16, 16);
	_delayMs(30);
	eeprom_write_buffer(preset * 48+32, data + 32, 16);
	_delayMs(30);
}

void synth_set_pb(uint16_t pb) {
	_synth_pb = pb;
}
void synth_set_mod(uint8_t mod) {
	_synth_mod = mod;
}

void synth_set_cc(uint8_t id, uint8_t cc) {
	_synth_cc[id] = cc;
}

void synth_reset_midi() {
	_synth_pb = 8192;
	_synth_mod = 0;
	_synth_last_midi = 0xFF;
	for(uint8_t i = 0; i < 5; i++) {
		_synth_cc[i] = 0;
	}
}

void synth_update() {
	for(uint8_t i = 0; i < 9; i++) {
		int16_t h = adcData[i] - _synth_knob_hlast[i];
		if (h > SYNTH_POT_HYST || h < -SYNTH_POT_HYST) {_synth_knob_active[i] = 1;}
		if(_synth_knob_active[i]) {_synth_knob[i] = adcData[i];}
	}

	uint16_t spread_r = _synth_knob[ADC_SPREAD];
	uint16_t spread_v = spread_r / OSC_SPREAD_DIV;
	uint16_t spread_h = spread_r % OSC_SPREAD_DIV;

	if(((spread_h > OSC_SPREAD_HYST) && (spread_h < OSC_SPREAD_DIV - OSC_SPREAD_HYST) && _synth_spread_prev != spread_v ) || (first_time == 1)) {
		_synth_spread_raw = spread_v * OSC_SPREAD_DIV + OSC_SPREAD_HDIV;
		_synth_spread_prev = spread_v;
	}

	uint16_t scale_r = _synth_knob[ADC_SCALE];
	uint16_t scale_v = scale_r / OSC_SCALE_DIV;
	uint16_t scale_h = scale_r % OSC_SCALE_DIV;

	if(((scale_h > OSC_SCALE_HYST) && (scale_h < OSC_SCALE_DIV - OSC_SCALE_HYST) && _synth_scale_prev != scale_v) || (first_time == 1)) {
		_synth_scale_raw = scale_v * OSC_SCALE_DIV + OSC_SCALE_HDIV;
		_synth_scale_prev = scale_v;
	}



	first_time = 0;
}

void synth_process() {


	uint32_t so = 0;

	//Waveform
	int16_t raw_wf = _synth_knob[ADC_WF] + lfoValue[LFO_TGT_WF] + (_synth_mod << 5);
	if (raw_wf < 0) {raw_wf = 0;}
	else if (raw_wf > 4095) {raw_wf = 4095;}
	uint16_t wf = 1023;
	wf = 1023-(raw_wf >> 2);
	wf = wf_lut[wf];


	//Coarse pitch
	/*
	int16_t coarsePitch = _synth_knob[ADC_COAR_PITCH] + lfoValue[LFO_TGT_COARSE];
	if(coarsePitch < 0) {coarsePitch = 0;}
	if(coarsePitch > 4095) {coarsePitch = 4095;}
	uint8_t coarseIndex = coarsePitch >> 7;
	*/
	if(midi_get_monophonic() != 0xFF) {_synth_last_midi = midi_get_monophonic();}
	uint32_t basePitch = midi_lut[24];
	if(_synth_last_midi != 0xFF) {basePitch = midi_lut[_synth_last_midi-33];}

	//uint32_t basePitch = coarse_lut[coarseIndex];

	//Fine pitch
	int32_t finePitch = _synth_knob[ADC_FINE_PITCH] + lfoValue[LFO_TGT_FINE] + (_synth_cc[SYNTH_CC_FINE] << 5);
	if(finePitch < 0) {finePitch = 0;}
	else if(finePitch > 4095) {finePitch = 4095;}
	uint16_t lut_fine_low = fine_lut[finePitch >> 4];
	uint16_t lut_fine_high = fine_lut[(finePitch >> 4) + 1];
	uint16_t fp_ready = lut_fine_low + ((lut_fine_low - lut_fine_high) * (finePitch & 0b1111) >> 4);
	basePitch = basePitch * fp_ready >> 14;

	//Coarse
	int32_t coarsePitch = (_synth_knob[ADC_COAR_PITCH]) + lfoValue[LFO_TGT_COARSE] + (_synth_cc[SYNTH_CC_COARSE] << 5);
	if(coarsePitch < 0) {coarsePitch = 0;}
	else if(coarsePitch > 4095) {coarsePitch = 4095;}
	uint8_t cp_ready = coarsePitch / 164;
	if(cp_ready > 24) {cp_ready = 24;}
	basePitch = basePitch * coarse_lut[cp_ready] >> 14;


	//Pitchbend
	finePitch = _synth_pb >> 2;
	if(finePitch < 0) {finePitch = 0;}
	else if(finePitch > 4095) {finePitch = 4095;}
	lut_fine_low = pitchbend_lut[finePitch >> 4];
	lut_fine_high = pitchbend_lut[(finePitch >> 4) + 1];
	fp_ready = lut_fine_low + ((lut_fine_low - lut_fine_high) * (finePitch & 0b1111) >> 4);
	basePitch = basePitch * fp_ready >> 14;

	//Unison
	int16_t unison = _synth_knob[ADC_UNI] + lfoValue[LFO_TGT_UNI] + (_synth_cc[SYNTH_CC_UNISON] << 5);
	if(unison < 0) {unison = 0;}
	else if (unison > 4095) {unison = 4095;}
	uint16_t uni_spread = 0;
	if(unison > (2048+UNISON_NULL)) {
		uni_spread = (unison - (2048+UNISON_NULL)) >> 2;
		uni_spread = uni_lut[uni_spread];
	}

	//Spread
	int16_t spread_v = _synth_spread_raw + lfoValue[LFO_TGT_SPREAD] + (_synth_cc[SYNTH_CC_SPREAD] << 5);
	if(spread_v < 0) {spread_v = 0;}
	else if (spread_v > 4095) {spread_v = 4095;}
	spread_v /= OSC_SPREAD_DIV;

	//Scale
	int16_t scale_v = _synth_scale_raw + lfoValue[LFO_TGT_SCALE] + (_synth_cc[SYNTH_CC_SCALE] << 5);
	if(scale_v < 0) {scale_v = 0;}
	else if (scale_v > 4095) {scale_v = 4095;}
	scale_v /= OSC_SCALE_DIV;


	for(uint8_t i = 0; i < NOSC; i++) {
		int16_t uni_index = i - 2;
		uint32_t cur_phinc = basePitch;
		//cur_phinc = cur_phinc * (1 + spread_lut[spread_v][i]);

		if(/*i != 0 && unison < 2*/ 0) {
			_synth_phase[i] = _synth_phase[0];
		} else {
			cur_phinc = cur_phinc * scales_lut[scale_v][spread_lut[spread_v][i]] >> 13;
			if(unison >= 2048) {
				cur_phinc = cur_phinc * (1024 + uni_spread * uni_index) >> 10;
			} else {
				uint32_t bp = basePitch * scales_lut[scale_v][0] >> 13;
				//bp += basePitch * scales_lut[scale_v][spread_lut[spread_v][5]] >> 13;
				//bp/=2;
				cur_phinc = bp + ((cur_phinc - bp)*(unison) >> 11);
			}
			_synth_phase[i] += cur_phinc;
		}


		uint32_t v = (_synth_phase[i] >> 6) & 2047;

		#ifdef SAWMODE
			if(v < (wf)) {v = v * 1023 / wf;}
			else {v = 1023 - (v-wf)*1023/(2047-wf);}
		#else
			if(v < 1023) {v = (v * wf >> 10);}
			else {v = 1023 - ((v -1024) * wf >> 10);}
		#endif


		//if(i == 0) {so = v;}


		if (unison > UNI_PHASE_TRANSITION) {
			so += v;
		} else {
			if(i == 0) {
				so += (v * (UNI_PHASE_TRANSITION + (UNI_PHASE_TRANSITION - unison))) >> 8;
			} else {
				so += (v * unison) >> 8;
			}
		}
	}

	if(_synth_drone || midi_get_monophonic() != 0xFF) {
		PWM_AUDIO_OUT = so / 6;
	} else {
		//PWM_AUDIO_OUT = 512;
	}
}


