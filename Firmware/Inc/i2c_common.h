/*
 * i2c_common.h
 *
 *  Created on: 10 дек. 2022 г.
 *      Author: Sx107
 */

#ifndef I2C_COMMON_H_
#define I2C_COMMON_H_

#define I2C_ERR_BERR 	(0b1 << 0)
#define I2C_ERR_ARLO 	(0b1 << 1)
#define I2C_ERR_OVR 	(0b1 << 2)
#define I2C_ERR_PEC 	(0b1 << 3)
#define I2C_ERR_I2CTMO 	(0b1 << 4)
#define I2C_ERR_TMO		(0b1 << 5) // This is NOT bit 14 SMBus timeout error!
#define I2C_ERR_BUSY 	(0b1 << 6) // I2C is busy.
#define I2C_ERR_OTHER	(0b1 << 7) // I2C is busy.

typedef enum {I2C_WRITE, I2C_READ} _i2c_wrmode;

#endif /* I2C_COMMON_H_ */
