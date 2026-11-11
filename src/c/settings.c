#include "settings.h"
#include "variants.h"

#define SETTINGS_PKEY 1

Settings g_settings;
static void (*s_on_change)(void);

static bool prv_read_bool(DictionaryIterator *iter, uint32_t key, bool *out) {
  Tuple *t = dict_find(iter, key);
  if (!t) {
    return false;
  }
  int32_t v = 0;
  switch (t->length) {
    case 1: v = t->value->int8; break;
    case 2: v = t->value->int16; break;
    default: v = t->value->int32; break;
  }
  *out = (v != 0);
  return true;
}

static void prv_inbox_received(DictionaryIterator *iter, void *context) {
  bool changed = false;
  changed |= prv_read_bool(iter, MESSAGE_KEY_BgWhite, &g_settings.bg_white);
  changed |= prv_read_bool(iter, MESSAGE_KEY_VibrateHourly, &g_settings.vibrate_hourly);
  changed |= prv_read_bool(iter, MESSAGE_KEY_VibrateDisconnect, &g_settings.vibrate_disconnect);
  changed |= prv_read_bool(iter, MESSAGE_KEY_FlipHorizontal, &g_settings.flip_horizontal);
  changed |= prv_read_bool(iter, MESSAGE_KEY_StarsByBrightness, &g_settings.stars_by_brightness);
  bool edge_buffer;
  if (prv_read_bool(iter, MESSAGE_KEY_EdgeBuffer, &edge_buffer)) {
    g_settings.edge_to_edge = !edge_buffer;
    changed = true;
  }
  if (changed) {
    persist_write_data(SETTINGS_PKEY, &g_settings, sizeof(g_settings));
    if (s_on_change) {
      s_on_change();
    }
  }
}

void settings_init(void (*on_change)(void)) {
  s_on_change = on_change;
  memset(&g_settings, 0, sizeof(g_settings));
  if (persist_exists(SETTINGS_PKEY)) {
    persist_read_data(SETTINGS_PKEY, &g_settings, sizeof(g_settings));
  }
#ifndef PBL_BW
  g_settings.bg_white = false;
#endif
#if defined(DEBUG_BG_WHITE) && defined(PBL_BW)
  g_settings.bg_white = true;
#endif
#ifdef DEBUG_FLIP
  g_settings.flip_horizontal = true;
#endif
  app_message_register_inbox_received(prv_inbox_received);
  app_message_open(128, 16);
}
