#include <ina260/ina260.h>
#include <twi/twi.h>


void ina260_init(struct INA260* device, uint8_t i2c_address) {
  twi_init();
  device->i2c_address = i2c_address;
  device->error = 0;
  device->voltage_bias_mv = 0;
  device->current_bias_ma = 2;
}

static uint16_t read_from_address(struct INA260* device, uint8_t address) {
  error_t* err = &device->error;
  uint8_t read_data[2] = {0, 0};
  twi_startWrite(device->i2c_address, err);
  twi_writeNoStop(address, err);
  twi_readWithStop(device->i2c_address, read_data, sizeof(read_data), err);
  return ((uint16_t)read_data[0] << 8) | read_data[1];
}

uint16_t ina260_read_configuration(struct INA260* device) {
  return read_from_address(device, 0);
}

uint16_t ina260_create_configuration(
    ina260_averaging_mode_t avg_mode,
    ina260_conversion_time_t voltage_conversion_time,
    ina260_conversion_time_t current_conversion_time,
    ina260_operating_mode_t operating_mode) {
  return (
      0x6000 |
      (avg_mode << 9) |
      (voltage_conversion_time << 6) |
      (current_conversion_time << 3) |
      operating_mode);
}

void ina260_write_configuration(
    struct INA260* device, uint16_t configuration_value) {
  error_t* err = &device->error;
  twi_startWrite(device->i2c_address, err);
  twi_writeNoStop(0, err);
  twi_writeNoStop(configuration_value >> 8, err);
  twi_writeNoStop(configuration_value & 0xFF, err);
  twi_stop(err);
}

void ina260_set_operating_mode(
    struct INA260* device,
    ina260_operating_mode_t operating_mode) {
  uint16_t config = ina260_read_configuration(device);
  config &= 0xFFF8;  // clear out mode bits
  config |= operating_mode;  // fill in operating mode
  ina260_write_configuration(device, config);
}

int16_t ina260_read_current_in_ma(struct INA260* device) {
  const int32_t current = (int16_t)read_from_address(device, 1) + device->current_bias_ma;
  return (int16_t)(current * 125 / 100);
}

uint16_t ina260_read_voltage_in_mv(struct INA260* device) {
  const uint32_t voltage = read_from_address(device, 2) + device->voltage_bias_mv;
  return (uint16_t)(voltage * 125 / 100);
}

uint16_t ina260_read_power_in_mw(struct INA260* device) {
  const uint16_t power = read_from_address(device, 3);
  return power * 10;
}


