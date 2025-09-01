/*
 * i2c1.h
 *
 *  Created on: Aug 28, 2023
 *      Author: Sx107
 */

#include "main.h"
#include "i2c_common.h"

#ifndef DRV_I2C_H_
#define DRV_I2C_H_

#define I2C1_TMO_VAL 50000

extern volatile uint8_t _i2c1_error;

void i2c1_init();
void i2c1_deinit();
void i2c1_reset();

// Error handling
void I2C1_ER_IRQHandler(void);
void i2c1_reset_error();
uint8_t i2c1_get_error();
uint8_t i2c1_handle_error();
uint8_t i2c1_waitBit(uint32_t bit);

// Manual functions
uint8_t i2c1_transmission_start(uint8_t addr, _i2c_wrmode mode, uint8_t nBytes, uint16_t busy_timeout);
uint8_t i2c1_transmission_end(_i2c_wrmode mode);
uint8_t i2c1_transmission_write8(uint8_t byte);
uint8_t i2c1_transmission_read8(uint8_t* byte);
uint8_t i2c1_transmission_busy();
uint8_t i2c1_nack();
void i2c1_nack_clear();


// Main write functions
uint8_t i2c1_write_blocking(uint8_t addr, uint16_t sz, const volatile uint8_t* buf, uint16_t busy_timeout);

// Main read functions
uint8_t i2c1_read(uint8_t addr, uint16_t sz, uint8_t* buf, uint16_t busy_timeout);
uint8_t i2c1_dr();

#endif /* DRV_I2C_H_ */

