#pragma once
#include <pebble.h>

void sky_init(void);
void sky_deinit(void);

// Draws the ecliptic circle, constellations, stars, Sun, planets and Moon.
void sky_draw(GContext *ctx);
