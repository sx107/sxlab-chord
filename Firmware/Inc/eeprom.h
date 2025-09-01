/*
 * eeprom.h
 *
 *  Created on: Sep 18, 2023
 *      Author: Sx107
 */

#include <main.h>

#ifndef EEPROM_H_
#define EEPROM_H_

uint8_t eeprom_write8(uint8_t addr, uint8_t data);
uint8_t eeprom_read8(uint8_t addr);

uint8_t eeprom_write16(uint8_t addr, uint16_t data);
uint16_t eeprom_read16(uint8_t addr);

uint8_t eeprom_write_buffer(uint8_t addr, const volatile uint8_t* buf, uint16_t len);
uint8_t eeprom_read_buffer(uint8_t addr, volatile uint8_t* buf, uint16_t len);

uint8_t eeprom_wait_available(uint16_t tmo);


// Common (not preset-wise)
#define EEPROM_CHECK_FIRST	192
#define EEPROM_MIDI_CHAN	194
#define EEPROM_HAS_PRESET3	195

#endif /* EEPROM_H_ */
