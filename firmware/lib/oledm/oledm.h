#ifndef OLEDM_OLEDM_H
#define OLEDM_OLEDM_H

#include <inttypes.h>
#include <error_codes.h>

// SSD1306 Driver
//
// Low-level interface
// Supports ATMega328P and ATTiny85
// Supports I2C
//
// General usage:
//
// void main() {
//   SSSD1306 display;
//
//   oledm_init(&display);
//   oledm_clear(0);
//
//   // Lets draw a filled rect
//   const uint8_t x = 10;
//   const uint8_t y = 1;
//   const uint8_t w = 30;
//   const uint8_t h = 2;  // 2*8 = 16 pixels
//   oledm_setBounds(&display, x, y, x+w, y+h);
//   uint8_t i=0;
//   const uint8_t count = w * h;
//   oledm_startData(&display);
//   for (; i < count; ++i) {
//     oledm_writeDataNoStop(&display, 0xFF);  // 8 white vertical pixels
//   }
//   oledm_stop(&display);
//
//   if (display.error) {
//     // handle error as needed
//   }
// }

typedef uint8_t bool_t;

// State object
struct OLEDM {
  // Error status.  Check this when convienent.  If it's not zero,
  // commands other than oledm_init() will return immediately without
  // doing anything.  This allows a more casual error-checking strategy at
  // the client-level (less code).
  error_t error;

  // Visible display dimensions.  Some libraries might need these for fitting
  // information, such as where to word-wrap.
  uint8_t visible_columns;
  uint8_t visible_rows;

  // Memory display dimensions.  Libraries that might want to render
  // off-screen may need these
  uint8_t memory_columns;
  uint8_t memory_rows;

  // Offset values, used to correctly center smaller displays.
  // For example, if using a 64x32 display, you might need an
  // column offset of 64 and a row offset of 32.  Or maybe not.
  // It depends on what part of memory the manufacturer chose
  // to connect to live pixels.  Just try different values and
  // see.
  uint8_t column_offset;
  uint8_t row_offset;

  // current startline.  Somewhere between zero (init) and
  // DISPLAY_MEMORY_ROWS - 1.  This value is used to implement scrolling
  // but could also be used for double buffering on small displays
  // For example, on a 64x32 a oledm_vscroll(4) would have the
  // practical effect of a buffer-swap.  You just purposely set the
  // row_offset to the off-screen data for this case.
  uint8_t start_line;

  // Option bits.  These are needed to change the initialization
  // sequence that certain displays need
  uint8_t option_bits;
};

// Option bit definitions
#define OLEDM_COM_LEFT_RIGHT_REMAP 0x01  // SETCOMPINS bit 5
#define OLEDM_ALTERNATE_COM_CONFIG 0x02  // SETCOMPINS bit 4

// Initializes the display structure and display.  In a generic way.
//  The process is:
//    OLEDM display;
//    oledm_basic_init(&display);
//    display.visible_columns = 64;  // Optional, depending on prefrence
//    oledm_start(&display);
//
//  Alternately, choose an init from ssd1306_configurations.h or make
//  a custom one...
//    OLEDM display;
//    oledm_init_62x32_a(&display);
//    oledm_start(&display);
//
// Args:
//  display: object to initialize
void oledm_basic_init(struct OLEDM* display);

// Send initial commands that initializes and turns on the display
void oledm_start(struct OLEDM* display);

// Set the display data window.  Written bytes will start at the upper
// right of this window and wrap to the left (similar to words on a page)
//
// Args:
//  display: initialized display object.
//  left_column: Column from 0-(DISPLAY_MEMORY_COLUMNS-1).  Zero is the leftmost column.
//  right_column: Column where wrapping will begin
//  top_row: row number from 0-(DISPLAY_MEMORY_ROWS-1).  Each row has
//    8 vertical pixels where the LSB is toward the top.  A zero value
//    is used to indicate the top of the display.
//  bottom_row: after this row, the data will wrap to the top right
//
// setBounds applies the current start_line and row/column offset.  This
// acounts for coordinate shifts on smaller displays (64x32) and
// when using scrolling.
//
// setMemoryBounds does not account for these things.  It's useful when
// these adjustments are a hinderance, such as when you want to change
// non-visible data without changing the start_line or if it want all
// data changed, whether visible or not.
void oledm_setBounds(struct OLEDM* display,
  uint8_t left_column, uint8_t top_row,
  uint8_t right_column, uint8_t bottom_row);
void oledm_setMemoryBounds(struct OLEDM* display,
  uint8_t left_column, uint8_t top_row,
  uint8_t right_column, uint8_t bottom_row);

// Commands to turn off the display and turn it back on.
// Note that oledm_start already turns on the display so there
// is no need to call oledm_display_on() unless oledm_display_off()
// was called first.
void oledm_display_off(struct OLEDM* display);
void oledm_display_on(struct OLEDM* display);

// Used to scroll the screen vertically up or down by a number of
// rows. Sends a SET_STARTLINE command, along with setting the start
// line in the buffer so that future writes happen where you would
// expect. Note that the hardware supports per-pixel scrolling, but
// this library can not support setBounds this way, so we stick with
// multiple of 8.
void oledm_vscroll(struct OLEDM* display, int8_t rows);

// Clear the entire screen (include non-visible parts on smaller displays)
void oledm_clear(struct OLEDM* display, uint8_t byte);

// Start providing data
void oledm_startData(struct OLEDM* display);
// Stop providing data
void oledm_stop(struct OLEDM* display);

// Provide data bytes
void oledm_writeDataNoStop(struct OLEDM* display, uint8_t byte);
static inline void oledm_writeDataWithStop(struct OLEDM* display, uint8_t byte) {
  oledm_writeDataNoStop(display, byte);
  oledm_stop(display);
}


#endif
