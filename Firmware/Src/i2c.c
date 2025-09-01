/*
 * i2c1.c
 *
 *  Created on: Aug 28, 2023
 *      Author: Sx107
 */

#include "clock.h"
#include "i2c.h"
#include "main.h"

volatile uint8_t _i2c1_error = 0;
//volatile uint32_t _i2c_dmaint = 0;

void i2c1_init() {
	_i2c1_error = 0;

	RCC->AHBENR |= RCC_AHBENR_GPIOBEN;
	// Set PB6, PB7 to AF
	MODIFY_REG(GPIOB->MODER, GPIO_MODER_MODER8,	0b10 << GPIO_MODER_MODER8_Pos);
	MODIFY_REG(GPIOB->MODER, GPIO_MODER_MODER9,	0b10 << GPIO_MODER_MODER9_Pos);
	// Set all of them to proper speed
	MODIFY_REG(GPIOB->OTYPER, GPIO_OTYPER_OT_8, GPIO_OTYPER_OT_8);
	MODIFY_REG(GPIOB->OTYPER, GPIO_OTYPER_OT_9, GPIO_OTYPER_OT_9);
	// Set all of them to proper speed
	MODIFY_REG(GPIOB->OSPEEDR, GPIO_OSPEEDR_OSPEEDR8, 0b11 << GPIO_OSPEEDR_OSPEEDR8_Pos);
	MODIFY_REG(GPIOB->OSPEEDR, GPIO_OSPEEDR_OSPEEDR9, 0b11 << GPIO_OSPEEDR_OSPEEDR9_Pos);
	// Set them to AF4 I2C
	MODIFY_REG(GPIOB->AFR[1], GPIO_AFRH_AFSEL8_Msk | GPIO_AFRH_AFSEL9_Msk, (1 << GPIO_AFRH_AFSEL8_Pos) | (1 << GPIO_AFRH_AFSEL9_Pos));

	// Enable I2C1 clock, reset it
	RCC->CFGR3 |= RCC_CFGR3_I2C1SW_SYSCLK;
	RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;
	I2C1->CR1 &= ~I2C_CR1_PE;
	_delayMs(1);

	// No global call, enable clock stretch, disable I2C
	MODIFY_REG(I2C1->CR1, I2C_CR1_GCEN | I2C_CR1_NOSTRETCH | I2C_CR1_PE, 0);

	I2C1->TIMINGR = (uint32_t)0x00B01A4B; // W. H. A. T.
	I2C1->OAR1 |= I2C_OAR1_OA1EN;
	I2C1->CR1 |= I2C_CR1_PE;

	I2C1->CR1 |= I2C_CR1_ERRIE | I2C_CR1_NACKIE;
	for(uint8_t i = 0; i < 0xFE; i++) {}

	NVIC_EnableIRQ(I2C1_IRQn);
	NVIC_SetPriority(I2C1_IRQn, 3);
}

uint8_t i2c1_nack() {
	return I2C1->ISR & I2C_ISR_NACKF;
}

void i2c1_nack_clear() {
	I2C1->ICR |= I2C_ICR_NACKCF;
}

void i2c1_deinit() {
	I2C1->ICR |=  I2C_ICR_NACKCF | I2C_ICR_BERRCF | I2C_ICR_ARLOCF | I2C_ICR_OVRCF | I2C_ICR_TIMOUTCF;
	I2C1->CR1 &= ~I2C_CR1_NACKIE | ~I2C_CR1_ERRIE;
	I2C1->CR1 &= ~I2C_CR1_PE;
}
void i2c1_reset() {
	I2C1->CR2 |= I2C_CR2_STOP;
	MODIFY_REG(GPIOB->MODER, GPIO_MODER_MODER8,	0b00 << GPIO_MODER_MODER8_Pos);
	MODIFY_REG(GPIOB->MODER, GPIO_MODER_MODER9,	0b00 << GPIO_MODER_MODER9_Pos);

	RCC->APB1ENR &= ~RCC_APB1ENR_I2C1EN;
	i2c1_init();
}


inline uint8_t i2c1_waitBit(uint32_t bit) {
	// Waits for a certain bit in I2C1->SR1 with a timeout
	//_i2c1_error = 0;
	uint16_t timeOut = 0;
	while(((I2C1->ISR) & bit) != bit) {
		if((timeOut++ > I2C1_TMO_VAL) && (I2C1_TMO_VAL != 0)) {
			_i2c1_error |= I2C_ERR_TMO;
			break;
		}
	}
	return _i2c1_error;
}

inline uint8_t i2c1_waitNBit(uint32_t bit) {
	// Waits for a certain bit in I2C1->SR1 with a timeout
	//_i2c1_error = 0;
	uint16_t timeOut = 0;
	while(((I2C1->ISR) & bit) == bit) {
		if((timeOut++ > I2C1_TMO_VAL) && (I2C1_TMO_VAL != 0)) {
			_i2c1_error |= I2C_ERR_TMO;
			break;
		}
	}
	return _i2c1_error;
}

void I2C1_IRQHandler(void) {
	_i2c1_error |= ((I2C1->ISR) >> 8) & 0b11111;
	//DMA1_Channel6->CCR &= ~DMA_CCR_EN;
	// SmBus errors are just ignored
	I2C1->ICR |=  I2C_ICR_NACKCF | I2C_ICR_BERRCF | I2C_ICR_ARLOCF | I2C_ICR_OVRCF | I2C_ICR_TIMOUTCF;
}
uint8_t i2c1_handle_error() {
	// Resets I2C, Sends stop condition, etc.
	return _i2c1_error;
}
void i2c1_reset_error() {
	_i2c1_error = 0;
}
uint8_t i2c1_get_error() {
	return _i2c1_error;
}

// Manual transmission functions
uint8_t i2c1_transmission_busy() {
	return ((I2C1->ISR) & I2C_ISR_BUSY) == I2C_ISR_BUSY ? 1 : 0;
}
uint8_t i2c1_transmission_start(uint8_t addr, _i2c_wrmode mode, uint8_t nBytes, uint16_t busy_timeout) {
	// Return if any error occurred earlier immediately, wait while I2C is busy
	if(_i2c1_error != 0) {return _i2c1_error;}
	uint16_t tmo_busy = busy_timeout;
	while((((I2C1->ISR) & I2C_ISR_BUSY) == I2C_ISR_BUSY) && (tmo_busy > 0)) {tmo_busy--;}
	if(tmo_busy == 0) {_i2c1_error |= I2C_ERR_BUSY;}

	if(_i2c1_error != 0) {return _i2c1_error;}

	// Send start condition
	I2C1->CR2 = (nBytes << 16) | (addr << 1) | I2C_CR2_START | (mode == I2C_READ ? I2C_CR2_RD_WRN : 0);
	i2c1_waitBit(I2C_ISR_TXE);
	return _i2c1_error;
}
uint8_t i2c1_transmission_end(_i2c_wrmode mode) {
	if(mode == I2C_WRITE) {
		uint16_t tmo = 1000;
		while(
			(((I2C1->ISR) & I2C_ISR_TC) != I2C_ISR_TC) &&
			(_i2c1_error == 0) &&
			(tmo > 0)
		) {tmo--;};

		if(tmo == 0) {_i2c1_error |= I2C_ERR_TMO;}
		if(_i2c1_error != 0) {return _i2c1_error;}

		// Send stop condition
		I2C1->CR2 |= I2C_CR2_STOP;
		(void) I2C1->ISR;
		I2C1->ICR |= I2C_ICR_STOPCF;
	} else {
		WAITBIT(I2C1->ISR, I2C_ISR_RXNE);
		I2C1->CR2 |= I2C_CR2_STOP;
		(void) I2C1->ISR;
		I2C1->ICR |= I2C_ICR_STOPCF;

	}
	return _i2c1_error;
}
uint8_t i2c1_transmission_write8(uint8_t byte) {
	I2C1->TXDR = byte;
	return i2c1_waitBit(I2C_ISR_TXE);
}
uint8_t i2c1_transmission_read8(uint8_t* byte) {
	RETERR(i2c1_waitBit(I2C_ISR_RXNE), _i2c1_error);
	*byte = I2C1->RXDR;
	return _i2c1_error;
}

// Main write function
uint8_t i2c1_write_blocking(uint8_t addr, uint16_t sz, const volatile uint8_t* buf, uint16_t busy_timeout) {
	// Transmission start
	RETERR(i2c1_transmission_start(addr, I2C_WRITE, sz, busy_timeout), _i2c1_error);

	// Send data
	for(uint16_t i = 0; i < sz; i++) {
		RETERR(i2c1_transmission_write8(buf[i]), _i2c1_error);
	}

	return i2c1_transmission_end(I2C_WRITE);
}
// Main read function
uint8_t i2c1_read(uint8_t addr, uint16_t sz, uint8_t* buf, uint16_t busy_timeout) {
	RETERR(i2c1_transmission_start(addr, I2C_READ, sz, busy_timeout), _i2c1_error);

	// Send data
	for(uint16_t i = 0; i < (sz-1); i++) {
		i2c1_transmission_read8(&(buf[i]));
	}

	// Stop
	RETERR(i2c1_transmission_end(I2C_READ), _i2c1_error);
	buf[sz-1] = I2C1->RXDR;
	return 0;
}

uint8_t i2c1_dr() {
	return I2C1->RXDR;
}
