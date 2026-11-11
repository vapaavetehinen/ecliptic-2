#!/bin/sh
# Build, install on an emulator and save a screenshot (plus a 4x enlargement).
# Usage: tools/shot.sh PLATFORM OUT.png
set -e
plat="$1"; out="$2"
pebble build >/dev/null 2>&1 || { pebble build 2>&1 | grep -v '^\[' | tail -30; exit 1; }
timeout 180 pebble install --emulator "$plat" 2>&1 | tail -1
sleep 2
pebble screenshot --emulator "$plat" --no-open "$out" >/dev/null 2>&1
python3 -c "
import sys
from PIL import Image
im=Image.open(sys.argv[1]); im.resize((im.width*4,im.height*4),Image.NEAREST).save(sys.argv[1][:-4]+'_x4.png'); print(im.size)" "$out"
