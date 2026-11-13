// Ecliptic 2: Ecliptic redrawn for larger Pebble displays.
// A port of the Ecliptic watchface by Konstantin Beliakov (GPL-3.0).
// Ported and modified by vapaavetehinen in November 2026.

#include <pebble.h>
#include "clock.h"
#include "layout.h"
#include "settings.h"
#include "sky.h"
#include "status.h"
#include "theme.h"

static Window *s_window;
static Layer *s_face_layer;

static void prv_face_draw(Layer *layer, GContext *ctx) {
  graphics_context_set_fill_color(ctx, theme_bg());
  graphics_fill_rect(ctx, layer_get_bounds(layer), 0, GCornerNone);
  sky_draw(ctx);
  clock_draw(ctx);
  status_draw(ctx);
}

static void prv_redraw(void) {
  if (s_face_layer) {
    layer_mark_dirty(s_face_layer);
  }
}

static void prv_settings_changed(void) {
  if (s_face_layer) {
    layout_init(layer_get_bounds(s_face_layer));
  }
  sky_invalidate();
  prv_redraw();
}

static void prv_tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  clock_update(tick_time);
  prv_redraw();
  if (g_settings.vibrate_hourly && (units_changed & HOUR_UNIT)) {
    vibes_short_pulse();
  }
}

static void prv_battery_handler(BatteryChargeState state) {
  status_set_battery(state);
  prv_redraw();
}

static void prv_connection_handler(bool connected) {
  status_set_connected(connected);
  prv_redraw();
  if (g_settings.vibrate_disconnect && !connected) {
    vibes_double_pulse();
  }
}

static void prv_window_load(Window *window) {
  Layer *root = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(root);
  layout_init(bounds);
  sky_init();

  s_face_layer = layer_create(bounds);
  layer_set_update_proc(s_face_layer, prv_face_draw);
  layer_add_child(root, s_face_layer);

  clock_update(NULL);
  status_set_battery(battery_state_service_peek());
  status_set_connected(connection_service_peek_pebble_app_connection());
}

static void prv_window_unload(Window *window) {
  layer_destroy(s_face_layer);
  s_face_layer = NULL;
  sky_deinit();
  layout_deinit();
}

static void prv_init(void) {
  settings_init(prv_settings_changed);

  s_window = window_create();
  window_set_background_color(s_window, GColorBlack);
  window_set_window_handlers(s_window, (WindowHandlers) {
    .load = prv_window_load,
    .unload = prv_window_unload,
  });
  window_stack_push(s_window, true);

  tick_timer_service_subscribe(MINUTE_UNIT, prv_tick_handler);
  battery_state_service_subscribe(prv_battery_handler);
  connection_service_subscribe((ConnectionHandlers) {
    .pebble_app_connection_handler = prv_connection_handler,
  });
}

static void prv_deinit(void) {
  tick_timer_service_unsubscribe();
  battery_state_service_unsubscribe();
  connection_service_unsubscribe();
  window_destroy(s_window);
}

int main(void) {
  prv_init();
  app_event_loop();
  prv_deinit();
}
