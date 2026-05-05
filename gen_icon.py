#!/usr/bin/env python3
"""
gen_icon.py  —  generates src/icon.png (92x64 px, required by fxSDK)
Run once before building:  python3 gen_icon.py
Requires Pillow:  pip install Pillow
"""
from PIL import Image, ImageDraw, ImageFont
import os

W, H = 92, 64
img = Image.new("RGB", (W, H), (0, 30, 80))      # dark navy background
d   = ImageDraw.Draw(img)

# border
d.rectangle([0,0,W-1,H-1], outline=(80,160,255), width=2)

# title text  (PIL built-in font — no external font needed)
d.text((6,  6), "MAT267", fill=(255,255,255))
d.text((6, 22), "  CAS",  fill=(100,200,255))

# small decorative symbols
d.text((6,  40), "\u222b\u222b\u222b  \u2207\xd7F", fill=(180,220,255))

out = os.path.join(os.path.dirname(__file__), "src", "icon.png")
img.save(out)
print(f"Saved {out}  ({W}x{H})")
