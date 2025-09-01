/*
 * midi.h
 *
 *  Created on: 26 сент. 2023 г.
 *      Author: Sx107
 */

#ifndef MIDI_H_
#define MIDI_H_

#include <main.h>

typedef enum {
	MIDI_MODE_RESET, // Waiting for the 1st status byte, ignoring all data bytes
	MIDI_MODE_SINGLE_WAIT, // Waiting for a single byte
	MIDI_MODE_1BYTE_WAIT, // Waiting for the 1st data byte for a 2-byte message
	MIDI_MODE_2BYTE_WAIT, // Waiting for the second data byte in a 2-byte message
	MIDI_MODE_SYSEX // Receiving a SYSEX, ignoring it, waiting for SysEx end (0xF0)
} midi_mode_t;

extern volatile uint8_t _midi_own_channel;

#define MIDI_ALLCHAN 0xFF

#define MIDI_CHAN(X) (X & 0x0F)	// Returns the channel from the status byte
#define MIDI_CMD(X) (X & 0xF0)	// Returns the command from the status byte

void midi_configure();
void handle_midi_message(uint8_t midi_cmd, uint8_t fb, uint8_t sb);
uint8_t midi_get_monophonic();
void midi_panic();
void midi_stack_clear();
uint8_t midi_is_connected();
void midi_reset();

#endif /* MIDI_H_ */
