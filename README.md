# Ecliptic 2

A Pebble watchface showing the positions of the Sun, the Moon and the five naked-eye planets along the ecliptic. This is a port of [Ecliptic](../ecliptic-pebble) by Konstantin Beliakov, redrawn natively for the Pebble Time 2 and the other current watches so that nothing is scaled.

The Sun is always at the top of the circle. Planets to its left are visible in the evening, planets to its right in the morning, and planets at the bottom all night. The Moon shows its current phase.

## Building and running

```sh
pebble build                          # all platforms
pebble install --emulator emery       # Pebble Time 2 emulator
tools/shot.sh emery shot.png          # build, install, screenshot
```

## Platforms

| Platform | Display | Layout |
|---|---|---|
| emery (Pebble Time 2) | 200x228 colour | redrawn at 92/66 of the original |
| gabbro (Pebble Round 2) | 260x260 round colour | redrawn at 108/66, status icons inside the circle |
| basalt (Pebble Time) | 144x168 colour | as the original |
| chalk (Pebble Time Round) | 180x180 round colour | original sizes on a wider circle |
| aplite, diorite, flint | 144x168 black and white | as the original, no stars |

## Settings

Flip horizontally (southern hemisphere), keep planets off the screen edge (Pebble Time 2, on by default), size stars by brightness (colour watches), vibrate on the hour, vibrate on disconnect, and a white background on black-and-white watches.

## Layout

See `AGENTS.md` for the source layout and `PORTING_PLAN.md` for the porting plan and status.

## Credits and licence

The original Ecliptic watchface, including all of the astronomy code, the star and constellation data and the design, is by Konstantin Beliakov. This port, by vapaavetehinen (November 2026), rewrites the drawing code and artwork for larger displays and replaces the settings page.

GPL-3.0, as the original (see `LICENSE`). The time font is Montserrat Bold, under the SIL Open Font License (`resources/fonts/Montserrat-OFL.txt`).
