#pragma once
#include <pebble.h>

void status_set_battery(BatteryChargeState state);
void status_set_connected(bool connected);

// Draws the Bluetooth, battery and charging indicators.
void status_draw(GContext *ctx);
