#include "common_ui.h"

#include <oledm/font/terminus8x16.h>
#include <pstr/pstr.h>

void display_voltage_common(struct Text* text, uint16_t volts_mv) {
  volts_mv += 50;  // for rounding the .1 V
  const uint8_t volts = (uint8_t)(volts_mv / 1000);
  if (volts < 10) {
    text_char(text, ' ');
  }
  text_pstr(text, u8_to_ps(volts));
  text_char(text, '.');
  text_pstr(
      text, 
      u8_to_ps((uint8_t)(
          (volts_mv % 1000) / 100)));
}

void button_captions(struct Text* text, char green_caption, char red_caption) {
  text->font = (struct FontASCII*)terminus8x16;
  text->options = TEXT_OPTION_INVERTED;

  char data[4] = {green_caption, ' ', ' ', red_caption};
  for (int i=0; i < 4; ++i) {
    text->row = i * 2;
    text->column = 120;
    text_char(text, data[i]);
  }

  text->options = 0;
}
