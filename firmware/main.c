// LED Lamp Project
// By: Matt Wachowski
//
// Designed for Atmega328p processor.

// General libs
#include <util/delay.h>

// String conversion
#include <pstr/pstr.h>

// INA260 power meter
#include <ina260/ina260.h>

// Needed for OLED display
#include <oledm/oledm.h>
#include <oledm/ssd1306_init.h>
#include <oledm/text.h>
#include <oledm/font/terminus8x16.h>

// Enter low power mode
#include <lowpower/lowpower.h>

// project-specific libraries
#include "awake_time.h"
#include "battery_confirm_dialog.h"
#include "battery_setup_ui.h"
#include "debug.h"
#include "eeprom_settings.h"
#include "fatal.h"
#include "input.h"
#include "led_control.h"
#include "status_display.h"

//
// I/O Connections:
//
// Also see:
//  input.c
//  led_control.c
//  fatal.c
//

//  A4 - I2C SDA connection for OLED / INA260 comms
//  A5 - I2C SCL connection for OLED / INA260 comms

//
// Other Settings
//

// How long to idle before turning off the display
#define STATUS_SHOW_TIME_MS 30000  

// How long to wait before entering low power mode
#define LOW_POWER_HOLDOFF_MS 500

// Add a deadzone to the current PWM value so that it's stable
#define PWM_DEADZONE 2

// Flags that determine what to power on and off
#define POWER_LED     0x01
#define POWER_DISPLAY 0x02
#define POWER_INPUT   0x04
#define POWER_INA260  0x08

// The address depends on the A0, A1 settings on the chip.  0x40
// is typical.
#define INA260_I2C_ADDRESS 0x40

// how long to stay in fast capture mode before turning to slow capture mode
#define INA260_FAST_SAMPLE_MS 250

//
// Global Data
//

// State of 128x64 OLED display
struct OLEDM display_g;
// Tracks current state of text (cursor position) on the OLED.
struct Text text_g;
// Tracks min/max range of the dimmer
struct DimmerConfig dimmer_config_g;
// Tracks current batery voltage
uint16_t battery_mv_g;
// The type of battery in use
uint8_t battery_type_g;
// minimum voltage of current battery
uint16_t battery_min_mv_g;
// number of battery cells (0 if custom)
uint8_t battery_cell_count_g;
// true if the battery was confirmed
bool_t battery_was_confirmed_g;
// sets the time when ina260 sampling show be changed from fast to slow
// if zero, then we are already at slow capture
uint32_t slow_sample_time_trigger_ms_g;

typedef enum states {
  STATE_ALL_OFF         = 0x00 |         0 |             0 |           0 |            0,
  STATE_BATTERY_CONFIRM = 0x10 |         0 | POWER_DISPLAY | POWER_INPUT | POWER_INA260,
  STATE_BATTERY_SETUP   = 0x20 |         0 | POWER_DISPLAY | POWER_INPUT | POWER_INA260,
  STATE_STATUS_ONLY     = 0x30 |         0 | POWER_DISPLAY | POWER_INPUT | POWER_INA260,
  STATE_LED_ONLY        = 0x40 | POWER_LED |             0 | POWER_INPUT | POWER_INA260,
  STATE_LED_WITH_STATUS = 0x50 | POWER_LED | POWER_DISPLAY | POWER_INPUT | POWER_INA260,
} states;

// Current state of lamp
states state_g;
// The last time the display was updated or a bottun/pot was used.
// note this setting is also used to keep lowpower mode awake for a bit
// so that the buttons can debounce.
uint32_t start_status_show_time_ms_g;
// last read PWM value.  Needed to support PWM_DEADZONE
uint8_t current_pwm_g;

// Coordinates communication with the INA260 current/voltage monitor
struct INA260 ina260_g;

//
// Implementation
//

// Asserts I2C device status.
static void check_i2c_errors(void) {
  if (display_g.error) {
    // The display update probably wont work...
    fatal(&text_g, "DISPLAY", u8_to_pshex(display_g.error));
  }
  if (ina260_g.error) {
    fatal(&text_g, "INA260", u8_to_pshex(ina260_g.error));
  }
}

// Sets the INA260 for either fast or slow sampling
static void config_ina260(bool_t fast_sample) {
  if (fast_sample) {
    ina260_write_configuration(
        &ina260_g,
        ina260_create_configuration(
          AVG_4_SAMPLES,
          CONVERSION_TIME_8244_US,
          CONVERSION_TIME_8244_US,
          OPERATING_MODE_CONTINUOUS_CURRENT_AND_VOLTAGE));
  } else {
    ina260_write_configuration(
        &ina260_g,
        ina260_create_configuration(
          AVG_128_SAMPLES,
          CONVERSION_TIME_8244_US,
          CONVERSION_TIME_8244_US,
          OPERATING_MODE_CONTINUOUS_CURRENT_AND_VOLTAGE));
  }
}

static bool_t update_led_brightness(bool_t use_deadzone) {
  uint8_t pwm = get_dimmer_value(&dimmer_config_g);
  if (pwm == current_pwm_g) {
    return FALSE;  // no change
  }

  // only apply deadzone if not at ends, otherwise we might not be able
  // to reach min and max due to deadzone overlapping them.
  if (use_deadzone &&
      pwm > dimmer_config_g.min_pwm &&
      pwm < dimmer_config_g.max_pwm) {
    if (pwm > current_pwm_g && (pwm - current_pwm_g) <= PWM_DEADZONE) {
      return FALSE;  // not enough change
    }
    if (pwm < current_pwm_g && (current_pwm_g - pwm) <= PWM_DEADZONE) {
      return FALSE;  // not enough change
    }
  }

  // enough of a change
  current_pwm_g = pwm;
  led_set_pwm(current_pwm_g);
  // reset show timer
  start_status_show_time_ms_g = awake_time_in_ms();

  // setup temporary fast sampling for better UI responsiveness
  if (!slow_sample_time_trigger_ms_g) {
    config_ina260(TRUE);
  }
  slow_sample_time_trigger_ms_g = start_status_show_time_ms_g + INA260_FAST_SAMPLE_MS;

  return TRUE;
}

static void change_state(states new_state) {
  DEBUG_STR("Chaange State: ");
  DEBUG_PSTRLN(u8_to_pshex(new_state));
  if ((new_state & POWER_LED) && !(state_g & POWER_LED)) {
    update_led_brightness(FALSE);
    led_power_on(current_pwm_g);
  } else if (!(new_state & POWER_LED) && (state_g & POWER_LED)) {
    led_power_off(current_pwm_g);
  }

  if ((new_state & POWER_INA260) && !(state_g & POWER_INA260)) {
    ina260_set_operating_mode(
        &ina260_g,
        OPERATING_MODE_CONTINUOUS_CURRENT_AND_VOLTAGE); 
  } else if (!(new_state & POWER_INA260) && (state_g & POWER_INA260)) {
    ina260_set_operating_mode(&ina260_g, OPERATING_MODE_POWER_DOWN2); 
  }

  if ((new_state & POWER_INPUT) && !(state_g & POWER_INPUT)) {
    input_wake();
  } else if (!(new_state & POWER_INPUT) && (state_g & POWER_INPUT)) {
    input_sleep();
  }

  if ((new_state & POWER_DISPLAY) && !(state_g & POWER_DISPLAY)) {
    oledm_display_on(&display_g);
  } else if (!(new_state & POWER_DISPLAY) && (state_g & POWER_DISPLAY)) {
    oledm_display_off(&display_g);
  }
  
  status_display_init();
  // Note: before changing this line, consider that the
  // variable is also used to track when to enter low power mode
  // in loop()
  start_status_show_time_ms_g = awake_time_in_ms();
  state_g = new_state;
}

// Enter battery confirmation menu
static void confirm_battery() {
  // This is called so that we are sure that the ina260 is powered up
  // for a reading.
  change_state(STATE_BATTERY_CONFIRM);
  battery_mv_g = ina260_read_voltage_in_mv(&ina260_g);
  struct EEPROMSettings* settings = eeprom_settings();
  battery_type_g = settings->battery_type;
  battery_cell_count_g = determine_battery_cell_count(battery_type_g, battery_mv_g);
  if (battery_type_g == BATTERY_TYPE_CUSTOM) {
    battery_min_mv_g = settings->custom_battery_min_mv;
  } else if (battery_cell_count_g > 0) {
    battery_min_mv_g =
      determine_min_battery_mv(battery_type_g, battery_cell_count_g);
  } else {
    battery_min_mv_g = 0;
  }

  if (battery_min_mv_g == 0) {
    battery_setup_init();
    change_state(STATE_BATTERY_SETUP);
  }
}

// Initialize hardware and ports
static void init(void) {
  awake_time_init();

  oledm_basic_init(&display_g);
  oledm_start(&display_g);
  oledm_clear(&display_g, 0x00);
  text_init(&text_g, terminus8x16, &display_g);
  DEBUG_STRLN("OLED Initialized");

  ina260_init(&ina260_g, INA260_I2C_ADDRESS);
  config_ina260(TRUE);
  slow_sample_time_trigger_ms_g = awake_time_in_ms() + INA260_FAST_SAMPLE_MS;
  DEBUG_STRLN("INA260 Initialized");
  check_i2c_errors();

  dimmer_config_g.min_pwm = 3;  // ~1%
  dimmer_config_g.max_pwm = 255;  // 100%
  input_init();
  DEBUG_STRLN("INPUT Initialized");

  battery_was_confirmed_g = FALSE;
  confirm_battery();
}


// Returns true iff it's time to turn off the display due to inactivity
static inline bool_t status_time_is_expired(void) {
  return (start_status_show_time_ms_g + STATUS_SHOW_TIME_MS) < awake_time_in_ms();
}

// Updates the OLED with current status
static void update_status(void) {
  status_display_render(
      &text_g,
      battery_mv_g,
      ina260_read_current_in_ma(&ina260_g),
      battery_min_mv_g,
      battery_type_g,
      battery_cell_count_g
  );
}

// STATE_ALL_OFF: Nothing is on, use as little power as possible.
static void state_all_off(bool_t red_pressed, bool_t green_pressed) {
  // Assume this is only called if one of the two buttons was pressed...
  if (!battery_was_confirmed_g) {
    confirm_battery();
  } else if (green_pressed) {
    change_state(STATE_LED_WITH_STATUS);
  } else if (red_pressed) {
    change_state(STATE_STATUS_ONLY);
  }
}

// STATE_BATTERY_CONFIRM
static void state_battery_confirm(bool_t red_pressed, bool_t green_pressed) {
  display_battery_confirm_dialog(
      &text_g,
      battery_mv_g,
      battery_type_g,
      battery_min_mv_g,
      battery_cell_count_g);
  if (green_pressed) {
    oledm_clear(&display_g, 0);
    battery_was_confirmed_g = TRUE;
    if (battery_mv_g > battery_min_mv_g) {
      change_state(STATE_LED_WITH_STATUS);
    } else {
      change_state(STATE_STATUS_ONLY);
    }
  } else if (red_pressed) {
    oledm_clear(&display_g, 0);
    battery_setup_init();
    change_state(STATE_BATTERY_SETUP);
  } else if (status_time_is_expired()) {
    change_state(STATE_ALL_OFF);
  }
}

static void state_battery_setup(bool_t red_pressed, bool_t green_pressed) {
  if (battery_setup_ui(&text_g, red_pressed, green_pressed, battery_mv_g)) {
    confirm_battery();
  } else if (status_time_is_expired()) {
    change_state(STATE_ALL_OFF);
  }
}

// STATE_STATUS_ONLY:
static void state_status_only(bool_t red_pressed, bool_t green_pressed) {
  update_status();

  if (green_pressed && (battery_mv_g >= battery_min_mv_g)) {
    change_state(STATE_LED_WITH_STATUS);
  } else if (red_pressed || status_time_is_expired()) {
    change_state(STATE_ALL_OFF);
  }
}

static void state_led_only(bool_t red_pressed, bool_t green_pressed) {
  const bool_t brightness_was_changed = update_led_brightness(TRUE);
  if (battery_mv_g < battery_min_mv_g) {
    change_state(STATE_STATUS_ONLY);
  } else if (green_pressed || brightness_was_changed) {
    change_state(STATE_LED_WITH_STATUS);
  } else if (red_pressed) {
    change_state(STATE_ALL_OFF);
  }
}

static void state_led_with_status(bool_t red_pressed, bool_t green_pressed) {
  update_led_brightness(TRUE);
  update_status();
  if (battery_mv_g < battery_min_mv_g) {
    change_state(STATE_STATUS_ONLY);
  } else if (green_pressed) {
    change_state(STATE_LED_ONLY);
  } else if (red_pressed) {
    change_state(STATE_ALL_OFF);
  }
}

static void loop(void) {
  check_i2c_errors();

  if (state_g & POWER_INA260) {
    if ((slow_sample_time_trigger_ms_g > 0) &&
        (awake_time_in_ms() > slow_sample_time_trigger_ms_g)) {
      // slow sampling for better reading stability
      config_ina260(FALSE); 
      slow_sample_time_trigger_ms_g = 0;
    }

    battery_mv_g = ina260_read_voltage_in_mv(&ina260_g);
  }

  const bool_t red_pressed = red_button_was_pressed();
  const bool_t green_pressed = green_button_was_pressed();

  if (red_pressed || green_pressed) {
    start_status_show_time_ms_g = awake_time_in_ms();
  }

  switch (state_g) {
    case STATE_ALL_OFF:
      state_all_off(red_pressed, green_pressed);
      break;
    case STATE_BATTERY_CONFIRM:
      state_battery_confirm(red_pressed, green_pressed);
      break;
    case STATE_BATTERY_SETUP:
      state_battery_setup(red_pressed, green_pressed);
      break;
    case STATE_STATUS_ONLY:
      state_status_only(red_pressed, green_pressed);
      break;
    case STATE_LED_ONLY:
      state_led_only(red_pressed, green_pressed);
      break;
    case STATE_LED_WITH_STATUS:
      state_led_with_status(red_pressed, green_pressed);
      break;
  }
}

void main(void) {
  DEBUG_INIT();
  init();
  while (1) {
    loop();
    if (state_g == STATE_ALL_OFF &&
        (awake_time_in_ms() > (start_status_show_time_ms_g + LOW_POWER_HOLDOFF_MS))) {
      // Waiting for keypresses
      DEBUG_STRLN("Power Down");
      lowpower_powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF);
    } else {
      DEBUG_STRLN("Power Idle");
      _delay_ms(30);
      /*  This is causing erratic behavior after working for a while
          - Seems ok on a breadboard
          - Seems ok when I add various delays around the command, but I don't know why
            and it's not stable across other changes.
          - Tried turning more stuff below "on" but it didn't seem to help.
      lowpower_idle(
          SLEEP_30MS,
          ADC_ON,
          TIMER2_OFF,
          TIMER1_ON,
          TIMER0_ON,
          SPI_OFF,
          USART0_ON,
          TWI_ON
      );
      */
    }
  }
}
