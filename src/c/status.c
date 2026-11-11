#include "status.h"
#include "glyphs.h"
#include "layout.h"
#include "theme.h"

static uint8_t s_charge_percent;
static bool s_is_charging;
static bool s_is_connected;

void status_set_battery(BatteryChargeState state) {
  s_charge_percent = state.charge_percent;
  s_is_charging = state.is_charging;
}

void status_set_connected(bool connected) {
  s_is_connected = connected;
}

void status_draw(GContext *ctx) {
  const Layout *l = &g_layout;
  const bool large = l->large_glyphs;
  const Glyph *bt = large ? &GLYPH_BT_L : &GLYPH_BT_S;
  const Glyph *charge = large ? &GLYPH_CHARGE_L : &GLYPH_CHARGE_S;
  // Battery: body, terminal nub, and a fill inset 2 px from the body.
  const int body_w = large ? 19 : 14;
  const int body_h = bt->h;
  const int nub_w = large ? 3 : 2;
  const int nub_h = large ? 5 : 3;
  const int fill_max = body_w - 4;
  const int gap = large ? 3 : 2;

  const GColor fg = theme_fg();
  graphics_context_set_antialiased(ctx, false);
  graphics_context_set_stroke_width(ctx, 1);
  graphics_context_set_stroke_color(ctx, fg);
  graphics_context_set_fill_color(ctx, fg);

  int x = l->status_anchor.x;
  const int y = l->status_anchor.y;
  if (l->status_centered) {
    int total = gap + nub_w + body_w;
    if (s_is_charging) {
      total += charge->w + gap;
    }
    if (s_is_connected) {
      total += 2 * gap + bt->w;
    }
    x += (total + gap) / 2;
  }

  if (s_is_charging) {
    x -= charge->w;
    glyph_draw(ctx, charge, GPoint(x, y), fg);
    x -= gap;
  }

  x -= gap + nub_w + body_w;
  graphics_draw_rect(ctx, GRect(x, y, body_w, body_h));
  graphics_fill_rect(ctx, GRect(x + body_w, y + (body_h - nub_h) / 2, nub_w, nub_h), 0, GCornerNone);
  graphics_fill_rect(ctx, GRect(x + 2, y + 2, s_charge_percent * fill_max / 100, body_h - 4), 0,
                     GCornerNone);

  if (s_is_connected) {
    x -= 2 * gap + bt->w;
    glyph_draw(ctx, bt, GPoint(x, y), fg);
  }
}
