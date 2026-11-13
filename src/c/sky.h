#pragma once
#include <pebble.h>

void sky_init(void);
void sky_deinit(void);

// Call when the layout or the flip setting has changed: the positions of the
// stars and constellation lines are kept between redraws.
void sky_invalidate(void);

// Draws the ecliptic circle, constellations, stars, Sun, planets and Moon.
void sky_draw(GContext *ctx);
