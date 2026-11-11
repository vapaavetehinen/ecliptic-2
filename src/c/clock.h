#pragma once
#include <pebble.h>

// Refreshes the time and date strings. Pass NULL to use the current time.
void clock_update(struct tm *now);

// Draws weekday, time, date and year.
void clock_draw(GContext *ctx);
