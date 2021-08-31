#include "battery_types.h"

struct BatteryProp {
  // Name of battery
  char name[BATTERY_NAME_LENGTH + 1];
  // minimum supported cell voltage
  uint16_t min_cell_mv;
  // minimum voltage used for cell detection
  uint16_t min_detect_cell_mv;
};

// I used tools/calc_cell_ranges.py to determine the min_detect_cell_mv values.
// e.g: ./calc_cell_ranges.py 4200
static const struct BatteryProp battery_prop[BATTERY_TYPE_COUNT] = {
  { .name = "Unkn", .min_cell_mv = 0, .min_detect_cell_mv = 0 },        // Should not be needed
  { .name = "Cust", .min_cell_mv = 0, .min_detect_cell_mv = 0 },        // User specified
  { .name = "LiPo", .min_cell_mv = 3500, .min_detect_cell_mv = 3680 },  // max_cell_mv = 4200
  { .name = "LiOn", .min_cell_mv = 3500, .min_detect_cell_mv = 3760 },  // max_cell_mv = 4300
  { .name = "LiFe", .min_cell_mv = 2500, .min_detect_cell_mv = 3210 },  // max_cell_mv = 3600
  { .name = "LiHv", .min_cell_mv = 3500, .min_detect_cell_mv = 3760 },  // max_cell_mv = 4300
  { .name = "NiCd", .min_cell_mv = 1100, .min_detect_cell_mv = 1370 },  // max_cell_mv = 1500
  { .name = "  Pb", .min_cell_mv = 1910, .min_detect_cell_mv = 1910 },  // max_cell_mv = 2100
  { .name = "Alkn", .min_cell_mv = 1000, .min_detect_cell_mv = 1370 },  // max_cell_mv = 1500
  { .name = "NiMh", .min_cell_mv = 1100, .min_detect_cell_mv = 1190 },  // max_cell_mv = 1300
};

const char* get_battery_name(battery_type type) {
  return battery_prop[type].name;
}

uint8_t determine_battery_cell_count(
    battery_type type, uint16_t current_voltage_mv) {
  if (type == BATTERY_TYPE_CUSTOM || type == BATTERY_TYPE_UNKNOWN) {
    return 0;
  }

  if (current_voltage_mv > MAX_SUPPORTED_BATTERY_MV) {
    return 0;
  }
  return current_voltage_mv / battery_prop[type].min_detect_cell_mv;
}

uint16_t determine_min_battery_mv(
    battery_type type, uint8_t battery_cell_count) {
  if (type == BATTERY_TYPE_CUSTOM || type == BATTERY_TYPE_UNKNOWN) {
    return 0;
  }
  const uint16_t min_mv = battery_cell_count * battery_prop[type].min_cell_mv;
  if (min_mv < MIN_SUPPORTED_BATTERY_MV) {
    return MIN_SUPPORTED_BATTERY_MV;
  }
  if (min_mv > MAX_SUPPORTED_BATTERY_MV) {
    return 0;
  }

  return min_mv;
}
