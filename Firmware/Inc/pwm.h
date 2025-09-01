/*
 * pwm.h
 *
 *  Created on: Jan 10, 2022
 *      Author: Sx107
 */

#ifndef PWM_H_
#define PWM_H_

#include <main.h>

void pwm_configure();

#define PWM_AUDIO_OUT (TIM1->CCR1)
#define PWM_LED1 (TIM3->CCR4) // AKA LED_WF3
#define PWM_LED2 (TIM1->CCR2) // AKA LED_WF2
#define PWM_LED3 (TIM1->CCR3) // AKA LED_WF1
#define PWM_LED4 (TIM1->CCR4) // AKA LED_WF6
#define PWM_LED5 (TIM3->CCR1) // AKA LED_WF5
#define PWM_LED6 (TIM3->CCR2) // AKA LED_WF4

extern volatile uint32_t* const PWM_LEDS[6];

#endif /* PWM_H_ */
