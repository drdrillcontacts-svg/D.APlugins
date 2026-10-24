#!/usr/bin/env python3
"""Matte 02: ten original function-led SVG proposals, no runtime changes."""
from pathlib import Path
import math,json,html,re,zipfile,xml.etree.ElementTree as ET
ROOT=Path(__file__).resolve().parents[1];OUT=ROOT/'Design/Concepts/Matte-02';OUT.mkdir(parents=True,exist_ok=True)
META=json.loads((ROOT/'Design/UI/Parameters.json').read_text())
# Only reuse SVG drawing primitives/value formatting; no Noir 01 layout or graph is executed.
ns={'__file__':str(ROOT/'tools/design_noir_concepts.py')};exec((ROOT/'tools/design_noir_concepts.py').read_text().split('for name,params in META.items():')[0],ns)
Base=ns['Drawing'];fmt=ns['value'];lab=ns['label']
PALETTE={'DVerb':'#9fad9a','DDelay':'#91aeb8','DComp':'#bc9c87','DEQ':'#b2bca1','DSat':'#c3a36e','DTrans':'#ba8e82','DStereo':'#9baebe','DSess':'#9cb9ac','DGate':'#a8ac8c','DFX':'#bcb6a7'}
ROLE={'DVerb':'ROOM / TAIL / DIFFUSION','DDelay':'REPEATS IN TIME','DComp':'DYNAMIC BALANCE','DEQ':'FOUR BANDS. ONE CLEAR VIEW.','DSat':'HARMONIC CHARACTER','DTrans':'ATTACK & BODY','DStereo':'MID / SIDE PERSPECTIVE','DSess':'FOCUS ON SIBILANCE','DGate':'SIGNAL / SILENCE','DFX':'A CONNECTED SIGNAL PATH'}
class D(Base):
 def __init__(self,name):
  self.name=name;self.accent=PALETTE[name];self.values={p['id']:p['initial'] for p in META[name]}
  self.parts=[f'''<svg xmlns="http://www.w3.org/2000/svg" width="1280" height="900" viewBox="0 0 1280 900" role="img" aria-labelledby="title desc">
<title id="title">D.A / {name} / Matte 02 design proposal</title><desc id="desc">Original function-led layout. Matte black and restrained product-specific accent. Static illustrative graphics, not live measurements.</desc>
<defs><linearGradient id="shell" x2="0" y2="1"><stop stop-color="#1d1f20"/><stop offset="1" stop-color="#151718"/></linearGradient>
<linearGradient id="dial" x2="0" y2="1"><stop stop-color="#343638"/><stop offset="1" stop-color="#202223"/></linearGradient>
<filter id="soft" x="-30%" y="-30%" width="160%" height="180%"><feDropShadow dx="0" dy="6" stdDeviation="8" flood-opacity=".4"/></filter></defs>
<rect width="1280" height="900" fill="#101213"/>
<rect x="31" y="28" width="1218" height="844" rx="19" fill="#090b0c" filter="url(#soft)"/>
<rect x="32" y="27" width="1216" height="844" rx="18" fill="url(#shell)" stroke="#303334"/>
<path d="M52 28 H1228" stroke="#ffffff" stroke-opacity=".045"/>
''']
  self.text('D.A / MATTE SERIES',80,77,11,'#929895',spacing=2.5)
  self.text(name,78,133,40,'#e0e2df',spacing=-1,weight=500)
  self.text(ROLE[name],278,128,11,self.accent,spacing=2)
  self.text('FACTORY / 50',1199,77,10,'#808784','end',2)
  self.text('Init / Design study',1098,126,13,'#babfba','end',.4)
  self.path('M1138 114 l-6 6 6 6 M1180 114 l6 6 -6 6','#8e9691',1.5)
  self.line(80,157,1200,157,'#343839')
 def panel(self,x,y,w,h,title=None):
  self.rect(x,y,w,h,'#151819','#292d2e',10)
  if title:self.text(title,x+22,y+30,10,'#858d88',spacing=2)
 def param(self,id,x,y,w=240,h=84,kind='bar'):
  p=next(p for p in META[self.name] if p['id']==id);v=self.values[id]
  label=lab(p)
  if id.endswith('shape'):label='SHAPE'
  if id.startswith('b') and id[1:2].isdigit():label={'freq':'FREQUENCY','gain':'GAIN','q':'Q','shape':'SHAPE'}[id[2:]]
  val='BELL' if id.endswith('shape') else fmt(self.name,p,v)
  self.group('param-'+id)
  self.raw('<title>'+html.escape(id+' / '+p['name'])+'</title>')
  self.text(label,x,y+17,10,'#969e98',spacing=1.2)
  if p['learnable']:self.circle(x-9,y+13,2,self.accent)
  if kind=='row':
   self.text(val,x+w,y+17,14,'#d1d6ce','end',.1);self.line(x,y+h-2,x+w,y+h-2,'#303536');self.end();return
  self.text(val,x,y+49,24 if w>200 else 18,'#d4d9d1',spacing=.2)
  if p['discrete']:
   self.text('SELECT',x+w,y+46,8,self.accent,'end',1.4)
  else:
   self.line(x,y+h-10,x+w,y+h-10,'#373c3d',2)
   frac=(v-p['min'])/(p['max']-p['min'])
   if p['unit']=='Hz':frac=math.log(v/p['min'])/math.log(p['max']/p['min'])
   self.line(x,y+h-10,x+w*frac,y+h-10,self.accent,2)
   self.rect(x+w*frac-3,y+h-17,6,14,'#c9cfc7','none',2)
  self.end()
 def curve(self,pts,color=None,width=2,opacity=1):self.path('M'+' L'.join(f'{x:.2f} {y:.2f}' for x,y in pts),color or self.accent,width,opacity=opacity)
 def envelope(self,x,y,w,h,decay=4,attack=.02):
  self.line(x,y+h/2,x+w,y+h/2,'#323738')
  for i in range(130):
   t=i/129;a=(1-math.exp(-t/max(attack,.001)))*math.exp(-t*decay)*h*.48*(.4+.6*abs(math.sin(i*1.71)))
   self.line(x+t*w,y+h/2-a,x+t*w,y+h/2+a,self.accent,1.7)
 def finish(self):
  self.line(80,803,1200,803,'#333839')
  for x,t in [(80,'CAPTURE INPUT'),(260,'REFERENCE'),(446,'LEARN ALL')]:self.text(t,x,838,10,self.accent if t=='LEARN ALL' else '#a0a79f',spacing=1.4)
  self.text('DETAILS / LOG',803,838,10,'#a0a79f',spacing=1.4)
  self.text('02 / STATIC DESIGN PROPOSAL',1200,838,9,'#6e7872','end',1.4)
  self.raw('</svg>');text='\n'.join(self.parts);root=ET.fromstring(text)
  ids=[e.get('id') for e in root.iter() if e.get('id')];assert len(ids)==len(set(ids))
  assert {i[6:] for i in ids if i.startswith('param-')}=={p['id'] for p in META[self.name]},self.name
  (OUT/(self.name+'.svg')).write_text(text)
for name,params in META.items():
 d=D(name)
 if name=='DVerb':
  d.panel(80,190,650,352,'EARLY SPACE / LATE FIELD')
  # Perspective room contours: no waveform screen / no central master knob.
  for i in range(13):
   x=142+i*12;y=248+i*5;w=498-i*24;h=210-i*10
   d.path(f'M{x} {y+h} V{y} L{x+w*.5} {y-25} L{x+w} {y} V{y+h} L{x+w*.5} {y+h+24} Z',d.accent,1,opacity=.12+i*.035)
  d.text('INPUT',108,513,9,'#717d73');d.text('DIFFUSE FIELD',702,513,9,d.accent,'end')
  d.panel(758,190,442,352,'SPACE TIMING')
  for i,id in enumerate(['decay','predelay','size']):d.param(id,788,242+i*96,378,82)
  other=['tone','density','diffusion','width','mix','lowcut','midcut','modulation','mode']
  for i,id in enumerate(other):
   row=i//5;n=5 if row==0 else 4;col=i%5;w=1120/n;d.param(id,80+col*w,583+row*108,w-36,86)
 elif name=='DDelay':
  d.panel(80,190,1120,234,'ECHO TIMELINE / ILLUSTRATIVE')
  d.line(130,350,1150,350,'#3a4143')
  for i,h in enumerate([115,86,63,45,31,20,12]):
   x=170+i*150;d.line(x,350,x,350-h,d.accent,3);d.circle(x,350-h,4,d.accent);d.text(str(i+1),x,385,10,'#85969a','middle',0)
  d.text('L',104,285,10,d.accent);d.text('R',104,321,10,'#6c797e')
  for i,id in enumerate(['time','sync','division','pingpong']):d.param(id,80+(i%2)*270,470+(i//2)*100,230,82)
  for i,id in enumerate(['feedback','filter']):d.param(id,700,470+i*100,500,82)
  for i,id in enumerate(['mix','width','modulation','output']):d.param(id,80+i*290,699,250,82)
 elif name=='DComp':
  d.panel(80,190,154,352,'REDUCTION')
  for i in range(23):d.rect(122,246+i*10,68,5,d.accent if i>9 else '#303638','none',1)
  d.text('GR',156,516,10,'#7e8982','middle')
  d.panel(260,190,474,352,'TRANSFER / ILLUSTRATIVE')
  for j in range(5):d.line(304,274+j*51,690,274+j*51,'#252b2c')
  d.line(304,494,690,249,'#48504d')
  d.path('M304 494 L520 357 Q535 348 549 346 L690 320',d.accent,3)
  for i,id in enumerate(['threshold','ratio','makeup','output']):d.param(id,790,204+i*86,410,78)
  for i,id in enumerate(['attack','release','knee','mix']):d.param(id,80+i*290,654,250,102)
 elif name=='DEQ':
  d.panel(80,190,1120,323,'FREQUENCY RESPONSE / ILLUSTRATIVE')
  for x,t in [(120,'20 Hz'),(340,'100'),(560,'1k'),(780,'5k'),(1140,'20k')]:d.line(x,247,x,449,'#2c3330');d.text(t,x,485,10,'#7e8b7d','middle',0)
  d.line(120,345,1160,345,'#424b40')
  pts=[(120+i*5.2,345-28*math.exp(-((i-37)/13)**2)+37*math.exp(-((i-78)/14)**2)-25*math.exp(-((i-147)/21)**2)) for i in range(201)];d.curve(pts,width=2.5)
  for b in range(1,5):
   x=80+(b-1)*285;d.panel(x,546,265,236,'BAND 0'+str(b))
   for j,s in enumerate(['freq','gain','q','shape']):d.param('b'+str(b)+s,x+22,590+j*42,221,36,'row')
 elif name=='DSat':
  d.panel(80,190,287,363,'DRIVE STAGE')
  # A compact off-centre input knob, used only where it suits the product.
  d.group('param-drive');d.circle(223,350,78,'#0b0e0f');d.circle(223,345,74,'url(#dial)','#454a45');d.line(174,302,187,314,d.accent,3)
  d.circle(186,449,2,d.accent);d.text('DRIVE',223,453,10,'#9a9f91','middle');d.text('6 dB',223,489,27,d.accent,'middle',.2);d.end()
  d.panel(397,190,803,363,'SOFT TRANSFER / ILLUSTRATIVE')
  d.line(443,380,1154,380,'#34392f');d.line(800,257,800,492,'#34392f')
  d.curve([(449+i*7,376-91*math.tanh((i-50)/20)) for i in range(101)],width=3)
  for i,id in enumerate(['tone','mode','density','mix','output']):d.param(id,80+i*228,654,190,102)
 elif name=='DTrans':
  for i,(title,decay,attack) in enumerate([('TRANSIENT / ATTACK',7,.005),('BODY / SUSTAIN',1.9,.06)]):
   x=80+i*580;d.panel(x,190,540,266,title);d.envelope(x+32,250,476,149,decay,attack)
  d.param('attack',80,503,540,112);d.param('sustain',660,503,540,112)
  for i,id in enumerate(['sensitivity','shape','mix','output']):d.param(id,80+i*290,678,250,100)
 elif name=='DStereo':
  d.panel(80,190,500,584,'STEREO FIELD / ILLUSTRATIVE')
  for r in [61,122,182]:d.circle(330,459,r,'none','#30383c')
  d.line(330,245,330,673,'#414b50');d.line(117,459,543,459,'#414b50')
  for j in range(9):d.curve([(330+(128+j*3)*math.sin(i/100*math.tau),459+105*math.sin(i/100*math.tau*2+j*.19)) for i in range(101)],width=1,opacity=.2)
  d.text('MID',330,728,11,d.accent,'middle');d.text('SIDE',535,463,9,'#7c8e98','end')
  for i,id in enumerate(['width','depth','center','side','mix','mono']):d.param(id,650,190+i*99,550,86)
 elif name=='DSess':
  d.panel(80,190,748,379,'DETECTION BAND / ILLUSTRATIVE')
  d.rect(576,247,117,245,'#202d27','none',5)
  for i in range(100):
   x=111+i*6.83;h=40+38*abs(math.sin(i*.15))+103*math.exp(-((i-74)/9)**2);d.line(x,497,x,497-h,d.accent if 67<i<85 else '#596a60',2)
  d.text('SIBILANCE',636,535,10,d.accent,'middle')
  for i,id in enumerate(['frequency','threshold','range']):d.param(id,880,212+i*120,320,100)
  for i,id in enumerate(['attack','release','focus','mix','output']):d.param(id,80+i*228,665,190,107)
 elif name=='DGate':
  d.panel(80,190,1120,265,'OPEN / HOLD / CLOSE')
  d.envelope(112,244,1056,145,2.8,.008)
  d.path('M112 408 H265 V244 H524 L656 408 H1168',d.accent,1.5)
  for x,t in [(240,'OPEN'),(450,'HOLD'),(716,'RELEASE'),(1090,'CLOSED')]:d.text(t,x,433,9,'#8c957a','middle')
  for i,id in enumerate(['threshold','range','hysteresis','attack','hold','release']):d.param(id,80+(i%3)*302,510+(i//3)*141,260,106)
  d.panel(1010,491,190,284,'FINAL TRIM');d.param('output',1034,586,142,108)
 elif name=='DFX':
  modules=[('SATURATION',['driveOn','drive']),('FILTER',['filterOn','filter']),('DELAY',['delayOn','time','feedback','delayMix']),('AMBIENCE',['reverbOn','decay','reverbMix'])]
  for i,(title,ids) in enumerate(modules):
   x=80+i*285;d.panel(x,190,265,451)
   d.text('0'+str(i+1),x+22,227,12,d.accent);d.text(title,x+22,267,14,'#d2d5cd',spacing=1.6)
   if i<3:d.path(f'M{x+268} 243 h13 m-4 -3 l4 3 -4 3','#737b70',1)
   for j,id in enumerate(ids):d.param(id,x+22,296+j*83,221,75)
  d.param('mix',80,689,540,91);d.param('output',660,689,540,91)
 d.finish()
(OUT/'READ_ME.txt').write_text('''D.A Plugins / MATTE 02 — ten function-led design proposals.
Revised after feedback: preserve matte black material and subtle shadows, NOT the reference layout.
Each plugin has its own restrained accent and working hierarchy. No universal central master knob,
no repeated purple waveform screen, no copied branding or reverse/chorus/tremolo controls.
DVerb: perspective room / timing strip. DDelay: echo timeline. DComp: GR / transfer.
DEQ: full-width EQ / four band strips. DSat: compact offset drive / transfer.
DTrans: dual envelope / paired faders. DStereo: field scope / M-S controls.
DSess: focused spectrum. DGate: state envelope. DFX: connected vertical module strips.
1280 x 900 standalone editable SVG; all 92 existing parameter IDs represented.
Graphs are illustrative, not measured or live. Example visuals do not imply a real saved preset.
Small accent dots mark only Learn-eligible controls; Details/Log is explicit and closed.
No plugin, DSP, runtime UI or factory presets changed. These are proposals for approval only.

ده کانسپت مستقل برای تأیید شما؛ مشکی مات با سایهٔ ظریف، بدون کپی چیدمان مرجع.
هر محصول ساختار و رنگ تأکیدی خودش را دارد. فایل‌ها وکتور مستقل و قابل ویرایش‌اند.
نمودارها نمایشی هستند؛ هنوز هیچ تغییری در رابط اجرایی پلاگین انجام نشده است.
''')
gallery='<!doctype html><html lang="en"><meta charset="utf-8"><meta name="viewport" content="width=device-width"><title>D.A / Matte 02</title><style>body{margin:0;padding:36px;background:#111415;color:#d4d9d1;font:16px Arial}h1{font-weight:400;letter-spacing:2px}p{color:#9da69e;line-height:1.6}main{display:grid;grid-template-columns:repeat(auto-fit,minmax(min(100%,480px),1fr));gap:28px}a{color:inherit;text-decoration:none}img{display:block;width:100%;border-radius:12px}h2{font-size:17px;font-weight:400}</style><h1>D.A / MATTE 02</h1><p>Ten original function-led proposals. Matte black, subtle shadows, individual accents.<br>Click a design to open its standalone editable SVG. Static visuals, not live audio.</p><main>'
for name in META:gallery+=f'<a href="{name}.svg"><h2>{name}</h2><img src="{name}.svg" alt="{name} Matte 02 proposal"></a>'
(OUT/'Preview.html').write_text(gallery+'</main></html>')
archive=OUT.parent/'DAPlugins_Matte02_10_SVG.zip' 
with zipfile.ZipFile(archive,'w',zipfile.ZIP_DEFLATED) as z:
 for p in sorted(OUT.iterdir()):
  if p.suffix in ['.svg','.html'] or p.name=='READ_ME.txt':z.write(p,'DAPlugins_Matte02/'+p.name)
with zipfile.ZipFile(archive) as z:assert z.testzip() is None;assert len([n for n in z.namelist() if n.endswith('.svg')])==10
print('PASS 10 SVG / 92 controls / unique IDs / ZIP CRC:',archive)
