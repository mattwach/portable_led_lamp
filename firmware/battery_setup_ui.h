#ifndef BATTERY_SETUP_UI
#define BATTERY_SETUP_UI

#include <error_codes.h>
#include <oledm/text.h>

#include "battery_types.h"

// Call to (re)initialize the state
void battery_setup_init(void);

// Returns true when the UI has completed.
bool_t battery_setup_ui(
    struct Text* text,
    bool_t red_pressed,
    bool_t green_pressed,
    uint16_t battery_mv);

#endif

