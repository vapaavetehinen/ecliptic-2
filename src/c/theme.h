#pragma once
#include <pebble.h>
#include "settings.h"

// Foreground / background. Only black-and-white displays can be inverted.
static inline GColor theme_fg(void) {
  return PBL_IF_BW_ELSE(g_settings.bg_white ? GColorBlack : GColorWhite, GColorWhite);
}

static inline GColor theme_bg(void) {
  return PBL_IF_BW_ELSE(g_settings.bg_white ? GColorWhite : GColorBlack, GColorBlack);
}
