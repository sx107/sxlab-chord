/*
 * lfo.c
 *
 *  Created on: Jan 11, 2022
 *      Author: Sx107
 */


#include <main.h>
#include <lfo.h>
#include <adc.h>
#include <pwm.h>
#include <tick.h>
#include <synth.h>
#include <midi.h>

#define LFO_TGT_DIV 683
#define LFO_TGT_HYST 200
#define LFO_POT_HYST 500
#define LFO_POT_NULL 48

volatile int16_t lfoValue[6];

volatile uint8_t _lfo_cur_div = 0;

uint16_t _lfo_prevRate;
uint16_t _lfo_prevWf;

uint8_t _lfo_rate_active = 0;
uint8_t _lfo_wf_active = 0;

uint8_t _lfo_first_time = 1;
volatile uint8_t _lfo_target = 0;

volatile uint16_t _lfo_rate[6] = {2047, 2047, 2047, 2047, 2047, 2047};
volatile uint16_t _lfo_wf[6] = {2047, 2047, 2047, 2047, 2047, 2047};
volatile uint32_t _lfo_ph_inc[6];
volatile uint32_t _lfo_phase[6];
volatile uint16_t _lfo_random[6] = {0xFACE, 0xFACE, 0xFACE, 0xFACE, 0xFACE, 0xFACE};
volatile uint8_t  _lfo_prevhp[6] = {0, 0, 0, 0, 0, 0};
volatile uint8_t  _lfo_current = 0;

/*
const uint16_t _lfo_freq_lut[] = {4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 6, 6, 6, 6, 6, 7, 7, 7, 7, 8, 8,
		8, 8, 9, 9, 9, 10, 10, 11, 11, 11, 12, 12, 13, 13, 14, 14, 15, 15,
		16, 16, 17, 17, 18, 19, 19, 20, 21, 22, 22, 23, 24, 25, 26, 27, 28,
		29, 30, 31, 32, 33, 35, 36, 37, 38, 40, 41, 43, 44, 46, 48, 49, 51,
		53, 55, 57, 59, 61, 64, 66, 68, 71, 73, 76, 79, 82, 85, 88, 91, 95,
		98, 102, 105, 109, 113, 117, 122, 126, 131, 135, 140, 146, 151, 156,
		162, 168, 174, 181, 187, 194, 201, 209, 216, 224, 232, 241, 250, 259,
		268, 278, 288, 299, 310, 321, 333, 345, 358, 371, 385, 399, 413, 428,
		444, 460, 477, 495, 513, 532, 551, 571, 592, 614, 636, 660, 684, 709,
		735, 762, 790, 819, 849, 880, 912, 945, 980, 1016, 1053, 1092, 1132,
		1173, 1216, 1261, 1307, 1355, 1405, 1456, 1509, 1565, 1622, 1681,
		1743, 1807, 1873, 1942, 2013, 2086, 2163, 2242, 2324, 2409, 2498,
		2589, 2684, 2782, 2884, 2990, 3099, 3213, 3331, 3453, 3579, 3710,
		3846, 3987, 4133, 4285, 4441, 4604, 4773, 4948, 5129, 5317, 5512,
		5714, 5923, 6140, 6365, 6598, 6840, 7090, 7350, 7619, 7898, 8188,
		8487, 8798, 9121, 9455, 9801, 10160, 10532, 10918, 11318, 11733,
		12163, 12608, 13070, 13549, 14045, 14560, 15093, 15646, 16219, 16813,
		17429, 18068, 18730, 19416, 20127, 20864, 21629, 22421, 23242, 24094,
		24976, 25891, 26840, 27823, 28842, 29899, 30994, 32129, 33306, 34527, 35791};
*/

const uint16_t _lfo_freq_lut[] = {4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 6, 6, 6, 6, 7, 7, 7, 7, 8, 8, 8,
8, 9, 9, 10, 10, 10, 11, 11, 11, 12, 12, 13, 13, 14, 14, 15, 15, 16,
17, 17, 18, 19, 19, 20, 21, 22, 23, 23, 24, 25, 26, 27, 28, 29, 30,
32, 33, 34, 35, 37, 38, 40, 41, 43, 44, 46, 48, 50, 52, 53, 56, 58,
60, 62, 65, 67, 70, 72, 75, 78, 81, 84, 87, 91, 94, 98, 101, 105,
109, 113, 118, 122, 127, 132, 137, 142, 147, 153, 159, 165, 171, 178,
185, 192, 199, 207, 215, 223, 231, 240, 250, 259, 269, 279, 290, 301,
313, 325, 337, 350, 363, 377, 392, 407, 422, 438, 455, 473, 491, 509,
529, 549, 570, 592, 615, 638, 663, 688, 714, 742, 770, 800, 830, 862,
895, 929, 965, 1002, 1040, 1080, 1121, 1164, 1209, 1255, 1303, 1353,
1405, 1458, 1514, 1572, 1632, 1695, 1760, 1827, 1897, 1969, 2045,
2123, 2204, 2289, 2376, 2467, 2562, 2660, 2762, 2867, 2977, 3091,
3209, 3332, 3460, 3592, 3730, 3872, 4021, 4174, 4334, 4500, 4672,
4851, 5037, 5230, 5430, 5638, 5853, 6077, 6310, 6552, 6802, 7063,
7333, 7614, 7905, 8208, 8522, 8848, 9187, 9538, 9904, 10283, 10676,
11085, 11509, 11950, 12407, 12882, 13375, 13887, 14418, 14970, 15543,
16138, 16756, 17397, 18063, 18755, 19473, 20218, 20992, 21795, 22629,
23496, 24395, 25329, 26298, 27305, 28350, 29435, 30562, 31731, 32946,
34207, 35516, 36876, 38287, 39753, 41274, 42854, 44494, 46197, 47966,
49802, 51708, 53687};

void lfo_update() {
	 uint32_t hystValue = adcData[ADC_LFO_TGT] % LFO_TGT_DIV;
	 uint32_t curTarget = adcData[ADC_LFO_TGT] / LFO_TGT_DIV;
	if (_lfo_first_time == 1 || ((curTarget != _lfo_target) && (hystValue > LFO_TGT_HYST) && (hystValue < LFO_TGT_DIV - LFO_TGT_HYST))) {

		if(!_lfo_first_time) {pwmled_timer = 2000;}

		_lfo_first_time = 0;
		_lfo_prevRate = adcData[ADC_LFO_RATE];
		_lfo_prevWf = adcData[ADC_LFO_WF];

		_lfo_rate_active = 0;
		_lfo_wf_active = 0;

		_lfo_target = curTarget;

	}

	int16_t h;
	h = adcData[ADC_LFO_RATE] - _lfo_prevRate;
	if (h > LFO_POT_HYST || h < -LFO_POT_HYST) {_lfo_rate_active = 1;}
	h = adcData[ADC_LFO_WF] - _lfo_prevWf;
	if (h > LFO_POT_HYST || h < -LFO_POT_HYST) {_lfo_wf_active = 1;}

	if (_lfo_rate_active == 1) {_lfo_rate[_lfo_target] = adcData[ADC_LFO_RATE];}
	if (_lfo_wf_active == 1) {_lfo_wf[_lfo_target] = adcData[ADC_LFO_WF];}

	for(uint8_t i = 0; i < 6; i++) {
		//Calculate LFO_PH_INC
		uint16_t val = _lfo_rate[i];
		uint16_t lut_low = _lfo_freq_lut[val >> 4];
		uint16_t lut_high = _lfo_freq_lut[(val >> 4) + 1];
		_lfo_ph_inc[i] = lut_low + ((lut_low - lut_high) * (val & 0b1111) >> 4);
	}
}

void lfo_process() {
	_lfo_current++;
	if(_lfo_current >= 6) {_lfo_current = 0;}
	uint8_t i = _lfo_current;
	//for(uint8_t i = 0; i < 6; i++) {
		_lfo_phase[i] += _lfo_ph_inc[i];
		// >> 16 >> 6 << 10
		// >> 16 >> 6 (lfo div) << 13 (lfo bits) << 3 (lfo_current) = >> 6
		int32_t lfoRawValue;

		if(midi_is_connected() && _synth_midisync) {lfoRawValue = lfo_sync_value(1+7-(_lfo_rate[i] >> (12 - 3)));}
		else {lfoRawValue = (_lfo_phase[i] >> 6) & 0x1FFF;}

		uint8_t lfoHalfPhase = (lfoRawValue >> 12) & 0b1;
		if (_lfo_wf[i] > 2048 - LFO_POT_NULL && _lfo_wf[i] < 2048 + LFO_POT_NULL) {
			lfoValue[i] = 0;
		}
		else if (_lfo_wf[i] < 2048) {
			//Triangle
			uint16_t lfoAmplitude = 2047 - _lfo_wf[i];
			lfoRawValue = lfoHalfPhase ? lfoRawValue & 0xFFF : (4095 - (lfoRawValue & 0xFFF));
			lfoValue[i] = (lfoRawValue-2048) * lfoAmplitude >> 11;
		} else if (_lfo_wf[i] < 3072) {
			// Square
			uint16_t lfoAmplitude = _lfo_wf[i] - 2048;
			lfoRawValue = lfoHalfPhase ? 0 : 4095;
			lfoValue[i] = (lfoRawValue-2048) * lfoAmplitude >> 10;
		} else {
			if (_lfo_prevhp[i] != lfoHalfPhase) {
				// Random
				/*
					for(uint8_t iter = 0; iter < 4; iter++) {
						uint16_t lsb = _lfo_random[i] & 0b1;
						_lfo_random[i] >>= 1;
						_lfo_random[i] ^= (-lsb) & 0xB400;
					}
				}
				uint16_t lfoSH = (_lfo_random[i] >> 4) & (0b1111 << 8);
				*/
				for(uint8_t iter = 0; iter < 8; iter++) {
					uint16_t lsb = _lfo_random[i] & 0b1;
					_lfo_random[i] >>= 1;
					_lfo_random[i] ^= (-lsb) & 0xB400;
				}
			}
			uint16_t lfoSH = (_lfo_random[i] >> 4) & (0xFF << 4);
			uint16_t lfoAmplitude = (_lfo_wf[i] - 3072);
			uint16_t lfoSquare = lfoHalfPhase == 1 ? 4095 : 0;
			//lfoValue[i] = (lfoSH-2048) * lfoAmplitude / 1023 + ;
			lfoValue[i] = ((lfoSquare-2048) * (1023-lfoAmplitude) >> 10) + ((lfoSH-2048) * lfoAmplitude >> 10);
		}

		_lfo_prevhp[i] = lfoHalfPhase;

	//}
}

void lfo_panic() {
	_lfo_prevRate = adcData[ADC_LFO_RATE];
	_lfo_prevWf = adcData[ADC_LFO_WF];
	_lfo_rate_active = 0;
	_lfo_wf_active = 0;
	for(uint8_t i = 0; i < 6; i++) {
		_lfo_rate[i] = 2047;
		_lfo_wf[i] = 2047;
	}
}
