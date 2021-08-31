#include "fatal.h"

#include "debug.h"

#include <oledm/oledm.h>
#include <oledm/font/terminus8x16.h>
#include <util/delay.h>

#define ONBOARD_LED 5

void fatal(struct Text* text, const char* msg, const uint8_t* pstr) {
  DDRB |= (1 << ONBOARD_LED);
  PORTB |= (1 << ONBOARD_LED);

  DEBUG_STRLN("FATAL");

  oledm_clear(text->display, 0x00);
  text->font = (struct FontASCII*)terminus8x16;
  text->row = 0;
  text->column = 0;
  text_str(text, "FATAL:");
  text->row += 2;
  text->column = 0;

  if (msg) {
    DEBUG_STRLN(msg);
    text_str(text, msg);
    text->row += 2;
    text->column = 0;
  }
  if (pstr) {
    DEBUG_PSTRLN(msg);
    text_pstr(text, pstr);
  }

  while (1) {
    _delay_ms(250);
    PORTB &= ~(1 << ONBOARD_LED);
    _delay_ms(250);
    PORTB |= (1 << ONBOARD_LED);
  }
}

