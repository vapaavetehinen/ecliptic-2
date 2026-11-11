# Porting plan: Ecliptic → Ecliptic 2 (crisp on Pebble Time 2)

Source: `../ecliptic-pebble` (Ecliptic 2.2.0, targets aplite + basalt, 144×168).
Target: this project, with emery (Pebble Time 2, 200×228, 64 colours) as the primary platform.

## Why the scaled-up original looks soft

The original is drawn entirely in hard-coded 144×168 pixel coordinates, with hand-made pixel art sized for that grid. Emery is 200×228, a ratio of about 1.39 × 1.36. That is not an integer, so stretching the framebuffer resamples every 1 px line, 9×9 glyph and bitmap-font edge unevenly.

The fix is to render natively at 200×228: recompute geometry from the real screen size, and replace each asset with one authored for the new size. Nothing should be scaled at runtime.

## What carries over and what changes

| Area | Files | Action |
|---|---|---|
| Astronomy and maths | `astrocalc.c/.h`, `my_math.c/.h` | Copy unchanged. No screen dependence. |
| Star and constellation data | `stars.c` (372 stars, constellation lines in ecliptic lat/lon) | Keep the data. Change only the projection (see below). |
| Screen projection | `screencalc.c`, `get_star_x/y` in `stars.c`, `flip_x` in `planets.c` | Rewrite against a layout struct. Hard-codes 72, 84, 66, 144. |
| Drawing | `planets.c`, `main.c` (ecliptic circle), `sb.c` | Rewrite sizes and offsets from layout values. |
| Text | `time.c` | New frames and fonts. |
| Bitmaps | `resources/images/*.png` | Redraw at emery sizes. |
| Settings and config page | `settings.h`, `pebble-js-app.js`, AppSync code in `main.c` | Replace (the config backend is defunct). |
| Build structure | `main.c` does `#include "planets.c"` etc. | Restructure into real modules. |

## Steps

### 1. Project setup
- Set `watchface: true` and the display name in `package.json`. Keep the fork's own UUID so it installs alongside the original.
- Start with `targetPlatforms: ["emery"]`. Add others in step 7.
- Carry over the LICENSE and credit the original author in the README.

### 2. Bring the code over as proper modules
- The original compiles as one translation unit: `main.c` includes `planets.c`, `time.c`, `sb.c`, which include `stars.c`, with shared `static` globals in headers. The fork's `wscript` compiles every `src/c/**/*.c` separately, so this will most likely not build as-is.
- Split into modules with headers: `astro` (unchanged maths), `layout`, `sky` (ecliptic, stars, planets, sun, moon), `clock` (time and date text), `status` (battery, Bluetooth), `settings`, `main`.
- Drop `InverterLayer` (removed from the current SDK). The white-background option becomes a colour choice at draw time.
- Checkpoint: builds and runs on emery with the original 144×168 numbers, sitting small in the corner. This proves the logic port before any visual work.

### 3. Introduce a single layout definition
- One `Layout` struct, filled once from `layer_get_bounds()` of the window root: centre point, ecliptic radius, sun radius, planet radius, moon radius, stroke widths, star-latitude scale, text frames, status-bar position.
- Emery values, derived from the original's proportions (radius 66 on a 144-wide screen, 6 px margin):

| Element | Original | Emery |
|---|---|---|
| Centre | (72, 84) | (100, 114) |
| Ecliptic radius | 66 | 92 |
| Sun disc radius | 10 | 14 |
| Planet disc radius / glyph | 5 / 9×9 | 7 / 13×13 |
| Moon disc radius / bitmap | 6 / 13×13 | 9 / 19×19 |
| Star latitude scale | 1 px per degree | about 1.4 px per degree |

- All projection goes through one function, `sky_point(angle, lat_offset)`, which also applies the horizontal flip. This replaces `scrX/scrY`, `get_star_x/y` and `flip_x`.
- Use odd glyph sizes so each has a true centre pixel, and round to whole pixels once, at the end of projection.

### 4. Redraw the sky natively
- Ecliptic circle and constellation lines: draw from layout values. Start with 1 px strokes, judged in the emulator. Whether 1 px reads too thin on the real display is a question for the final on-watch session.
- Stars: first milestone is a faithful recreation, one pixel per star with the existing colour rules.
- Star variant (after the recreation is done): map magnitude to size (for example 1 px for faint, 2×2 or a small plus for the brightest). Build it behind a compile-time flag so both can be checked in the emulator during development. The choice is made on the real watch at the end.
- Moon phase: the crescent is built from hand-tuned line segments with offsets of ±3, ±5, ±6. Reimplement it as a function of the moon radius (per-row terminator fill) instead of scaling those constants.
- Keep antialiasing off for the small glyphs, as the original does. Decide on the circle and lines by emulator screenshot comparison.

### 5. Assets
- Redraw as pixel art at the target size; do not resample the originals. Planet symbols 13×13, full-moon texture 19×19, Bluetooth and charge icons about 7×10, menu icon per current SDK guidance.
- Use platform-tagged files (`mars.png` and `mars~emery.png`) so the right asset is selected at build time with no code branching.
- Keep the planet glyphs as white-on-transparent palette bitmaps so the existing palette-recolour trick still works.

### 6. Text
- Time: the original uses Bitham 42 Bold, about 130 px wide inside a 132 px circle. The equivalent on emery is roughly 58 px. Build both options behind a compile-time flag, check each in the emulator, and choose on the real watch at the end:
  - Custom: a bundled font resource at about 58 px, limited to digits and the colon, chosen to resemble Bitham (the font must have a licence that allows redistribution).
  - System: the closest large system numbers font available on emery (LECO 60 is the candidate; confirm against the SDK docs).
- Date lines: Gothic 18 → Gothic 24, frames taken from the layout.
- The original packs weekday, month and year into one text layer with blank lines around the time. Split into separate layers (weekday above, date below) so each can be positioned exactly.
- Status bar: battery outline and fill drawn from layout values, icons from step 5.

### 7. Settings
- Decided: rebuild with Clay. The config page depends on SetPebble (`x.SetPebble.com`), which no longer works. Replace with Clay (`pebble-clay` package) and declare `messageKeys` for: background colour, hourly vibration, disconnect vibration, horizontal flip.
- Replace AppSync with a plain inbox handler and one persisted settings struct.

### 8. Other platforms
All watches are in scope, in this order:
- Emery first, until it is right.
- Rectangular platforms next (basalt, diorite, flint, aplite if memory allows). The layout struct gets the original 144×168 values and the original assets are the untagged defaults, so these should match the original app.
- Round screens last (chalk 180×180, gabbro; confirm gabbro's resolution in the SDK docs). Each needs its own layout: the ecliptic circle sized to the round display, and the status icons moved out of the corner, which is cut off on a round screen. Gabbro may also need its own asset sizes.

### 9. Verification
- All development and interim checking uses the emulators only. Follow the loop in `AGENTS.md`: build, install on the emulator for the platform being worked on, screenshot, read logs. Nothing is installed on the real watch during this phase.
- Add a debug build flag that overrides the date so specific cases can be screenshotted: new moon, full moon, thin crescents on both sides, planets clustered near the sun, flip on and off, 12 h and 24 h, longest weekday and month names ("Wednesday", "September").
- Compare against a basalt screenshot of the original at the same date to confirm the composition matches.
- The real Pebble Time 2 is used only once, at the very end, after the ports for all platforms are complete (see the final section).

## Decisions (confirmed)

1. **Time font**: build both a custom ~58 px font and a system-font version, then pick on the real Pebble Time 2 at the end.
2. **Platform scope**: emery first, then the other rectangular watches, then the round watches. All are in scope.
3. **Use of extra space**: faithful recreation first. Stars sized by brightness is then explored as a variant and compared on the real Pebble Time 2 at the end.
4. **Settings**: rebuild with Clay.

## Status

Complete. Steps 1 to 9 are implemented and checked in the emulators on all seven platforms, and the final comparison on the real Pebble Time 2 is done.

Changes from the plan made during implementation:

- **Planet symbols and status icons** are stored as bitmasks in code (`src/c/glyphs.c`, generated from pixel art in `tools/gen_glyphs.py`) instead of platform-tagged PNG files. This draws identically on colour and black-and-white displays, needs no per-minute bitmap loading, and removes the palette-recolour trick. The full-moon texture is still a platform-tagged PNG.
- **Moon radius on emery** is 8 (17x17 texture), not 9. 6 x 92/66 is 8.4.
- **Custom time font** is Montserrat Bold at 54 px on emery, not 58. At 58 "00:00" is 173 px wide and collides with planets on the circle; 54 gives 161 px, matching the original's fit. Gabbro uses 64 px.
- **Clay** comes from `@rebble/clay`, because `pebble-clay` does not support flint or gabbro.
- **White background** is offered on black-and-white watches only. In the original it had no effect on colour watches.
- **Round watches**: status icons sit inside the circle under the year, since the top-right corner is cut off.

## Final comparison on the real Pebble Time 2

Only after every platform port is complete and verified in the emulators. Not used for interim development or testing.

- Install to the watch through the phone with `PEBBLE_PHONE=<phone address> pebble install`.
- Install each variant build in turn, waiting for a go-ahead between installs:
  1. Time in the custom font, one-pixel stars.
  2. Time in the system font, one-pixel stars.
  3. Brightness-sized stars, with whichever font was preferred.
- Also judge the 1 px stroke weight of the ecliptic circle and constellation lines while the watch is in hand.
- The chosen font and star style become the defaults, and the unused variant code and font resource are removed.

## Outcome of the final comparison

Compared on the real Pebble Time 2 on 2026-11-11.

- **Time font**: custom (Montserrat Bold). The system-font variant has been removed.
- **Stars**: sizing by brightness is a setting on colour watches ("Size stars by brightness"), off by default, so the default matches the original.
- **Line weight**: 1 px for the ecliptic circle and constellation lines is fine on the real display.
- **Edge buffer**: added after the comparison. On the Pebble Time 2 the circle is slightly smaller by default (radius 88 instead of 92) so planets keep a 4 px gap from the screen edge. The setting "Keep planets off the screen edge" turns this off to get the edge-to-edge layout. The 144x168 watches keep the original layout: a smaller circle there pushes planets into the time digits. Round watches never had edge contact.
