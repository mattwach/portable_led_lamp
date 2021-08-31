#ifndef STATUS_DISPLAY_H
#define STATUS_DISPLAY_H

#include "battery_types.h"
#include <oledm/text.h>

// resets display-caching state
void status_display_init(void);

// Renders the status display
void status_display_render(
    struct Text* text,
    uint16_t battery_mv,
    uint16_t battery_ma,
    uint16_t minimum_mv,
    battery_type batt_type,
    uint8_t cell_count
);

#endif

