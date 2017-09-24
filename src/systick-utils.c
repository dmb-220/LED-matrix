#include "systick-utils.h"

/* Milisekundes talpinantis kintamasis. */
volatile uint32_t miliseconds;

/**
 * Sisteminio laikmačio apdorojimo paprogramė.
 * Jei nesikompiliuoja, ieškoti tokios pat paprogramės
 * implementacijos stm32f10x_it.c faile ir ten
 * užkomentuoti.
 */
void SysTick_Handler(void) {
	miliseconds++;
}

void init_systick_utils(void) {
	miliseconds = 0;

	/* Nustatom sisteminio laikmačio parametrus ir įjungiam pertraukimą. */
	SysTick->LOAD=TIMER_TICK;
	SysTick->VAL=TIMER_TICK;
	SysTick->CTRL=	SysTick_CTRL_CLKSOURCE_Msk |
	                SysTick_CTRL_TICKINT_Msk   |
	                SysTick_CTRL_ENABLE_Msk;
}

uint32_t millis(void) {
	return miliseconds;
}

void delay(uint32_t ms) {
	uint32_t start_millis = millis();
	while ((millis() - start_millis) < ms);
}
