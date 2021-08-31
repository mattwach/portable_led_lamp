#include "led_control.h"
#include <util/delay.h>

#include <avr/io.h>

// PD7 (D7) - LED power.  Set high for ON.  Set low (pr preferably Hi-Z)
// for OFF.
#define LED_POWER_PORT PORTD
#define LED_POWER_DDR DDRD
#define LED_POWER_PIN 7

// LED PWM signal is fixed at PB1 (D9, OC1A)

#define MAX_TIMER_VALUE ((uint32_t)F_CPU / 400)  // Updates will be around 400Hz
#define FET_POWER_TIME_MS 2  // give the FET a little time to turn on and off completly

// how fast to turn on and off
#define PWM_STEP 3

void led_power_on(uint8_t pwm) {
  //
  // PWM Setup
  //
  DDRB |= (1 << 1);  // Set PB1 (PWM) as an output
  OCR1A = 0;
  TCCR1A = 0x82;  // COM1A1 - Clear OC1A on compare match
                  // WGM11 - PWM mode with ICR1 as top
  TCCR1B = 0x19;  // WGM13, WGM12 - PWM Mode with ICR1 on top
                  // CS10 - Full speed for now
  ICR1 = MAX_TIMER_VALUE;  // maximum count value (1023?)

  // Turn on PFET
  LED_POWER_DDR |= (1 << LED_POWER_PIN);
  LED_POWER_PORT |= (1 << LED_POWER_PIN);
  _delay_ms(FET_POWER_TIME_MS);

  // work toward full brightness
  for (uint8_t pwmi = 0; pwmi < pwm; pwmi += PWM_STEP) {
    led_set_pwm(pwmi);
    _delay_ms(1);
  }
  led_set_pwm(pwm);
}

void led_power_off(uint8_t start_pwm) {
  // work toward min brightness
  for (int16_t pwmi = start_pwm; pwmi > 0; pwmi -= PWM_STEP) {
    led_set_pwm((uint8_t)pwmi);
    _delay_ms(1);
  }
  led_set_pwm(0);

  // Turn off FET (Hi-Z)
  LED_POWER_DDR &= ~(1 << LED_POWER_PIN);
  LED_POWER_PORT &= ~(1 << LED_POWER_PIN);
  _delay_ms(FET_POWER_TIME_MS);

  // Turn off PWM
  TCCR1B = 0x18; // timer is stopped
  DDRB &= ~(1 << 1);  // Set PB1 (PWM) as an input
}

void led_set_pwm(uint8_t pwm) {
  OCR1A = (uint16_t)((uint32_t)pwm * MAX_TIMER_VALUE / 255);
}
