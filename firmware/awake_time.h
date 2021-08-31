#ifndef AWAKE_TIME_H
#define AWAKE_TIME_H

#include <inttypes.h>

// Uses timer0 with rollover to estimate current uptime

// Initialize timer.  Calling this function enables interrupts.
void awake_time_init(void);

// Returns awake in ms.  As implied by the name, power down time is not counted.
uint32_t awake_time_in_ms(void);

#endif
