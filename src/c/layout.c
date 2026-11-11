#include "layout.h"
#include "settings.h"

Layout g_layout;
static GFont s_custom_font;

void layout_init(GRect bounds) {
  Layout *l = &g_layout;
  const int16_t w = bounds.size.w;
  const int16_t h = bounds.size.h;
  l->width = w;
  l->center = GPoint(w / 2, h / 2);

#if PBL_DISPLAY_WIDTH >= 260
  // Pebble Round 2 (260x260): the original proportions x 108/66, the
  // largest circle that leaves room for the Sun at the top.
  l->radius = 108;
  l->lat_scale = (108 * 256) / 66;
  l->sun_r = 16;
  l->planet_r = 8;
  l->moon_r = 10;
  l->large_glyphs = true;
  l->text_font = fonts_get_system_font(FONT_KEY_GOTHIC_28);
  l->weekday_y = 59;
  l->date_y = 148;
  l->year_y = 177;
  if (!s_custom_font) {
    s_custom_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_TIME_64));
  }
  l->time_font = s_custom_font;
  l->time_y = 80;
  l->status_anchor = GPoint(w / 2, 212);
  l->status_centered = true;
#elif PBL_DISPLAY_WIDTH >= 200
  // Pebble Time 2 (200x228): the original proportions x 92/66.
  // 92 puts a planet's outline exactly on the screen edge, as the original
  // does. 88 keeps a 4 px gap.
  l->radius = g_settings.edge_to_edge ? 92 : 88;
  l->lat_scale = (l->radius * 256) / 66;
  l->sun_r = 14;
  l->planet_r = 7;
  l->moon_r = 8;
  l->large_glyphs = true;
  l->text_font = fonts_get_system_font(FONT_KEY_GOTHIC_24);
  l->weekday_y = 52;
  l->date_y = 128;
  l->year_y = 153;
  if (!s_custom_font) {
    s_custom_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_TIME_54));
  }
  l->time_font = s_custom_font;
  l->time_y = 71;
  l->status_anchor = GPoint(w - 3, 3);
  l->status_centered = false;
#else
  // 144x168 and Pebble Time Round (180x180): the original Ecliptic sizes.
  l->sun_r = 10;
  l->planet_r = 5;
  l->moon_r = 6;
  l->large_glyphs = false;
  l->text_font = fonts_get_system_font(FONT_KEY_GOTHIC_18);
  l->time_font = fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD);
#if defined(PBL_ROUND)
  // Same elements on a slightly wider circle, status icons moved inside.
  l->radius = 74;
  l->lat_scale = 256;
  l->weekday_y = 46;
  l->time_y = 58;
  l->date_y = 100;
  l->year_y = 118;
  l->status_anchor = GPoint(w / 2, 147);
  l->status_centered = true;
#else
  l->radius = 66;
  l->lat_scale = 256;
  l->weekday_y = 40;
  l->time_y = 52;
  l->date_y = 94;
  l->year_y = 112;
  l->status_anchor = GPoint(w - 2, 2);
  l->status_centered = false;
#endif
#endif
}

void layout_deinit(void) {
  if (s_custom_font) {
    fonts_unload_custom_font(s_custom_font);
    s_custom_font = NULL;
  }
}
