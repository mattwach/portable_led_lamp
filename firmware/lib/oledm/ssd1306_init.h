#ifndef SSD1306_INIT_H
#define SSD1306_INIT_H
// Init routines for some known displays
//
// These can be used in place of oledm_init()

#include "oledm.h"

void ssd1306_64x32_a_init(struct OLEDM* display);

void ssd1306_128x32_a_init(struct OLEDM* display);

#endif
