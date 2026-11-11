#pragma once
#include <pebble.h>

// Ecliptic coordinates in degrees x 100.
typedef struct {
  int16_t lat1;
  uint16_t lon1;
  int16_t lat2;
  uint16_t lon2;
} ConstLine;

typedef struct {
  int16_t lat;
  uint16_t lon;
  uint8_t mag10;   // visual magnitude x 10
  uint8_t colour;  // StarColour
} Star;

enum StarColour {
  STAR_WHITE = 0,
  STAR_LIGHT_GRAY,
  STAR_DARK_GRAY,
  STAR_ORANGE,
  STAR_RAJAH,
  STAR_ELECTRIC_BLUE,
};

extern const ConstLine CONST_LINES[];
extern const uint16_t CONST_LINE_COUNT;

#ifdef PBL_COLOR
extern const Star STARS[];
extern const uint16_t STAR_COUNT;
#endif
