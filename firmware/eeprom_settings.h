#ifndef EEPROM_SETTINGS_H
#define EEPROM_SETTINGS_H

#include <inttypes.h>

struct EEPROMSettings {
  uint32_t signature;  // Used to detect if the EEPROM was ever written
  uint8_t version;  // Used for conversions, if desired
  uint8_t battery_type;  // The type of battery in use
  uint16_t custom_battery_min_mv;  // Minimum voltage only used for custom batteries
};

// Allows for reading and changing of settings
struct EEPROMSettings* eeprom_settings(void);

// writes current settings
void write_eeprom_settings();

#endif
