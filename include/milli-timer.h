#ifndef MILLI_TIMER_H
#define MILLI_TIMER_H

#include <stdint.h>

typedef struct {
	uint32_t next;
	uint8_t armed;
} MilliTimer;

MilliTimer new_milli_timer(void);
/// poll until the timer fires
/// @param ms Periodic repeat rate of the time, omit for a one-shot timer.
uint8_t mt_poll(MilliTimer* t, uint32_t ms);
/// Return the number of milliseconds before the timer will fire
uint32_t mt_remaining(MilliTimer* t);
/// Returns true if the timer is not armed
uint8_t mt_idle(MilliTimer* t);
/// set the one-shot timeout value
/// @param ms Timeout value. Timer stops once the timer has fired.
void mt_set(MilliTimer* t, uint32_t ms);

#endif
