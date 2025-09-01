/*
 * butled.h
 *
 *  Created on: Sep 10, 2023
 *      Author: Sx107
 */

#ifndef BUTLED_H_
#define BUTLED_H_

#include <main.h>

void butled_configure();
void butled_on(uint8_t butled);
void butled_off(uint8_t butled);
uint8_t but_check(uint8_t butled);
void but_update();
uint8_t but_released(uint8_t but);

#define BUTLED_DRONE 0
#define BUTLED_SAVE 1
#define BUTLED_M1 2
#define BUTLED_M2 3
#define BUTLED_M3 4

#endif /* BUTLED_H_ */
