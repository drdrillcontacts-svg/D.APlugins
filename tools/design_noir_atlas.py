#!/usr/bin/env python3
"""Noir Atlas: 10 different design systems × all 10 plugins. SVG concepts only."""
from pathlib import Path
import json,math,html,zipfile,xml.etree.ElementTree as ET
ROOT=Path(__file__).resolve().parents[1];OUT=ROOT/'Design/Concepts/Noir-Atlas';OUT.mkdir(parents=True,exist_ok=True)
META=json.loads((ROOT/'Design/UI/Parameters.json').read_text())
STYLES=[
 ('01','Graphite Console','کنسول گرافیتی',1320,880,'#a7afa2','Arial, sans-serif','console'),
 ('02','Atelier','آتلیه',1400,900,'#c4b7a2','Georgia, serif','editorial'),
 ('03','Orbital','مداری',1100,1040,'#9daeb2','Arial, sans-serif','orbital'),
 ('04','Quiet Objects','اشیای آرام',1200,840,'#b7b9ac','Arial, sans-serif','floating'),
 ('05','Monolith','مونولیت',960,1240,'#a6aca9','Arial, sans-serif','vertical'),
 ('06','Desk Instruments','ابزار رومیزی',1440,760,'#c0a58d','Arial, sans-serif','compact'),
 ('07','Split Architecture','معماری نامتقارن',1380,920,'#9caeaa','Arial, sans-serif','split'),
 ('08','Signal Lab','آزمایشگاه سیگنال',1280,940,'#a5b3a3','Courier New, monospace','lab'),
 ('09','Soft Blocks','بلوک‌های نرم',1180,1000,'#beb5aa','Arial, sans-serif','pads'),
 ('10','Horizon','افق',1480,900,'#a8b1be','Arial, sans-serif','cinematic')]
ROLE={'DVerb':'REVERBERATION','DDelay':'STEREO DELAY','DComp':'DYNAMICS','DEQ':'FOUR-BAND EQ','DSat':'SATURATION','DTrans':'TRANSIENT SHAPING','DStereo':'STEREO IMAGE','DSess':'SIBILANCE CONTROL','DGate':'NOISE GATE','DFX':'MULTI EFFECT'}
COLORS={'DVerb':'#a2b29e','DDelay':'#9fb7be','DComp':'#beaa95','DEQ':'#b5bba0','DSat':'#c2aa7a','DTrans':'#bd9c8f','DStereo':'#a5b3c3','DSess':'#a2b8ad','DGate':'#afb397','DFX':'#beb8ab'}
LABEL={'predelay':'PRE-DELAY','modulation':'MOD DEPTH','frequency':'DETECT FREQ','pingpong':'ROUTING','mode':'CHARACTER','lowcut':'LOW CUT','midcut':'MID CUT','driveOn':'DRIVE ON','filterOn':'FILTER ON','delayOn':'DELAY ON','reverbOn':'REVERB ON','delayMix':'DELAY MIX','reverbMix':'SPACE MIX','sync':'TEMPO SYNC','division':'DIVISION','mono':'MONO CHECK','sensitivity':'SENSITIVITY'}
OPTIONS={'DVerb:mode':['ROOM','HALL','PLATE','SHIMMER'],'DDelay:pingpong':['STEREO','PING PONG'],'DDelay:sync':['FREE MS','SYNC'],'DDelay:division':['1/32','1/32 D','1/32 T','1/16','1/16 D','1/16 T','1/8','1/8 D','1/8 T','1/4','1/4 D','1/4 T','1/2','1/2 D','1/1'],'DSat:mode':['SOFT','HARD','ASYMMETRIC'],'DStereo:mono':['STEREO','MONO']}
def label(p):
 i=p['id']
 if i.startswith('b') and i[1:2].isdigit():return 'B'+i[1]+' '+{'freq':'FREQ','gain':'GAIN','q':'Q','shape':'SHAPE'}[i[2:]]
 return LABEL.get(i,p['name'].upper())
def value(n,p):
 v=p['initial'];i=p['id'];u=p['unit']
 if p['discrete']:return 'BELL' if i.endswith('shape') else OPTIONS.get(n+':'+i,['OFF','ON'])[int(v)]
 if u=='Hz':return f'{v/1000:g} kHz' if v>=1000 else f'{v:g} Hz'
 if u=='dB':return f'{v:g} dB'
 if u==':1':return f'{v:g}:1'
 if u in ['ms','s']:return f'{v:g} {u}'
 if u=='%' or i in ['mix','width','density','diffusion','modulation','depth','sensitivity','focus','feedback','delayMix','reverbMix']:return f'{v*100:g}%'
 return f'{v:g}'
class SVG:
 def __init__(self,n,s):
  self.n=n;self.s=s;self.w=s[3];self.h=s[4];self.c=s[5] if s[7] in ['editorial','lab','vertical'] else COLORS[n];self.font=s[6];self.parts=[]
  self.raw(f'''<svg xmlns="http://www.w3.org/2000/svg" width="{self.w}" height="{self.h}" viewBox="0 0 {self.w} {self.h}" role="img" aria-labelledby="title desc"><title id="title">{n} / {s[1]} / Noir Atlas</title><desc id="desc">Original static design proposal. Graphs are illustrative, not live or measured. All existing parameters represented. No DSP changes.</desc><defs>
<linearGradient id="matte" x2="0" y2="1"><stop stop-color="#242729"/><stop offset="1" stop-color="#17191b"/></linearGradient>
<linearGradient id="dial" x2="0" y2="1"><stop stop-color="#35383a"/><stop offset="1" stop-color="#202325"/></linearGradient>
<filter id="shadow" x="-40%" y="-40%" width="180%" height="190%"><feDropShadow dx="0" dy="8" stdDeviation="10" flood-color="#000" flood-opacity=".42"/></filter>
<filter id="smallshadow" x="-50%" y="-50%" width="200%" height="210%"><feDropShadow dx="0" dy="4" stdDeviation="4" flood-color="#000" flood-opacity=".5"/></filter></defs>''')
  self.rect(0,0,self.w,self.h,"#101416","none",0)
 def raw(self,t):self.parts.append(t)
 def text(self,t,x,y,size=13,color='#a0a7a6',align='start',track=1,font=None,weight=400):
  self.raw(f'<text x="{x:.2f}" y="{y:.2f}" fill="{color}" font-family="{font or self.font}" font-size="{size}" font-weight="{weight}" text-anchor="{align}" letter-spacing="{track}">{html.escape(str(t))}</text>')
 def rect(self,x,y,w,h,fill='#171a1c',stroke='#303537',r=8,shadow=False):self.raw(f'<rect x="{x}" y="{y}" width="{w}" height="{h}" rx="{r}" fill="{fill}" stroke="{stroke}"'+(' filter="url(#shadow)"' if shadow else '')+'/>')
 def path(self,p,c=None,width=1,opacity=1,fill='none'):self.raw(f'<path d="{p}" fill="{fill}" stroke="{c or self.c}" stroke-width="{width}" opacity="{opacity}" stroke-linejoin="round" stroke-linecap="round"/>')
 def line(self,a,b,c,d,col='#3d4547',width=1):self.path(f'M{a} {b} L{c} {d}',col,width)
 def circle(self,x,y,r,fill='none',stroke='#3b4245',width=1):self.raw(f'<circle cx="{x}" cy="{y}" r="{r}" fill="{fill}" stroke="{stroke}" stroke-width="{width}"/>')
 def param(self,p,x,y,w,h=90,kind='slider'):
  self.raw(f'<g id="param-{p["id"]}"><title>{html.escape(p["name"])}</title>')
  if kind=='tile':self.rect(x,y,w,h,'url(#matte)','#35393a',16,True);x+=18;y+=14;w-=36;h-=28
  if kind=='row':
   self.text(label(p),x,y+18,11,'#a9afad',track=.8);self.text(value(self.n,p),x+w,y+18,17,'#d8dbd5','end',.2)
   self.line(x,y+h-3,x+w,y+h-3,'#333a3c');
  elif kind in ['knob','encoder']:
   r=min(31,h*.25);cx=x+w/2;cy=y+25+r
   self.text(label(p),cx,y+11,10,'#b2b7b3','middle',.7)
   self.circle(cx,cy+3,r+3,'#0c0f10','#1b1f20');self.circle(cx,cy,r,'url(#dial)','#454b4d')
   f=(p['initial']-p['min'])/(p['max']-p['min']);a=math.radians(-225+270*f)
   self.line(cx+math.cos(a)*r*.56,cy+math.sin(a)*r*.56,cx+math.cos(a)*r*.86,cy+math.sin(a)*r*.86,self.c,2)
   self.text(value(self.n,p),cx,cy+r+23,14,'#d2d7d1','middle',.1)
  elif kind=='vertical':
   cx=x+w/2;self.text(label(p),cx,y+18,9,'#aab2af','middle',.2)
   if p['discrete']:
    self.rect(x+9,y+58,w-18,45,'#191e20','#40494a',4);self.text(value(self.n,p),cx,y+85,11,self.c,'middle',0)
   else:
    self.line(cx,y+53,cx,y+h-65,'#414a4c',3)
    f=(p['initial']-p['min'])/(p['max']-p['min']);yy=y+h-65-f*(h-118)
    self.rect(cx-17,yy-6,34,12,'#969f9b','#c0c7c0',2)
   self.text(value(self.n,p),cx,y+h-22,13,'#d0d6cf','middle',0)
  else:
   self.text(label(p),x,y+16,10,'#a8b1ac',track=.7)
   self.text(value(self.n,p),x,y+48,26 if w>200 else 19,'#d4dad2',track=.2)
   if p['discrete']:self.text('SELECT',x+w,y+44,8,self.c,'end',1)
   else:
    self.line(x,y+h-13,x+w,y+h-13,'#3a4345',2)
    f=(p['initial']-p['min'])/(p['max']-p['min'])
    if p['unit']=='Hz':f=math.log(p['initial']/p['min'])/math.log(p['max']/p['min'])
    self.line(x,y+h-13,x+f*w,y+h-13,self.c,2);self.rect(x+f*w-3,y+h-19,6,12,'#c6cdc4','none',2)
  if p['learnable']:self.circle(x+w-3,y+12,2,self.c,'none')
  self.raw('</g>')
 def graph(self,x,y,w,h):
  n=self.n;k=self.s[7];self.raw('<g id="illustrative-visual">')
  if k=='lab':
   for i in range(1,12):self.line(x+i*w/12,y,x+i*w/12,y+h,'#293235',.6)
   for i in range(1,5):self.line(x,y+i*h/5,x+w,y+i*h/5,'#293235',.6)
  if n=='DVerb':
   if k in ['orbital','editorial','floating','split']:
    for j in range(13):
     t=j/17;xx=x+w*(.08+t*.27);yy=y+h*(.08+t*.26);ww=w*(.84-t*.54);hh=h*(.84-t*.52)
     self.path(f'M{xx} {yy+hh} V{yy} L{xx+ww/2} {yy-h*.04} L{xx+ww} {yy} V{yy+hh} L{xx+ww/2} {yy+hh+h*.04} Z',width=1,opacity=.13+j*.045)
   elif k=='cinematic':
    for j in range(17):
     pts=[(x+i*w/120,y+h*.75-h*.5*math.exp(-i/40)*abs(math.sin(i*.065+j*.13))-j*2) for i in range(121)]
     self.path('M'+' L'.join(f'{a:.1f} {b:.1f}' for a,b in pts),width=1,opacity=.15+j*.025)
   else:
    for i in range(135):
     t=i/134;a=h*.4*(1-math.exp(-t*35))*math.exp(-t*3.5)*(.3+.7*abs(math.sin(i*1.82)))
     self.line(x+t*w,y+h/2-a,x+t*w,y+h/2+a,self.c,1.4)
  elif n=='DDelay':
   self.line(x,y+h*.78,x+w,y+h*.78,'#3a464b')
   for j in range(8):
    xx=x+w*(.06+j*.128);amp=h*.66*math.exp(-j*.3)
    self.line(xx,y+h*.78,xx,y+h*.78-amp,self.c,3);self.circle(xx,y+h*.78-amp,3,self.c,'none')
    if h>160:self.text(str(j+1),xx,y+h*.95,10,'#83969c','middle',0)
  elif n in ['DComp','DSat']:
   self.line(x,y+h,x+w,y,'#465051');self.line(x,y+h/2,x+w,y+h/2,'#303a3c')
   pts=[]
   for i in range(101):
    t=i/100;z=t if t<.52 else .52+(t-.52)*.25
    if n=='DSat':z=.5+.43*math.tanh((t-.5)*5)
    pts.append((x+t*w,y+h*(1-z)))
   self.path('M'+' L'.join(f'{a:.1f} {b:.1f}' for a,b in pts),width=2.5)
  elif n=='DEQ':
   self.line(x,y+h/2,x+w,y+h/2,'#465044')
   pts=[(x+i*w/150,y+h/2-h*.15*math.exp(-((i-25)/11)**2)+h*.22*math.exp(-((i-64)/14)**2)-h*.18*math.exp(-((i-112)/16)**2)) for i in range(151)]
   self.path('M'+' L'.join(f'{a:.1f} {b:.1f}' for a,b in pts),width=2)
   for t in [.12,.33,.57,.82]:self.circle(x+t*w,y+h*.5,3,self.c,'none')
  elif n in ['DTrans','DGate']:
   for i in range(131):
    t=i/130;a=h*.39*(math.exp(-t*7)+.22*math.exp(-t))*abs(math.sin(i*1.6));self.line(x+t*w,y+h/2-a,x+t*w,y+h/2+a,self.c,1.4)
   if n=='DGate':self.path(f'M{x} {y+h*.87} H{x+w*.12} V{y+h*.12} H{x+w*.49} L{x+w*.68} {y+h*.87} H{x+w}',width=1.5,opacity=.6)
   else:self.path(f'M{x} {y+h*.8} Q{x+w*.06} {y-h*.2} {x+w*.16} {y+h*.2} Q{x+w*.4} {y+h*.7} {x+w} {y+h*.8}',width=1,opacity=.6)
  elif n=='DStereo':
   cx=x+w/2;cy=y+h/2;r=min(w,h)*.46
   for rr in [.5,1]:self.circle(cx,cy,r*rr,'none','#374449')
   self.line(cx-r,cy,cx+r,cy,'#374449');self.line(cx,cy-r,cx,cy+r,'#374449')
   for j in range(9):
    self.path('M'+' L'.join(f'{cx+r*.83*math.sin(i/100*math.tau):.1f} {cy+r*.6*math.sin(i/100*math.tau*2+j*.14):.1f}' for i in range(101)),width=1,opacity=.2)
  elif n=='DSess':
   self.rect(x+w*.69,y,w*.16,h,'#24352d','none',3)
   for i in range(100):
    hh=h*(.12+.16*abs(math.sin(i*.12))+.56*math.exp(-((i-76)/8)**2));xx=x+w*i/100
    self.line(xx,y+h,xx,y+h-hh,self.c if i>68 else '#586d64',max(1,w/240))
  elif n=='DFX':
   vertical=w<h*1.5
   for j,t in enumerate(['DRIVE','FILTER','DELAY','SPACE']):
    xx=x if vertical else x+j*w/4;yy=y+j*h/4 if vertical else y;ww=w if vertical else w*.21;hh=h*.18 if vertical else h
    self.rect(xx,yy,ww,hh,'#191e20','#46504c',5);self.text(t,xx+ww/2,yy+hh/2+4,11,self.c,'middle',1)
    if j<3:
     if vertical:self.line(xx+ww/2,yy+hh,xx+ww/2,yy+h/4,'#59635e')
     else:self.line(xx+ww,yy+hh/2,xx+w/4,yy+hh/2,'#59635e')
  self.raw('</g>')
 def foot(self):
  y=self.h-43;self.line(50,y-27,self.w-50,y-27,'#343e40')
  self.text('CAPTURE   /   REFERENCE   /   LEARN',54,y,10,self.c,track=1.1)
  self.text('DETAILS / LOG',self.w*.58,y,10,'#a7b0aa',track=1)
  self.text('CONCEPT / '+self.s[0]+' / NO LIVE AUDIO',self.w-54,y,8,'#77847d','end',.8)
 def finish(self,path):
  self.raw('</svg>');s='\n'.join(self.parts);r=ET.fromstring(s);ids=[a.get('id') for a in r.iter() if a.get('id')]
  assert len(ids)==len(set(ids)),path
  assert {a[6:] for a in ids if a.startswith('param-')}=={p['id'] for p in META[self.n]},path
  assert not any(a.tag.endswith('image') for a in r.iter()),path
  path.write_text(s)
def grid(d,ps,x,y,w,h,cols,kind='slider'):
 cols=min(cols,len(ps));rows=math.ceil(len(ps)/cols);cw=w/cols;rh=h/rows
 for i,p in enumerate(ps):
  gap=18 if kind!='tile' else 12
  d.param(p,x+(i%cols)*cw,y+(i//cols)*rh,cw-gap,rh-15,kind)
def build(n,s):
 d=SVG(n,s);w=d.w;h=d.h;ps=META[n];k=s[7]
 if k not in ['floating','editorial','cinematic']:
  d.rect(24,22,w-48,h-44,'url(#matte)','#3b4244',4 if k in ['lab','console'] else 18,True)
 if k=='console':
  d.rect(40,38,w-80,92,'#15191a','#3d4547',5);d.text('D.A',66,77,15,d.c);d.text(n,171,96,33,'#d7ddd5',track=0);d.text('GRAPHITE / '+ROLE[n],w-65,78,11,d.c,'end',2)
  for xx in [48,w-48]:
   for yy in [50,h-51]:d.circle(xx,yy,3,'#111516','#59635f');d.line(xx-2,yy,xx+2,yy,'#657069')
  d.rect(63,151,w-126,297,'#111718','#303c3f',5);d.graph(98,181,w-196,227)
  grid(d,ps,65,493,w-130,h-582,8,'knob');d.text('50 FACTORY PROGRAMS',w-68,121,9,'#829188','end')
 elif k=='editorial':
  d.text('D.A / ATELIER',70,62,11,d.c,track=3,font='Arial, sans-serif');d.text(n,64,151,78,'#d4cec1',track=-3)
  d.text(ROLE[n],w-74,77,12,d.c,'end',2,font='Arial, sans-serif');d.text('50 / factory',w-74,119,15,'#8f948c','end',0)
  d.line(70,183,w-70,183,'#5c5f56');d.graph(91,233,w*.52,420)
  d.text('SOUND, CONSIDERED.',86,718,13,'#8f988e',track=3)
  grid(d,ps,w*.64,213,w*.30,h-335,2,'row')
 elif k=='orbital':
  d.text('D.A / ORBITAL',w/2,75,12,d.c,'middle',3);d.text(n,w/2,136,48,'#d4ddd8','middle',-1)
  cx=w/2;cy=h*.49
  for r in [184,204,213]:d.circle(cx,cy,r,'#151b1d' if r==184 else 'none','#39464a' if r!=213 else '#283539')
  for i in range(48):
   a=i/48*math.tau;d.line(cx+math.cos(a)*221,cy+math.sin(a)*221,cx+math.cos(a)*(228 if i%4==0 else 224),cy+math.sin(a)*(228 if i%4==0 else 224),'#63777b',1)
  d.graph(cx-132,cy-111,264,222);d.text(ROLE[n],cx,cy+270,11,d.c,'middle',2)
  cut=math.ceil(len(ps)/2)
  for j,group in enumerate([ps[:cut],ps[cut:]]):
   step=min(95,(h-330)/len(group))
   for i,p in enumerate(group):d.param(p,69 if j==0 else w-269,195+i*step,200,step-8,'row')
  d.text('50 FACTORY / SCOPED LEARNING',cx,h-150,10,'#829591','middle',1)
 elif k=='floating':
  d.text('D.A',52,67,13,d.c,track=2);d.text(n,49,127,43,'#d9ded7',track=-1);d.text('QUIET OBJECTS / '+ROLE[n],w-55,77,10,d.c,'end',1.6)
  d.rect(48,167,695,330,'#1b2122','#303b3c',30,True);d.graph(83,207,625,245)
  d.rect(769,167,383,330,'url(#matte)','#3d4645',30,True)
  first=ps[:3]
  for i,p in enumerate(first):d.param(p,804,196+i*94,313,81,'row')
  grid(d,ps[3:],55,540,w-110,h-631,math.ceil(len(ps[3:])/2),'slider')
 elif k=='vertical':
  d.text('M O N O L I T H',w/2,68,11,d.c,'middle',2);d.text(n,w/2,134,51,'#d5ddd6','middle',-.5)
  d.rect(63,171,w-126,306,'#101718','#384447',2);d.graph(94,214,w-188,213)
  d.text(ROLE[n],w/2,460,10,d.c,'middle',2)
  cols=min(8,len(ps));rows=math.ceil(len(ps)/cols)
  grid(d,ps,59,525,w-118,h-623,cols,'vertical')
 elif k=='compact':
  d.rect(39,38,w-78,h-76,'#171b1c','#3d4546',38,True)
  d.text('D.A / DESK INSTRUMENTS',77,83,11,d.c,track=2);d.text(n,75,133,35,'#d7dcd6',track=-1);d.text(ROLE[n],w-80,110,11,d.c,'end',2)
  d.rect(75,161,w-150,208,'#111719','#3b4546',19);d.graph(108,192,w-216,143)
  grid(d,ps,74,411,w-148,h-492,8,'knob')
 elif k=='split':
  d.rect(39,39,259,h-78,'#12191a','#2b383b',8);d.text('D.A / SPLIT',65,83,11,d.c,track=2);d.text(n,63,151,40,'#d7dfd7',track=-1)
  d.text(ROLE[n],65,191,10,d.c,track=1)
  d.param(ps[0],67,302,202,155,'slider');d.text('50 / FACTORY',68,h-169,10,'#84958d',track=2)
  d.rect(330,59,w-389,350,'#131b1d','#39474a',7);d.graph(372,104,w-474,261)
  grid(d,ps[1:],339,470,w-398,h-565,4,'slider')
 elif k=='lab':
  d.text('D.A / SIGNAL LAB / '+s[0],59,64,11,d.c,track=1);d.text(n,57,126,40,'#cbd7ca',track=-1)
  d.text(ROLE[n],w-60,75,11,d.c,'end',1);d.text('50 FACTORY / PARAMETER VIEW',w-60,120,10,'#819486','end',0)
  d.rect(59,157,w-118,278,'#11191b','#46554f',0);d.graph(85,181,w-170,230)
  for x in [59,w-59]:
   for y in [157,435]:d.line(x-5,y,x+5,y,d.c);d.line(x,y-5,x,y+5,d.c)
  grid(d,ps,62,487,w-124,h-580,4,'row')
 elif k=='pads':
  d.text('D.A / SOFT BLOCKS',61,74,11,d.c,track=2);d.text(n,58,137,46,'#dadbd2',track=-1);d.text(ROLE[n],w-61,100,11,d.c,'end',1.4)
  d.rect(61,173,w-122,239,'#111719','#303a3b',23,True);d.graph(95,209,w-190,165)
  grid(d,ps,61,458,w-122,h-552,4,'tile')
 elif k=='cinematic':
  d.text('D.A / HORIZON',64,61,10,d.c,track=4);d.text(n,59,143,68,'#d3dbe0',track=-2)
  d.text(ROLE[n],w-66,100,13,d.c,'end',3);d.text('50 / FACTORY PROGRAMS',w-66,143,10,'#7b8e9b','end',1)
  d.line(63,178,w-63,178,'#3e4b54');d.graph(76,209,w-152,267);d.line(63,502,w-63,502,'#3e4b54')
  grid(d,ps,65,556,w-130,h-655,8,'slider')
 d.foot();folder=OUT/(s[0]+'_'+s[1].replace(' ','_'));folder.mkdir(exist_ok=True);d.finish(folder/(n+'.svg'))
for s in STYLES:
 for n in META:build(n,s)
CSS='''body{margin:0;background:#101314;color:#d4dad3;font:15px Arial,sans-serif;padding:32px}h1{font-weight:400;letter-spacing:2px}p{color:#a1aca6;line-height:1.8;max-width:1000px}a{color:#c0d0c6;text-decoration:none}nav{display:flex;gap:18px;flex-wrap:wrap;margin:26px 0}main{display:grid;grid-template-columns:repeat(auto-fit,minmax(min(100%,480px),1fr));gap:26px}article{background:#1b2021;border:1px solid #34403e;border-radius:12px;padding:18px}img{display:block;width:100%;height:390px;object-fit:contain;background:#0e1214}h2{font-weight:400;font-size:21px}small{color:#96a39a}select{background:#222b2b;color:#e2e8df;padding:12px;border:1px solid #63756c;font-size:17px}header{position:sticky;top:0;background:#101314ed;padding:12px 0;z-index:1}'''
def page(title,body):return f'<!doctype html><html><meta charset="utf-8"><meta name="viewport" content="width=device-width"><title>{title}</title><style>{CSS}</style>{body}</html>'
index='<h1>D.A / NOIR ATLAS</h1><p dir="rtl">۱۰ استایل متفاوت، هرکدام برای هر ۱۰ پلاگین؛ مجموعاً ۱۰۰ طرح مستقل. روی یک استایل بزنید تا تمام پلاگین‌های آن را ببینید. برای مقایسهٔ منصفانهٔ استایل‌ها، صفحهٔ مقایسه را باز کنید و یک پلاگین ثابت انتخاب کنید. این طرح‌ها فقط برای انتخاب مسیر طراحی هستند.</p><nav><a href="Compare.html">COMPARE STYLES / مقایسهٔ استایل‌ها</a></nav><main>'
compare='<header><h1>ONE PLUGIN / TEN DESIGN SYSTEMS</h1><nav><a href="index.html">ALL STYLES</a></nav><label>Plugin: <select id="product">'+''.join(f'<option>{n}</option>' for n in META)+'</select></label></header><p dir="rtl">از منوی بالا پلاگین را عوض کنید؛ هر ده طرح هم‌زمان برای همان پلاگین نمایش داده می‌شوند.</p><main>'
for s in STYLES:
 folder=s[0]+'_'+s[1].replace(' ','_')
 index+=f'<article><a href="{folder}/index.html"><h2>{s[0]} / {s[1]} — {s[2]}</h2><img loading="lazy" src="{folder}/DVerb.svg" alt="{s[1]} DVerb"><p>10 plugins / {s[3]} × {s[4]} / {s[7]}</p></a></article>'
 compare+=f'<article><h2>{s[0]} / {s[1]} — {s[2]}</h2><a data-style="{folder}" href="{folder}/DVerb.svg"><img loading="lazy" src="{folder}/DVerb.svg" alt="{s[1]}"></a></article>'
 body=f'<h1>{s[0]} / {s[1]} — {s[2]}</h1><nav><a href="../index.html">ALL STYLES</a><a href="../Compare.html">COMPARE</a></nav><main>'
 for n in META:body+=f'<article><a href="{n}.svg"><h2>{n}</h2><img loading="lazy" src="{n}.svg" alt="{n}"></a></article>'
 (OUT/folder/'index.html').write_text(page(s[1],body+'</main>'))
(OUT/'index.html').write_text(page('D.A / Noir Atlas',index+'</main>'))
compare+='''</main><script>document.getElementById('product').addEventListener('change',e=>{document.querySelectorAll('[data-style]').forEach(a=>{a.href=a.dataset.style+'/'+e.target.value+'.svg';a.querySelector('img').src=a.href;});});</script>'''
(OUT/'Compare.html').write_text(page('Compare ten styles',compare))
manifest={'styles':[{'id':s[0],'name':s[1],'persian':s[2],'layout':s[7],'width':s[3],'height':s[4]} for s in STYLES],'plugins':list(META),'designs':100,'parameter_representations':sum(len(x) for x in META.values())*10,'runtime_changes':False}
(OUT/'Manifest.json').write_text(json.dumps(manifest,ensure_ascii=False,indent=2))
(OUT/'READ_ME.txt').write_text('''D.A / NOIR ATLAS — 10 distinct styles × 10 products = 100 editable SVG proposals.

ابتدا index.html را باز کنید. هر پوشه یک استایل با هر ده پلاگین دارد.
برای انتخاب راحت‌تر، Compare.html را باز کنید؛ یک پلاگین ثابت را در ده استایل
می‌بینید و می‌توانید پلاگین را از منو عوض کنید. روی هر تصویر بزنید تا SVG آن باز شود.
بهتر است ابتدا استایل موردعلاقه را انتخاب کنید، سپس جزئیات پلاگین‌ها را اصلاح کنیم.

01 Graphite Console: physical rack frame, recessed display, compact rotary controls.
02 Atelier: editorial serif typography, large graphic with quiet numeric side columns.
03 Orbital: circular visual instrument, perimeter markers, two flanking parameter rails.
04 Quiet Objects: separate floating soft objects, asymmetric graphic and timing object.
05 Monolith: portrait chassis, tall vertical channel faders and embedded display.
06 Desk Instruments: wide low desktop unit, rounded window and a row of small encoders.
07 Split Architecture: left identity/control rail, asymmetric right workspace.
08 Signal Lab: square geometry, monospace numerics, calibrated grid and rows.
09 Soft Blocks: sculpted tactile parameter pads with subtle shadows, no shared huge knob.
10 Horizon: cinematic panoramic graphic, broad typography and thin control strips.

Ten separate compositions, not ten colour swaps. Matte black remains the family material.
No mandatory purple light or copied third-party branding. Each product has a restrained accent.
Every style includes all existing parameter IDs (920 total representations across 100 designs).
Learn indicators appear only on eligible controls. DETAILS / LOG is explicit, not an open popup.
All curves, meters, fields and room diagrams are illustrative, NOT measured or live.
Concept-only visualizations can differ from displayed initial parameter values.
These are proposals for approval, not implemented editors or new DSP.
All SVGs are standalone, editable vector/text documents without linked images or network assets.
System font fallbacks: Arial, Georgia and Courier New. Fonts are not embedded; appearance may
vary slightly across systems. HTML preview works locally after extracting the ZIP.

هیچ تغییری در DSP یا رابط اجرایی پلاگین‌ها اعمال نشده است. فقط طرح برای تأیید شما.
''')
archive=OUT.parent/'DAPlugins_NoirAtlas_100_SVG.zip'
with zipfile.ZipFile(archive,'w',zipfile.ZIP_DEFLATED) as z:
 for p in sorted(OUT.rglob('*')):
  if p.is_file():z.write(p,'DAPlugins_NoirAtlas/'+str(p.relative_to(OUT)))
with zipfile.ZipFile(archive) as z:
 assert z.testzip() is None
 assert len([n for n in z.namelist() if n.endswith('.svg')])==100
print('PASS: 100 SVG / 10 distinct layouts / 920 parameter representations / no duplicate IDs / CRC:',archive)
