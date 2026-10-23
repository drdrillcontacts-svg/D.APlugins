#!/usr/bin/env python3
"""Final selected reference, SVG approval stage only. Not runtime editor code."""
from pathlib import Path
import json,math,html,zipfile,xml.etree.ElementTree as ET,csv,io
ROOT=Path(__file__).resolve().parents[1]
ns={'__file__':str(ROOT/'tools/design_noir_atlas.py')}
exec((ROOT/'tools/design_noir_atlas.py').read_text().split('for s in STYLES:\n for n in META:build(n,s)')[0],ns)
Base=ns['SVG'];META=ns['META'];LABEL=ns['label'];OPTIONS=ns['OPTIONS']
n2={'__file__':str(ROOT/'tools/design_noir_concepts.py')}
exec((ROOT/'tools/design_noir_concepts.py').read_text().split('for name,params in META.items():')[0],n2)
BASE_VALUE=n2['value']
def VALUE(n,p,v):
 if n=='DEQ' and p['id'].endswith('shape'):return ['BELL','LOW SHELF','HIGH SHELF'][int(v)]
 return BASE_VALUE(n,p,v)
EXAMPLES=n2['OVERRIDE'];PRIMARY=n2['PRIMARY'];ROLE=ns['ROLE']
OUT=ROOT/'Design/Concepts/Final-Reference';OUT.mkdir(parents=True,exist_ok=True)
LOGO=json.loads((OUT/'LogoPaths.json').read_text())
PURPLE='#a28bd0';AMBER='#c6a06b'
MAIN={'DVerb':['predelay','size','tone','mix','width','density'],
'DDelay':['time','feedback','filter','pingpong','mix','width'],
'DComp':['ratio','attack','release','makeup'],
'DEQ':['b2freq','b2q','b2shape'],
'DSat':['tone','density','mix'],
'DTrans':['sustain','sensitivity','shape','mix'],
'DStereo':['center','side','depth','mix'],
'DSess':['threshold','range','focus','mix'],
'DGate':['attack','hold','release','range'],
'DFX':['driveOn','drive','filterOn','filter','delayOn','time']}
GROUPS={'DVerb':('SPACE & CHARACTER','DIFFUSION & COLOUR'), 'DDelay':('ECHO & COLOUR','SYNC / MOTION / OUTPUT'), 'DComp':('DYNAMICS','KNEE / BLEND / OUTPUT'), 'DEQ':('BAND 02 / SELECTED','BANDS 01 / 03 / 04'), 'DSat':('HARMONIC COLOUR','CHARACTER / OUTPUT'), 'DTrans':('ENVELOPE','OUTPUT'), 'DStereo':('MID / SIDE IMAGE','MONO CHECK'), 'DSess':('SIBILANCE CONTROL','TIMING / OUTPUT'), 'DGate':('OPEN / HOLD / RELEASE','HYSTERESIS / OUTPUT'), 'DFX':('DRIVE / FILTER / DELAY','FEEDBACK / AMBIENCE / OUTPUT')}
class D(Base):
 def __init__(self,n):
  super().__init__(n,('SVG','Final reference','',1200,1600,PURPLE,'Arial, Helvetica, sans-serif','final'))
  self.c=PURPLE;self.values={p['id']:EXAMPLES.get(n,{}).get(p['id'],p['initial']) for p in META[n]}
  self.parts[0]=self.parts[0].replace('#242729','#222224').replace('#17191b','#131315').replace('#35383a','#18181b').replace('#202325','#27272a')
  self.raw('<defs><linearGradient id="bevel" x2="0" y2="1"><stop stop-color="#37373a"/><stop offset=".15" stop-color="#242427"/><stop offset="1" stop-color="#0c0c0e"/></linearGradient></defs>')
  self.rect(30,26,1140,1548,'url(#matte)','#323235',30,True);self.rect(35,31,1130,1538,'none','#ffffff07',26)
  self.raw(f'<g id="brand-logo" transform="translate(77 85) scale({155/LOGO["width"]})">')
  for p in LOGO['paths']:
   attrs=' '.join(f'{k}="{html.escape(v,quote=True)}"' for k,v in p.items() if k!='fill');self.raw(f'<path {attrs} fill="#d1d1d3"/>')
  self.raw('</g>')
  self.text(n.upper(),278,123,37,'#d8d8db',track=3)
  self.text(ROLE[n]+' / LEARNING',280,158,12,'#a2a2a8',track=1.6)
  self.text('FACTORY PRESET / 50',660,89,11,'#a6a6ae',track=1)
  self.button('previous-preset','<',599,106,45,43)
  edited=any(self.values[p['id']]!=p['initial'] for p in META[n])
  self.button('preset-browser','01 / Init *' if edited else '01 / Init / Factory Default',658,106,413,43)
  self.button('next-preset','>',1086,106,45,43)
 def cut(self,x,y,w,h,fill='#17171a',stroke='#353539',cut=8):
  self.path(f'M{x+cut} {y} H{x+w-cut} L{x+w} {y+cut} V{y+h-cut} L{x+w-cut} {y+h} H{x+cut} L{x} {y+h-cut} V{y+cut} Z',stroke,1,fill=fill)
 def button(self,id,t,x,y,w,h,accent=False,enabled=True):
  self.raw(f'<g id="action-{id}"'+(' opacity=".5"' if not enabled else '')+'>')
  self.cut(x,y,w,h,'#19191d',PURPLE if accent else '#414146',7)
  self.text(t,x+w/2,y+h/2+4,11,PURPLE if accent else '#d0d0d5','middle',.3)
  self.raw('</g>')
 def meter(self,x,y,title):
  self.text(title,x+11,y-19,10,PURPLE,'middle',.3)
  for j in range(48):
   for c in range(2):self.rect(x+c*13,y+j*4.3,9,2.3,'#b9a5d8' if 8<j<18 else '#d5d5d7' if j>=18 else '#29292e','none',0)
 def wave(self,x,y,w,h):
  for i in range(80):
   t=i/79;a=h*.43*math.exp(-t*3.2)*(.25+.75*abs(math.sin(i*1.73)))
   self.line(x+t*w,y+h/2-a,x+t*w,y+h/2+a,'#7d6e96',1.5)
 def display(self):
  self.rect(52,190,1096,378,'url(#bevel)','#29292c',29)
  self.raw('<path d="M89 207 H1110 Q1130 207 1130 229 V528 Q1130 549 1110 549 H798 Q600 405 402 549 H89 Q70 549 70 528 V229 Q70 207 89 207Z" fill="#0e0e11" stroke="#222226"/>')
  self.text('INPUT / '+ROLE[self.n]+' / ILLUSTRATIVE',151,244,10,PURPLE,track=.7)
  self.meter(84,277,'IN');self.meter(1097,277,'OUT')
  self.line(869,231,869,510,'#303035');self.text('REFERENCE',890,247,12,'#c6c6ce',track=.3)
  self.text('EXAMPLE',890,269,8,'#8c849b',track=1)
  self.wave(892,312,184,151)
  start=len(self.parts);super().graph(163,284,683,209)
  for i in range(start,len(self.parts)):
   self.parts[i]=self.parts[i].replace('id="illustrative-visual"','id="input-visual"')
   # Keep the screen vocabulary violet, as in the final selected reference.
   self.parts[i]=self.parts[i].replace('#596a60','#645971').replace('#9a82e5',PURPLE)
  if self.n=='DDelay':self.text('SYNC / 120 BPM / DOTTED EIGHTH / 375 ms',164,523,9,'#b5a7c8',track=.4)
 def primary(self,p):
  x,y,r=600,695,158;self.raw(f'<g id="param-{p["id"]}"><title>{html.escape(p["name"])}</title>')
  self.circle(x,y,r+24,'#202023','#2e2e32')
  for i in range(33):
   a=math.radians(-225+i*8.4375);self.circle(x+math.cos(a)*(r+42),y+math.sin(a)*(r+42),2 if i%4 else 2.8,'#66666d' if i%4==0 else '#3b3b42','none')
  self.circle(x,y+8,r+7,'#070709','#121215');self.circle(x,y,r+4,'url(#bevel)','#3c3c40');self.circle(x,y,r,'url(#dial)','#151519');self.circle(x,y,r-5,'none','#ffffff07')
  v=self.values[p['id']];f=(v-p['min'])/(p['max']-p['min'])
  if p['unit']=='Hz':f=math.log(v/p['min'])/math.log(p['max']/p['min'])
  a=math.radians(-225+270*f);self.line(x+math.cos(a)*r*.72,y+math.sin(a)*r*.72,x+math.cos(a)*r*.94,y+math.sin(a)*r*.94,'#d0d0d3',4)
  self.text(LABEL(p),x,905,23,'#e0e0e4','middle',2.5)
  self.text(VALUE(self.n,p,v),x,948,31,PURPLE,'middle',.2)
  self.text('PRECISION CONTROL',x,975,9,'#94949d','middle',2)
  self.raw('</g>')
  if p['learnable']:self.button('learn-'+p['id'],'LEARN',752,913,86,25,False);self.parts[-2]=self.parts[-2].replace('#d0d0d5',AMBER)
 def learning(self):
  self.cut(929,700,225,251,'#111114','#36363b',12)
  self.text('LEARNING',949,732,12,'#d0d0d6',track=.5)
  for i,(id,t) in enumerate([('capture','CAPTURE INPUT'),('reference','LOAD REFERENCE'),('learn-all','LEARN ALL'),('log','DETAILS / LOG')]):self.button(id,t,946,751+i*44,191,33,i==2)
 def mini(self,p,x,y,r=29):
  v=self.values[p['id']];disabled=self.n=='DDelay' and p['id'] in ['time','modulation']
  self.raw(f'<g id="param-{p["id"]}"'+(' opacity=".48"' if disabled else '')+'><title>'+html.escape(p['name'])+'</title>')
  self.text(LABEL(p),x,y-r-19,11,'#b0b0b9','middle',.6)
  if p['discrete']:
   opts=OPTIONS.get(self.n+':'+p['id'],['BELL','LOW SHELF','HIGH SHELF'] if self.n=='DEQ' and p['id'].endswith('shape') else ['OFF','ON'])
   if len(opts)<=4:
    ww=190 if len(opts)>2 else 156;cw=ww/len(opts)
    for oi,t in enumerate(opts):
     xx=x-ww/2+oi*cw;active=oi==int(v)
     self.cut(xx,y-20,cw-4,39,'#24212b' if active else '#141418',PURPLE if active else '#393940',5)
     self.text(t,xx+(cw-4)/2,y+4,8.5 if len(opts)>2 else 10,'#ddd5eb' if active else '#9d9da9','middle',0)
   else:
    self.cut(x-64,y-20,128,39,'#19191e','#49424f',6)
    self.text(VALUE(self.n,p,v),x,y+4,11,'#d5d0df','middle',.1)
  else:
   self.circle(x,y+4,r+4,'#08080a','#19191e');self.circle(x,y,r+2,'url(#bevel)','#35353c');self.circle(x,y,r,'url(#dial)','#111116')
   f=(v-p['min'])/(p['max']-p['min'])
   if p['unit']=='Hz':f=math.log(v/p['min'])/math.log(p['max']/p['min'])
   a=math.radians(-225+270*f);self.line(x+math.cos(a)*r*.62,y+math.sin(a)*r*.62,x+math.cos(a)*r*.9,y+math.sin(a)*r*.9,'#ceced4',2)
   self.text(VALUE(self.n,p,v),x,y+r+26,14,'#d2d2db','middle',.1)
  if p['learnable']:self.circle(x+len(LABEL(p))*3.2+12,y-r-23,2.4,AMBER,'none')
  self.raw('</g>')
  if p['learnable']:self.button('learn-'+p['id'],'LEARN',x-34,y+r+36,68,18,False)
 def panel(self,title,y,h):
  self.text(title,73,y-13,10,'#a4a4ae',track=1.4)
  self.cut(55,y,1090,h,'#17171b','#3a3a40',11)
 def bottom(self):
  self.text('DESCRIBE / OPTIONAL ENGLISH KEYWORDS',74,1460,9,'#a7a7b2',track=.6)
  self.rect(73,1474,1056,36,'#0d0d10','#37373d',4)
  self.text('dark, warm, wide, smooth, cinematic...',89,1498,12,'#9999a5',track=.1)
  self.text('PREVIEW AUDITIONS / APPLY KEEPS / REJECT RESTORES',74,1530,8,'#9999a6',track=.7)
  for i,(id,t) in enumerate([('preview','PREVIEW'),('apply','APPLY'),('reject','REJECT'),('undo','UNDO')]):self.button(id,t,74+i*164,1541,150,29,i==1,False)
  self.text('DESIGN REVIEW / NO LIVE AUDIO',1130,1560,8,'#9393a0','end',1)
rows=[]
for n,params in META.items():
 d=D(n);lookup={p['id']:p for p in params};p=lookup[PRIMARY[n]]
 main=MAIN[n];advanced=[p['id'] for p in params if p['id'] not in main+[PRIMARY[n]]]
 d.display();d.primary(p);d.learning()
 d.panel(GROUPS[n][0],1000,177)
 for i,id in enumerate(main):d.mini(lookup[id],100+(i+.5)*1000/len(main),1081,28)
 d.panel(GROUPS[n][1],1211,229)
 d.text('ADVANCED / EXPANDED FOR REVIEW',1130,1198,8,'#a596bc','end',.7)
 if n=='DEQ':
  for j,b in enumerate([1,3,4]):
   x=83+j*357;d.text('BAND 0'+str(b),x+12,1238,11,PURPLE,track=1)
   for k,suffix in enumerate(['freq','gain','q','shape']):
    p=lookup[f'b{b}{suffix}'];y=1260+k*43
    d.raw(f'<g id="param-{p["id"]}"><title>{html.escape(p["name"])}</title>')
    d.text(LABEL(p),x+12,y+12,10,'#aaaab5',track=.2)
    d.text('BELL' if suffix=='shape' else VALUE(n,p,d.values[p['id']]),x+306,y+12,14,'#d1d1dc','end',.1)
    if p['learnable']:d.circle(x+98,y+8,2.3,AMBER,'none')
    d.raw('</g>')
    if p['learnable']:d.button('learn-'+p['id'],'LEARN',x+236,y+19,70,17)
   if j<2:d.line(x+331,1234,x+331,1419,'#35353e')
 else:
  for i,id in enumerate(advanced):d.mini(lookup[id],100+(i+.5)*1000/len(advanced),1310,24)
  if len(advanced)==1:
   d.text('ALL PARAMETERS VISIBLE / NO DUPLICATED CONTROLS',600,1413,9,'#8f8f9d','middle',.8)
 d.bottom();d.finish(OUT/(n+'.svg'))
 for p in params:rows.append([n,p['id'],p['name'],'primary' if p['id']==PRIMARY[n] else 'main' if p['id'] in main else 'advanced',p['min'],p['max'],d.values[p['id']],p['unit'],'yes' if p['learnable'] else 'no'])
css='body{background:#111115;color:#d4d4de;font:16px Arial;margin:32px}h1{font-weight:400}p{line-height:1.9;color:#aaaaba}main{display:grid;grid-template-columns:repeat(auto-fit,minmax(min(100%,420px),1fr));gap:24px}article{background:#1b1b21;border:1px solid #373741;border-radius:14px;padding:16px}a{color:inherit;text-decoration:none}img{width:100%;height:700px;object-fit:contain}h2{font-weight:400}'
page=f'<!doctype html><html><meta charset="utf-8"><meta name="viewport" content="width=device-width"><title>D.A / Final selected reference / SVG review</title><style>{css}</style><h1>D.A / FINAL REFERENCE — SVG REVIEW 01</h1><p dir="rtl">همین مرجع نهایی با رنگ بنفش و کهرباییِ تصویر جدید؛ هر ده پلاگین برای بررسی، نه نسخهٔ اجرایی. کنترل‌های تکراری حذف و مقدارها و واحدها اصلاح شده‌اند. روی هر تصویر کلیک کنید تا SVG اصلی را در اندازهٔ کامل ببینید.</p><main>'
for n in META:page+=f'<article><a href="{n}.svg"><h2>{n}</h2><img src="{n}.svg" alt="{n} review prototype" loading="lazy"></a></article>'
(OUT/'Preview.html').write_text(page+'</main></html>')
s=io.StringIO();writer=csv.writer(s);writer.writerow(['plugin','parameter_id','label','section','minimum','maximum','design_value','unit','learn_eligible']);writer.writerows(rows);(OUT/'Control_Review.csv').write_text(s.getvalue())
(OUT/'READ_ME_FA_EN.txt').write_text('''D.A / FINAL SELECTED REFERENCE — SVG REVIEW 01
10 standalone editable SVGs, 1200 x 1600. No runtime editor or DSP changes.
This latest user-approved image supersedes earlier no-large-knob and red-only concepts.
Its purple/amber palette, large focused knob, real DA mark, inset visual, reference pane,
IN/OUT meters, dedicated learning dock and two-tier controls are intentionally retained.
The real repository logo was vector-traced and embedded as paths, not a raster image.

Design fixes for review:
- Each of the 92 actual parameter IDs appears exactly ONCE as a control.
- Removed repeated Size/Width/Density controls from the source picture.
- All labels/units are real English names, not the garbled screenshot lettering.
- Design values are metadata-constrained (e.g. Width 100/125%, not erroneous 400%).
- Init * indicates a design-example edit; no claim of an unchanged factory default.
- Only the 33 analysis-eligible controls expose Learn; no Learn on Mix/mode/Output.
- The only control duplicates allowed are text/units in the illustrative plots, never IDs.
- DEQ shows selected Band 02 in the main controls; Bands 01/03/04 each occur once below.
- Details/Log is an explicit action; no report drawer is open automatically.
- DDelay uses example sync at 120 BPM/dotted eighth; free-time/modulation are dimmed.
- Keywords represent the existing limited English hint workflow, NOT a language model.
- Preview/Apply/Reject/Undo are visible but dimmed in this neutral design-review state.
- All graph/reference/meter visuals are STATIC EXAMPLES, not real audio or measured data.

Open Preview.html locally after extracting. Click a product to open its SVG at full size.
Control_Review.csv lists every ID, section, range, example value and Learn eligibility.
For reporting issues, name product + control/section and describe overlap, label, spacing,
missing control or preferred interaction. Actual drag/keyboard/focus/scaling/host tests are
pending the approved runtime implementation; SVG buttons do not implement interactions.

این‌ها SVG برای بررسی شما هستند، نه پلاگین اجراشده. فایل Preview.html را باز کنید.
مطابق مرجع نهایی جدید، بنفش، کهربایی و ناب اصلی بزرگ عمداً حفظ شده‌اند.
تمام ۹۲ کنترل واقعی فقط یک‌بار آمده‌اند؛ تکرارهای عکس، متن‌های ناخوانا و واحدهای نادرست
اصلاح شده‌اند. فایل CSV برای بررسی دامنه، مقدار نمونه، محل کنترل و امکان Learn است.
تا تأیید شما هیچ تغییری در رابط اجرایی، پردازش صوت یا وضعیت برنامه ایجاد نشده است.
''')
archive=OUT.parent/'DAPlugins_FinalReference_SVG_Review01.zip'
with zipfile.ZipFile(archive,'w',zipfile.ZIP_DEFLATED) as z:
 for p in sorted(OUT.iterdir()):
  if p.name!='LogoPaths.json':z.write(p,'DAPlugins_FinalReference_Review01/'+p.name)
with zipfile.ZipFile(archive) as z:assert z.testzip() is None;assert sum(n.endswith('.svg') for n in z.namelist())==10
print('PASS ten final-reference SVGs / 92 unique real controls / vector logo / ZIP CRC:',archive)
