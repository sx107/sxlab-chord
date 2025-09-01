/*
 * eeprom.c
 *
 *  Created on: Sep 18, 2023
 *      Author: Sx107
 */


#include "main.h"
#include "i2c.h"
#include "eeprom.h"

uint8_t eeprom_read8(uint8_t addr) {
	RETERR(i2c1_transmission_start(0b1010000, I2C_WRITE, 1, 10000), 0);
	RETERR(i2c1_transmission_write8(addr), 0);
	RETERR(i2c1_transmission_end(I2C_WRITE), 0);
	RETERR(i2c1_transmission_start(0b1010000, I2C_READ, 1, 10000), 0);
	RETERR(i2c1_transmission_end(I2C_READ), 0);
	return i2c1_dr();
}
uint8_t eeprom_write8(uint8_t addr, uint8_t data) {
	RETERR(i2c1_transmission_start(0b1010000 , I2C_WRITE, 2, 10000), i2c1_get_error());
	RETERR(i2c1_transmission_write8(addr), i2c1_get_error());
	RETERR(i2c1_transmission_write8(data), i2c1_get_error());
	i2c1_transmission_end(I2C_WRITE);
	return i2c1_get_error();
}

uint8_t eeprom_write16(uint8_t addr, uint16_t data) {
	RETERR(i2c1_transmission_start(0b1010000 , I2C_WRITE, 3, 10000), i2c1_get_error());
	RETERR(i2c1_transmission_write8(addr), i2c1_get_error());
	RETERR(i2c1_transmission_write8(data & 0xFF), i2c1_get_error());
	RETERR(i2c1_transmission_write8(data >> 8), i2c1_get_error());
	i2c1_transmission_end(I2C_WRITE);
	return i2c1_get_error();
}
uint16_t eeprom_read16(uint8_t addr) {
	uint8_t v = 0;
	RETERR(i2c1_transmission_start(0b1010000 , I2C_WRITE, 1, 10000), 0);
	RETERR(i2c1_transmission_write8(addr), 0);
	RETERR(i2c1_transmission_end(I2C_WRITE), 0);
	RETERR(i2c1_transmission_start(0b1010000 , I2C_READ, 2, 10000), 0);
	RETERR(i2c1_transmission_read8(&v), 0);
	RETERR(i2c1_transmission_end(I2C_READ), 0);
	return (v) | ((uint16_t)i2c1_dr() << 8);
}

uint8_t eeprom_write_buffer(uint8_t addr, const volatile uint8_t* buf, uint16_t len) {
	if(len == 0) {return i2c1_get_error();}
	RETERR(i2c1_transmission_start(0b1010000 , I2C_WRITE, 1 + len, 10000), i2c1_get_error());
	RETERR(i2c1_transmission_write8(addr), i2c1_get_error());
	RETERR(i2c1_transmission_write8(buf[0]), i2c1_get_error());
	//i2c1_transmission_ack_disable();
	for(uint8_t i = 1; i < len; i++) {
		RETERR(i2c1_transmission_write8(buf[i]), i2c1_get_error());
	}
	i2c1_transmission_end(I2C_WRITE);
	return i2c1_get_error();
}

uint8_t eeprom_read_buffer(uint8_t addr, volatile uint8_t* buf, uint16_t len) {
	RETERR(i2c1_transmission_start(0b1010000 , I2C_WRITE, 1, 10000), 0);
	RETERR(i2c1_transmission_write8(addr), 0);
	RETERR(i2c1_transmission_end(I2C_WRITE), 0);
	return i2c1_read(0b1010000, len, buf, 10000);
}

uint8_t eeprom_wait_available(uint16_t timeout) {
	if(i2c1_get_error() != 0) {return i2c1_get_error();}
	uint16_t tmo;
	for(tmo = 0; tmo < timeout; tmo++) {
		i2c1_reset_error();
		i2c1_nack_clear();
		i2c1_transmission_start(0b1010000 , I2C_WRITE, 2, 10000);
		i2c1_transmission_write8(0);
		i2c1_transmission_end(I2C_WRITE);
		if (i2c1_nack()) {
			i2c1_handle_error();
			i2c1_reset_error();
			for(uint16_t d = 0; d < 1000; d++) {__asm("NOP");}
			continue;
		} else {
			if(i2c1_get_error() == 0) {break;}
		}
	}
	if(tmo == timeout) {_i2c1_error |= I2C_ERR_OTHER; return i2c1_get_error();}
	return 0;
}
