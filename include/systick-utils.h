#ifndef SYSTICK_UTILS_H
#define SYSTICK_UTILS_H

#include "stm32f10x.h"
/* Procesoriaus dažnis ir laikmačio intervalas */
#define F_CPU 		72000000UL
#define TIMER_TICK	F_CPU/1000-1

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Sisteminio laikmačio inicializavimo funkcija
 */
void init_systick_utils(void);

/**
 * Funkcija, grąžinanti einamąsias milisekundes.
 * Milisekundės skaičiuojamos nuo procesoriaus
 * pagrindinės programos užsikūrimo pradžios.
 */
uint32_t millis(void);

/**
 * „Busy-wait“ pauzės funkcija. Pauzė nurodoma
 * milisekundėmis
 */
void delay(uint32_t ms);

#ifdef __cplusplus
}
#endif

#endif
