#include "sky.h"
#include "astrocalc.h"
#include "glyphs.h"
#include "layout.h"
#include "my_math.h"
#include "settings.h"
#include "stars_data.h"
#include "theme.h"
#include "variants.h"

#define MOON_UPDATE_FREQ 0.05     // days: 20 times per day
#define PLANETS_UPDATE_FREQ 1.0   // days

#define ECLIPTIC_COLOR   COLOR_FALLBACK(GColorSunsetOrange, theme_fg())
#define CONST_LINE_COLOR COLOR_FALLBACK(GColorMidnightGreen, theme_fg())
#define SUN_COLOR        COLOR_FALLBACK(GColorIcterine, theme_fg())
#define MOON_COLOR       COLOR_FALLBACK(GColorYellow, theme_fg())
#define MOON_BG_COLOR    COLOR_FALLBACK(GColorOxfordBlue, theme_bg())
#define MERCURY_COLOR    COLOR_FALLBACK(GColorLightGray, theme_fg())
#define VENUS_COLOR      COLOR_FALLBACK(GColorPastelYellow, theme_fg())
#define MARS_COLOR       COLOR_FALLBACK(GColorOrange, theme_fg())
#define JUPITER_COLOR    COLOR_FALLBACK(GColorChromeYellow, theme_fg())
#define SATURN_COLOR     COLOR_FALLBACK(GColorBrass, theme_fg())

enum { MERCURY, VENUS, MARS, JUPITER, SATURN, PLANET_COUNT };

// Angles are measured from the Sun (top of the circle), clockwise on screen.
static struct {
  float sun_lon;
  float planet_angle[PLANET_COUNT];
  int moon_angle;
  double moon_date;
  double planets_date;
} s_sky;

#ifdef PBL_COLOR
static GBitmap *s_moon_bitmap;
#endif

// Screen positions of the constellation line ends (two per line) and the
// stars. Projecting them is most of the work of a redraw, and the result
// only changes with the Sun's longitude, the layout or the flip setting, so
// it is kept between redraws. Either array may be NULL if memory ran short;
// those points are then projected on every redraw.
static GPoint *s_line_points;
#ifdef PBL_COLOR
static GPoint *s_star_points;
#endif
static bool s_points_valid;

void sky_invalidate(void) {
  s_points_valid = false;
}

void sky_init(void) {
  s_sky.moon_date = -1e9;
  s_sky.planets_date = -1e9;
  s_line_points = malloc(CONST_LINE_COUNT * 2 * sizeof(GPoint));
#ifdef PBL_COLOR
  s_star_points = malloc(STAR_COUNT * sizeof(GPoint));
#endif
  s_points_valid = false;
#ifdef PBL_COLOR
  s_moon_bitmap = gbitmap_create_with_resource(RESOURCE_ID_MOON);
#endif
}

void sky_deinit(void) {
  free(s_line_points);
  s_line_points = NULL;
#ifdef PBL_COLOR
  free(s_star_points);
  s_star_points = NULL;
#endif
#ifdef PBL_COLOR
  gbitmap_destroy(s_moon_bitmap);
#endif
}

static void prv_update(double d) {
  const bool moon_due = my_fabs(d - s_sky.moon_date) > MOON_UPDATE_FREQ;
  const bool planets_due = my_fabs(d - s_sky.planets_date) > PLANETS_UPDATE_FREQ;
  if (moon_due || planets_due) {
    s_sky.sun_lon = getSunLongitude(d);
    s_points_valid = false;
  }
  if (moon_due) {
    s_sky.moon_angle = rev(s_sky.sun_lon - getMoonLongitude(d));
    s_sky.moon_date = d;
  }
  if (planets_due) {
    s_sky.planet_angle[MERCURY] = rev(s_sky.sun_lon - getMercuryLongitude(d));
    s_sky.planet_angle[VENUS] = rev(s_sky.sun_lon - getVenusLongitude(d));
    s_sky.planet_angle[MARS] = rev(s_sky.sun_lon - getMarsLongitude(d));
    s_sky.planet_angle[JUPITER] = rev(s_sky.sun_lon - getJupiterLongitude(d));
    s_sky.planet_angle[SATURN] = rev(s_sky.sun_lon - getSaturnLongitude(d));
    s_sky.planets_date = d;
  }
#ifdef DEBUG_MOON_ANGLE
  s_sky.moon_angle = DEBUG_MOON_ANGLE;
#endif
}

// The single projection from sky to screen. angle is degrees from the Sun,
// lat is ecliptic latitude in degrees (0 = on the ecliptic circle).
// A horizontal flip mirrors left-right and swaps north and south, as the
// sky appears from the southern hemisphere.
static GPoint prv_sky_point(float angle, float lat) {
  const Layout *l = &g_layout;
  const float offset = lat * l->lat_scale / 256;
  if (g_settings.flip_horizontal) {
    const float rad = l->radius - offset;
    return GPoint(my_rint(l->center.x - sind(angle) * rad),
                  my_rint(l->center.y - cosd(angle) * rad));
  }
  const float rad = l->radius + offset;
  return GPoint(my_rint(l->center.x + sind(angle) * rad),
                my_rint(l->center.y - cosd(angle) * rad));
}

// end is 0 or 1.
static GPoint prv_line_point(int line, int end) {
  const ConstLine *c = &CONST_LINES[line];
  return end == 0 ? prv_sky_point(s_sky.sun_lon - c->lon1 / 100.0f, c->lat1 / 100.0f)
                  : prv_sky_point(s_sky.sun_lon - c->lon2 / 100.0f, c->lat2 / 100.0f);
}

#ifdef PBL_COLOR
static GPoint prv_star_point(int star) {
  const Star *s = &STARS[star];
  return prv_sky_point(s_sky.sun_lon - s->lon / 100.0f, s->lat / 100.0f);
}
#endif

// Brings the kept points up to date. Call after prv_update.
static void prv_project_points(void) {
  if (s_points_valid) {
    return;
  }
  if (s_line_points) {
    for (int i = 0; i < CONST_LINE_COUNT * 2; i++) {
      s_line_points[i] = prv_line_point(i / 2, i % 2);
    }
  }
#ifdef PBL_COLOR
  if (s_star_points) {
    for (int i = 0; i < STAR_COUNT; i++) {
      s_star_points[i] = prv_star_point(i);
    }
  }
#endif
  s_points_valid = true;
}

static void prv_draw_const_lines(GContext *ctx) {
  graphics_context_set_stroke_color(ctx, CONST_LINE_COLOR);
  for (int i = 0; i < CONST_LINE_COUNT; i++) {
    GPoint a = s_line_points ? s_line_points[2 * i] : prv_line_point(i, 0);
    GPoint b = s_line_points ? s_line_points[2 * i + 1] : prv_line_point(i, 1);
    graphics_draw_line(ctx, a, b);
  }
}

#ifdef PBL_COLOR
static GColor prv_star_colour(uint8_t colour) {
  switch (colour) {
    case STAR_LIGHT_GRAY: return GColorLightGray;
    case STAR_DARK_GRAY: return GColorDarkGray;
    case STAR_ORANGE: return GColorOrange;
    case STAR_RAJAH: return GColorRajah;
    case STAR_ELECTRIC_BLUE: return GColorElectricBlue;
    default: return GColorWhite;
  }
}

static void prv_draw_stars(GContext *ctx) {
  for (int i = 0; i < STAR_COUNT; i++) {
    const Star *s = &STARS[i];
    GPoint p = s_star_points ? s_star_points[i] : prv_star_point(i);
    GColor colour = prv_star_colour(s->colour);
    graphics_context_set_stroke_color(ctx, colour);
    if (g_settings.stars_by_brightness) {
      if (s->mag10 <= 15) {
        // Brightest: a small plus.
        graphics_draw_line(ctx, GPoint(p.x - 1, p.y), GPoint(p.x + 1, p.y));
        graphics_draw_line(ctx, GPoint(p.x, p.y - 1), GPoint(p.x, p.y + 1));
        continue;
      }
      if (s->mag10 <= 23) {
        // Bright: a 2x2 block.
        graphics_context_set_fill_color(ctx, colour);
        graphics_fill_rect(ctx, GRect(p.x, p.y, 2, 2), 0, GCornerNone);
        continue;
      }
    }
    graphics_draw_pixel(ctx, p);
  }
}
#endif

static void prv_draw_sun(GContext *ctx) {
  const Layout *l = &g_layout;
  const GPoint p = GPoint(l->center.x, l->center.y - l->radius);
  graphics_context_set_fill_color(ctx, SUN_COLOR);
  graphics_fill_circle(ctx, p, l->sun_r);
  graphics_context_set_stroke_color(ctx, theme_bg());
  graphics_draw_circle(ctx, p, l->sun_r - 1);
  graphics_draw_pixel(ctx, p);
}

static void prv_draw_planet(GContext *ctx, int planet, const Glyph *small, const Glyph *large,
                            GColor colour) {
  const Layout *l = &g_layout;
  const Glyph *glyph = l->large_glyphs ? large : small;
  const GPoint p = prv_sky_point(s_sky.planet_angle[planet], 0);
  graphics_context_set_fill_color(ctx, colour);
  graphics_fill_circle(ctx, p, l->planet_r);
  glyph_draw(ctx, glyph, GPoint(p.x - glyph->w / 2, p.y - glyph->h / 2), theme_bg());
  graphics_context_set_stroke_color(ctx, theme_bg());
  graphics_draw_circle(ctx, p, l->planet_r + 1);
}

// Half-width of a filled circle of radius r on the row dy from its centre.
static int prv_half_width(int r, int dy) {
  const int limit = r * r + r - dy * dy;
  int w = 0;
  while ((w + 1) * (w + 1) <= limit) {
    w++;
  }
  return w;
}

static void prv_draw_moon(GContext *ctx) {
  const Layout *l = &g_layout;
  const int angle = s_sky.moon_angle;
  const int r = l->moon_r;
  const GPoint p = prv_sky_point(angle, 0);

  // Outline, then the full lit disc.
  graphics_context_set_fill_color(ctx, theme_bg());
  graphics_fill_circle(ctx, p, r + 1);
  graphics_context_set_fill_color(ctx, MOON_COLOR);
  graphics_fill_circle(ctx, p, r);

  const bool full = (angle >= 168) && (angle < 192);
  if (full) {
#ifdef PBL_COLOR
    GRect box = gbitmap_get_bounds(s_moon_bitmap);
    box.origin = GPoint(p.x - box.size.w / 2, p.y - box.size.h / 2);
    graphics_context_set_compositing_mode(ctx, GCompOpSet);
    graphics_draw_bitmap_in_rect(ctx, s_moon_bitmap, box);
#endif
    return;
  }

  // Shadowed disc, leaving a thin lit rim.
  graphics_context_set_fill_color(ctx, MOON_BG_COLOR);
  graphics_fill_circle(ctx, p, r - 1);
  if ((angle < 10) || (angle >= 350)) {
    return;  // new moon
  }

  // Lit part: on each row, from the lit limb to the terminator. The
  // terminator is an ellipse whose half-width follows the phase angle.
  // Past the Sun going clockwise (angle < 180) the Moon is waning and lit
  // on its left; a horizontal flip mirrors that.
  const float t = -cosd(angle);
  const bool lit_left = (angle < 180) != g_settings.flip_horizontal;
  graphics_context_set_stroke_color(ctx, MOON_COLOR);
  for (int dy = -(r - 1); dy <= r - 1; dy++) {
    const int w = prv_half_width(r - 1, dy);
    const int edge = my_rint(t * w);
    const int x0 = lit_left ? -w : -edge;
    const int x1 = lit_left ? edge : w;
    graphics_draw_line(ctx, GPoint(p.x + x0, p.y + dy), GPoint(p.x + x1, p.y + dy));
  }
}

void sky_draw(GContext *ctx) {
  const Layout *l = &g_layout;
#ifdef DEBUG_DAY
  const double d = DEBUG_DAY;
#else
  const double d = getDayFrom2000();
#endif
  prv_update(d);
  prv_project_points();

  graphics_context_set_antialiased(ctx, false);
  graphics_context_set_stroke_width(ctx, 1);

  graphics_context_set_stroke_color(ctx, ECLIPTIC_COLOR);
  graphics_draw_circle(ctx, l->center, l->radius);

  prv_draw_const_lines(ctx);
#ifdef PBL_COLOR
  prv_draw_stars(ctx);
#endif
  prv_draw_sun(ctx);
  prv_draw_planet(ctx, SATURN, &GLYPH_SATURN_S, &GLYPH_SATURN_L, SATURN_COLOR);
  prv_draw_planet(ctx, JUPITER, &GLYPH_JUPITER_S, &GLYPH_JUPITER_L, JUPITER_COLOR);
  prv_draw_planet(ctx, MARS, &GLYPH_MARS_S, &GLYPH_MARS_L, MARS_COLOR);
  prv_draw_planet(ctx, MERCURY, &GLYPH_MERCURY_S, &GLYPH_MERCURY_L, MERCURY_COLOR);
  prv_draw_planet(ctx, VENUS, &GLYPH_VENUS_S, &GLYPH_VENUS_L, VENUS_COLOR);
  prv_draw_moon(ctx);
}
