#include "input.h"

#include <adc/adc.h>
#include <misc/debounce.h>

#include "awake_time.h"

#include <avr/interrupt.h>

// A0 - Center tap for brightness control potentiometer 
#define POT_ADC_CHANNEL 0

// PD4 (D4) Pot power supply. This is a digital output so that
// it can be turned off in sleep mode to save power.
#define POT_POWER_PORT PORTD
#define POT_POWER_DDR DDRD
#define POT_POWER_PIN 4
// Add some deadzone at the end of the pot since it might not
// be possible to be get to or be stable at these values.
#define MIN_ADC_VALUE 23
#define MAX_ADC_VALUE 1000

// PD3 (D3) - Green Button (uses internal pullup, active LOW)
// Note, this is using INT1 so changing it will require also
// reworking the interrupt logic.
#define GREEN_BUTTON_PORT PORTD
#define GREEN_BUTTON_PIN 3

//
// Globals
//

struct Debounce red_button_g;
struct Debounce green_button_g;

//
// Implementation
//


void input_init(void) {
  // Red Button init
  RED_BUTTON_PORT |= (1 << RED_BUTTON_PIN);  // internal pullup
  debounce_init(&red_button_g, 250);
  EICRA |= (1 << ISC01);  // falling edge trigger
  EIMSK |= (1 << INT0);

  // Green button init
  GREEN_BUTTON_PORT |= (1 << GREEN_BUTTON_PIN);  // internal pullup
  debounce_init(&green_button_g, 250);
  EICRA |= (1 << ISC11);  // falling edge trigger
  EIMSK |= (1 << INT1);

  // Pot hardware setup
  POT_POWER_DDR |= (1 << POT_POWER_PIN);  // set as output
  POT_POWER_PORT |= (1 << POT_POWER_PIN); // turn it on
}

void input_sleep(void) {
  POT_POWER_DDR &= ~(1 << POT_POWER_PIN);  // set as input (hi-z)
  POT_POWER_PORT &= ~(1 << POT_POWER_PIN); // disable internal pullup
}

void input_wake() {
  POT_POWER_DDR |= (1 << POT_POWER_PIN);  // set as output
  POT_POWER_PORT |= (1 << POT_POWER_PIN); // turn it on
}

uint8_t get_dimmer_value(const struct DimmerConfig* config) {
  // Use uint32_t so that we don't overflow on the scaling op.
  uint32_t adc_value = adc_read16(
      POT_ADC_CHANNEL,
      ADC_REF_VCC,
      ADC_PRESCALER_128);
  if (adc_value < MIN_ADC_VALUE) {
    adc_value = MIN_ADC_VALUE;
  } else if (adc_value > MAX_ADC_VALUE) {
    adc_value = MAX_ADC_VALUE;
  }

  adc_value -= MIN_ADC_VALUE;

  // Need to rescale adc_value (0-1023) to PWM (min_pwm-max_pwm)
  return config->min_pwm + (uint8_t)(
      adc_value * (config->max_pwm - config->min_pwm) /
      (MAX_ADC_VALUE - MIN_ADC_VALUE)
  );
}

bool_t red_button_was_pressed(void) {
  if (red_button_g.edge_triggered) {
    red_button_g.edge_triggered = 0;
    return 1;
  }
  return 0;
}

bool_t green_button_was_pressed(void) {
  if (green_button_g.edge_triggered) {
    green_button_g.edge_triggered = 0;
    return 1;
  }
  return 0;
}

// Red button interrupt
ISR(INT0_vect) {
    debounce_sample_edge_only(&red_button_g, awake_time_in_ms());
}

// Green button interrupt
ISR(INT1_vect) {
    debounce_sample_edge_only(&green_button_g, awake_time_in_ms());
}
