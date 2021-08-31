#include "debug.h"
#include "input.h"
#include <util/delay.h>
#include <avr/io.h>

#ifdef DEBUG
#include <uart/uart.c>
#endif

#define ONBOARD_LED 5
#define RED_BUTTON_PORT PORTD
#define RED_BUTTON_PIN 2

void DEBUG_INIT() {
  DDRB |= (1 << ONBOARD_LED);
  PORTB |= (1 << ONBOARD_LED);

  RED_BUTTON_PORT |= (1 << RED_BUTTON_PIN);  // internal pullup
  _delay_ms(30);  // give it a chance
  while ((RED_BUTTON_PIND & (1 << RED_BUTTON_PIN)) == 0) {
    _delay_ms(5000);
  }

  PORTB &= ~(1 << ONBOARD_LED);  // Off

#ifdef DEBUG
 uart_init(9600);
 uart_strln("DEBUG_INIT");
#endif
}
