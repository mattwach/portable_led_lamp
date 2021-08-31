#include "eeprom_settings.h"

#include <avr/eeprom.h>

#include "battery_types.h"

struct EEPROMSettings settings;

#define EEPROM_ADDRESS 0
#define SIGNATURE 0x1ED5E771
#define VERSION 1

static void init_eeprom_settings(void) {
  settings.signature = SIGNATURE;
  settings.version = VERSION;
  settings.battery_type = BATTERY_TYPE_UNKNOWN;
  settings.custom_battery_min_mv = MAX_SUPPORTED_BATTERY_MV;
}

static void read_eeprom_settings(void) {
  eeprom_read_block(&settings, EEPROM_ADDRESS, sizeof(settings));
  if (settings.signature != SIGNATURE || settings.version != VERSION) {
    init_eeprom_settings();
  }
}

struct EEPROMSettings* eeprom_settings(void) {
  if (settings.signature != SIGNATURE || settings.version != VERSION) {
    read_eeprom_settings();
  }
  return &settings;
}

void write_eeprom_settings() {
  eeprom_update_block(&settings, EEPROM_ADDRESS, sizeof(settings));
}
