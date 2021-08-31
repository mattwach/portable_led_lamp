#ifndef INPUT_H
#define INPUT_H

#include <error_codes.h>

// Needed by both input.h and debug.h
// PD2 (D2) - Red Button (using internal pullup, thus active is LOW)
// Note, this is using INT0 so changing it will require also
// reworking the interrupt logic.
#define RED_BUTTON_PORT PORTD
#define RED_BUTTON_PIND PIND
#define RED_BUTTON_PIN 2


// Encapsulates input routines
struct DimmerConfig {
  uint8_t min_pwm;  // When dimmer is all the way to the left
  uint8_t max_pwm;  // When dimmer is all the way to the right
};

// Initializes input hardware and structures.
// Note: awake_time_init() MUST be called before calling this function
//  to allow for proper button debounce.
void input_init(void);

// put the input in low power state and wake it back up
void input_sleep(void);
void input_wake(void);

// Returns the dimmer value as a PWM value
uint8_t get_dimmer_value(const struct DimmerConfig* config);

// Returns true if the red/grees button was pressed since the last call.  Clears on call.
bool_t red_button_was_pressed(void);
bool_t green_button_was_pressed(void);

#endif
