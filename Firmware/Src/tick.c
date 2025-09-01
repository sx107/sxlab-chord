/*
 * tick.c
 *
 *  Created on: Sep 11, 2023
 *      Author: Sx107
 */

#include <main.h>
#include <tick.h>

#include <clock.h>
#include <pwm.h>
#include <butled.h>
#include <lfo.h>
#include <synth.h>
#include <midi.h>
#include <eeprom.h>

void tick_configure() {
	NVIC_SetPriority(TIM17_IRQn, 3);
	NVIC_EnableIRQ(TIM17_IRQn);

	RCC->APB2ENR |= RCC_APB2ENR_TIM17EN;
	TIM17->PSC = 0;
	TIM17->ARR = 48000;
	TIM17->DIER |= TIM_DIER_UIE;
	TIM17->CR1 |= TIM_CR1_ARPE;
	__DSB();
	TIM17->CR1 |= TIM_CR1_CEN;
}

volatile uint32_t pwmled_timer;
volatile uint32_t butled_timer = 0;
volatile uint8_t butled_animation = 0;

volatile uint8_t command_issued = 0;
volatile uint8_t command_ignore = 0;

volatile uint8_t last_preset = 0xFF;

#define BUTLED_ANIM_DRONE	(0b1 << 0)
#define BUTLED_ANIM_SAVE	(0b1 << 1)
#define BUTLED_ANIM_M1		(0b1 << 2)
#define BUTLED_ANIM_M2		(0b1 << 3)
#define BUTLED_ANIM_M3		(0b1 << 4)

volatile uint8_t prev_midi_in = 0;

volatile uint16_t midi_chan_blink = 0;

void tick_but_command() {
	// Check if all are released.
	uint8_t all_released = 1;
	for(uint8_t i = 0; i < 5; i++) {
		if(but_check(i)) {all_released = 0;}
	}

	// When a command is issued, all buttons are ignored until all buttons are released at the same time!
	if(command_issued) {
		if(all_released) {command_issued = 0;}
		return;
	}

	// Check M1-M3 commands
	if(	(but_released(BUTLED_M1) && !but_released(BUTLED_M2) && !but_released(BUTLED_M3) && !but_check(BUTLED_M2) && !but_check(BUTLED_M3)) ||
		(but_released(BUTLED_M2) && !but_released(BUTLED_M1) && !but_released(BUTLED_M3) && !but_check(BUTLED_M1) && !but_check(BUTLED_M3)) ||
		(but_released(BUTLED_M3) && !but_released(BUTLED_M2) && !but_released(BUTLED_M1) && !but_check(BUTLED_M2) && !but_check(BUTLED_M1)))
	{
		command_issued = 1;
		if(but_check(BUTLED_DRONE) && but_check(BUTLED_SAVE)) {
			// Settings
			if(but_released(BUTLED_M1)) {
				// Midi channel select
				if      (_midi_own_channel == 0xFF) {_midi_own_channel = 0;}
				else if (_midi_own_channel == 0) {_midi_own_channel = 1;}
				else if (_midi_own_channel == 1) {_midi_own_channel = 0xFF;}
				else {_midi_own_channel = 0xFF;}
				eeprom_write8(EEPROM_MIDI_CHAN, _midi_own_channel);
				_delayMs(30);
			} else if (but_released(BUTLED_M2)) {
				// Midi sync on/off
				if(!command_ignore) {
					_synth_midisync = !_synth_midisync;
				}
			} else if (but_released(BUTLED_M3)) {
				// Panic!
				if(!command_ignore) {
					lfo_panic();
					midi_panic();
					butled_animation = 0b11111;
					butled_timer = 500;
				}
			}
		} else if (but_check(BUTLED_SAVE)) {
			if(but_released(BUTLED_M1)) {
				if(!command_ignore) {
					synth_save_preset(0);
					butled_animation = BUTLED_ANIM_SAVE | BUTLED_ANIM_M1;
					butled_timer = 500;
				}
				// Save preset 1
			} else if (but_released(BUTLED_M2)) {
				// Save preset 2
				if(!command_ignore) {
					synth_save_preset(1);
					butled_animation = BUTLED_ANIM_SAVE | BUTLED_ANIM_M2;
					butled_timer = 500;
				}
			} else if (but_released(BUTLED_M3)) {
				// Save preset 3
				if(!command_ignore) {
					synth_save_preset(2);
					butled_animation = BUTLED_ANIM_SAVE | BUTLED_ANIM_M3;
					butled_timer = 500;
				}
			}
		} else if (but_check(BUTLED_DRONE)) {
			// Select sound? Idk
		} else {
			if(but_released(BUTLED_M1)) {
				// Load preset 1
				if(!command_ignore) {
					synth_load_preset(0);
					last_preset = 0;
				}

			} else if (but_released(BUTLED_M2)) {
				// Load preset 2
				if(!command_ignore) {
					synth_load_preset(1);
					last_preset = 1;
				}
			} else if (but_released(BUTLED_M3)) {
				// Load preset 3
				if(!command_ignore) {
					synth_load_preset(2);
					last_preset = 2;
				}
			}
		}
		return;
	}

	// Save command
	if(but_released(BUTLED_SAVE) && !but_check(BUTLED_DRONE) && !but_check(BUTLED_M1) && !but_check(BUTLED_M2) && !but_check(BUTLED_M3) && !but_released(BUTLED_DRONE)) {
		command_issued = 1;
		if(all_released) {
			// Quick-save
			if(!command_ignore) {
				synth_save_preset(3);
				eeprom_write8(EEPROM_HAS_PRESET3, 1);
				_delayMs(30);
				butled_animation = BUTLED_ANIM_SAVE | BUTLED_ANIM_M1 | BUTLED_ANIM_M2 | BUTLED_ANIM_M3;
				butled_timer = 500;
			}
		}
		return;
	}

	// Drone command issued
	if(but_released(BUTLED_DRONE) && !but_check(BUTLED_DRONE) && !but_check(BUTLED_M1) && !but_check(BUTLED_M2) && !but_check(BUTLED_M3) && !but_released(BUTLED_SAVE)) {
		command_issued = 1;
		if (all_released) {
			// Drone on-off
			if(!command_ignore) {
				_synth_drone = !_synth_drone;
			}
		}
		return;
	}

	// If any of the buttons released at this point, no known command is issued.
	for(uint8_t i = 0; i < 5; i++) {
		if(but_released(i)) {command_issued = 1;}
	}
}


void TIM17_IRQHandler() {
	but_update();

	if(!midi_is_connected() && prev_midi_in) {
		midi_reset();
	} else if (midi_is_connected() && !prev_midi_in) {
		midi_reset();
	}
	prev_midi_in = midi_is_connected();


	if(pwmled_timer > 0) {
		for(uint8_t i = 0; i < 6; i++) {*PWM_LEDS[i] = 0;}
		*PWM_LEDS[_lfo_target] = ((pwmled_timer >> 6) & 0b1) == 0 ? 1023 : 0;
		pwmled_timer--;
	} else {
		for(uint8_t i = 0; i < 6; i++) {
			*PWM_LEDS[i] = (2048 + lfoValue[i]) >> 2;
		}
	}

	if(butled_timer > 0) {
		uint8_t flash = ((butled_timer >> 6) & 0b1) == 0 ? 1 : 0;
		for(uint8_t i = 0; i < 5; i++) {
			if (flash && (butled_animation & (0b1 << i))) {butled_on(i);}
			else {butled_off(i);}
		}
		command_ignore = 1;
		butled_timer--;
	} else {
		command_ignore = 0;
	}

	tick_but_command();


	if(butled_timer == 0) {
		if(but_check(BUTLED_DRONE) && but_check(BUTLED_SAVE)) {
			butled_on(BUTLED_DRONE);
			butled_on(BUTLED_SAVE);
			if(midi_is_connected()) {butled_on(BUTLED_M3);}
			else {butled_off(BUTLED_M3);}

			midi_chan_blink += 1;

			if(_midi_own_channel == 0xFF) {
				if((midi_chan_blink >> 6) & 0b1) {butled_on(BUTLED_M1);}
				else {butled_off(BUTLED_M1);}
			} else if (_midi_own_channel == 0) {butled_off(BUTLED_M1);}
			else {butled_on(BUTLED_M1);}

			if(_synth_midisync) {butled_on(BUTLED_M2);}
			else {butled_off(BUTLED_M2);}
		} else {
			butled_off(BUTLED_SAVE);
			if (_synth_drone) {butled_on(BUTLED_DRONE);}
			else {butled_off(BUTLED_DRONE);}

			if(last_preset != 0xFF) {
				if(last_preset == 0) {butled_on(BUTLED_M1); butled_off(BUTLED_M2); butled_off(BUTLED_M3);}
				else if (last_preset == 1) {butled_off(BUTLED_M1); butled_on(BUTLED_M2); butled_off(BUTLED_M3);}
				else if (last_preset == 2) {butled_off(BUTLED_M1); butled_off(BUTLED_M2); butled_on(BUTLED_M3);}
			} else {
				butled_off(BUTLED_M1);
				butled_off(BUTLED_M2);
				butled_off(BUTLED_M3);
			}
		}
	}

	synth_update();
	lfo_update();


	TIM17->SR &= ~TIM_SR_UIF;
}
