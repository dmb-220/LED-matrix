#include "milli-timer.h"
#include "systick-utils.h"

MilliTimer new_milli_timer(void) {
	MilliTimer new_timer;
	new_timer.armed = 0;
	new_timer.next = 0;
	return new_timer;
}

uint8_t mt_poll(MilliTimer* t, uint32_t ms) {
	uint8_t ready = 0;
    if (t->armed) {
    	uint32_t remain = t->next - millis();
        // since remain is unsigned, it will overflow to large values when
        // the timeout is reached, so this test works as long as poll() is
        // called no later than 5535 millisecs after the timer has expired
        if (remain <= 60000)
            return 0;
        // return a value between 1 and 255, being msecs+1 past expiration
        // note: the actual return value is only reliable if poll() is
        // called no later than 255 millisecs after the timer has expired
        ready = -remain;
    }
    mt_set(t, ms);
    return ready;
}

uint8_t mt_idle(MilliTimer* t) {
	return !t->armed;
}

uint32_t mt_remaining(MilliTimer* t) {
	uint32_t remain = t->armed ? t->next - millis() : 0;
    return remain <= 60000 ? remain : 0;
}

void mt_set(MilliTimer* t, uint32_t ms) {
    t->armed = ms != 0;
    if (t->armed) {
        t->next = millis() + ms - 1;
    }
}
