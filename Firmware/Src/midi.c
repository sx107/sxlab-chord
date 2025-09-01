/*
 * midi.c
 *
 *  Created on: 26 сент. 2023 г.
 *      Author: Sx107
 */


#include <midi.h>
#include <synth.h>
#include <lfo.h>

volatile uint8_t _midi_status = 0x00;
volatile midi_mode_t _midi_mode = MIDI_MODE_RESET;
volatile uint8_t _midi_firstbyte = 0x00;
volatile uint8_t _midi_own_channel = MIDI_ALLCHAN;

#define MIDI_IS_STAT(X) ((X & 0x80) == 0x80)
#define MIDI_IS_DATA(X) ((X & 0x80) == 0)

void midi_configure() {
	midi_stack_clear();

	RCC->AHBENR |= RCC_AHBENR_GPIOBEN;
	RCC->APB2ENR |= RCC_APB2ENR_USART1EN;

	// First the MIDIDET
	MODIFY_REG(GPIOB->MODER, GPIO_MODER_MODER10_Msk, 0b00 << GPIO_MODER_MODER10_Pos);
	MODIFY_REG(GPIOB->PUPDR, GPIO_PUPDR_PUPDR10_Msk, 0b10 << GPIO_PUPDR_PUPDR10_Pos);


	// Configure UART
	MODIFY_REG(GPIOB->MODER, GPIO_MODER_MODER7_Msk, 0b10 << GPIO_MODER_MODER7_Pos);
	MODIFY_REG(GPIOB->AFR[0] , GPIO_AFRL_AFSEL7_Msk, 0b00 << GPIO_AFRL_AFSEL7_Pos);

	USART1->BRR = 1536; //48000000 / 31250
	MODIFY_REG(USART1->CR1, USART_CR1_M_Msk | USART_CR1_PCE_Msk | USART_CR1_TE_Msk, USART_CR1_RE);
	MODIFY_REG(USART1->CR2, USART_CR2_STOP_Msk, 0 << USART_CR2_STOP_Pos);

	NVIC_EnableIRQ(USART1_IRQn);
	NVIC_SetPriority(USART1_IRQn, 2);

	USART1->CR1 |= USART_CR1_UE | USART_CR1_RXNEIE;
}

uint8_t midi_is_connected() {
	return (GPIOB->IDR & GPIO_IDR_10) >> 10;
}

void USART1_IRQHandler() {
	uint8_t rb = USART1->RDR;

	if(USART1->ISR & USART_ISR_ORE) {
		_midi_status = 0;
		_midi_mode = MIDI_MODE_RESET;
	}
	USART1->ICR |= USART_ICR_ORECF;



	if(((USART1->ISR) & USART_ISR_ORE) != 0) {
		_midi_mode = MIDI_MODE_RESET;
		return;
	}

	// Ignore all the realtime bytes, they must not affect the parsing.
	if(rb == 0xF8 || rb == 0xFA || rb == 0xFB || rb == 0xFC || rb == 0xFE || rb == 0xFF) {
		if(rb == 0xF8) {lfo_sync_tick();}
		else if(rb == 0xFA) {lfo_sync_start();}
		else if(rb == 0xFB) {lfo_sync_continue();}
		else if(rb == 0xFC) {lfo_sync_stop();}
		return;
	}

	if(((_midi_mode == MIDI_MODE_RESET) || (_midi_mode == MIDI_MODE_SYSEX)) && MIDI_IS_DATA(rb)) {
		// Simply wait. We don't know the running status at this point anyways
		return;
	}

	if MIDI_IS_STAT(rb) {
		// Immedeately change the running status
		_midi_status = rb;
		uint8_t cmd = MIDI_CMD(rb);
		if(rb == 0xF0) {_midi_mode = MIDI_MODE_SYSEX;}
		else if(cmd == 0xF0) {_midi_mode = MIDI_MODE_RESET;} // Any other 0xFx message, including sysex end. Start ignoring everything except status bytes
		else if(cmd == 0xC0 || cmd == 0xD0) {_midi_mode = MIDI_MODE_SINGLE_WAIT;} // Channel and pressure, only non-0xFx messages with one byte
		else {_midi_mode = MIDI_MODE_1BYTE_WAIT;}
	} else {
		if (MIDI_CMD(_midi_status) == 0xF0) {return;} // Just to be sure.
		if(_midi_mode == MIDI_MODE_1BYTE_WAIT) {
			// Store the first byte, wait for second
			_midi_firstbyte = rb;
			_midi_mode = MIDI_MODE_2BYTE_WAIT;
		} else if (_midi_mode == MIDI_MODE_2BYTE_WAIT) {
			// Handle 2-byte midi message
			if ((MIDI_CMD(_midi_status) == 0x90) && (rb == 0)) { // Implicit note off
				if ((_midi_own_channel == MIDI_ALLCHAN) || (MIDI_CHAN(_midi_status) == _midi_own_channel)) {
					handle_midi_message(0x80 | MIDI_CHAN(_midi_status), _midi_firstbyte, 0);
				}
			} else {
				if ((_midi_own_channel == MIDI_ALLCHAN) || (MIDI_CHAN(_midi_status) == _midi_own_channel)) {
					handle_midi_message(_midi_status, _midi_firstbyte, rb);
				}
			}
			_midi_mode = MIDI_MODE_1BYTE_WAIT;
		} else if (_midi_mode == MIDI_MODE_SINGLE_WAIT) {
			if ((_midi_own_channel == MIDI_ALLCHAN) || (MIDI_CHAN(_midi_status) == _midi_own_channel)) {
				handle_midi_message(_midi_status, rb, 0);
			}
		}
	}
}

uint8_t _midi_stack_backwards[128];
uint8_t _midi_stack_forwards[128];
uint8_t _midi_last_note = 0xFF;

void _midi_push_note(uint8_t note) {
	if(_midi_last_note == note || _midi_stack_backwards[note] != 0xFF || _midi_stack_forwards[note] != 0xFF) {return;}
	if(_midi_last_note != 0xFF) {_midi_stack_forwards[_midi_last_note] = note; _midi_stack_backwards[note] = _midi_last_note;}
	else {_midi_stack_backwards[note] = 0xFF;}
	_midi_stack_forwards[note] = 0xFF;
	_midi_last_note = note;
}

void _midi_pop_note(uint8_t note) {
	if (note ==  _midi_last_note) {
		_midi_last_note = _midi_stack_backwards[note];
		_midi_stack_backwards[note] = 0xFF;
		_midi_stack_forwards[note] = 0xFF;
		if(_midi_last_note != 0xFF) {_midi_stack_forwards[_midi_last_note] = 0xFF;}
		return;
	}

	if(_midi_stack_backwards[note] != 0xFF) {
		_midi_stack_forwards[_midi_stack_backwards[note]] = _midi_stack_forwards[note];
	}
	if(_midi_stack_forwards[note] != 0xFF) {
		_midi_stack_backwards[_midi_stack_forwards[note]] = _midi_stack_backwards[note];
	}

	_midi_stack_backwards[note] = 0xFF;
	_midi_stack_forwards[note] = 0xFF;
}

void midi_stack_clear() {
	for(uint8_t i = 0; i < 128; i++) {
		_midi_stack_forwards[i] = 0xFF;
		_midi_stack_backwards[i] = 0xFF;
	}
	_midi_last_note = 0xFF;
}

uint8_t midi_get_monophonic() {
	if(_midi_last_note < 33 || _midi_last_note > 81) {return 0xFF;}
	return _midi_last_note;
}

void handle_midi_message(uint8_t cmd, uint8_t fb, uint8_t sb) {
	switch(MIDI_CMD(cmd)) {
		case 0xE0:
			synth_set_pb((uint16_t)fb + ((uint16_t)sb << 7));
			break;
		case 0xB0:
			if (fb == 1) {synth_set_mod(sb);}			// CC1: Modulation
			else if (fb == 75) {synth_set_cc(0, sb);}
			else if (fb == 76) {synth_set_cc(1, sb);}
			else if (fb == 77) {synth_set_cc(2, sb);}
			else if (fb == 78) {synth_set_cc(3, sb);}
			else if (fb == 79) {synth_set_cc(4, sb);}
			else if(fb == 123) {midi_stack_clear();}			// CC123: All notes off
			break;
		case 0x90:
			// Add note to list.
			if(fb >= 33 && fb <= 81) {_midi_push_note(fb);}
			break;
		case 0x80:
			// Delete note from list.
			if(fb >= 33 && fb <= 81) {_midi_pop_note(fb);}
			break;
	}
}

void midi_panic() {
	midi_stack_clear();
	_midi_status = 0x00;
	_midi_mode = MIDI_MODE_RESET;
	synth_reset_midi();
}

void midi_reset() {
	midi_panic();
	synth_set_cc(0, 0);
	synth_set_cc(1, 0);
	synth_set_cc(2, 0);
	synth_set_cc(3, 0);
	synth_set_cc(4, 0);
}

