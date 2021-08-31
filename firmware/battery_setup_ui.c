#include "battery_setup_ui.h"

#include <oledm/oledm.h>
#include <oledm/font/terminus8x16.h>
#include <oledm/font/terminus16x32_numbers.h>

#include "common_ui.h"
#include "eeprom_settings.h"

typedef enum setup_state {
  CHOOSING_BATTERY_TYPE,
  CHOOSING_VOLTAGE,
} setup_state;

typedef enum volt_edit_place {
  EDIT_TENS,
  EDIT_ONES,
  EDIT_POINT_ONES
} volt_edit_place;

// Global state
battery_type selected_type_g;
setup_state setup_state_g;
uint16_t custom_min_mv_g;
volt_edit_place volt_edit_place_g;

void battery_setup_init() {
  setup_state_g = CHOOSING_BATTERY_TYPE;
  selected_type_g = BATTERY_TYPE_CUSTOM;

  const struct EEPROMSettings* settings = eeprom_settings();
  custom_min_mv_g = settings->custom_battery_min_mv;
  if (custom_min_mv_g < MIN_SUPPORTED_BATTERY_MV) {
    custom_min_mv_g == MIN_SUPPORTED_BATTERY_MV;
  } else if (custom_min_mv_g > MAX_SUPPORTED_BATTERY_MV) {
    custom_min_mv_g = MAX_SUPPORTED_BATTERY_MV; 
  }
  volt_edit_place_g = EDIT_TENS;
}

static void save_settings(void) {
  struct EEPROMSettings* settings = eeprom_settings();
  settings->battery_type = selected_type_g;
  if (selected_type_g == BATTERY_TYPE_CUSTOM) {
    settings->custom_battery_min_mv = custom_min_mv_g;
  }
  write_eeprom_settings();
}

static bool_t choose_battery_ui(
    struct Text* text,
    bool_t red_pressed,
    bool_t green_pressed,
    uint16_t battery_mv) {
  if (red_pressed) {
    ++selected_type_g;
    if (selected_type_g >= BATTERY_TYPE_COUNT) {
      selected_type_g = BATTERY_TYPE_CUSTOM;
    }
  } 

  button_captions(text, 'Y', '>');
  text->font = (struct FontASCII*)terminus8x16;
  text->row = 0;
  text->column = 0;
  text->options = selected_type_g == BATTERY_TYPE_CUSTOM ?
    TEXT_OPTION_INVERTED : 0;
  text_str(text, "Custom ");
  display_voltage_common(text, battery_mv);
  text_char(text, 'V');

  text->row = 2;
  uint8_t col = 0;
  for (uint8_t i=BATTERY_TYPE_CUSTOM + 1; i < BATTERY_TYPE_COUNT; ++i) {
    text->column = col * 8 * 5;
    text->options = selected_type_g == i ?
      TEXT_OPTION_INVERTED : 0;
    text_str(text, get_battery_name(i));
    ++col;
    if (col == 3) {
      text->row += 2;
      col = 0;
    }
  }

  text->options = 0;

  if (green_pressed) {
    oledm_clear(text->display, 0);
    if (selected_type_g == BATTERY_TYPE_CUSTOM) {
       setup_state_g = CHOOSING_VOLTAGE;
    } else {
      save_settings();
      return TRUE; 
    }
  }

  return FALSE;
}

static bool_t choose_custom_voltage_ui(
    struct Text* text, bool_t red_pressed, bool_t green_pressed) {
  if (green_pressed) {
    switch (volt_edit_place_g) {
      case EDIT_TENS:
        volt_edit_place_g = EDIT_ONES;
        break;
      case EDIT_ONES:
        volt_edit_place_g = EDIT_POINT_ONES;
        break;
      case EDIT_POINT_ONES:
        save_settings();
        oledm_clear(text->display, 0);
        return TRUE;
    }
  }

  if (red_pressed) {
    switch (volt_edit_place_g) {
      case EDIT_TENS:
        custom_min_mv_g += 10000;
        if (custom_min_mv_g > MAX_SUPPORTED_BATTERY_MV) {
          custom_min_mv_g %= 10000;
        }
        break;
      case EDIT_ONES:
        if ((custom_min_mv_g / 1000 % 10) == 9) {
          custom_min_mv_g -= 9000;
        } else {
          custom_min_mv_g += 1000;
        }
        break;
      case EDIT_POINT_ONES:
        if ((custom_min_mv_g / 100 % 10) == 9) {
          custom_min_mv_g -= 900;
        } else {
          custom_min_mv_g += 100;
        }
        break;
    }
  }


  while (custom_min_mv_g > MAX_SUPPORTED_BATTERY_MV) {
    custom_min_mv_g -= 100;
  }
  while (custom_min_mv_g < MIN_SUPPORTED_BATTERY_MV) {
    custom_min_mv_g += 100;
  }

  button_captions(text, volt_edit_place_g == EDIT_POINT_ONES ? 'Y' : '>', '^');

  text->font = (struct FontASCII*)terminus8x16;
  text->row = 0;
  text->column = 8;
  text->options = 0;
  text_str(text, "Min Volts:");

  text->font = (struct FontASCII*)terminus16x32_numbers;
  text->row = 2;
  text->column = 32;
  text->options = volt_edit_place_g == EDIT_TENS ? TEXT_OPTION_INVERTED : 0;
  text_char(text, '0' + (custom_min_mv_g / 10000));
  text->options = volt_edit_place_g == EDIT_ONES ? TEXT_OPTION_INVERTED : 0;
  text_char(text, '0' + ((custom_min_mv_g / 1000) % 10));
  text->options = 0;
  text_char(text, '.');
  text->options = volt_edit_place_g == EDIT_POINT_ONES ? TEXT_OPTION_INVERTED : 0;
  text_char(text, '0' + ((custom_min_mv_g / 100) % 10));
  text->options = 0;

  return FALSE;
}

bool_t battery_setup_ui(
    struct Text* text,
    bool_t red_pressed,
    bool_t green_pressed,
    uint16_t battery_mv) {
  if (setup_state_g == CHOOSING_BATTERY_TYPE) {
    return choose_battery_ui(text, red_pressed, green_pressed, battery_mv);
  } else {
    return choose_custom_voltage_ui(text, red_pressed, green_pressed);
  }
}
