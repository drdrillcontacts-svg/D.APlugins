"""High-resolution DESIGN PREVIEWS, not native screenshots or analysis results.
Uses the same 1200x1120 Layouts.json and real parameter registry as the JUCE editor.
Example traces/meters illustrate the visual language; the native UI uses live audio instead.
"""
import base64,html,json,math,random,io
from pathlib import Path
from PIL import Image
ROOT=Path(__file__).resolve().parents[1];OUT=ROOT/'Design/UI-Previews';OUT.mkdir(exist_ok=True)
LAYOUTS=json.loads((ROOT/'Design/UI/Layouts.json').read_text());META=json.loads((ROOT/'Design/UI/Parameters.json').read_text())
FONT=base64.b64encode((ROOT/'Design/Fonts/RajdhaniMedium.ttf').read_bytes()).decode()
LOGO=base64.b64encode((ROOT/'Design/Brand/DAmark.png').read_bytes()).decode()
random.seed(90127);im=Image.new('RGB',(64,64));im.putdata([tuple([int(12+(6 if (x+y)%8<4 else 2)+random.random()*2)]*3) for y in range(64) for x in range(64)])
b=io.BytesIO();im.save(b,format='PNG');TEXTURE=base64.b64encode(b.getvalue()).decode()
RED='#ed173b';WHITE='#d0d2d2';GREY='#92999b'
class SVG:
 def __init__(self):self.a=[]
 def put(self,s):self.a.append(s)
 def text(self,s,x,y,size=14,colour=GREY,anchor='start',spacing=0,weight=500):
  self.put(f'<text x="{x}" y="{y}" font-family="Rajdhani" font-weight="{weight}" font-size="{size}" letter-spacing="{spacing}" fill="{colour}" text-anchor="{anchor}">{html.escape(str(s))}</text>')
 def path(self,d,stroke,sw=1,fill='none',extra=''):self.put(f'<path d="{d}" fill="{fill}" stroke="{stroke}" stroke-width="{sw}" {extra}/>')
 def line(self,x1,y1,x2,y2,c='#343b3d',w=1):self.put(f'<path d="M{x1} {y1}L{x2} {y2}" stroke="{c}" stroke-width="{w}"/>')
 def rect(self,x,y,w,h,fill,stroke='none',rx=0):self.put(f'<rect x="{x}" y="{y}" width="{w}" height="{h}" rx="{rx}" fill="{fill}" stroke="{stroke}"/>')
 def circle(self,x,y,r,fill,stroke='none',sw=1,extra=''):self.put(f'<circle cx="{x}" cy="{y}" r="{r}" fill="{fill}" stroke="{stroke}" stroke-width="{sw}" {extra}/>')
def bevel(x,y,w,h,c=10):return f'M{x+c} {y}H{x+w-c}L{x+w} {y+c}V{y+h-c}L{x+w-c} {y+h}H{x+c}L{x} {y+h-c}V{y+c}Z'
def panel(d,r,accent=False):
 x,y,w,h=r;d.path(bevel(*r),'#353c3e',1,'url(#panel)');d.path(bevel(x+3,y+3,w-6,h-6,8),'#000',2);d.line(x+12,y+2,x+w-12,y+2,'#555e61',.35)
 if accent:d.line(x+1,y+18,x+1,y+h-18,'#a4142a',1.3)
def button(d,s,x,y,w,h,active=False):
 d.path(bevel(x,y,w,h,5),RED if active else '#3c4345',1,'url(#redButton)' if active else 'url(#button)');d.line(x+7,y+1,x+w-7,y+1,'#596064',.3)
 d.text(s,x+w/2,y+h*.5+4,11 if h<24 else 13,RED if active else WHITE,'middle')
def meter(d,x,y,w,h,db=-9,scale=False):
 for i in range(40):
  threshold=-72+i*72/40;on=db>=threshold
  d.rect(x,y+h-(i+1)*h/40,w,h/40*.58,('#ed173b' if threshold>-6 else '#c21a34' if threshold>-30 else '#c3c8c9') if on else '#202628')
 if scale:
  for i in range(7):d.text(str(-i*12),x+w+8,y+i*h/6+4,11)
def valtext(m):
 v=m['initial'];id=m['id'];unit=m['unit'];percent=unit=='%' or id in ['mix','width','density','diffusion','modulation','sensitivity','depth','focus','delayMix','reverbMix']
 if percent:return f'{round(v*100)}%'
 if unit=='Hz':return f'{v/1000:.1f} kHz' if v>=1000 else f'{v:.0f} Hz'
 if unit=='ms':return f'{v:.1f} ms' if v<10 else f'{v:.0f} ms'
 if unit==':1':return f'{v:.1f}:1'
 if unit in ['s','dB']:return f'{v:.1f} {unit}'
 return f'{v:.2f}'
def polar(cx,cy,r,a):return cx+math.sin(a)*r,cy-math.cos(a)*r
def arc(cx,cy,r,start,end):
 x,y=polar(cx,cy,r,start);a,b=polar(cx,cy,r,end);return f'M{x:.3f} {y:.3f}A{r:.3f} {r:.3f} 0 {int(end-start>math.pi)} 1 {a:.3f} {b:.3f}'
def knob(d,m,x,y,w,h):
 # This mirrors the native slider's rotary area and lower value/learn labels.
 top=22;sliderheight=h-top-20;texth=18 if h<150 else 24;rotaryh=sliderheight-texth
 diameter=max(16,min(w,rotaryh)-(4 if min(w,rotaryh)<90 else 16));cx=x+w/2;cy=y+top+rotaryh/2;r=diameter/2
 position=(m['initial']-m['min'])/(m['max']-m['min'])
 if m['unit']=='Hz' or m['id'] in ['time','release']:
  c=math.sqrt(m['min']*m['max']);skew=math.log(.5)/math.log((c-m['min'])/(m['max']-m['min']));position=position**skew
 start=math.pi*1.22;end=math.pi*2.78;angle=start+position*(end-start)
 d.circle(cx+1,cy+5,r+2,'#000',extra='filter="url(#shadow)"')
 d.circle(cx,cy,r,'url(#steel)');d.circle(cx,cy,r-1.5,'#020304');d.circle(cx,cy,r-4,'none','#242a2c',1)
 d.path(arc(cx,cy,r-7,start,end),'#414749',1)
 if position>0:
  d.path(arc(cx,cy,r-7,start,angle),RED,7,extra='opacity=".22" filter="url(#glow)"');d.path(arc(cx,cy,r-7,start,angle),RED,2.6,extra='stroke-linecap="round"')
 d.circle(cx,cy,r-12,'url(#face)','#3a4042',.7);d.circle(cx,cy,r-13.5,'none','#000',1)
 for j in range(56):
  a=j*math.tau/56;px,py=polar(cx,cy,r-14,a);d.path(f'M{cx} {cy}L{px} {py}','#879095',.2,extra='opacity=".14"')
 # Opposing low-opacity triangular highlights suggest brushed radial machining.
 for a,opacity in [(0,.08),(math.pi,.07)]:
  p1=polar(cx,cy,r-14,a-.18);p2=polar(cx,cy,r-14,a+.5);d.path(f'M{cx} {cy}L{p1[0]} {p1[1]}L{p2[0]} {p2[1]}Z','none',0,'#d7dddf',f'opacity="{opacity}"')
 for j in range(11):
  a=start+(end-start)*j/10;p=polar(cx,cy,r-1,a);q=polar(cx,cy,r-3,a);d.line(*p,*q,GREY if j%5==0 else '#454b4d',.7)
 p=polar(cx,cy,r-17,angle);q=polar(cx,cy,r-26,angle);d.line(*p,*q,WHITE,1.4)
 d.text(valtext(m),cx,y+h-27,17,WHITE,'middle');button(d,'LEARN',cx-min(70,w-8)/2,y+h-18,min(70,w-8),17)
def waveform(d,x,y,w,h,seed):
 rng=random.Random(seed)
 for i in range(int(w)):
  t=i/w;peak=(.13+abs(math.sin(i*.83))*rng.uniform(.3,.85))*(math.exp(-t*3.2)+.05);height=peak*h*.46;d.line(x+i,y+h/2-height,x+i,y+h/2+height,'#858d91',.7)
 d.line(x,y+h/2,x+w,y+h/2,'#343b3d',.6)
def graph(d,name,x,y,w,h,meta):
 for i in range(8):d.line(x+w*i/7,y,x+w*i/7,y+h,'#202729',.5)
 for i in range(5):d.line(x,y+h*i/4,x+w,y+h*i/4,'#202729',.5)
 m={p['id']:p['initial'] for p in meta}
 if name=='DStereo':
  cx=x+w/2;cy=y+h/2;d.circle(cx,cy,65,'none','#394143',.6);d.line(cx-70,cy,cx+70,cy,'#394143',.5);d.line(cx,cy-65,cx,cy+65,'#394143',.5)
  rng=random.Random(7)
  for i in range(330):
   a=rng.gauss(0,13);b=rng.gauss(0,28);d.circle(cx+a,cy+b,.8,RED,extra=f'opacity="{rng.uniform(.15,.7):.2f}"')
  d.text('L / R',cx,y+h+14,10,GREY,'middle');return
 if name=='DSess':
  fc=m['frequency'];px=x+math.log(fc/20)/math.log(1000)*w;d.rect(px-22,y,44,h,'#3b0b16');d.line(px,y,px,y+h,RED,.6)
 def curve(offset=0):
  pts=[]
  for i in range(256):
   t=i/255
   if name=='DComp':
    db=-60+t*60;over=max(0,db-m['threshold']);v=(db-over*(1-1/m['ratio'])+m['makeup']+60)/60
   elif name=='DGate':
    db=-80+t*80;v=t-(m['range']/80 if db<m['threshold'] else 0)
   elif name=='DSat':v=math.tanh((t*2-1)*10**(m['drive']/20))*.45+.5
   elif name=='DEQ':v=.5
   elif name=='DTrans':v=max(0,.2+math.sin(i*.1)**8*.5)*math.exp(-t*.6)
   else:v=max(.02,(.72-.42*t+math.sin(i*.13)*.035+math.sin(i*.34)*.025)*math.exp(-t*.38)-offset*.018)
   pts.append((x+t*w,y+h-v*h))
  return 'M'+'L'.join(f'{a:.2f} {b:.2f}' for a,b in pts)
 if name=='DVerb':
  for j in range(12):d.path(curve(j),RED,.6,extra=f'opacity="{.13+(11-j)*.018}"')
 path=curve();d.path(path+f'L{x+w} {y+h}L{x} {y+h}Z','none',0,'url(#spectralFill)')
 d.path(path,RED,4,extra='opacity=".2" filter="url(#glow)"');d.path(path,RED,1.2)
 labels=['20','50','100','200','500','1k','5k','20k'] if name not in ['DComp','DGate','DSat','DTrans'] else ['INPUT','','','','','','','OUTPUT']
 for i,t in enumerate(labels):d.text(t,x+w*i/7,y+h+13,10,GREY,'middle')
def render(spec):
 name=spec['name'];meta=META[name];byid={m['id']:m for m in meta};d=SVG()
 d.put('''<svg xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink" viewBox="0 0 1200 1120" width="2400" height="2240"><defs>''')
 d.put(f'<style>@font-face{{font-family:Rajdhani;src:url(data:font/ttf;base64,{FONT})}}text{{font-family:Rajdhani}}</style>')
 d.put(f'<pattern id="weave" width="64" height="64" patternUnits="userSpaceOnUse"><image width="64" height="64" href="data:image/png;base64,{TEXTURE}"/></pattern>')
 d.put('''<linearGradient id="panel" x1="0" y1="0" x2="1" y2="1"><stop stop-color="#181c1e"/><stop offset="1" stop-color="#060809"/></linearGradient>
 <linearGradient id="button" x1="0" y1="0" x2="0" y2="1"><stop stop-color="#1c2123"/><stop offset="1" stop-color="#080a0b"/></linearGradient>
 <linearGradient id="redButton" x1="0" y1="0" x2="0" y2="1"><stop stop-color="#361018"/><stop offset="1" stop-color="#080a0b"/></linearGradient>
 <linearGradient id="steel" x1="0" y1="0" x2="0" y2="1"><stop stop-color="#555b5d"/><stop offset=".3" stop-color="#13181a"/><stop offset="1" stop-color="#040506"/></linearGradient>
 <linearGradient id="face" x1="0" y1="0" x2="1" y2="1"><stop stop-color="#3b4143"/><stop offset=".37" stop-color="#15191b"/><stop offset=".61" stop-color="#090c0e"/><stop offset="1" stop-color="#050708"/></linearGradient>
 <linearGradient id="spectralFill" x1="0" y1="0" x2="0" y2="1"><stop stop-color="#ed173b" stop-opacity=".18"/><stop offset="1" stop-color="#ed173b" stop-opacity="0"/></linearGradient>
 <filter id="glow" x="-50%" y="-50%" width="200%" height="200%"><feGaussianBlur stdDeviation="3"/></filter>
 <filter id="shadow" x="-50%" y="-50%" width="200%" height="200%"><feGaussianBlur stdDeviation="4"/></filter>
 <clipPath id="graphClip"><rect x="325" y="200" width="555" height="140"/></clipPath></defs>''')
 d.rect(0,0,1200,1120,'#030405');d.path(bevel(20,20,1160,1080,28),'#50575a',1,'url(#weave)');d.path(bevel(24,24,1152,1072,26),'#9b1328',.8);d.path(bevel(26,26,1148,1068,24),'#020303',4)
 d.path('M190 26L280 126H920L1010 26','#010203',9);d.path('M190 26L280 126H920L1010 26','#4b5254',1);d.line(207,28,291,120,'#a4142a',.8);d.line(993,28,909,120,'#a4142a',.8)
 d.line(51,134,1149,134,'#484f51');d.line(51,137,1149,137,'#000',3)
 d.put(f'<image x="74" y="55" width="89" height="40" href="data:image/png;base64,{LOGO}" opacity=".76"/>');d.text('P L U G I N S',120,108,11,WHITE,'middle')
 for i,c in enumerate(name.upper()):d.text(c,600-len(name)*15+i*30+15,78,36,RED if i==0 else WHITE,'middle')
 d.text(spec['subtitle'],600,105,13,GREY,'middle',1.8);d.text('DESIGN / 0.3',1133,84,12,GREY,'end')
 panel(d,[45,152,1110,236]);d.line(304,173,304,354);d.line(899,173,899,354)
 d.text('INPUT',64,186,13,WHITE);d.text('REFERENCE',913,186,13,WHITE);d.text('OUT',1119,185,13,WHITE,'middle')
 meter(d,66,208,6,135,-12);meter(d,77,208,6,135,-13);waveform(d,100,209,190,127,1)
 waveform(d,914,209,182,127,2);meter(d,1111,208,6,135,-9);meter(d,1122,208,6,135,-10)
 d.text('REFERENCE / EXAMPLE',914,358,12);d.text('AUDIO-GUIDED LEARNING  /  READY',325,186,12,RED)
 d.put('<g clip-path="url(#graphClip)">');graph(d,name,325,200,555,140,meta);d.put('</g>')
 captions={'DVerb':'SPECTRAL HISTORY','DDelay':'ECHO / FREQUENCY FIELD','DComp':'DYNAMICS TRANSFER','DEQ':'FOUR-BAND RESPONSE','DSat':'HARMONIC TRANSFER','DTrans':'TRANSIENT / SUSTAIN','DStereo':'STEREO VECTORSCOPE','DSess':'SIBILANCE DETECTION','DGate':'GATE TRANSFER','DFX':'CHAIN / FREQUENCY FIELD'}
 d.text(captions[name],880,361,11,GREY,'end')
 for section in spec['sections']:panel(d,section['rect'],True);d.text(section['title'],section['rect'][0]+15,section['rect'][1]+31,14,WHITE)
 lr=spec['learningRect'];ar=spec['advancedRect'];panel(d,lr,True);panel(d,ar);panel(d,[1038,lr[1],116,lr[3]])
 d.text('L E A R N   M Y   S O U N D',lr[0]+20,lr[1]+30,16,WHITE);d.text(spec['advancedTitle'],ar[0]+20,ar[1]+30,16,WHITE)
 if name not in ['DEQ','DFX']:d.text(spec['slogan'],600,666,12,GREY,'middle',.6)
 for c in spec['controls']:
  m=byid[c['id']];x,y,w,h=c['rect'];caption=c['label']
  if caption:d.text(caption,x+w/2,y+15,12 if w<100 else 14,WHITE,'middle')
  if c['options']:
   top=0 if not caption else 22
   if h>180:top=80
   count=len(c['options']);bh=min(32,max(18,h-top-19))
   for i,opt in enumerate(c['options']):button(d,opt,x+i*w/count,y+top,w/count-4,bh,int(m['initial'])==i)
   button(d,'LEARN',x+w/2-32,y+top+bh+2,64,16)
  else:knob(d,m,x,y,w,h)
 for m in spec['modules']:button(d,m['title'],*m['rect'])
 x,y,w,h=lr;button(d,'SUGGESTIONS',x+w-149,y+12,132,24)
 if name in ['DEQ','DFX']:button(d,'LEARN ALL',x+16,y+50,432,39,True)
 else:
  d.circle(600,516,49,'url(#steel)');d.circle(600,516,40,'none',RED,2);d.circle(600,516,45,'none',RED,6,extra='opacity=".1" filter="url(#glow)"');d.circle(600,516,34,'#080a0b');d.text('LEARN ALL',600,520,13,RED,'middle')
 button(d,'INPUT-AWARE LEARN',x+16,y+113,268,30);button(d,'LOAD FILE',x+296,y+113,152,30)
 d.text('NO SETTINGS CHANGE UNTIL YOU APPLY',x+18,y+266,11,'#5e686c');d.text('DESCRIBE THE SOUND',x+18,y+171,12);d.rect(x+16,y+179,432,64,'#0a0c0e','#303638',3)
 d.text('Dark, warm, wide, smooth, cinematic…',x+29,y+204,15,GREY)
 for j,t in enumerate(['PREVIEW','APPLY','REJECT','UNDO']):button(d,t,x+16+j*110,y+h-42,102,27,t=='APPLY')
 if name=='DDelay':d.text('L  >  FILTER  >  FEEDBACK  >  R',775,981,12,GREY,'middle')
 if name in ['DComp','DSess']:
  d.text('GAIN REDUCTION',775,907,13,GREY,'middle');d.text('3.1 dB',775,949,38,RED,'middle');d.text('-12.0 dBFS   /   -9.0 dBFS',775,975,18,WHITE,'middle')
 if name=='DGate':
  d.text('GAIN REDUCTION',855,774,13,GREY,'middle');d.text('0.0 dB',855,823,38,RED,'middle');d.text('-12.0 / -12.0 dBFS',855,865,18,WHITE,'middle')
 if name=='DStereo':
  d.text('CHANNEL CORRELATION',775,837,13,GREY,'middle');d.text('+0.82',775,884,36,WHITE,'middle');d.text('-1                     0                     +1',775,922,12,GREY,'middle')
 if name in ['DTrans','DSat']:
  d.text('INPUT / OUTPUT',775,837,13,GREY,'middle');d.text('-12.0 dBFS   /   -9.0 dBFS',775,877,18,WHITE,'middle')
  if name=='DTrans':d.text('LIVE ENVELOPE ABOVE / MANUAL CONTROL ALWAYS',775,919,12,GREY,'middle')
 if name=='DEQ':d.text('BROAD TONAL GUIDANCE / FOUR INDEPENDENT BANDS',775,805,15,WHITE,'middle');d.text('INPUT / OUTPUT',775,883,13,GREY,'middle');d.text('-12.0 dBFS   /   -9.0 dBFS',775,925,18,WHITE,'middle')
 meter(d,1058,ar[1]+33,11,ar[3]-85,-12);meter(d,1078,ar[1]+33,11,ar[3]-85,-9,True);d.text('IN / OUT',1095,ar[1]+ar[3]-17,12,GREY,'middle')
 d.line(48,1041,1152,1041);d.text('READY / INPUT-AWARE / MANUAL CONTROL ALWAYS',310,1055,12,GREY)
 d.put(f'<image x="71" y="1062" width="48" height="21" href="data:image/png;base64,{LOGO}" opacity=".42"/>');d.text(name.upper(),146,1077,15,GREY,spacing=2)
 d.text('DESIGN PREVIEW · ILLUSTRATIVE SIGNAL, NOT A NATIVE SCREENSHOT',1125,1077,10,GREY,'end')
 d.put('</svg>');(OUT/f'{name}.svg').write_text(''.join(d.a))
for s in LAYOUTS:render(s)
print('Exported 10 vector design studies (example audio is explicitly illustrative)')
