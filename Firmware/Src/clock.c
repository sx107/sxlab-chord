/*
 * clock.c
 *
 *  Created on: Jan 9, 2022
 *      Author: Sx107
 */


#include <main.h>
#include <stm32f0xx.h>

void clock_configure(void) {
	// Set flash latency to 1 (since target clock > 24 MHz)
	FLASH->ACR |= FLASH_ACR_LATENCY;
	WAITBIT(FLASH->ACR, FLASH_ACR_LATENCY);


	// Set CSS to ON, enable HSE
	RCC->CR |= RCC_CR_CSSON | RCC_CR_HSEON;
	WAITBIT(RCC->CR, RCC_CR_HSERDY);

	// PLL: x6, Prediv = 0, Source = HSE (6 * 8 = 48 Mhz)
	MODIFY_REG(RCC->CFGR2, RCC_CFGR2_PREDIV, RCC_CFGR2_PREDIV_DIV1);
	MODIFY_REG(RCC->CFGR, RCC_CFGR_PLLMUL | RCC_CFGR_PLLSRC, RCC_CFGR_PLLMUL6 | RCC_CFGR_PLLSRC_HSE_PREDIV);


	/*
	// Internal RC
	RCC->CR |= RCC_CR_HSION;
	WAITBIT(RCC->CR, RCC_CR_HSIRDY);

	MODIFY_REG(RCC->CFGR, RCC_CFGR_PLLMUL | RCC_CFGR_PLLSRC, RCC_CFGR_PLLMUL12 | RCC_CFGR_PLLSRC_HSI_DIV2);
	*/

	//Enable PLL
	RCC->CR |= RCC_CR_PLLON;
	WAITBIT(RCC->CR, RCC_CR_PLLRDY);

	// HPRE: Div1, PPRE: Div1 (AHB, APB bus clock = 48 Mhz)
	MODIFY_REG(RCC->CFGR, RCC_CFGR_HPRE | RCC_CFGR_PPRE, RCC_CFGR_HPRE_DIV1 | RCC_CFGR_PPRE_DIV1);

	// Set clock source to PLL
	MODIFY_REG(RCC->CFGR, RCC_CFGR_SW, RCC_CFGR_SW_PLL);
	WAITVAL(RCC->CFGR, RCC_CFGR_SWS, RCC_CFGR_SWS_PLL);

	// ADC Clock
	RCC->CR2 |= RCC_CR2_HSI14ON;
	WAITBIT(RCC->CR2, RCC_CR2_HSI14RDY);
	// MODIFY_REG(RCC->CR2, RCC_CR2_HSI14CAL, 16 << RCC_CR2_HSI14CAL_Pos); // Not required?

	SystemCoreClockUpdate();

	__disable_irq();
	SysTick->LOAD = 6000;
	SysTick->CTRL = SysTick_CTRL_ENABLE_Msk;
	SysTick->VAL = 0UL;
	__enable_irq();
}

void _delayMs(uint32_t del) {
	SysTick->VAL = 0;
	while (del) {
		if ((SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk) != 0U)
		{
			del--;
	    }
	}
}
