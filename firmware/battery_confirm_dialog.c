#include "battery_confirm_dialog.h"

#include <oledm/font/terminus8x16.h>
#include <oledm/font/terminus16x32_numbers.h>
#include <pstr/pstr.h>

#include "common_ui.h"

void display_battery_confirm_dialog(
    struct Text* text,
    uint16_t mv,
    battery_type type,
    uint16_t min_custom_voltage_mv,
    uint8_t cell_count) {

  button_captions(text, 'Y', 'N');

  text->font = (struct FontASCII*)terminus16x32_numbers;
  text->row = 1;
  text->column = 12;
  display_voltage_common(text, mv);

  text->font = (struct FontASCII*)terminus8x16;
  text->row = 3;
  text_char(text, 'V');

  text->row = 6;
  if (type == BATTERY_TYPE_CUSTOM) {
    text->column = 8;
    text_str(text, "Custom ");
    display_voltage_common(text, min_custom_voltage_mv);
    text_char(text, 'V');
  } else {
    text->column = 32;
    text_str(text, get_battery_name(type));
    text_char(text, ' ');
    text_pstr(text, u8_to_ps(cell_count));
    text_char(text, 'S');
  }
  text_char(text, '?');
}
