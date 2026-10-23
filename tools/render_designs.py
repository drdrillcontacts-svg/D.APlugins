"""High-resolution DESIGN PREVIEWS, not native screenshots or analysis results.
Uses the same 1200x820 Layouts.json and real parameter registry as the JUCE editor.
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
 d.text(valtext(m),cx,y+h-27,17,WHITE,'middle')
 if m.get('learnable'):button(d,'LEARN',cx-min(70,w-8)/2,y+h-18,min(70,w-8),17)
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
def render(spec,advanced=False):
 name=spec['name'];meta=META[name];byid={m['id']:m for m in meta};d=SVG()
 d.put('<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 1200 820" width="2400" height="1640"><defs>')
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
 <clipPath id="graphClip"><rect x="114" y="177" width="570" height="155"/></clipPath></defs>''')

 d.rect(0,0,1200,820,'#030405')
 d.rect(20,20,1160,784,'url(#steel)','#7b8082',15)
 d.rect(23,23,1154,778,'#000',rx=12);d.rect(27,27,1146,770,'url(#weave)','#444a4c',9)
 d.line(42,799,1158,799,RED,5);d.line(42,799,1158,799,'#f34a63',.7)
 panel(d,[40,38,1120,87])
 d.put(f'<image x="67" y="57" width="106" height="46" href="data:image/png;base64,{LOGO}" opacity=".88"/>')
 d.text(name.upper(),208,79,33,WHITE,spacing=4);d.text(spec['subtitle'],210,103,12,GREY,spacing=1)
 d.path('M950 40L900 122H924L974 40Z','none',0,'#ed173b','opacity=".13"');d.line(950,40,900,122,RED,.7)
 d.text('FACTORY PRESET / 50',610,62,11,GREY)
 button(d,'<',570,72,30,30);button(d,'01 / Init / Factory Default',610,72,275,30);button(d,'>',895,72,30,30)
 d.text('AUDIO GUIDED',1126,75,14,RED,'end');d.text('DESIGN / 0.4',1126,101,12,GREY,'end')
 panel(d,[48,136,882,236]);panel(d,spec['learningRect']);panel(d,[48,404,1104,236])
 meter(d,64,180,7,151,-12);meter(d,75,180,7,151,-13);d.text('IN',60,161,12,WHITE)
 d.text('READY / MANUAL CONTROL ALWAYS',114,161,12,RED)
 d.put('<g clip-path="url(#graphClip)">');graph(d,name,114,177,570,155,meta);d.put('</g>')
 d.line(701,150,701,351);d.text('REFERENCE',715,163,13,WHITE);waveform(d,715,182,140,146,2)
 d.text('EXAMPLE TRACE',715,355,11,GREY);meter(d,881,180,7,151,-9);meter(d,893,180,7,151,-10);d.text('OUT',886,161,12,WHITE)
 captions={'DVerb':'SPECTRAL HISTORY','DDelay':'ECHO / FREQUENCY FIELD','DComp':'DYNAMICS TRANSFER','DEQ':'FOUR-BAND RESPONSE','DSat':'HARMONIC TRANSFER','DTrans':'TRANSIENT / SUSTAIN','DStereo':'STEREO VECTORSCOPE','DSess':'SIBILANCE DETECTION','DGate':'GATE TRANSFER','DFX':'CHAIN / FREQUENCY FIELD'}
 d.text(captions[name],684,355,11,GREY,'end')
 metrics='GR 3.1 dB' if name in ['DComp','DSess','DGate'] else 'CORR +0.82' if name=='DStereo' else 'IN -12.0 / OUT -9.0 dBFS'
 d.text(metrics,114,355,11,WHITE)
 d.text('LEARNING',956,170,15,WHITE)
 button(d,'INPUT-AWARE LEARN',956,185,180,30);button(d,'LOAD FILE',956,224,180,30)
 button(d,'LEARN ALL',956,263,180,34,True);button(d,'DETAILS / LOG',956,308,180,30)
 d.text(spec['advancedTitle'] if advanced else spec['slogan'],64,394,13,GREY,spacing=.7)
 button(d,'MAIN CONTROLS' if advanced else 'ADVANCED',978,376,160,28,advanced)
 for c in spec['controls']:
  if c['advanced']!=advanced:continue
  m=byid[c['id']];x,y,w,h=c['rect'];caption=c['label']
  if caption:d.text(caption,x+w/2,y+15,12 if w<100 else 14,WHITE,'middle')
  if c['style']=='menu':
   button(d,c['options'][int(m['initial'])]+'  v',x,y,w,h)
  elif c['options']:
   top=0 if not caption else 22
   if h>180:top=80
   count=len(c['options']);bh=min(32,max(18,h-top-19))
   for i,opt in enumerate(c['options']):button(d,opt,x+i*w/count,y+top,w/count-4,bh,int(m['initial'])==i)
   if c['learnable']:button(d,'LEARN',x+w/2-32,y+top+bh+2,64,16)
  else:knob(d,m,x,y,w,h)
 if not advanced:
  for m in spec['modules']:button(d,m['title'],*m['rect'])
 d.text('DESCRIBE / OPTIONAL ENGLISH KEYWORDS',68,657,12,GREY)
 d.rect(68,666,1064,40,'#0a0c0e','#303638',3);d.text('Dark, warm, wide, smooth, cinematic…',82,691,15,GREY)
 d.text('PREVIEW AUDITIONS / APPLY KEEPS / REJECT RESTORES',68,725,11,GREY)
 d.line(68,727,1132,727)
 for i,t in enumerate(['PREVIEW','APPLY','REJECT','UNDO']):button(d,t,68+i*162,738,150,28,t=='APPLY')
 d.text('DESIGN STUDY / ILLUSTRATIVE AUDIO, NOT A NATIVE SCREENSHOT',1132,756,10,GREY,'end')
 d.text('READY / INPUT-AWARE / MANUAL CONTROL ALWAYS',68,790,12,GREY)
 for x,y in [(34,34),(1166,34),(34,790),(1166,790)]:
  d.circle(x,y,5,'#050607','#4d5355',.8);d.line(x-2.5,y,x+2.5,y,'#4d5355',1)
 d.put('</svg>');(OUT/f"{name}{'-Advanced' if advanced else ''}.svg").write_text(''.join(d.a))
for s in LAYOUTS:
 render(s);render(s,True)
print('Exported 20 vector design studies: ten main + ten Advanced panels (illustrative, not native screenshots)')
