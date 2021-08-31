#ifndef COMMON_UI_H
#define COMMON_UI_H

#include <oledm/text.h>
#include <inttypes.h>

// Renders MV as 12.1 (or similar)
void display_voltage_common(struct Text* text, uint16_t volts_mv);

// Displays 2 letter captions on the right border by the buttons
void button_captions(struct Text* text, char green_caption, char red_caption);
#endif

