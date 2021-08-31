#include "awake_time.h"
#include <avr/interrupt.h>

volatile uint32_t awake_time_value_g;

#define CYCLES_PER_MS (F_CPU / 1000)
#define CLOCK_DIVIDER 1024

void awake_time_init(void) {
  awake_time_value_g = 0;

  TCCR0A = 0x00;  // Normal mode

  TIMSK0 = 1 << TOIE0; // Enable counter overflow interrupt
  TCNT0 = 0;  // starting value

  // Turn on the timer.
  // The choice for clock select:
  //   - We assume F_CPU = 8000000
  //   - We would like ~10ms accuracy
  // (F_CPU / 1000) is cycles per ms, assume 8000
  // So the /1 counter would give 8000 incremets per ms
  // If we use the /1024 counter, then we have 7.8125 cycles
  // per ms
  TCCR0B = 0x05;

  // Interrupts must be enabled for the overflow to be called
  sei();
}

uint32_t awake_time_in_ms(void) {
  // need to stop the clock while reading it.  Otherwise there is an edge race
  // condition between awake_time_value_g and TCNT0
  TCCR0B = 0x00;
  const uint32_t timer_value = awake_time_value_g | TCNT0;
  TCCR0B = 0x05;  // back on

  return (uint32_t)((uint64_t)timer_value * CLOCK_DIVIDER / CYCLES_PER_MS);
}

ISR(TIMER0_OVF_vect) {
  if (awake_time_value_g < 0x3F0000) { 
    awake_time_value_g += 0x100;
  } else {
    awake_time_value_g = 0;
  }
}
