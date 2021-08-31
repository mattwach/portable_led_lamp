#include "debounce.h"

void debounce_init(struct Debounce* db, uint32_t hold_time_ms) {
  db->edge_triggered = FALSE;
  db->pressed = FALSE;
  db->hold_time_ms_ = hold_time_ms;
  db->last_sample_time_ms_ = 0;
}

void debounce_sample(struct Debounce* db, uint32_t time_ms, uint8_t is_pressed) {
  if (db->edge_triggered) {
    // Wait for this to be cleared before doing anything
    return;
  }

  if ((time_ms > db->last_sample_time_ms_) &&
      (db->last_sample_time_ms_ + db->hold_time_ms_) > time_ms) {
    // It's too soon to change the value
    return;
  }

  const uint8_t previous_is_pressed = db->pressed;
  db->pressed = is_pressed;
  if (previous_is_pressed != is_pressed) {
    // We only update the sample time on an edge trigger condition
    // because otherwise every sample would push the debounce out
    // a bit which could cause some legitimate signals to be filtered.
    db->last_sample_time_ms_ = time_ms;
    db->edge_triggered = TRUE;
  }
}

void debounce_sample_edge_only(struct Debounce* db, uint32_t time_ms) {
  if (db->edge_triggered) {
    // Wait for this to be cleared before doing anything
    return;
  }

  if ((time_ms > db->last_sample_time_ms_) &&
      (db->last_sample_time_ms_ + db->hold_time_ms_) > time_ms) {
    // It's too soon to change the value
    return;
  }

  db->last_sample_time_ms_ = time_ms;
  db->edge_triggered = TRUE;
}
