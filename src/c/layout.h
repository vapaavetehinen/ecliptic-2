#pragma once
#include <pebble.h>

// Every screen-dependent number lives here. Nothing else hard-codes pixels.
typedef struct {
  // Sky
  GPoint center;        // centre of the ecliptic circle
  int16_t radius;       // ecliptic circle radius
  int16_t lat_scale;    // pixels per degree of ecliptic latitude, x 256
  int16_t sun_r;
  int16_t planet_r;
  int16_t moon_r;
  bool large_glyphs;    // 13 px planet symbols and 11 px status icons

  // Text: top of each text box, full width, centred
  int16_t width;
  int16_t weekday_y;
  int16_t time_y;
  int16_t date_y;
  int16_t year_y;
  GFont text_font;
  GFont time_font;

  // Status icons. On rectangular displays they are laid out leftwards from
  // this top-right corner; on round displays they are centred on its x.
  GPoint status_anchor;
  bool status_centered;
} Layout;

extern Layout g_layout;

// Computes the layout for the given screen bounds and current settings.
// Safe to call again when a setting that affects layout changes.
void layout_init(GRect bounds);
void layout_deinit(void);
