// Defines an error type and reserves error codes across all libraries

#ifndef ERROR_CODES_H
#define ERROR_CODES_H

#include <inttypes.h>

typedef uint8_t bool_t;
#define FALSE 0
#define TRUE 1

typedef uint8_t error_t;

// TWI Codes
#define TWI_NO_ACK_ERROR 0x01             // The slave did not acknowledge  all data
#define TWI_MISSING_START_CON_ERROR 0x02  // Generated Start Condition not detected on bus
#define TWI_MISSING_STOP_CON_ERROR 0x03   // Generated Stop Condition not detected on bus
#define TWI_ARB_LOST_ERROR 0x04           // Bus arbitration failed (try again?)
#define TWI_INTERNAL_ERROR 0x05           // The code ended up in an unexpected state

// SSD1306 Codes
#define SSD1306_BAD_FONT_ID_ERROR 0x10

#endif

