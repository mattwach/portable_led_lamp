#ifndef LED_CONTROL_H
#define LED_CONTROL_H

#include <inttypes.h>

// Provides control interface for LEDs

// Turn on LED and set brightness
void led_power_on(uint8_t pwm);

// Turn OFF LED via PFET, turns off PWM as well
void led_power_off(uint8_t start_pwm);

// Change led brightness - assumes that led_power_on was called already.
void led_set_pwm(uint8_t pwm);

#endif
