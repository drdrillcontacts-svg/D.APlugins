#!/usr/bin/env python3
"""Synchronize Design/UI-Previews with the approved Final Reference SVGs (1200 x 1240, per-plugin Noir colors)."""
from pathlib import Path
import subprocess, sys

ROOT = Path(__file__).resolve().parents[1]
subprocess.run([sys.executable, str(ROOT / 'tools/design_final_reference.py')], check=True)
SRC = ROOT / 'Design/Concepts/Final-Reference'
OUT = ROOT / 'Design/UI-Previews'
OUT.mkdir(exist_ok=True)
for svg in sorted(SRC.glob('D*.svg')):
    text = svg.read_text()
    (OUT / svg.name).write_text(text)
    (OUT / f'{svg.stem}-Advanced.svg').write_text(text)
print('Synchronized 20 UI-Previews SVGs with approved Final Reference SVGs.')
