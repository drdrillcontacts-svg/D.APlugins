#!/usr/bin/env python3
"""Standalone editable SVG concept studies only. Does not change runtime layouts or DSP."""
from pathlib import Path
import json,math,html,zipfile,hashlib,xml.etree.ElementTree as ET
ROOT=Path(__file__).resolve().parents[1]
OUT=ROOT/'Design/Concepts/Noir-01';OUT.mkdir(parents=True,exist_ok=True)
META=json.loads((ROOT/'Design/UI/Parameters.json').read_text())
PRIMARY={'DVerb':'decay','DDelay':'division','DComp':'threshold','DEQ':'b2gain','DSat':'drive','DTrans':'attack','DStereo':'width','DSess':'frequency','DGate':'threshold','DFX':'mix'}
DESCRIPTIONS={'DVerb':'SPACE / REVERBERATION','DDelay':'TIME / STEREO ECHO','DComp':'CONTROL / COMPRESSION','DEQ':'SCULPT / FOUR-BAND EQUALIZER','DSat':'COLOUR / SATURATION','DTrans':'SHAPE / TRANSIENT DESIGN','DStereo':'IMAGE / STEREO FIELD','DSess':'FOCUS / SIBILANCE CONTROL','DGate':'ISOLATE / NOISE GATE','DFX':'FLOW / MULTI EFFECT'}
LABELS={'mode':'CHARACTER','predelay':'PRE-DELAY','lowcut':'LOW CUT','midcut':'MID CUT','pingpong':'ROUTING','modulation':'MODULATION','frequency':'DETECT FREQ','sync':'TEMPO','division':'DIVISION','mono':'MONO MONITOR','driveOn':'SATURATION','filterOn':'FILTER','delayOn':'DELAY','reverbOn':'AMBIENCE','delayMix':'DELAY MIX','reverbMix':'REVERB MIX'}
CHOICES={'DVerb:mode':['ROOM','HALL','PLATE','SHIMMER'],'DDelay:pingpong':['STEREO','PING PONG'],'DDelay:sync':['FREE MS','SYNC'],'DDelay:division':['1/32','1/32 D','1/32 T','1/16','1/16 D','1/16 T','1/8','1/8 D','1/8 T','1/4','1/4 D','1/4 T','1/2','1/2 D','1/1'],'DSat:mode':['SOFT','HARD','ASYMMETRIC'],'DStereo:mono':['STEREO','MONO']}
OVERRIDE={'DVerb':{'decay':2.4},'DDelay':{'sync':1,'division':7},'DEQ':{'b1gain':1.5,'b2gain':-2.5,'b3gain':2,'b4gain':1},'DTrans':{'attack':3.5,'sustain':-1.5},'DStereo':{'width':1.25}}
class Drawing:
 def __init__(self,name):
  self.name=name;self.parts=[f'''<svg xmlns="http://www.w3.org/2000/svg" width="1000" height="1280" viewBox="0 0 1000 1280" role="img" aria-labelledby="title description">
<title id="title">D.A Plugins / {name} / Noir 01 concept</title>
<desc id="description">Editable design proposal, not a running plugin. Original D.A branding; matte graphite, sculpted controls, restrained violet and amber. Graphs are illustrative, not live readings.</desc>
<defs>
 <linearGradient id="body" x2=".8" y2="1"><stop stop-color="#19191b"/><stop offset=".4" stop-color="#111113"/><stop offset="1" stop-color="#19191b"/></linearGradient>
 <linearGradient id="bevel" x2="0" y2="1"><stop stop-color="#343436"/><stop offset=".15" stop-color="#202022"/><stop offset="1" stop-color="#09090b"/></linearGradient>
 <linearGradient id="dial" x2="0" y2="1"><stop stop-color="#151517"/><stop offset="1" stop-color="#29292c"/></linearGradient>
 <radialGradient id="face"><stop stop-color="#222225"/><stop offset=".8" stop-color="#1d1d20"/><stop offset="1" stop-color="#131315"/></radialGradient>
 <linearGradient id="fade" x2="0" y2="1"><stop stop-color="#9580ef" stop-opacity=".28"/><stop offset="1" stop-color="#9580ef" stop-opacity="0"/></linearGradient>
 <filter id="shadow" x="-40%" y="-40%" width="180%" height="190%"><feDropShadow dx="0" dy="13" stdDeviation="15" flood-color="#000" flood-opacity=".7"/></filter>
 <filter id="glow" x="-100%" y="-100%" width="300%" height="300%"><feGaussianBlur stdDeviation="9"/></filter>
</defs>
<rect width="1000" height="1280" fill="#0d0d0f"/>
<rect x="39" y="33" width="922" height="1214" rx="31" fill="#080809" filter="url(#shadow)"/>
<rect x="40" y="34" width="920" height="1212" rx="30" fill="url(#body)" stroke="#29292b"/>
<rect x="44" y="38" width="912" height="1204" rx="27" fill="none" stroke="#ffffff" stroke-opacity=".025"/>
''']
 def raw(self,s):self.parts.append(s)
 def text(self,s,x,y,size=14,color='#939398',anchor='start',spacing=1.6,weight=400):
  self.raw(f'<text x="{x}" y="{y}" font-family="Arial, Helvetica, sans-serif" font-size="{size}" font-weight="{weight}" fill="{color}" text-anchor="{anchor}" letter-spacing="{spacing}">{html.escape(str(s))}</text>')
 def rect(self,x,y,w,h,fill='#141416',stroke='#2a2a2d',r=7):self.raw(f'<rect x="{x}" y="{y}" width="{w}" height="{h}" rx="{r}" fill="{fill}" stroke="{stroke}"/>')
 def path(self,path,color='#9d88eb',width=2,fill='none',opacity=1):self.raw(f'<path d="{path}" fill="{fill}" stroke="{color}" stroke-width="{width}" opacity="{opacity}" stroke-linecap="round" stroke-linejoin="round"/>')
 def circle(self,x,y,r,fill,stroke='none'):self.raw(f'<circle cx="{x}" cy="{y}" r="{r}" fill="{fill}" stroke="{stroke}"/>')
 def line(self,x1,y1,x2,y2,color='#3c3c40',width=1):self.path(f'M{x1} {y1} L{x2} {y2}',color,width)
 def group(self,id):self.raw(f'<g id="{id}">')
 def end(self):self.raw('</g>')
def value(name,p,v):
 id=p['id'];opts=CHOICES.get(name+':'+id)
 if p['discrete']:
  return opts[int(v)] if opts else ('ON' if v else 'OFF')
 if p['unit']=='Hz':return f'{v/1000:g} kHz' if v>=1000 else f'{v:g} Hz'
 if p['unit']=='dB':return f'{v:+g} dB' if v else '0 dB'
 if p['unit']==':1':return f'{v:g}:1'
 if p['unit']=='s':return f'{v:.2f} s'
 if p['unit']=='ms':return f'{v:g} ms'
 if p['unit']=='%' or id in ['mix','width','density','diffusion','modulation','sensitivity','depth','focus','feedback','delayMix','reverbMix']:return f'{round(v*100):d}%'
 return f'{v:g}'
def label(p):return LABELS.get(p['id'],p['name'].upper())
def knob(d,x,y,r,p,v,hero=False):
 d.group('param-'+p['id'])
 d.raw(f'<title>{html.escape(p["id"]+" / "+p["name"])}</title>')
 if hero:
  d.circle(x,y,r+23,'#1c1c1f');d.circle(x,y,r+23,'none','#28282c')
  for i in range(31):
   a=math.radians(-225+i*9);d.circle(round(x+math.cos(a)*(r+39),2),round(y+math.sin(a)*(r+39),2),1.7 if i%5 else 2.2,'#727276' if i%5==0 else '#36363b')
 d.raw(f'<circle cx="{x}" cy="{y+5}" r="{r+4}" fill="#050506" filter="url(#shadow)"/>')
 d.circle(x,y,r+4,'url(#bevel)');d.circle(x,y,r,'url(#dial)','#38383c');d.circle(x,y,r-4,'none','#ffffff08')
 fraction=(v-p['min'])/(p['max']-p['min'])
 if p['unit']=='Hz':fraction=math.log(v/p['min'])/math.log(p['max']/p['min'])
 a=math.radians(-225+270*fraction)
 d.line(round(x+math.cos(a)*r*.70,2),round(y+math.sin(a)*r*.70,2),round(x+math.cos(a)*r*.91,2),round(y+math.sin(a)*r*.91,2),'#d8d6dc',4 if hero else 2)
 if hero:
  d.text(label(p),x,y+r+66,19,'#d8d6dd','middle',3,600)
  d.text(value(d.name,p,v),x,y+r+105,27,'#a795ed','middle',1,400)
  if p['learnable']:
   d.text('LEARN',x+150,y+r+66,9,'#d19a50','middle',2)
 else:
  d.text(value(d.name,p,v),x,y+r+24,12,'#c7c5ce','middle',.3)
  d.text(label(p),x,y-r-17,10,'#929198','middle',1.2)
  if p['learnable']:d.circle(x+40,y-r-21,2,'#bb8a4d')
 d.end()
def graph(d,name):
 d.group('illustrative-display')
 d.text(DESCRIPTIONS[name],91,193,11,'#8a8990',spacing=2.4)
 d.circle(865,189,2.5,'#72628f');d.text('STUDY',881,193,10,'#66636e',anchor='end',spacing=1)
 x0,x1=140,860;cy=300
 if name in ['DVerb','DDelay','DGate','DTrans']:
  for i in range(121):
   x=x0+i*6;z=i/120
   if name=='DVerb':amp=(1-math.exp(-z*22))*math.exp(-z*3.6)*70*(.5+.5*abs(math.sin(i*1.91)))
   elif name=='DDelay':amp=sum(math.exp(-((z-c)/.024)**2)*h for c,h in [(.12,78),(.33,57),(.54,39),(.75,25),(.96,13)])
   elif name=='DGate':amp=(10+65*math.exp(-((z-.36)/.10)**2))*(.35+.65*abs(math.sin(i*1.4)))
   else:amp=(72*math.exp(-z*9)+23*math.exp(-z*1.8))*(.3+.7*abs(math.sin(i*1.3)))
   d.line(x,cy-amp,x,cy+amp,'#9a82e5',2.4)
  if name=='DDelay':
   for i,lab in enumerate(['1','2','3','4']):d.text(lab,228+i*152,399,10,'#66626f','middle',0)
  if name=='DGate':d.line(132,cy+15,868,cy+15,'#b78a4f',1)
 elif name=='DEQ':
  for x in [190,345,500,655,810]:d.line(x,230,x,365,'#232228')
  d.line(130,300,870,300,'#38353f')
  for j,(center,amp) in enumerate([(230,15),(390,-27),(620,22),(800,11)]):
   points=[(130+i*5,300-amp*math.exp(-((130+i*5-center)/62)**2)) for i in range(149)]
   d.path('M'+' L'.join(f'{x} {y:.2f}' for x,y in points),'#796794',1,opacity=.55)
   d.circle(center,300-amp,4,'#b9a3f1')
  points=[(130+i*5,300-sum(amp*math.exp(-((130+i*5-c)/62)**2) for c,amp in [(230,15),(390,-27),(620,22),(800,11)])) for i in range(149)]
  d.path('M'+' L'.join(f'{x} {y:.2f}' for x,y in points),'#b29aee',2.5)
  for x,t in [(150,'20'),(330,'100'),(500,'1k'),(680,'5k'),(850,'20k Hz')]:d.text(t,x,397,10,'#6a6671','middle',0)
 elif name in ['DComp','DSat']:
  d.line(230,370,770,370,'#2b292f');d.line(230,370,230,227,'#2b292f');d.line(260,365,742,230,'#48434f')
  pts=[]
  for i in range(101):
   x=260+i*4.8
   y=365-i*1.35 if i<55 else 365-55*1.35-(i-55)*.3375
   if name=='DSat':y=300-67*math.tanh((i-50)/24)
   pts.append((x,y))
  d.path('M'+' L'.join(f'{x:.2f} {y:.2f}' for x,y in pts),'#b19aed',3)
  d.text('INPUT',230,399,10,'#77727e');d.text('TRANSFER',770,399,10,'#77727e','end')
 elif name=='DStereo':
  d.line(370,390,630,230,'#2d2934');d.line(370,230,630,390,'#2d2934')
  for j in range(13):
   pts=[]
   for i in range(151):
    a=i/150*math.tau;x=500+(85+j*1.5)*math.sin(a)*math.sin(a*.03+j*.13);y=305+70*math.cos(a)
    pts.append((x,y))
   d.path('M'+' L'.join(f'{x:.2f} {y:.2f}' for x,y in pts),'#9e83d8',1,opacity=.23)
  d.text('L',347,316,12,'#77727e');d.text('R',650,316,12,'#77727e')
 elif name=='DSess':
  for i in range(100):
   x=150+i*7;h=16+25*abs(math.sin(i*.13))+45*math.exp(-((i-76)/10)**2)
   d.line(x,370,x,370-h,'#554866' if i<67 else '#aa8de6',2.5)
  d.path('M585 246 Q655 345 735 246','#ce9d62',1.5)
  d.text('SIBILANCE FOCUS / 6 kHz',500,399,10,'#8e849e','middle')
 elif name=='DFX':
  for i,(short,long) in enumerate([('01','DRIVE'),('02','FILTER'),('03','DELAY'),('04','SPACE')]):
   x=161+i*177;d.rect(x,261,145,87,'#121114','#333039',12);d.text(short,x+15,284,10,'#796a91');d.text(long,x+72,316,13,'#c0aecf','middle',2)
   if i<3:d.path(f'M{x+150} 305 h21 m-5 -4 l5 4 -5 4','#7c668f',1)
 d.text('CAPTURE',93,445,11,'#9b979f');d.circle(81,441,3,'#79747e')
 d.text('REFERENCE',215,445,11,'#706b77');d.text('DETAILS',887,445,11,'#9983ce','end')
 d.end()
for name,params in META.items():
 d=Drawing(name);vals={p['id']:OVERRIDE.get(name,{}).get(p['id'],p['initial']) for p in params}
 d.group('top-bar');d.text('D.A',78,100,25,'#d4d1d8',spacing=-1,weight=600)
 d.rect(300,65,376,46,'#0f0f11','#38373d',6);d.text('Factory presets / 50',317,94,13,'#aaa6b0',spacing=.7)
 d.line(581,65,581,111);d.line(629,65,629,111)
 d.path('M609 82 l-6 6 6 6','#bbb6c1',1.7);d.path('M648 82 l6 6 -6 6','#bbb6c1',1.7)
 d.rect(691,65,46,46,'#111113','#38373d',6);d.path('M706 79 h13 v18 h-13 z m4 5 h5 m-5 5 h5','#a7a0b0',1)
 for y in [80,88,96]:d.line(890,y,911,y,'#a9a5af',1.7)
 d.end()
 d.rect(57,137,886,357,'url(#bevel)','#29282e',28)
 d.raw('<path d="M85 149 H915 Q931 149 931 166 V458 Q931 480 910 480 H668 Q500 382 332 480 H90 Q70 480 70 458 V166 Q70 149 85 149Z" fill="#0d0c10" stroke="#202024"/>')
 d.text(name.upper(),91,236,25,'#d2cdd9',spacing=4,weight=500)
 d.text('RESET',887,237,10,'#6b656f','end',2)
 graph(d,name)
 p=next(p for p in params if p['id']==PRIMARY[name])
 knob(d,500,636,155,p,vals[p['id']],True)
 if name=='DDelay':d.text('SYNC / 120 BPM / 375 ms',500,920,10,'#827591','middle',1.5)
 elif name=='DEQ':d.text('SELECTED / BAND 02',500,920,10,'#827591','middle',1.5)
 else:d.text('PRECISION CONTROL',500,920,9,'#57525d','middle',2)
 if name=='DEQ':
  for b in range(1,5):
   x=87+(b-1)*213;d.rect(x,948,192,163,'#141316','#4e405d' if b==2 else '#252329',10)
   d.text('BAND '+str(b),x+16,972,11,'#b2a0ce' if b==2 else '#8a828f',spacing=2)
   for j,(suffix,lab) in enumerate([('freq','FREQ'),('gain','GAIN'),('q','Q'),('shape','SHAPE')]):
    p=next(p for p in params if p['id']==f'b{b}{suffix}');v=vals[p['id']]
    d.group(('mirror-' if p['id']==PRIMARY[name] else 'param-')+p['id']);d.text(lab,x+16,1001+j*28,9,'#746e7d',spacing=1)
    out='BELL' if suffix=='shape' else value(name,p,v)
    d.text(out,x+176,1001+j*28,12,'#bcb5c6','end',.3)
    if p['learnable']:d.circle(x+61,997+j*28,2,'#b38a52')
    d.end()
 else:
  other=[p for p in params if p['id']!=PRIMARY[name]];count=len(other);rows=2 if count>6 else 1;cols=math.ceil(count/rows)
  for i,p in enumerate(other):
   row=i//cols;col=i%cols;number=min(cols,count-row*cols);x=119+(col+.5)*(762/number);y=985+row*101 if rows==2 else 1035
   if p['discrete']:
    d.group('param-'+p['id']);d.text(label(p),x,y-42,10,'#918899','middle',1)
    v=value(name,p,vals[p['id']]);d.rect(x-53,y-23,106,38,'#18141e','#493a5c',7)
    d.text(v,x,y+1,10,'#b5a0d9','middle',.5);d.end()
   else:
    if name=='DDelay' and p['id'] in ['time','modulation']:d.raw('<g opacity=".36">')
    knob(d,x,y,22,p,vals[p['id']])
    if name=='DDelay' and p['id'] in ['time','modulation']:d.end()
 d.line(83,1141,917,1141,'#28252e')
 for x,t,c in [(165,'CAPTURE INPUT','#8d8597'),(382,'REFERENCE','#8d8597'),(602,'LEARN ALL','#d39c55'),(817,'DETAILS / LOG','#8d8597')]:
  d.text(t,x,1178,10,c,'middle',1.3)
 d.text('D.A PLUGINS / NOIR 01',84,1220,9,'#625b6d',spacing=2)
 d.text('DESIGN STUDY / NO LIVE AUDIO',916,1220,8,'#625b6d','end',1.3)
 for x in [66,934]:
  for y in [537,1209]:
   for j in range(3):d.circle(x+(j%2)*9*(1 if x<500 else -1),y+(j//2)*9,2.8,'#222126','#111113')
 d.raw('</svg>')
 text='\n'.join(d.parts);ET.fromstring(text)
 for p in params:assert f'id="param-{p["id"]}"' in text,(name,p['id'])
 (OUT/(name+'.svg')).write_text(text)
readme='''D.A Plugins / NOIR 01 — proposals for approval, not a finished interface.
Ten independently editable standalone SVG files, 1000 × 1280 each.
Original D.A identity; matte graphite, soft sculpted knobs, restrained violet display
and amber learning accents, inspired by the supplied mood reference, not its branding.
No raster images or external asset/font/network dependencies. Text stays editable;
Arial/Helvetica/system sans-serif fallbacks. All existing parameter IDs are represented.
Hero controls: DVerb Decay; DDelay Division; DComp Threshold; DEQ selected-band Gain;
DSat Drive; DTrans Attack; DStereo Width; DSess Detection Frequency; DGate Threshold;
DFX Mix. DEQ uses four complete band cards; DFX shows its actual four-stage route.
Only audio-informed parameters get the small amber eligibility marker. LEARN ALL,
Reference and Capture are design representations of existing workflows, not new DSP.
DETAILS / LOG is explicit and closed. No automatic popup is depicted.
Graphs are illustrative vector drawings, not measured responses or live audio.
Values are design examples; no claim that this is a selected real factory preset.
DDelay illustration uses Sync, dotted eighth at 120 BPM; free Time/Modulation are dimmed.
No chorus/tremolo/reverse module from the reference is invented for the D.A products.
This folder is separate from runtime UI assets; no plugin/editor/DSP changes were made.

این‌ها فقط ده پیشنهاد طراحی برای بررسی شما هستند، نه رابط نهایی یا نسخهٔ اجرایی.
پس از تأیید شما، طراحی به کنترل‌های واقعی تبدیل می‌شود. نمودارها نمایشی‌اند.
همهٔ کنترل‌های فعلی حفظ شده‌اند؛ جزئیات و گزارش فقط با انتخاب صریح باز می‌شوند.
فایل‌ها وکتور مستقل و قابل ویرایش‌اند؛ بدون عکسِ جاسازی‌شده یا وابستگی به اینترنت.
'''
(OUT/'READ_ME.txt').write_text(readme)
archive=OUT.parent/'DAPlugins_Noir01_10_SVG.zip'
with zipfile.ZipFile(archive,'w',zipfile.ZIP_DEFLATED) as z:
 for p in sorted(OUT.iterdir()):
  if p.suffix=='.svg' or p.name=='READ_ME.txt':z.write(p,'DAPlugins_Noir01/'+p.name)
with zipfile.ZipFile(archive) as z:
 assert len([n for n in z.namelist() if n.endswith('.svg')])==10
 assert z.testzip() is None
print('PASS: 10 standalone SVGs, all 92 parameter IDs, XML validation and ZIP CRC',archive)
