#!/usr/bin/env python3
"""Build the full-moon texture for a given moon radius.

Usage: python3 tools/gen_moon.py RADIUS out.png [preview.png]

The art is a hand-placed map of maria (darker patches) on a lit disc, drawn
on a 17x17 grid for radius 8 and a 21x21 grid for radius 10. Colours are from the Pebble 64-colour palette,
the same ones the original 13x13 texture uses.
"""
import sys
from PIL import Image

PALETTE = {
    '.': (0, 0, 0, 0),
    'a': (255, 255, 85, 255),    # yellow
    'c': (255, 170, 0, 255),     # chrome yellow (maria)
    'd': (255, 255, 170, 255),   # pastel yellow (highlands)
    'e': (255, 170, 85, 255),    # rajah (maria)
}

ART = {
    8: """
......aaaaa......
....ccccaaaad....
..acccccccaaaaa..
..ccceeccaaaaaa..
.ccceeeeddccaaaa.
.cceeaceecaacdca.
acceeceaacaaceeca
acceecaaaddcceaaa
accceaaadddccaaaa
aacceccdddddaacaa
aacccccdddddaaaca
.aacccdddddddaaa.
.aaaadddddddaaaa.
..aaadddddaaaaa..
..aaaadddaaaaaa..
....aaaddaaaa....
......aaaaa......
""",
    10: """
.......aaaaaaa.......
.....cccccaaaaadd....
...acccccccccaaaaaa..
..acccceecccaaaaaaa..
..cccceeeeccaaaaaaaa.
.cccceeeeeedccccaaaa.
.ccceeeacceedaaacdca.
accceeacceeacaaceeeca
accceecceaaacaaaceeca
accceeecaaadddcceeaaa
acccceeaaaddddccaaaaa
aaccceeccdddddddaacaa
aaccccccddddddddaaaca
aacccccccddddddddaaaa
.aaccccdddddddddaaaa.
.aaaacdddddddddaaaaa.
..aaaaddddddddaaaaaa.
..aaaaadddddddaaaaa..
...aaaaaddddaaaaaa...
.....aaaadddaaaa.....
.......aaaaaaa.......
""",
}

r = int(sys.argv[1])
rows = ART[r].strip('\n').split('\n')
n = 2 * r + 1
assert len(rows) == n and all(len(x) == n for x in rows)


def half_width(radius, dy):
    limit = radius * radius + radius - dy * dy
    w = 0
    while (w + 1) * (w + 1) <= limit:
        w += 1
    return w


im = Image.new('RGBA', (n, n), (0, 0, 0, 0))
for y, row in enumerate(rows):
    hw = half_width(r, y - r)
    for x, ch in enumerate(row):
        inside = abs(x - r) <= hw
        if inside:
            im.putpixel((x, y), PALETTE['a' if ch == '.' else ch])
im.save(sys.argv[2])
if len(sys.argv) > 3:
    bg = Image.new('RGBA', (n + 4, n + 4), (0, 0, 0, 255))
    bg.alpha_composite(im, (2, 2))
    bg.resize(((n + 4) * 12, (n + 4) * 12), Image.NEAREST).save(sys.argv[3])
