#ifndef LIB_UTIL_DEBOUNCE_H
#define LIB_UTIL_DEBOUNCE_H
// Simple debounce logic that works in an interrupt handler or
// poll configuration.  Will detect both edges and continuous holds.
//
// Poll example:
//
// void main(void) {
//    struct Debounce db;
//    debounce_init(&db, 8);
//    uint32_t time_ms = 0;
//    for (;; ++time_ms) {
//      // Read PB0
//      debounce_sample(&db, time_ms, PINB & 0x01);
//      // Mix and match edge_triggered and pressed to meet your needs.
//      if (db.edge_triggered && db.pressed) {
//         // reset state
//         db.edge_triggered = false;
//      }
//      _delay_ms(1);
//    }
// }
//
// Interrupt example
//
// struct Debounce db;
// void main(void) {
//    time_measure_init();
//    debounce_init(&db, 8);
//    // Setup pin change interrupt on PB0
//    GIMSK |= 1 << INT0;
//
//    while (1) {
//      lowpower_powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF); 
//      // Mix and match edge_triggered and pressed to meet your needs.
//      if (db.edge_triggered && db.pressed) {
//         // reset state with interrupts off
//         cli();
//         db.edge_triggered = false;
//         sei();
//         // do stuff here...
//      }
//    }
// }
//
// ISR(PCINT0_vect)
// {
//     debounce_sample(&db, time_measure_ms(), PINB & 0x01); 
// }

#include <error_codes.h>

struct Debounce {
  volatile bool_t edge_triggered;  // != 0 if edge triggered.  State will not change
                                   // until this is cleared 
  volatile bool_t pressed;         // Whether the button is pressed.

  // Internal vars
  uint8_t hold_time_ms_;  // How long to filter away debounce signals
  volatile uint32_t last_sample_time_ms_;  // the last time a sample was recorded
#ifdef __linux__
} __attribute__((packed));
#else
};
#endif

// Initialize
//
//  db: Debounce structure
//  hold_time_ms: The length of time to filter away debounce signals
void debounce_init(struct Debounce* db, uint32_t hold_time_ms);


// Take a sample.  Afterwords, db->edge_triggered and db->pressed will be available as
// needed.
//
//   db: Debounce structure
//   time_ms: A timetamp, in milliseconds
//   is_pressed: any non-zero value is considered "yes"
void debounce_sample(struct Debounce* db, uint32_t time_ms, uint8_t is_pressed);

// This version doesn't track pressed but only edge_triggered
// It is for the case where you want to know if a button was pressed but
// but do not need to explicitly track the current state of the button.
void debounce_sample_edge_only(struct Debounce* db, uint32_t time_ms);

#endif
