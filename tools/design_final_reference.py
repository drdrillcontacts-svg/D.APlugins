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

def ctrl_footprint(n,p,force_knob=False):
 if p['discrete'] and not force_knob:
  opts=OPTIONS.get(n+':'+p['id'],['BELL','LOW SHELF','HIGH SHELF'] if n=='DEQ' and p['id'].endswith('shape') else ['OFF','ON'])
  if opts==['OFF','ON']:return 140
  if len(opts)==2:return 264
  if len(opts)==3:return 420
  if len(opts)==4:return 428
 return 104

def layout_row(n,ids,lookup):
 weights=[max(ctrl_footprint(n,lookup[i],i==PRIMARY[n])+28,140) for i in ids]
 total=sum(weights);cur=75.0;span=1050.0;xs=[]
 for w in weights:
  slot=span*(w/total);xs.append(cur+slot/2.0);cur+=slot
 return xs

class D(Base):
 def text(self,t,x,y,size=13,color='#a0a7a6',align='start',track=1,font=None,weight=400):
  # Logo is vector geometry; the product-name text is intentionally untouched.
  if str(t)==self.n.upper() and x==278 and y==123:
   return super().text(t,x,y,size,color,align,track,font,weight)
  original_size=size
  size=16 if size<14 else size+6 if size<=19 else size+2
  track=min(track,.55) if original_size<20 else track
  if original_size<14:weight=max(weight,600)
  elif original_size<=19:weight=max(weight,500)
  if color.startswith('#') and len(color)==7:
   rgb=[int(color[i:i+2],16) for i in (1,3,5)]
   if max(rgb)<185:color='#'+''.join(f'{min(v+28,255):02x}' for v in rgb)
  return super().text(t,x,y,size,color,align,track,font,weight)
 def __init__(self,n):
  super().__init__(n,('SVG','Final reference','',1200,1240,PURPLE,'Arial, Helvetica, sans-serif','final'))
  self.c=PURPLE;self.values={p['id']:EXAMPLES.get(n,{}).get(p['id'],p['initial']) for p in META[n]}
  self.parts[0]=self.parts[0].replace('#242729','#222224').replace('#17191b','#131315').replace('#35383a','#18181b').replace('#202325','#27272a')
  self.raw('''<defs>
<linearGradient id="bevel" x2="0" y2="1"><stop stop-color="#37373a"/><stop offset=".15" stop-color="#242427"/><stop offset="1" stop-color="#0c0c0e"/></linearGradient>
<linearGradient id="analog-skirt" x1=".2" y1="0" x2=".8" y2="1"><stop stop-color="#575362"/><stop offset=".45" stop-color="#2a2931"/><stop offset="1" stop-color="#0e0e12"/></linearGradient>
<radialGradient id="analog-body" cx="35%" cy="30%" r="70%"><stop stop-color="#4d4957"/><stop offset=".55" stop-color="#24232a"/><stop offset="1" stop-color="#121116"/></radialGradient>
<linearGradient id="analog-cap" x1="0" y1="0" x2="1" y2="1"><stop stop-color="#433f4d"/><stop offset=".5" stop-color="#232129"/><stop offset="1" stop-color="#322f3b"/></linearGradient>
<linearGradient id="switch-active" x1="0" y1="0" x2="0" y2="1"><stop stop-color="#372e47"/><stop offset="1" stop-color="#211c2b"/></linearGradient>
<linearGradient id="switch-idle" x1="0" y1="0" x2="0" y2="1"><stop stop-color="#232329"/><stop offset="1" stop-color="#151519"/></linearGradient>
</defs>''')
  self.rect(30,26,1140,1188,'url(#matte)','#323235',30,True);self.rect(35,31,1130,1178,'none','#ffffff07',26)
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
 def screw(self,x,y):
  self.circle(x,y,4.2,'#141418','#3a3942',1);self.line(x-2.4,y-1.2,x+2.4,y+1.2,'#565460',1.1)
 def button(self,id,t,x,y,w,h,accent=False,enabled=True):
  h=max(h,26)
  self.raw(f'<g id="action-{id}"'+(' opacity=".75"' if not enabled else '')+'>')
  self.cut(x,y,w,h,'url(#switch-active)' if accent else '#19191d',PURPLE if accent else '#43434a',7)
  self.text(t,x+w/2,y+h/2+5,11,PURPLE if accent else '#d4d4dc','middle',.3)
  self.raw('</g>')
 def meter(self,x,y,title):
  self.text(title,x+11,y-19,10,PURPLE,'middle',.3)
  for j in range(44):
   for c in range(2):self.rect(x+c*13,y+j*4.3,9,2.3,'#b9a5d8' if 7<j<16 else '#d5d5d7' if j>=16 else '#29292e','none',0)
 def display(self):
  self.rect(52,184,1096,352,'url(#bevel)','#29292c',29)
  self.raw('<path d="M89 200 H1110 Q1130 200 1130 222 V502 Q1130 523 1110 523 H89 Q70 523 70 502 V222 Q70 200 89 200Z" fill="#0e0e11" stroke="#222226"/>')
  self.text('INPUT / '+ROLE[self.n]+' / ILLUSTRATIVE',151,236,10,PURPLE,track=.7)
  self.meter(84,268,'IN');self.meter(1097,268,'OUT')
  start=len(self.parts);super().graph(163,272,900,196)
  for i in range(start,len(self.parts)):
   self.parts[i]=(self.parts[i]
    .replace('id="illustrative-visual"','id="input-visual"')
    .replace('#596a60','#645971')
    .replace('#24352d','#211a2b')
    .replace('#586d64','#685c79')
    .replace('#191e20','#17151d')
    .replace('#46504c','#473f54')
    .replace('#59635e','#625872')
    .replace('#465044','#3b3547')
    .replace('#374449','#383244')
    .replace('#9a82e5',PURPLE))
  if self.n=='DDelay':self.text('SYNC / 120 BPM / DOTTED EIGHTH / 375 ms',164,500,9,'#b5a7c8',track=.4)
 def learning(self):
  self.cut(55,552,1090,68,'#111114','#36363b',10)
  self.text('LEARNING',76,592,12,'#d0d0d6',track=.5)
  for i,(id,t) in enumerate([('capture','CAPTURE INPUT'),('reference','REFERENCE...'),('learn-all','LEARN ALL'),('log','DETAILS / LOG')]):self.button(id,t,230+i*219,568,205,36,i==2)
 def analog_knob(self,x,y,r,f):
  # Outer calibration scale (21 analog hardware ticks from -225 deg to +45 deg)
  for ti in range(21):
   tf=ti/20.0;ang=math.radians(-225+270*tf);major=(ti%5==0)
   r0=r+6.5;r1=r+(13.0 if major else 10.2)
   active=tf<=f+1e-4
   col=AMBER if (active and major) else (PURPLE if active else ('#928e9e' if major else '#55535f'))
   self.line(x+math.cos(ang)*r0,y+math.sin(ang)*r0,x+math.cos(ang)*r1,y+math.sin(ang)*r1,col,2.1 if major else 1.3)
  # Hardware drop shadow + recessed mounting ring + sloped metallic skirt
  self.raw(f'<circle cx="{x:.2f}" cy="{y+5:.2f}" r="{r+4:.2f}" fill="#050507" filter="url(#smallshadow)"/>')
  self.circle(x,y,r+4.5,'#0b0b0f','#26252d',1.2)
  self.circle(x,y,r+3,'url(#analog-skirt)','#44414e',1)
  # 28 fluted / knurled analog grip ridges around circumference
  for ki in range(28):
   kang=math.radians(ki*360.0/28.0)
   top_lit=math.cos(kang+math.radians(45))>0
   kcol='#6a6677' if top_lit else '#1c1b22'
   self.line(x+math.cos(kang)*(r-1.2),y+math.sin(kang)*(r-1.2),x+math.cos(kang)*(r+2.8),y+math.sin(kang)*(r+2.8),kcol,1.8)
  # Sculpted 3D analog body + brushed center cap
  self.circle(x,y,r-1.5,'url(#analog-body)','#4f4b5a',1.1)
  self.circle(x,y,r*.62,'url(#analog-cap)','#5c5768',1)
  # High-contrast analog needle pointer with casing & warm amber tip dot
  a=math.radians(-225+270*f)
  self.line(x+math.cos(a)*r*.18,y+math.sin(a)*r*.18,x+math.cos(a)*r*.93,y+math.sin(a)*r*.93,'#111116',4.8)
  self.line(x+math.cos(a)*r*.20,y+math.sin(a)*r*.20,x+math.cos(a)*r*.92,y+math.sin(a)*r*.92,'#f4ebd9',2.6)
  self.circle(x+math.cos(a)*r*.78,y+math.sin(a)*r*.78,2.1,AMBER,'none')
 def mini(self,p,x,y,r=36,force_knob=False,label_gap=24):
  v=self.values[p['id']];disabled=self.n=='DDelay' and p['id'] in ['time','modulation']
  self.raw(f'<g id="param-{p["id"]}"'+(' opacity=".48"' if disabled else '')+'><title>'+html.escape(p['name'])+'</title>')
  lbl=LABEL(p);label_y=y-r-label_gap
  self.text(lbl,x,label_y,11,'#bcbcc6','middle',.5)
  if p['discrete'] and not force_knob:
   opts=OPTIONS.get(self.n+':'+p['id'],['BELL','LOW SHELF','HIGH SHELF'] if self.n=='DEQ' and p['id'].endswith('shape') else ['OFF','ON'])
   if len(opts)<=4:
    if opts==['OFF','ON']:btn_w,gap=64,12
    elif len(opts)==2:btn_w,gap=124,16
    elif len(opts)==3:btn_w,gap=132,12
    else:btn_w,gap=98,12
    total_w=len(opts)*btn_w+(len(opts)-1)*gap
    self.rect(x-total_w/2-6,y-27,total_w+12,54,'#0d0d11','#2d2d35',9)
    for oi,t in enumerate(opts):
     xx=x-total_w/2+oi*(btn_w+gap);active=oi==int(v)
     self.cut(xx,y-21,btn_w,42,'url(#switch-active)' if active else 'url(#switch-idle)',PURPLE if active else '#45454f',6)
     if active:self.rect(xx+btn_w/2-14,y-17,28,3,AMBER,'none',1.5)
     self.text(t,xx+btn_w/2,y+7,10,'#efeaf8' if active else '#b0b0bc','middle',.2)
   else:
    self.cut(x-68,y-21,136,42,'#19191e','#49424f',6)
    self.text(VALUE(self.n,p,v),x,y+5,11,'#d5d0df','middle',.1)
  else:
   f=(v-p['min'])/(p['max']-p['min'])
   if p['unit']=='Hz':f=math.log(v/p['min'])/math.log(p['max']/p['min'])
   self.analog_knob(x,y,r,f)
   self.text(VALUE(self.n,p,v),x,y+r+28,14,'#dedee8','middle',.1)
  if p['learnable']:self.circle(x+len(lbl)*5.8+12,label_y-6,3.1,AMBER,'none')
  self.raw('</g>')
  if p['learnable']:self.button('learn-'+p['id'],'LEARN',x-38,y+r+37,76,26,False)
 def panel(self,title,y,h):
  self.text(title,73,y-12,10,'#b0b0ba',track=1.2)
  self.cut(55,y,1090,h,'#17171b','#3a3a42',11)
  self.rect(65,y+8,1070,h-16,'none','#25252c',8)
  for sx in (76,1124):
   for sy in (y+18,y+h-18):self.screw(sx,sy)
 def bottom(self):
  self.text('PREVIEW AUDITIONS / APPLY KEEPS / REJECT RESTORES',74,1167,8,'#9999a6',track=.7)
  for i,(id,t) in enumerate([('preview','PREVIEW'),('apply','APPLY'),('reject','REJECT'),('undo','UNDO')]):self.button(id,t,74+i*164,1178,150,29,i==1,False)
  self.text('DESIGN REVIEW / NO LIVE AUDIO',1130,1197,8,'#9393a0','end',1)

rows=[]
for n,params in META.items():
 d=D(n);lookup={p['id']:p for p in params};p=lookup[PRIMARY[n]]
 main=[PRIMARY[n]]+MAIN[n];advanced=[p['id'] for p in params if p['id'] not in main+[PRIMARY[n]]]
 d.display();d.learning()
 d.panel(GROUPS[n][0],654,214)
 main_xs=layout_row(n,main,lookup)
 for id,x in zip(main,main_xs):d.mini(lookup[id],x,748,36,force_knob=id==PRIMARY[n])
 d.panel(GROUPS[n][1],904,232)
 d.text('ADVANCED / EXPANDED FOR REVIEW',1130,892,8,'#a596bc','end',.7)
 if n=='DEQ':
  for j,b in enumerate([1,3,4]):
   cx=68+j*356;d.rect(cx,914,348,214,'#131318','#302e3a',9)
   d.rect(cx+8,920,332,34,'#0e0e12','#262430',6)
   d.text('BAND 0'+str(b),cx+18,943,11,PURPLE,track=.8)
   ps=lookup[f'b{b}shape']
   d.raw(f'<g id="param-{ps["id"]}"><title>{html.escape(ps["name"])}</title>')
   d.text(LABEL(ps),cx+138,943,10,'#b0b0bc',track=.2)
   d.cut(cx+238,924,94,26,'url(#switch-active)',PURPLE,5)
   d.text('BELL',cx+285,943,10,'#efeaf8','middle',.2)
   d.raw('</g>')
   for k,suffix in enumerate(['freq','gain','q']):
    d.mini(lookup[f'b{b}{suffix}'],cx+62+k*112,1025,25,label_gap=20)
 else:
  adv_xs=layout_row(n,advanced,lookup)
  for id,x in zip(advanced,adv_xs):d.mini(lookup[id],x,1002,35)
  if len(advanced)==1:
   d.text('ALL PARAMETERS VISIBLE / NO DUPLICATED CONTROLS',600,1118,9,'#8f8f9d','middle',.8)
 d.bottom();d.finish(OUT/(n+'.svg'))
 for p in params:rows.append([n,p['id'],p['name'],'main' if p['id'] in main else 'advanced',p['min'],p['max'],d.values[p['id']],p['unit'],'yes' if p['learnable'] else 'no'])
css='body{background:#111115;color:#d4d4de;font:16px Arial;margin:32px}h1{font-weight:400}p{line-height:1.9;color:#aaaaba}main{display:grid;grid-template-columns:repeat(auto-fit,minmax(min(100%,420px),1fr));gap:24px}article{background:#1b1b21;border:1px solid #373741;border-radius:14px;padding:16px}a{color:inherit;text-decoration:none}img{width:100%;height:auto;object-fit:contain}h2{font-weight:400}'
page=f'<!doctype html><html><meta charset="utf-8"><meta name="viewport" content="width=device-width"><title>D.A / Final selected reference / SVG review</title><style>{css}</style><h1>D.A / FINAL REFERENCE — SVG REVIEW 05 / ANALOG KNOBS</h1><p dir="rtl">نسخهٔ بررسی SVG با ناب‌های آنالوگ بزرگ‌تر و خواناتر، فاصله‌گذاری اصلاح‌شدهٔ دکمه‌های انتخابی (از جمله Stereo / Ping Pong در DDelay) و پنل جداگانهٔ مرجع. روی هر تصویر کلیک کنید تا SVG اصلی را در اندازهٔ کامل ببینید.</p><main>'
for n in META:page+=f'<article><a href="{n}.svg"><h2>{n}</h2><img src="{n}.svg" alt="{n} review prototype" loading="lazy"></a></article>'
(OUT/'Preview.html').write_text(page+'</main><details><summary>Optional Reference panel / پنل جداگانهٔ مرجع</summary><p>Closed by default in the proposed interface. SVG actions are illustrative only.</p><img src="Reference-Panel.svg" alt="Optional reference panel empty state"></details></html>')
s=io.StringIO();writer=csv.writer(s,lineterminator="\n");writer.writerow(['plugin','parameter_id','label','section','minimum','maximum','design_value','unit','learn_eligible']);writer.writerows(rows);(OUT/'Control_Review.csv').write_text(s.getvalue())
(OUT/'READ_ME_FA_EN.txt').write_text('''D.A / FINAL SELECTED REFERENCE — SVG REVIEW 05 / ANALOG KNOBS
10 standalone editable SVGs, 1200 x 1240. No runtime editor or DSP changes.
This is a review proposal, pending explicit user approval.
Its purple/amber palette, real DA mark, inset visual, optional separate reference panel,
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
- Review fix 01: removed the optional English-keywords label, field and placeholder from all ten SVGs.
- Review fix 02: removed the large focused knob, housing, tick dots and pointer from all ten SVGs.
  Focused parameter name/value and eligible Learn action remain; small knobs were retained.
- Review fix 03: all non-brand text enlarged (minimum 16 SVG px), less letter spacing,
  stronger small-label weight/contrast, larger small-button height and wider choices.
  Product name and logo are unchanged. DVerb Character and EQ Learn placement adjusted
  only to prevent collisions from the larger typography.
- Review fix 04: removed the screen notch/arc left by the deleted large knob.
  Learning actions now form one horizontal toolbar. The focused numeric field and
  control panels move up, and the canvas is shorter; no blank centre is retained.
- Review fix 05: the former large-knob parameter returns as the FIRST small knob in
  the main control row, using the same radius as its neighbours. No separate
  primary readout is duplicated. The canvas is another 60 px shorter (1240 px tall).
- Review fix 06: reference waveform and heading removed from every main screen.
  REFERENCE... represents access to a separate optional panel, closed by default.
  Reference-Panel.svg shows its empty state; no file or analysis is fabricated.
- Review fix 07: redesigned all rotary controls as larger analog hardware knobs
  (knurled grip ridges, 21-step outer calibration ticks, brushed cap, high-contrast
  needle pointer with amber tip), separated segmented switch buttons with explicit
  gaps and housing (fixing DDelay Stereo / Ping Pong overlap), and refined panel
  framing across all ten SVGs.
- Preview/Apply/Reject/Undo are visible but dimmed in this neutral design-review state.
- All graph/reference/meter visuals are STATIC EXAMPLES, not real audio or measured data.

Open Preview.html locally after extracting. Click a product to open its SVG at full size.
Control_Review.csv lists every ID, section, range, example value and Learn eligibility.
For reporting issues, name product + control/section and describe overlap, label, spacing,
missing control or preferred interaction. Actual drag/keyboard/focus/scaling/host tests are
pending the approved runtime implementation; SVG buttons do not implement interactions.

این‌ها SVG برای بررسی شما هستند، نه پلاگین اجراشده. فایل Preview.html را باز کنید.
ناب‌ها در هر ده طرح به شکل آنالوگ بزرگ‌تر و خواناتر طراحی شده‌اند و فاصلهٔ بین دکمه‌های
انتخابی (مثل Stereo و Ping Pong در DDelay) اصلاح شده است.
تا تأیید شما هیچ تغییری در رابط اجرایی، پردازش صوت یا وضعیت برنامه ایجاد نشده است.
''')
(OUT/'Reference-Panel.svg').write_text('''<svg xmlns="http://www.w3.org/2000/svg" width="800" height="440" viewBox="0 0 800 440">
<title>Optional reference panel — empty state design proposal</title>
<rect width="800" height="440" rx="18" fill="#111114"/>
<g font-family="Arial, Helvetica, sans-serif" font-size="18" fill="#d0d0d6">
<text x="32" y="48" fill="#a28bd0" font-size="24">REFERENCE</text>
<text x="660" y="48">CLOSE</text>
<text x="32" y="100">Optional workspace — opened only when requested</text>
<rect x="32" y="128" width="736" height="160" rx="10" fill="#0e0e11" stroke="#414146"/>
<text x="400" y="192" text-anchor="middle">No reference file selected</text>
<text x="400" y="228" text-anchor="middle" fill="#a9a9b5">Load a file when you need reference-based analysis.</text>
<rect x="32" y="316" width="220" height="48" rx="7" fill="#19191d" stroke="#a28bd0"/>
<text x="142" y="347" text-anchor="middle" fill="#a28bd0">LOAD REFERENCE</text>
<text x="32" y="408" font-size="16">SVG proposal only — no file loaded, no analysis performed.</text>
</g></svg>''')
archive=OUT.parent/'DAPlugins_FinalReference_SVG_Review05_AnalogKnobs.zip'
with zipfile.ZipFile(archive,'w',zipfile.ZIP_DEFLATED) as z:
 for p in sorted(OUT.iterdir()):
  if p.name!='LogoPaths.json':z.write(p,'DAPlugins_FinalReference_Review05_AnalogKnobs/'+p.name)
with zipfile.ZipFile(archive) as z:assert z.testzip() is None;assert sum(n.endswith('.svg') for n in z.namelist())==11
print('PASS ten final-reference SVGs / 92 unique real controls / vector logo / ZIP CRC:',archive)
