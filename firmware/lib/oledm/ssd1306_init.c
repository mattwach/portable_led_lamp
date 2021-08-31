#include "ssd1306_init.h"

#include <string.h>

void ssd1306_64x32_a_init(struct OLEDM* display) {
  memset(display, 0, sizeof(struct OLEDM));
  display->visible_columns = 64;
  display->visible_rows = 4;
  display->memory_columns = 128;
  display->memory_rows = 8;
  display->column_offset = 32;
  display->row_offset = 4;
  display->option_bits = OLEDM_ALTERNATE_COM_CONFIG;
}

void ssd1306_128x32_a_init(struct OLEDM* display) {
  memset(display, 0, sizeof(struct OLEDM));
  display->visible_columns = 128;
  display->visible_rows = 4;
  display->memory_columns = 128;
  display->memory_rows = 8;
  display->option_bits = OLEDM_COM_LEFT_RIGHT_REMAP;
}