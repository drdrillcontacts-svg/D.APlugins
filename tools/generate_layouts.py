"""Rack collection 02. Shared 1200 x 820 native/editor design geometry.
All 90 parameter IDs are retained. Advanced is a second control page, not deleted functionality.
Learn visibility is exported from the core policy, never decided by visual styling.
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
'DSess':('FREQUENCY DE-ESSER','DETECTION ENVELOPE','FOCUS THE BAND. PRESERVE THE VOICE.'),
'DGate':('GATE / LEVEL CONTROL','GATE CHARACTER','KEEP THE PERFORMANCE. CONTROL THE FLOOR.'),
'DFX':('MODULAR MULTI-EFFECTS','MASTER CONTROLS','SATURATION > FILTER > DELAY > AMBIENCE')}

MAIN={
'DVerb':['predelay','decay','tone','mix'],
'DDelay':['time','feedback','mix','width'],
'DComp':['threshold','ratio','attack','release','makeup'],
'DEQ':['b1gain','b2gain','b3gain','b4gain'],
'DSat':['drive','tone','density','mix'],
'DTrans':['attack','sustain','sensitivity','mix'],
'DStereo':['width','center','side','mono'],
'DSess':['frequency','threshold','range','mix'],
'DGate':['threshold','range','attack','release','output'],
'DFX':['drive','filter','time','mix']}
NAMES={'predelay':'PRE DELAY','lowcut':'LOW CUT','midcut':'MID CUT','frequency':'FREQUENCY','makeup':'MAKE UP','pingpong':'ROUTING','mode':'CHARACTER','driveOn':'SATURATION','filterOn':'FILTER','delayOn':'DELAY','reverbOn':'AMBIENCE','delayMix':'ECHO MIX','reverbMix':'SPACE MIX','mono':'MONO MONITOR','b1gain':'LOW GAIN','b2gain':'LOW MID GAIN','b3gain':'PRESENCE GAIN','b4gain':'AIR GAIN'}
CHOICES={'DVerb':{'mode':['ROOM','HALL','PLATE','SHIMMER']},'DDelay':{'pingpong':['STEREO','PING PONG']},'DSat':{'mode':['SOFT','HARD','ASYMMETRIC']},'DStereo':{'mono':['STEREO','MONO']}}
layouts=[]
for name,metadata in META.items():
 subtitle,title,slogan=TITLES[name]
 spec={'name':name,'subtitle':subtitle,'advancedTitle':title,'slogan':slogan,
       'canvas':[1200,820],'controls':[],'sections':[],'modules':[],
       'learningRect':[940,140,212,224],'analysisRect':[54,142,870,224]}
 primary=MAIN[name]
 remaining=[m['id'] for m in metadata if m['id'] not in primary]
 for advanced,ids in [(False,primary),(True,remaining)]:
  for i,id in enumerate(ids):
   m=next(m for m in metadata if m['id']==id)
   options=CHOICES.get(name,{}).get(id,[])
   if name=='DEQ' and id.endswith('shape'):options=['BELL','L-SHELF','H-SHELF']
   if id.endswith('On'):options=['OFF','ON']
   if not advanced:
    slots=[68,350,632,914] if len(ids)==4 else [48,274,500,726,952]
    rect=[slots[i],410,212 if len(ids)==4 else 180,188]
   else:
    cols=4 if len(ids)<=8 else 6
    width=1080/cols;rows=(len(ids)+cols-1)//cols
    rect=[60+(i%cols)*width,410+(i//cols)*114,width-10,108 if rows>1 else 184]
   if advanced and name=='DEQ':
    band=int(id[1])-1
    rect=[60+276*band+(130 if id[2:]=='q' else 0),410,124,164]
    if id.endswith('shape'):rect=[60+276*band,588,264,44]
   if advanced and name=='DFX':
    toggles=[v for v in remaining if v.endswith('On')]
    others=[v for v in remaining if not v.endswith('On')]
    if id in toggles:rect=[60+276*toggles.index(id),410,264,54]
    else:rect=[60+216*others.index(id),469,204,164]
   caption=NAMES.get(id,id.upper())
   if name=='DEQ' and advanced:caption='BAND '+id[1]+' / '+{'freq':'FREQ','q':'Q','shape':'SHAPE'}[id[2:]]
   if name=='DEQ' and id.endswith('shape'):caption=''
   spec['controls'].append({'id':id,'label':caption,'rect':rect,'style':'choice' if options else 'rotary',
                            'options':options,'advanced':advanced,'learnable':m['learnable']})
 scopes=list(dict.fromkeys(m['module'] for m in metadata if m['learnable']))
 for i,scope in enumerate(scopes):
  w=1080/len(scopes)
  spec['modules'].append({'title':'LEARN '+scope.upper(),'scope':scope,'rect':[60+i*w,610,w-12,25]})
 layouts.append(spec)
(ROOT/'Design/UI/Layouts.json').write_text(json.dumps(layouts,indent=2)+'\n')
print('Rack collection 02:',len(layouts),'products /',sum(len(s['controls']) for s in layouts),'controls /',sum(c['learnable'] for s in layouts for c in s['controls']),'Learn buttons')
