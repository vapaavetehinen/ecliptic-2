#pragma once
#include <pebble.h>

typedef struct {
  bool bg_white;            // black-and-white displays only
  bool vibrate_hourly;
  bool vibrate_disconnect;
  bool flip_horizontal;
  bool stars_by_brightness; // colour displays only
  bool edge_to_edge;        // Pebble Time 2: planets may touch the screen edge
} Settings;

extern Settings g_settings;

// Loads persisted settings and starts listening for changes from the phone.
// on_change is called after any setting is updated.
void settings_init(void (*on_change)(void));
