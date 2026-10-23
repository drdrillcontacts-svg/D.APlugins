"""Pure data validation for the native editor's ten product-specific layouts."""
import json
from pathlib import Path
root=Path(__file__).resolve().parents[1]
metadata=json.loads((root/'Design/UI/Parameters.json').read_text())
layouts=json.loads((root/'Design/UI/Layouts.json').read_text())
assert len(layouts)==10 and len({s['name'] for s in layouts})==10
for layout in layouts:
 name=layout['name'];controls=layout['controls'];ids=[c['id'] for c in controls]
 assert set(ids)=={m['id'] for m in metadata[name]} and len(ids)==len(set(ids)),name
 for c in controls:
  x,y,w,h=c['rect'];assert 0<x<x+w<1200 and 0<y<y+h<1120,(name,c)
  m=next(m for m in metadata[name] if m['id']==c['id'])
  if c['options']:assert len(c['options'])==int(m['max']-m['min']+1),(name,c)
 # Parameter components must never overlap each other (different visual topologies included).
 for i,a in enumerate(controls):
  for b in controls[i+1:]:
   x,y,w,h=a['rect'];xx,yy,ww,hh=b['rect']
   assert x+w<=xx+1e-6 or xx+ww<=x+1e-6 or y+h<=yy+1e-6 or yy+hh<=y+1e-6,(name,a['id'],b['id'])
 if name=='DEQ':assert {m['scope'] for m in layout['modules']}=={f'Band {b}' for b in range(1,5)}
 if name=='DFX':assert len(layout['sections'])==4 and len(layout['modules'])==4
 assert (root/f'Design/UI-Previews/{name}.svg').exists()
 print('PASS',name,len(ids),'parameters, non-overlapping controls, choice ranges, design export')
assert sum(len(s['controls']) for s in layouts)==90
