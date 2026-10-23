"""Generate the declarative layout shared by the JUCE editor and design exports.
No processor parameter is renamed or dropped. Run the metadata exporter before changing ranges.
Coordinates use a fixed 1200 x 1120 logical canvas; JUCE scales it uniformly.
"""
import json
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1]
META=json.loads((ROOT/'Design/UI/Parameters.json').read_text())
TITLES={
'DVerb':('REVERB LEARNING','SPACE & CHARACTER','ROOM / HALL / PLATE / SHIMMER'),
'DDelay':('STEREO DELAY','ECHO ARCHITECTURE','TIME / FEEDBACK / MOVEMENT'),
'DComp':('DYNAMICS COMPRESSOR','DYNAMICS CHARACTER','CONTROL THE ENERGY. KEEP THE FEEL.'),
'DEQ':('FOUR-BAND EQUALIZER','TONAL OBJECTIVE','FOUR BANDS. ONE COHERENT SOUND.'),
'DSat':('HARMONIC SATURATION','SATURATION CHARACTER','SOFT / HARD / ASYMMETRIC'),
'DTrans':('TRANSIENT DESIGNER','ENVELOPE BEHAVIOUR','ATTACK / BODY / SUSTAIN'),
'DStereo':('STEREO DESIGNER','MID / SIDE FIELD','WIDTH WITHOUT LOSING THE CENTRE.'),
'DSess':('INTELLIGENT DE-ESSER','DETECTION ENVELOPE','FOCUS THE BAND. PRESERVE THE VOICE.'),
'DGate':('GATE & EXPANDER','GATE CHARACTER','KEEP THE PERFORMANCE. CONTROL THE FLOOR.'),
'DFX':('MODULAR MULTI-EFFECTS','MASTER CONTROLS','SATURATION > FILTER > DELAY > AMBIENCE')}
MAIN={
'DVerb':['predelay','size','decay','tone','mix','width'],
'DDelay':['time','feedback','filter','mix','width','output'],
'DComp':['threshold','ratio','attack','release','makeup','output'],
'DSat':['drive','tone','density','mix','output'],
'DTrans':['attack','sustain','sensitivity','shape','mix','output'],
'DStereo':['width','depth','center','side','mix','mono'],
'DSess':['frequency','threshold','range','focus','mix','output'],
'DGate':['threshold','attack','hold','release','range','output']}
NAMES={'predelay':'PRE-DELAY','lowcut':'LOW CUT','midcut':'MID CUT','frequency':'DETECT FREQ','makeup':'MAKEUP','pingpong':'STEREO ROUTING','mode':'CHARACTER','driveOn':'SATURATION','filterOn':'FILTER','delayOn':'DELAY','reverbOn':'AMBIENCE','delayMix':'ECHO MIX','reverbMix':'SPACE MIX','mono':'MONO MONITOR'}
CHOICES={'DVerb':{'mode':['ROOM','HALL','PLATE','SHIMMER']},'DDelay':{'pingpong':['STEREO','PING-PONG']},'DSat':{'mode':['SOFT','HARD','ASYMMETRIC']},'DStereo':{'mono':['STEREO','MONO']}}
layouts=[]
for name,metadata in META.items():
 subtitle,advanced,slogan=TITLES[name]
 spec={'name':name,'subtitle':subtitle,'advancedTitle':advanced,'slogan':slogan,'controls':[],'sections':[],'modules':[],'primaryStyle':'standard'}
 def add(id,x,y,w=136,h=184,style='rotary',options=None):
  m=next(m for m in metadata if m['id']==id)
  spec['controls'].append({'id':id,'label':NAMES.get(id,id.upper()),'rect':[x,y,w,h],'style':style,'options':options or []})
 def module(title,scope,rect):spec['modules'].append({'title':title,'scope':scope,'rect':rect})
 if name=='DEQ':
  spec['primaryStyle']='bands'
  for b in range(4):
   x=48+b*278
   spec['sections'].append({'title':['01 / LOW','02 / LOW MID','03 / PRESENCE','04 / AIR'][b],'rect':[x,412,266,274]})
   for j,suffix in enumerate(['freq','gain','q']):
    add(f'b{b+1}{suffix}',x+7+j*84,449,84,157)
    spec['controls'][-1]['label']=['FREQ','GAIN','Q'][j]
   add(f'b{b+1}shape',x+12,610,242,40,'choice',['BELL','LOW SHELF','HIGH SHELF'])
   spec['controls'][-1]['label']=''
   module(f'LEARN BAND {b+1}',f'Band {b+1}',[x+16,654,234,24])
  spec['learningRect']=[48,712,464,312];spec['advancedRect']=[530,712,494,312]
 elif name=='DFX':
  spec['primaryStyle']='chain'
  groups=[('SATURATION','driveOn',['drive'],'Saturation'),('FILTER','filterOn',['filter'],'Filter'),('DELAY','delayOn',['time','feedback','delayMix'],'Delay'),('AMBIENCE','reverbOn',['decay','reverbMix'],'Ambience')]
  for b,(title,on,ids,scope) in enumerate(groups):
   x=48+b*278;spec['sections'].append({'title':f'0{b+1} / {title}','rect':[x,412,266,274]})
   add(on,x+170,420,84,46,'toggle',['OFF','ON']);spec['controls'][-1]['label']=''
   for j,id in enumerate(ids):add(id,x+6+j*(254/len(ids)),469,254/len(ids),170)
   module('LEARN MODULE',scope,[x+16,652,234,26])
  add('mix',559,756,200,192);add('output',793,756,200,192)
  spec['learningRect']=[48,712,464,312];spec['advancedRect']=[530,712,494,312]
 else:
  spec['learningRect']=[48,684,464,340];spec['advancedRect']=[530,684,494,340]
  primary=MAIN[name];slots=[68,224,380,684,840,996]
  if len(primary)==5:slots=[68,230,392,805,970]
  for i,id in enumerate(primary):
   opts=CHOICES.get(name,{}).get(id,[])
   add(id,slots[i],422,136,212,'choice' if opts else 'rotary',opts)
  remaining=[m for m in metadata if m['id'] not in primary]
  # Reverb mode belongs above the reference panel, just like the supplied sample.
  if name=='DVerb':
   add('mode',64,722,432,70,'choice',CHOICES[name]['mode'])
   rest=[m for m in remaining if m['id']!='mode']
  else:rest=remaining
  for j,m in enumerate(rest):
   options=CHOICES.get(name,{}).get(m['id'],[])
   if options:add(m['id'],550,736,450,70,'choice',options)
   else:
    # Five small reverb controls in a 3 + 2 arrangement; other advanced groups fit 1 row.
    x=550+(j%3)*150;y=734+(j//3)*129
    if name=='DDelay':y=842
    add(m['id'],x,y,144,123)
  # Module actions are not decorative: each routes to the corresponding scope.
  scopes=[]
  for m in metadata:
   if m['module'] not in scopes:scopes.append(m['module'])
  for j,scope in enumerate(scopes):
   module('LEARN '+scope.upper(),scope,[548+j*(460/len(scopes)),991,450/len(scopes),24])
 # Consistency tests: no missing controls or duplicate controls, every rectangle within logical canvas.
 ids=[c['id'] for c in spec['controls']]
 assert set(ids)=={m['id'] for m in metadata},(name,ids)
 assert len(ids)==len(set(ids)),name
 for c in spec['controls']:
  x,y,w,h=c['rect'];assert x>=0 and y>=0 and x+w<=1200 and y+h<=1120,(name,c)
 layouts.append(spec)
(ROOT/'Design/UI/Layouts.json').write_text(json.dumps(layouts,indent=2)+'\n')
print('Validated 10 layouts /',sum(len(s['controls']) for s in layouts),'unique parameter controls')
