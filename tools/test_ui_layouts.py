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
  x,y,w,h=c['rect'];assert 0<x<x+w<1200 and 0<y<y+h<820,(name,c)
  m=next(m for m in metadata[name] if m['id']==c['id'])
  assert c['learnable']==m['learnable'],(name,c['id'],'Learn policy mismatch')
  if c['options']:assert len(c['options'])==int(m['max']-m['min']+1),(name,c)
 # Parameter components must never overlap each other (different visual topologies included).
 for i,a in enumerate(controls):
  for b in controls[i+1:]:
   if a['advanced']!=b['advanced']:continue
   x,y,w,h=a['rect'];xx,yy,ww,hh=b['rect']
   assert x+w<=xx+1e-6 or xx+ww<=x+1e-6 or y+h<=yy+1e-6 or yy+hh<=y+1e-6,(name,a['id'],b['id'])
 if name=='DEQ':assert {m['scope'] for m in layout['modules']}=={f'Band {b}' for b in range(1,5)}
 if name=='DFX':assert {m['scope'] for m in layout['modules']}=={'Saturation','Filter','Ambience'}
 assert (root/f'Design/UI-Previews/{name}.svg').exists()
 assert (root/f'Design/UI-Previews/{name}-Advanced.svg').exists()
 for m in layout['modules']:
  assert any(p['module']==m['scope'] and p['learnable'] for p in metadata[name]),(name,m)
 assert layout['canvas']==[1200,820]
 print('PASS',name,len(ids),'parameters, non-overlapping controls, choice ranges, design export')
assert sum(len(s['controls']) for s in layouts)==92

assert sum(c['learnable'] for s in layouts for c in s['controls'])==33
editor=(root/'Source/PluginEditor.cpp').read_text()
# Completion must never open the log. Only an explicit button can toggle it.
completion=editor[editor.index('if (ready(learnWork))'):editor.index('analysis.setParameters')]
assert 'drawer.setVisible' not in completion and 'drawer.toFront' not in completion
assert 'DETAILS / LOG' in (root/'Source/PluginEditor.h').read_text()
print('PASS 33 selective Learn controls / 59 manual controls / log stays opt-in')

banks=json.loads((root/'Design/Presets/FactoryPresets.json').read_text())
assert len(banks['banks'])==10
for bank in banks['banks']:
 assert len(bank['presets'])==50 and len({p['name'] for p in bank['presets']})==50
 assert len({tuple(sorted(p['parameters'].items())) for p in bank['presets']})==50
 assert all(set(p['parameters'])=={m['id'] for m in metadata[bank['plugin']]} for p in bank['presets'])
delay=next(s for s in layouts if s['name']=='DDelay')
assert next(c for c in delay['controls'] if c['id']=='division')['options']==banks['divisions']
assert 'drawer.setVisible(false)' in editor
print('PASS 500 exported preset snapshots / tempo-menu enum agreement / hidden initial report')
