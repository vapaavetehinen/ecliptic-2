#pragma once
#include <pebble.h>

// A small one-colour pixel-art shape. Bit (w - 1 - x) of rows[y] is set
// where the pixel at (x, y) is inked.
typedef struct {
  const uint16_t *rows;
  uint8_t w;
  uint8_t h;
} Glyph;

extern const Glyph GLYPH_MERCURY_S, GLYPH_VENUS_S, GLYPH_MARS_S, GLYPH_JUPITER_S, GLYPH_SATURN_S;
extern const Glyph GLYPH_BT_S, GLYPH_CHARGE_S;
extern const Glyph GLYPH_MERCURY_L, GLYPH_VENUS_L, GLYPH_MARS_L, GLYPH_JUPITER_L, GLYPH_SATURN_L;
extern const Glyph GLYPH_BT_L, GLYPH_CHARGE_L;

// Draws the glyph with its top-left corner at origin, in the given colour.
void glyph_draw(GContext *ctx, const Glyph *glyph, GPoint origin, GColor colour);
