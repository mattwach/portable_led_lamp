#include "status_display.h"

#include <oledm/font/terminus8x16.h>
#include <oledm/font/terminus16x32_numbers.h>
#include <oledm/text.h>

#include <pstr/pstr.h>

#include "awake_time.h"
#include "common_ui.h"

uint16_t cached_battery_mv_g; 
uint16_t cached_battery_ma_g;
bool_t cached_one_time_g;

static void display_voltage(struct Text* text, uint16_t battery_mv) {
  text->row = 0;
  text->column = 0;
  display_voltage_common(text, battery_mv);
}

static void display_current(struct Text* text, uint16_t battery_ma) {
  text->row = 4;
  text->column = 0;
  // if current 0-9 then preceed with 3 spaces
  // if 10-99, 2 spaces
  // if 100-999 1 space
  // > 999 no spaces
  for (uint16_t place = 1000; (place > battery_ma) && (place > 0); place /= 10) {
    text_char(text, ' ');
  }
  text_pstr(text, u16_to_ps(battery_ma));
}

// Display labels for voltage, current and min
static void display_labels(struct Text* text) {
  text->row = 0;
  text->column = 128 - (8 * 3);
  text_str(text, "min");

  text->row = 2;
  text->column = 16 * 4;
  text_char(text, 'V');

  text->row = 6;
  text->column = 16 * 4;
  text_char(text, 'm');
  text_char(text, 'A');
}

static void display_minimum_mv(struct Text* text, uint16_t minimum_mv, bool_t too_low) {
  text->row = 2;
  text->column = 128 - (5 * 8);
  if (too_low && ((awake_time_in_ms() / 1000) & 1)) {
    text->options = TEXT_OPTION_INVERTED;
  }
  display_voltage_common(text, minimum_mv);
  text_char(text, 'V');
  text->options = 0;
}

static void display_battery_type(struct Text* text, uint16_t battery_type) {
  text->row = 4;
  text->column = 128 - (8 * BATTERY_NAME_LENGTH);
  text_str(text, get_battery_name(battery_type));
}

static void display_cell_count(struct Text* text, uint8_t cell_count) {
  if (cell_count == 0) {
    return;
  }
  text->row = 6;
  const uint8_t chars_needed = cell_count > 9 ? 3 : 2;
  text->column = 128 - (8 * chars_needed);
  text_pstr(text, u8_to_ps(cell_count));
  text_char(text, 'S');
}

void status_display_init(void) {
  cached_battery_mv_g = 0xFFFF;
  cached_battery_ma_g = 0xFFFF;
  cached_one_time_g = FALSE;
}

void status_display_render(
    struct Text* text,
    uint16_t battery_mv,
    uint16_t battery_ma,
    uint16_t minimum_mv,
    battery_type batt_type,
    uint8_t cell_count
) {
  if (!cached_one_time_g) {
    text->font = (struct FontASCII*)terminus8x16;
    display_labels(text);
    display_minimum_mv(text, minimum_mv, battery_mv < minimum_mv);
    display_battery_type(text, batt_type);
    display_cell_count(text, cell_count);
    cached_one_time_g = TRUE;
  }

  if (cached_battery_mv_g != battery_mv) {
    text->font = (struct FontASCII*)terminus16x32_numbers;
    display_voltage(text, battery_mv);
    cached_battery_mv_g = battery_mv;
    text->font = (struct FontASCII*)terminus8x16;
  }

  if (cached_battery_ma_g != battery_ma) {
    text->font = (struct FontASCII*)terminus16x32_numbers;
    display_current(text, battery_ma);
    cached_battery_ma_g = battery_ma;
    text->font = (struct FontASCII*)terminus8x16;
  }
}

