#ifndef BATTERY_CONFIRM_DIALOG
#define BATTERY_CONFIRM_DIALOG

#include <inttypes.h>
#include <oledm/text.h>

#include "battery_types.h"

void display_battery_confirm_dialog(
    struct Text* text,
    uint16_t mv,
    battery_type type,
    uint16_t min_mv,
    uint8_t cell_count);

#endif

