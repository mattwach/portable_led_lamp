#ifndef INA260_INA260_H
#define INA260_INA260_H
// Communication interface with INA260

#include <inttypes.h>
#include <error_codes.h>

typedef enum ina260_averaging_mode {
  AVG_1_SAMPLE = 0,
  AVG_4_SAMPLES = 1,
  AVG_16_SAMPLES = 2,
  AVG_64_SAMPLES = 3,
  AVG_128_SAMPLES = 4,
  AVG_256_SAMPLES = 5,
  AVG_512_SAMPLES = 6,
  AVG_1024_SAMPLES = 7
} ina260_averaging_mode_t;

typedef enum ina260_conversion_time {
  CONVERSION_TIME_140_US = 0,
  CONVERSION_TIME_204_US = 1,
  CONVERSION_TIME_332_US = 2,
  CONVERSION_TIME_588_US = 3,
  CONVERSION_TIME_1100_US = 4,
  CONVERSION_TIME_2116_US = 5,
  CONVERSION_TIME_4156_US = 6,
  CONVERSION_TIME_8244_US = 7
} ina260_conversion_time_t;

typedef enum ina260_operating_mode {
  OPERATING_MODE_POWER_DOWN = 0,
  OPERATING_MODE_TRIGGERED_CURRENT = 1,
  OPERATING_MODE_TRIGGERED_VOLTAGE = 2,
  OPERATING_MODE_TRIGGERED_CURRENT_AND_VOLTAGE = 3,
  OPERATING_MODE_POWER_DOWN2 = 4,
  OPERATING_MODE_CONTINUOUS_CURRENT = 5,
  OPERATING_MODE_CONTINUOUS_VOLTAGE = 6,
  OPERATING_MODE_CONTINUOUS_CURRENT_AND_VOLTAGE = 7
} ina260_operating_mode_t;

struct INA260 {
  // The i2c address of the device
  uint8_t i2c_address;

  // Error status.  Check this when convienent.  If it's not zero,
  // commands other than ina260_init() will return immediately without
  // doing anything.  This allows a more casual error-checking strategy at
  // the client-level (less code).
  error_t error;
  
  // these biases can be set to calibrate against measured values
  int16_t voltage_bias_mv;
  int16_t current_bias_ma;
};

// Initialize data structure.  Note that 0x40-0x4F are possible devic addresses,
// depending on how the A0 and A1 pins on the INA260 are biased (comon config is
// GND, GND -> 0x40, see the datasheet for more detaiuls).
void ina260_init(struct INA260* device, uint8_t i2c_address);

// returns the configuration register value.  Use the associated decode
// functions to decode bits.
uint16_t ina260_read_configuration(struct INA260* device);
static inline ina260_averaging_mode_t ina260_parse_averaging_mode(uint16_t configuration_value) {
  return (configuration_value >> 9) & 0x07;
}
static inline ina260_conversion_time_t ina260_parse_voltage_conversion_time(uint16_t configuration_value) {
  return (configuration_value >> 6) & 0x07;
}
static inline ina260_conversion_time_t ina260_parse_current_conversion_time(uint16_t configuration_value) {
  return (configuration_value >> 3) & 0x07;
}
static inline ina260_operating_mode_t ina260_parse_operating_mode(uint16_t configuration_value) {
  return configuration_value & 0x07;
}

uint16_t ina260_create_configuration(
    ina260_averaging_mode_t avg_mode,
    ina260_conversion_time_t voltage_conversion_time,
    ina260_conversion_time_t current_conversion_time,
    ina260_operating_mode_t operating_mode);

// Writes the configuration register.
void ina260_write_configuration(struct INA260* device, uint16_t configuration_value);

// Special configuration that just sets operating mode.  Useful for powerup/down
void ina260_set_operating_mode(struct INA260* device, ina260_operating_mode_t operating_mode);

// read current. Note that the LSB is actuall 1.25 Ma so down expect accuracy down
// to 1 ma
int16_t ina260_read_current_in_ma(struct INA260* device);

// read voltage.   Accuracy is 1/25 mV max.
uint16_t ina260_read_voltage_in_mv(struct INA260* device);

// read power.  Accuracy is 10mV max.
uint16_t ina260_read_power_in_mw(struct INA260* device);


#endif

