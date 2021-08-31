#include "oledm.h"

#include <avr/pgmspace.h>
#include <string.h>
#include <error_codes.h>
#include "oledm_driver_common.inc"

enum ESsd1306Commands
{
    SSD1306_SETLOWCOLUMN     = 0x00,
    SSD1306_SETHIGHCOLUMN    = 0x10,
    SSD1306_MEMORYMODE       = 0x20,
    SSD1306_COLUMNADDR       = 0x21,
    SSD1306_PAGEADDR         = 0x22,
    SSD1306_DEACTIVATE_SCROLL = 0x2E,
    SSD1306_SETSTARTLINE     = 0x40,
    SSD1306_DEFAULT_ADDRESS  = 0x78,
    SSD1306_SETCONTRAST      = 0x81,
    SSD1306_CHARGEPUMP       = 0x8D,
    SSD1306_SEGREMAP         = 0xA0,
    SSD1306_DISPLAYALLON_RESUME = 0xA4,
    SSD1306_DISPLAYALLON     = 0xA5,
    SSD1306_NORMALDISPLAY    = 0xA6,
    SSD1306_INVERTDISPLAY    = 0xA7,
    SSD1306_SETMULTIPLEX     = 0xA8,
    SSD1306_DISPLAYOFF       = 0xAE,
    SSD1306_DISPLAYON        = 0xAF,
    SSD1306_SETPAGE          = 0xB0,
    SSD1306_COMSCANINC       = 0xC0,
    SSD1306_COMSCANDEC       = 0xC8,
    SSD1306_SETDISPLAYOFFSET = 0xD3,
    SSD1306_SETDISPLAYCLOCKDIV = 0xD5,
    SSD1306_SETPRECHARGE     = 0xD9,
    SSD1306_SETCOMPINS       = 0xDA,
    SSD1306_SETVCOMDETECT    = 0xDB,

    SSD1306_SWITCHCAPVCC     = 0x02,
    SSD1306_NOP              = 0xE3,
    SSD1306_DELAY            = 0xE3,
};

enum ESsd1306MemoryMode 
{   
    HORIZONTAL_ADDRESSING_MODE  = 0x00,
    VERTICAL_ADDRESSING_MODE    = 0x01,
    PAGE_ADDRESSING_MODE        = 0x02,
};  

// Memory rows.  Even if using a smaller screen, like 64x32, the SSD1306 chip
// still has an internal memory of 128x64.  This is important in some cases,
// such as while scrolling.
#define DISPLAY_MEMORY_ROWS 8
#define DISPLAY_MEMORY_COLUMNS 128

void oledm_basic_init(struct OLEDM* display) {
  memset(display, 0, sizeof(struct OLEDM));
  display->visible_columns = DISPLAY_MEMORY_COLUMNS;
  display->visible_rows = DISPLAY_MEMORY_ROWS;
  display->memory_columns = DISPLAY_MEMORY_COLUMNS;
  display->memory_rows = DISPLAY_MEMORY_ROWS;
  display->option_bits = OLEDM_ALTERNATE_COM_CONFIG;
}

void oledm_start(struct OLEDM* display) {
  oledm_ifaceInit();

  error_t* err = &(display->error);

  oledm_startCommands(err);
  oledm_command(SSD1306_DISPLAYOFF, err);

  // Horizontal addressing mode.  This makes it easier to
  // update the entire display.
  oledm_command(SSD1306_MEMORYMODE, err);
  oledm_command(0x00, err);

  // For proper up to down display, COMSCAN
  // needs to be inverted from default
  oledm_command(SSD1306_COMSCANDEC, err);

  oledm_command(SSD1306_SETSTARTLINE | display->start_line, err);

  // Default: SSD1306_SETCONTRAST, 0x7F

  // For proper left-to-right display, thh SSD1306_SEGREMAP
  // needs to be revered from default.
  oledm_command(SSD1306_SEGREMAP | 0x01, err);
   
  //SSD1306_NORMALDISPLAY
  //SSD1306_SETMULTIPLEX, 63,
  //SSD1306_SETDISPLAYOFFSET, 0x00
  //SSD1306_SETDISPLAYCLOCKDIV, 0x80,
  //SSD1306_SETPRECHARGE, 0x22

  //SSD1306_SETCOMPINS, 0x22,  128x32 display
  uint8_t compins = 0x02;
  if (display->option_bits & OLEDM_ALTERNATE_COM_CONFIG) {
    compins |= 1 << 4;
  }
  if (display->option_bits & OLEDM_COM_LEFT_RIGHT_REMAP) {
    compins |= 1 << 5;
  }
  oledm_command(SSD1306_SETCOMPINS, err);
  oledm_command(compins, err);

  // Default: SSD1306_SETVCOMDETECT, 0x20

  // Enable charge pump
  oledm_command(SSD1306_CHARGEPUMP, err);
  oledm_command(0x14, err); 

  // Turn the display back on
  oledm_command(SSD1306_DISPLAYALLON_RESUME, err);
  oledm_command(SSD1306_DISPLAYON, err);
  oledm_stop(display);
}

void oledm_setMemoryBounds(
    struct OLEDM* display,
    uint8_t left_column,
    uint8_t top_row,
    uint8_t right_column,
    uint8_t bottom_row) {

  error_t* err = &(display->error);

  oledm_startCommands(err);
  oledm_command(SSD1306_COLUMNADDR, err);
  oledm_command(left_column & (DISPLAY_MEMORY_COLUMNS - 1), err);
  oledm_command(right_column & (DISPLAY_MEMORY_COLUMNS - 1), err);

  oledm_command(SSD1306_PAGEADDR, err);
  oledm_command(top_row & (DISPLAY_MEMORY_ROWS - 1), err);
  oledm_command(bottom_row & (DISPLAY_MEMORY_ROWS - 1), err);
  oledm_stop(display);
}

void oledm_writeDataNoStop(struct OLEDM* display, uint8_t byte){ 
  oledm_ifaceWriteData(byte, &(display->error));
}

void oledm_vscroll(struct OLEDM* display, int8_t rows) {
  
  // Even though there may be less than display->visible_rows visible on
  // a smaller display, the memory still supports 8 rows.  The logic
  // needs to respect this fact or you'll end up showing regions
  // that can not be updated.
  display->start_line = (display->start_line + rows) & (DISPLAY_MEMORY_ROWS - 1);

  error_t* err = &(display->error);
  oledm_startCommands(err);
  oledm_command(SSD1306_SETSTARTLINE | (display->start_line * 8), err);
  oledm_stop(display);
}

void oledm_clear(struct OLEDM* display, uint8_t byte) {
  if (display->error) {
    return;
  }

  oledm_setMemoryBounds(
    display,
    0,
    0,
    DISPLAY_MEMORY_COLUMNS - 1,
    DISPLAY_MEMORY_ROWS - 1);

  error_t* err = &(display->error);
  uint16_t idx = 0;
  const uint16_t end = DISPLAY_MEMORY_COLUMNS * DISPLAY_MEMORY_ROWS;
  
  oledm_startData(display);
  for (; idx < end; ++idx) {
    oledm_ifaceWriteData(byte, &(display->error));
  }
  oledm_stop(display);
}

void oledm_display_off(struct OLEDM* display) {
  error_t* err = &(display->error);
  oledm_startCommands(err);
  oledm_command(SSD1306_DISPLAYOFF, err);
  oledm_stop(display);
}

void oledm_display_on(struct OLEDM* display) {
  error_t* err = &(display->error);
  oledm_startCommands(err);
  oledm_command(SSD1306_DISPLAYON, err);
  oledm_stop(display);
}
