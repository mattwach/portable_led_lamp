#ifndef FATAL_H
#define FATAL_H

#include <inttypes.h>
#include <oledm/text.h>

// Called when something unexpected happens.  Clears
// The display, shows the given message and freezes
// Both msg and pstr are optional - you can pass a NULL (0),
// to omit them.
void fatal(struct Text* text, const char* msg, const uint8_t* pstr);

#endif
