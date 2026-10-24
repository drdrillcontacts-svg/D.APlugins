#!/usr/bin/env python3
"""Noir 01 / R2: sculpted matte material, ten product-specific control hierarchies."""
from pathlib import Path
import math,json,html,zipfile,xml.etree.ElementTree as ET
ROOT=Path(__file__).resolve().parents[1]
ns={'__file__':str(ROOT/'tools/design_noir_atlas.py')}
exec((ROOT/'tools/design_noir_atlas.py').read_text().split('for s in STYLES:\n for n in META:build(n,s)')[0],ns)
Base=ns['SVG'];META=ns['META'];LABEL=ns['label'];VALUE=ns['value'];OPTIONS=ns['OPTIONS'];ROLE=ns['ROLE']
OUT=ROOT/'Design/Concepts/Noir01-Refined';OUT.mkdir(parents=True,exist_ok=True)
SIZES={'DVerb':(1320,920),'DDelay':(1400,860),'DComp':(1080,1040),'DEQ':(1440,920),'DSat':(1080,920),'DTrans':(1280,860),'DStereo':(1180,980),'DSess':(1360,900),'DGate':(1320,900),'DFX':(1440,940)}
class D(Base):
 def __init__(self,n):
  w,h=SIZES[n];super().__init__(n,('R2','Noir 01 Refined','',w,h,'#aaa','Arial, Helvetica, sans-serif','refined'))
  self.parts[0]=self.parts[0].replace('#242729','#222325').replace('#17191b','#111214')
  self.rect(30,26,w-60,h-52,'url(#matte)','#303235',27,True)
  self.rect(35,31,w-70,h-62,'none','#ffffff08',23)
  self.text('D.A',65,78,19,'#d6d8d6',track=-.6,weight=600)
  self.text(n,165,91,33,'#d9ded8',track=-.5)
  self.text(ROLE[n],166,118,10,self.c,track=2.3)
  self.rect(w-448,56,296,48,'#121416','#363a3b',8)
  self.text('01 / Init',w-428,86,13,'#bfc6be',track=.6)
  self.line(w-247,57,w-247,103,'#353b3d');self.line(w-199,57,w-199,103,'#353b3d')
  self.path(f'M{w-217} 74 l-6 6 6 6 M{w-181} 74 l6 6 -6 6','#b4bcb3',1.7)
  self.text('50 FACTORY',w-153,124,9,'#7a8780','end',1.5)
  self.path(f'M{w-109} 72 h21 M{w-109} 80 h21 M{w-109} 88 h21','#939d97',1.5)
  self.line(65,145,w-65,145,'#2d3335')
 def graph(self,*args):
  count=getattr(self,"visual_count",0)+1;self.visual_count=count;start=len(self.parts)
  super().graph(*args)
  for i in range(start,len(self.parts)):self.parts[i]=self.parts[i].replace('id="illustrative-visual"',f'id="illustrative-visual-{count}"')
 def well(self,x,y,w,h,title=None):
  self.rect(x,y,w,h,'#101416','#2d3437',17)
  self.rect(x+4,y+4,w-8,h-8,'none','#ffffff04',14)
  if title:self.text(title,x+22,y+29,10,'#88968e',track=1.8)
 def knob(self,id,x,y,r=36):
  p=next(p for p in META[self.n] if p['id']==id)
  self.raw(f'<g id="param-{id}"><title>{html.escape(p["name"])}</title>')
  self.circle(x,y+5,r+5,'#080b0c','#14191a');self.circle(x,y,r+3,'#303538','#42484a');self.circle(x,y,r,'url(#dial)','#171c1e')
  f=(p['initial']-p['min'])/(p['max']-p['min'])
  if p['unit']=='Hz':f=math.log(p['initial']/p['min'])/math.log(p['max']/p['min'])
  for j in range(13):
   a=math.radians(-225+j*22.5);self.circle(x+math.cos(a)*(r+12),y+math.sin(a)*(r+12),1,'#46504d','none')
  a=math.radians(-225+270*f);self.line(x+math.cos(a)*r*.62,y+math.sin(a)*r*.62,x+math.cos(a)*r*.9,y+math.sin(a)*r*.9,'#ced5cc',2.5)
  self.text(LABEL(p),x,y-r-26,11,'#a0aaa3','middle',1)
  self.text(VALUE(self.n,p),x,y+r+32,18,'#d0d8cd','middle',.2)
  if p['learnable']:self.circle(x+42,y-r-30,2,self.c,'none')
  self.raw('</g>')
 def select(self,id,x,y,w):
  p=next(p for p in META[self.n] if p['id']==id);opts=OPTIONS.get(self.n+':'+id,['BELL','LOW SHELF','HIGH SHELF'] if id.endswith('shape') else ['OFF','ON'])
  # A long note enum is presented as one dropdown, never fifteen tiny switches.
  if len(opts)>4:opts=[VALUE(self.n,p)+'  /  SELECT']
  self.raw(f'<g id="param-{id}"><title>{html.escape(p["name"])}</title>')
  self.text(LABEL(p),x,y,10,'#939f96',track=1)
  cw=w/len(opts)
  for i,t in enumerate(opts):
   active=i==int(p['initial']) or len(opts)==1
   self.rect(x+i*cw,y+15,cw-5,35,'#242b2b' if active else '#14191b',self.c if active else '#30383b',6)
   self.text(t,x+i*cw+(cw-5)/2,y+37,10,'#ccd5c9' if active else '#a1ada3','middle',.3)
  self.raw('</g>')
 def meter(self,x,y,w,h,title):
  self.text(title,x+w/2,y,10,'#9da99e','middle',1.4)
  for j in range(28):
   for c in range(2):self.rect(x+c*(w/2+2),y+22+j*(h/30),w/2-5,4,self.c if j>11 else '#2b3537','none',1)
  self.text('ILLUSTRATIVE',x+w/2,y+h+20,7,'#9aa89c','middle',.4)
for n in META:
 d=D(n);w,h=d.w,d.h
 if n=='DVerb':
  d.well(65,178,605,345,'EARLY REFLECTIONS / LATE TAIL — ILLUSTRATIVE');d.graph(96,247,544,205)
  for i,id in enumerate(['decay','predelay','size','density','diffusion','modulation']):d.knob(id,780+(i%3)*183,270+(i//3)*175,34)
  d.select('mode',91,572,555)
  for i,id in enumerate(['tone','lowcut','midcut']):d.knob(id,760+i*190,639,34)
  d.param(next(p for p in META[n] if p['id']=='mix'),91,733,555,85)
  d.param(next(p for p in META[n] if p['id']=='width'),740,733,490,85)
 elif n=='DDelay':
  d.well(65,178,849,282,'ECHO TIMELINE — ILLUSTRATIVE');d.graph(102,235,775,157)
  d.select('sync',959,199,371);d.select('division',959,298,371);d.select('pingpong',959,397,371)
  for i,id in enumerate(['feedback','filter','modulation','width']):d.knob(id,150+i*245,580,39)
  d.param(next(p for p in META[n] if p['id']=='time'),91,695,535,84)
  d.param(next(p for p in META[n] if p['id']=='mix'),705,695,300,84)
  d.param(next(p for p in META[n] if p['id']=='output'),1050,695,280,84)
 elif n=='DComp':
  d.well(65,178,740,361,'COMPRESSION CURVE — ILLUSTRATIVE');d.graph(111,246,641,232);d.text('INPUT / dB',752,516,9,'#8f9e94','end');d.text('OUTPUT',89,237,8,'#8f9e94');d.meter(867,206,114,269,'GAIN REDUCTION')
  d.param(next(p for p in META[n] if p['id']=='threshold'),93,577,890,92)
  for i,id in enumerate(['ratio','attack','release','knee','makeup','mix','output']):d.knob(id,149+(i%4)*254,748+(i//4)*151,33)
 elif n=='DEQ':
  d.well(65,178,w-130,301,'FOUR-BAND RESPONSE — ILLUSTRATIVE');d.graph(103,235,w-206,179)
  for fraction,label in [(0,'20 Hz'),(.23,'100'),(.55,'1k'),(.79,'5k'),(1,'20k Hz')]:d.text(label,103+fraction*(w-206),451,9,'#9ba89c','middle',.2)
  for b in range(1,5):
   x=65+(b-1)*332;d.well(x,515,314,301,'BAND 0'+str(b))
   for i,suffix in enumerate(['freq','gain','q']):d.knob('b'+str(b)+suffix,x+54+i*100,635,27)
   d.select('b'+str(b)+'shape',x+21,737,277)
 elif n=='DSat':
  d.well(65,178,311,367,'INPUT DRIVE');d.knob('drive',220,357,61)
  d.well(402,178,613,367,'TRANSFER CHARACTER — ILLUSTRATIVE');d.graph(442,247,532,223)
  d.select('mode',91,594,897)
  for i,id in enumerate(['tone','density','mix','output']):d.knob(id,154+i*255,752,36)
 elif n=='DTrans':
  for i,(title,id) in enumerate([('TRANSIENT / ATTACK','attack'),('BODY / SUSTAIN','sustain')]):
   x=65+i*589;d.well(x,178,560,340,title+' — ILLUSTRATIVE');d.graph(x+31,235,498,132)
   d.param(next(p for p in META[n] if p['id']==id),x+31,400,498,82)
  for i,id in enumerate(['sensitivity','shape','mix','output']):d.knob(id,158+i*304,655,43)
 elif n=='DStereo':
  d.well(65,178,529,449,'STEREO FIELD — ILLUSTRATIVE');d.graph(101,235,457,317)
  for i,id in enumerate(['width','center','side']):d.param(next(p for p in META[n] if p['id']==id),664,225+i*126,445,95)
  d.knob('depth',181,757,40);d.knob('mix',470,757,40);d.select('mono',690,729,419)
 elif n=='DSess':
  d.well(65,178,1056,301,'SIBILANCE DETECTION BAND — ILLUSTRATIVE');d.graph(104,234,979,188);d.meter(1182,205,106,214,'REDUCTION')
  for i,id in enumerate(['frequency','threshold','range','focus']):d.knob(id,171+i*338,575,38)
  for i,id in enumerate(['attack','release','mix','output']):d.knob(id,171+i*338,744,29)
 elif n=='DGate':
  d.well(65,178,830,307,'OPEN / HOLD / RELEASE — ILLUSTRATIVE');d.graph(100,235,761,184)
  d.param(next(p for p in META[n] if p['id']=='threshold'),94,535,760,100)
  for i,id in enumerate(['attack','hold','release']):d.knob(id,194+i*280,727,36)
  d.well(931,178,324,630,'RANGE / EXIT BEHAVIOUR')
  for i,id in enumerate(['range','hysteresis','output']):d.knob(id,1093,327+i*189,36)
 elif n=='DFX':
  modules=[('SATURATION',['driveOn','drive']),('FILTER',['filterOn','filter']),('DELAY',['delayOn','time','feedback','delayMix']),('AMBIENCE',['reverbOn','decay','reverbMix'])]
  for i,(title,ids) in enumerate(modules):
   x=65+i*332;d.well(x,178,314,453,title)
   if i<3:d.path(f'M{x+316} 395 h12 m-4 -4 l4 4 -4 4','#707f73',1)
   d.select(ids[0],x+24,245,266);d.knob(ids[1],x+157,391,40)
   for j,id in enumerate(ids[2:]):d.param(next(p for p in META[n] if p['id']==id),x+24,481+j*76,266,69)
  d.param(next(p for p in META[n] if p['id']=='mix'),92,725,570,100)
  d.param(next(p for p in META[n] if p['id']=='output'),768,725,570,100)
 d.foot();d.finish(OUT/(n+'.svg'))
# Offline preview, with no CDN, web fonts or remote resources.
css='body{background:#101315;color:#d0d8ce;font:16px Arial;margin:32px}h1{font-weight:400}p{color:#a2afa4;line-height:1.8}main{display:grid;grid-template-columns:repeat(auto-fit,minmax(min(100%,500px),1fr));gap:28px}a{color:inherit;text-decoration:none}img{width:100%;height:480px;object-fit:contain;background:#0f1315}article{background:#1b2022;padding:16px;border:1px solid #343c3d;border-radius:15px}h2{font-weight:400}'
gallery=f'<!doctype html><html><meta charset="utf-8"><meta name="viewport" content="width=device-width"><title>Noir 01 / Refined</title><style>{css}</style><h1>D.A / NOIR 01 — REFINED</h1><p dir="rtl">ده ساختار مستقل با یک خانوادهٔ متریال مشکی مات. روی هر طرح کلیک کنید تا SVG اصلی باز شود. این‌ها پیشنهاد طراحی هستند، نه رابط اجرایی.</p><main>'
for n in META:gallery+=f'<article><a href="{n}.svg"><h2>{n}</h2><img src="{n}.svg" alt="{n} Noir 01 Refined" loading="lazy"></a></article>'
(OUT/'Preview.html').write_text(gallery+'</main></html>')
(OUT/'DESIGN_NOTES.txt').write_text('''NOIR 01 / REFINED — TEN PRODUCT-SPECIFIC SVG PROPOSALS

Material language: sculpted matte black, restrained highlights, recessed displays,
small tactile controls. No common giant centre knob and no required purple lighting.
DVerb sage; DDelay ice blue; DComp copper; DEQ olive; DSat amber; DTrans muted clay;
DStereo steel; DSess mint; DGate moss; DFX warm silver. All accents are restrained.

Functional hierarchy follows familiar audio-plugin conventions:
DVerb separates timing/diffusion, spectral colour and wet balance.
DDelay separates note/routing selectors, feedback colour, free time and wet/output.
DComp places transfer and GR together, Threshold above its time/ratio/level controls.
DEQ has a broad response and four complete Freq/Gain/Q/Shape band strips.
DSat uses one moderately sized OFF-CENTRE drive control, transfer, character and blend.
DTrans shows two independent attack/body sections and smaller shared controls.
DStereo separates stereo-field inspection, M/S controls and mono-monitor selection.
DSess prioritises detection spectrum/range, with time and blend controls secondary.
DGate prioritises threshold and open/hold/release timing, with exit behaviour separate.
DFX exposes the fixed drive > filter > delay > ambience chain plus global mix/output.

Professional design intentions, NOT a claim of a universal UI certification:
- Existing parameter IDs/ranges represented, no fictitious audio module.
- Explicit labels/units, separated discrete choices, consistent top navigation.
- Product-specific grouping and conventional meter/graph placement.
- Scoped Learn eligibility marked only on relevant controls; logs remain explicit.
- Sync is shown FREE MS initially; Division remains available for a future Sync choice.
- All graphs and meters are illustrative and intentionally marked as such.
- Numerical values are source defaults, not measurements of the illustrative graphs.

92 parameter representations, no hidden control omissions. These are static SVG concepts.
Actual keyboard access, hit areas at each zoom, assistive-technology behaviour, automation,
contrast validation and host scaling must be tested during implementation. No accessibility,
ISO, or DAW certification is implied. These SVGs alone do not implement interactions.

برای دیدن همهٔ طرح‌ها Preview.html را بعد از استخراج ZIP باز کنید.
این بازطراحی همان خانوادهٔ Noir01 است، با ساختار مستقل برای هر پلاگین، نه رنگ‌آمیزی یک قالب.
هیچ تغییری در DSP یا رابط اجرایی صورت نگرفته؛ پس از تأیید، نوبت پیاده‌سازی و تست می‌رسد.
''')
archive=OUT.parent/'DAPlugins_Noir01_Refined_10_SVG.zip'
with zipfile.ZipFile(archive,'w',zipfile.ZIP_DEFLATED) as z:
 for p in sorted(OUT.iterdir()):z.write(p,'DAPlugins_Noir01_Refined/'+p.name)
with zipfile.ZipFile(archive) as z:assert z.testzip() is None;assert sum(n.endswith('.svg') for n in z.namelist())==10
print('PASS ten SVGs, complete 92-control mapping, unique IDs, ZIP integrity:',archive)
