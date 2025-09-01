/*
 * tick.h
 *
 *  Created on: Sep 11, 2023
 *      Author: Sx107
 */

#ifndef TICK_H_
#define TICK_H_

#include <main.h>

extern volatile uint32_t pwmled_timer;
extern volatile uint32_t butled_timer;
extern volatile uint8_t butled_animation;

void tick_configure();

#endif /* TICK_H_ */
