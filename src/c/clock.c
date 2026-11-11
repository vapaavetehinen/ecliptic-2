#include "clock.h"
#include "layout.h"
#include "theme.h"

static char s_time_text[8];
static char s_weekday_text[16];
static char s_date_text[16];
static char s_year_text[8];

void clock_update(struct tm *now) {
  time_t t;
  if (now == NULL) {
    t = time(NULL);
    now = localtime(&t);
  }
  if (clock_is_24h_style()) {
    strftime(s_time_text, sizeof(s_time_text), "%H:%M", now);
    strftime(s_weekday_text, sizeof(s_weekday_text), "%A", now);
  } else {
    strftime(s_time_text, sizeof(s_time_text), "%l:%M", now);
    if (s_time_text[0] == ' ') {
      memmove(s_time_text, &s_time_text[1], sizeof(s_time_text) - 1);
    }
    strftime(s_weekday_text, sizeof(s_weekday_text), "%A %p", now);
  }
  strftime(s_date_text, sizeof(s_date_text), "%B %d", now);
  strftime(s_year_text, sizeof(s_year_text), "%Y", now);
}

static void prv_draw_line(GContext *ctx, const char *text, GFont font, int16_t y, int16_t h) {
  graphics_draw_text(ctx, text, font, GRect(0, y, g_layout.width, h),
                     GTextOverflowModeTrailingEllipsis, GTextAlignmentCenter, NULL);
}

void clock_draw(GContext *ctx) {
  const Layout *l = &g_layout;
  graphics_context_set_text_color(ctx, theme_fg());
  prv_draw_line(ctx, s_weekday_text, l->text_font, l->weekday_y, 30);
  prv_draw_line(ctx, s_time_text, l->time_font, l->time_y, 80);
  prv_draw_line(ctx, s_date_text, l->text_font, l->date_y, 30);
  prv_draw_line(ctx, s_year_text, l->text_font, l->year_y, 30);
}
