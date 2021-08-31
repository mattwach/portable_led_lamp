# Assumes that probrams are nested two deep.  Ifg this is not true,
# the caller can override this var before including this file.
ROOT ?= ../..
ROOT_LIB ?= $(ROOT)/lib

OS=$(shell uname)
ifeq ($(OS),Darwin)
  ARDUINO ?= /Users/mattwach/Library/Arduino15/packages/arduino

  AVR_PATH ?= $(ARDUINO)/tools/avr-gcc/4.9.2-atmel3.5.4-arduino2/bin
  AVR_DUDE ?= $(ARDUINO)/tools/avrdude/6.3.0-arduino9/bin/avrdude
  AVR_DUDE_CONF ?= $(ARDUINO)/tools/avrdude/6.3.0-arduino9/etc/avrdude.conf
  SERIAL_DEV ?= $(shell ls /dev/cu.wchusbserial* | head -1)
endif

ifneq (,$(wildcard /usr/bin/avr-gcc))
 AVR_PATH ?= /usr/bin
 AVR_DUDE ?= /usr/bin/avrdude
 AVR_DUDE_CONF ?= $(ROOT)/avrdude.conf
 SERIAL_DEV ?= $(shell ls /dev/ttyUSB* | head -1)
else
  ARDUINO ?= ${HOME}/arduino-1.8.12
 AVR_PATH ?= $(ARDUINO)/hardware/tools/avr/bin
 AVR_DUDE ?= $(AVR_PATH)/avrdude
 AVR_DUDE_CONF ?= $(ARDUINO)/hardware/tools/avr/etc/avrdude.conf
 SERIAL_DEV ?= $(shell ls /dev/ttyUSB* | head -1)
endif

MCU ?= atmega328p

ifeq ($(MCU),attiny85)
  PROGRAMMER ?= stk500v1
  BAUD ?= 19200
		DISABLE_AUTO_ERASE ?=
  F_CPU ?= 1000000
else
  PROGRAMMER ?= arduino
  BAUD ?= 57600
		DISABLE_AUTO_ERASE ?= -D
  F_CPU ?= 16000000
endif

FLAGS ?= \
  -mmcu=$(MCU) \
  -g \
  -Os \
  -fdata-sections \
  -ffunction-sections \
  -std=c11 \

CFLAGS ?= \
  $(FLAGS) \
  -c \
  -DF_CPU=$(F_CPU) \

LFLAGS ?= \
  $(FLAGS) \
  -Wl,--gc-sections \

INCLUDES ?= \
  -I$(ROOT_LIB)


FILES ?= \
  main.o
