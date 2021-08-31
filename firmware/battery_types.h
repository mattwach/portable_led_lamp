#ifndef BATTERY_TYPES_H
#define BATTERY_TYPES_H

#include <inttypes.h>

// Note: Inserting / deleting battery types can shift the enum relative
// to stored EEPROM settings (appending is OK).
// If you must insert/delete, increment the VERSION field in eeprom_settings.c
typedef enum battery_type {
  BATTERY_TYPE_UNKNOWN,
  BATTERY_TYPE_CUSTOM,
  BATTERY_TYPE_LIPO,
  BATTERY_TYPE_LION,
  BATTERY_TYPE_LIFE,
  BATTERY_TYPE_LIHV,
  BATTERY_TYPE_NICAD,
  BATTERY_TYPE_PB,
  BATTERY_TYPE_ALKALINE,
  BATTERY_TYPE_NIMH,
} battery_type;

#define BATTERY_TYPE_COUNT 10
// maximum voltage under which battery ranges are confirmed
// as not-overlapping
#define MAX_SUPPORTED_BATTERY_MV 30000
// The minimum supported voltage by the system
#define MIN_SUPPORTED_BATTERY_MV 6000

// Retrns a string name for a battery.
// All battery names have the same length
#define BATTERY_NAME_LENGTH 4
const char* get_battery_name(battery_type type);

// Tries to determine the cell count.  Returns zero of the cell count can not be determined,
// including if a custom battery was specified.
uint8_t determine_battery_cell_count(
    battery_type type,
    uint16_t current_voltage_mv);

// Determines the low voltage cutoff.  with custom batteries, this just returns 0
// which is not intended to be used.
uint16_t determine_min_battery_mv(
    battery_type type,
    uint8_t battery_cell_count);

#endif

