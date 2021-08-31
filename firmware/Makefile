PROJECT_NAME=led_lamp
MCU?=atmega328p
# 3.3V Mini Pro runs at 8Mhz
F_CPU=8000000

ROOT=.
include $(ROOT)/vars.mak

# Add $(ROOT_LIB)/spi/spi.o below if using an OLED that is
# SPI-based.
FILES := \
  awake_time.o \
  battery_confirm_dialog.o \
  battery_setup_ui.o \
  battery_types.o \
  common_ui.o \
  debug.o \
  eeprom_settings.o \
  fatal.o \
  input.o \
  led_control.o \
  main.o \
  status_display.o \
  $(ROOT_LIB)/ina260/ina260.o \
  $(ROOT_LIB)/lowpower/lowpower.o \
  $(ROOT_LIB)/misc/debounce.o \
  $(ROOT_LIB)/oledm/ssd1306.o \
  $(ROOT_LIB)/oledm/oledm_i2c.o \
  $(ROOT_LIB)/oledm/text.o \
  $(ROOT_LIB)/oledm/font/terminus8x16.o \
  $(ROOT_LIB)/oledm/font/terminus16x32_numbers.o \
  $(ROOT_LIB)/oledm/ssd1306_init.o \
  $(ROOT_LIB)/pstr/pstr.o \
  $(ROOT_LIB)/twi/twi.o \

include $(ROOT)/rules.mak

