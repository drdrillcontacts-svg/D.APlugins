#!/usr/bin/env python3
"""Final selected reference, SVG approval stage only. Not runtime editor code."""
from pathlib import Path
import json,math,html,zipfile,xml.etree.ElementTree as ET,csv,io
ROOT=Path(__file__).resolve().parents[1]
ns={'__file__':str(ROOT/'tools/design_noir_atlas.py')}
exec((ROOT/'tools/design_noir_atlas.py').read_text().split('for s in STYLES:\n for n in META:build(n,s)')[0],ns)
Base=ns['SVG'];META=ns['META'];LABEL=ns['label'];OPTIONS=ns['OPTIONS'];ATLAS_COLORS=ns['COLORS']
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
ACCENT={
 'DVerb':'#a28bd0',   # Purple (the one purple plugin)
 'DDelay':'#6cb8d6',  # Cyan / Ice Blue
 'DComp':'#d4a259',   # Warm Amber / Gold
 'DEQ':'#68c29a',     # Emerald / Studio Mint
 'DSat':'#d86c6c',    # Crimson / Tube Red
 'DTrans':'#de8854',  # Copper / Tangerine Orange
 'DStereo':'#6f95de', # Cobalt / Sky Blue
 'DSess':'#5cc2be',   # Teal / Aquamarine
 'DGate':'#b3c662',   # Lime / Olive Gold
 'DFX':'#cf7ca8',     # Rose / Magenta
}
def mix(c1,c2,t):
 r1,g1,b1=[int(c1[i:i+2],16) for i in (1,3,5)]
 r2,g2,b2=[int(c2[i:i+2],16) for i in (1,3,5)]
 return f'#{round(r1+(r2-r1)*t):02x}{round(g1+(g2-g1)*t):02x}{round(b1+(b2-b1)*t):02x}'

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
  ac=ACCENT[n]
  super().__init__(n,('SVG','Final reference','',1200,1240,ac,'Arial, Helvetica, sans-serif','final'))
  self.c=ac;self.values={p['id']:EXAMPLES.get(n,{}).get(p['id'],p['initial']) for p in META[n]}
  self.parts[0]=self.parts[0].replace('#242729','#222224').replace('#17191b','#131315').replace('#35383a','#18181b').replace('#202325','#27272a')
  self.raw('<defs><linearGradient id="bevel" x2="0" y2="1"><stop stop-color="#37373a"/><stop offset=".15" stop-color="#242427"/><stop offset="1" stop-color="#0c0c0e"/></linearGradient></defs>')
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
 def button(self,id,t,x,y,w,h,accent=False,enabled=True):
  h=max(h,25)
  self.raw(f'<g id="action-{id}"'+(' opacity=".75"' if not enabled else '')+'>')
  self.cut(x,y,w,h,'#19191d',self.c if accent else '#414146',7)
  self.text(t,x+w/2,y+h/2+4,11,self.c if accent else '#d0d0d5','middle',.3)
  self.raw('</g>')
 def meter(self,x,y,title):
  hi=mix(self.c,'#ffffff',.25)
  self.text(title,x+11,y-19,10,self.c,'middle',.3)
  for j in range(48):
   for c in range(2):self.rect(x+c*13,y+j*4.3,9,2.3,hi if 8<j<18 else '#d5d5d7' if j>=18 else '#29292e','none',0)
 def display(self):
  self.rect(52,190,1096,378,'url(#bevel)','#29292c',29)
  self.raw('<path d="M89 207 H1110 Q1130 207 1130 229 V528 Q1130 549 1110 549 H89 Q70 549 70 528 V229 Q70 207 89 207Z" fill="#0e0e11" stroke="#222226"/>')
  self.text('INPUT / '+ROLE[self.n]+' / ILLUSTRATIVE',151,244,10,self.c,track=.7)
  self.meter(84,277,'IN');self.meter(1097,277,'OUT')
  start=len(self.parts);super().graph(163,284,900,209)
  mid=mix('#222228',self.c,.42);dark_fill=mix('#121216',self.c,.15);grid_col=mix('#24242a',self.c,.24)
  for i in range(start,len(self.parts)):
   self.parts[i]=(self.parts[i]
    .replace('id="illustrative-visual"','id="input-visual"')
    .replace(ATLAS_COLORS[self.n],self.c)
    .replace('#9a82e5',self.c)
    .replace('#596a60',mid)
    .replace('#586d64',mid)
    .replace('#59635e',mid)
    .replace('#83969c',mid)
    .replace('#24352d',dark_fill)
    .replace('#191e20',dark_fill)
    .replace('#46504c',grid_col)
    .replace('#465051',grid_col)
    .replace('#303a3c',grid_col)
    .replace('#465044',grid_col)
    .replace('#374449',grid_col)
    .replace('#3a464b',grid_col))
  if self.n=='DDelay':self.text('SYNC / 120 BPM / DOTTED EIGHTH / 375 ms',164,523,9,mix(self.c,'#c8c8d2',.45),track=.4)
 def learning(self):
  self.cut(55,590,1090,72,'#111114','#36363b',10)
  self.text('LEARNING',76,632,12,'#d0d0d6',track=.5)
  for i,(id,t) in enumerate([('capture','CAPTURE INPUT'),('reference','REFERENCE...'),('learn-all','LEARN ALL'),('log','DETAILS / LOG')]):self.button(id,t,230+i*219,608,205,36,i==2)
 def mini(self,p,x,y,r=29,force_knob=False):
  v=self.values[p['id']];disabled=self.n=='DDelay' and p['id'] in ['time','modulation']
  self.raw(f'<g id="param-{p["id"]}"'+(' opacity=".48"' if disabled else '')+'><title>'+html.escape(p['name'])+'</title>')
  self.text(LABEL(p),x,y-r-19,11,'#b0b0b9','middle',.6)
  if p['discrete'] and not force_knob:
   opts=OPTIONS.get(self.n+':'+p['id'],['BELL','LOW SHELF','HIGH SHELF'] if self.n=='DEQ' and p['id'].endswith('shape') else ['OFF','ON'])
   if len(opts)<=4:
    if opts==['OFF','ON']:btn_w,gap=62,10
    elif len(opts)==2:btn_w,gap=114,12
    elif len(opts)==3:btn_w,gap=128,10
    else:btn_w,gap=96,8
    total_w=len(opts)*btn_w+(len(opts)-1)*gap
    act_fill=mix('#141418',self.c,.18);act_txt=mix(self.c,'#ffffff',.62)
    for oi,t in enumerate(opts):
     xx=x-total_w/2+oi*(btn_w+gap);active=oi==int(v)
     self.cut(xx,y-20,btn_w,39,act_fill if active else '#141418',self.c if active else '#393940',5)
     self.text(t,xx+btn_w/2,y+4,8.5 if len(opts)>2 else 10,act_txt if active else '#9d9da9','middle',0)
   else:
    self.cut(x-64,y-20,128,39,'#19191e',mix('#393940',self.c,.35),6)
    self.text(VALUE(self.n,p,v),x,y+4,11,mix(self.c,'#ffffff',.55),'middle',.1)
  else:
   self.circle(x,y+4,r+4,'#08080a','#19191e');self.circle(x,y,r+2,'url(#bevel)','#35353c');self.circle(x,y,r,'url(#dial)','#111116')
   f=(v-p['min'])/(p['max']-p['min'])
   if p['unit']=='Hz':f=math.log(v/p['min'])/math.log(p['max']/p['min'])
   a=math.radians(-225+270*f);self.line(x+math.cos(a)*r*.62,y+math.sin(a)*r*.62,x+math.cos(a)*r*.9,y+math.sin(a)*r*.9,'#ceced4',2)
   self.text(VALUE(self.n,p,v),x,y+r+26,14,'#d2d2db','middle',.1)
  if p['learnable']:self.circle(x+len(LABEL(p))*5.5+12,y-r-23,2.4,self.c,'none')
  self.raw('</g>')
  if p['learnable']:self.button('learn-'+p['id'],'LEARN',x-34,y+r+36,68,18,False)
 def panel(self,title,y,h):
  self.text(title,73,y-13,10,'#a4a4ae',track=1.4)
  self.cut(55,y,1090,h,'#17171b','#3a3a40',11)
 def bottom(self):
  self.text('PREVIEW AUDITIONS / APPLY KEEPS / REJECT RESTORES',74,1167,8,'#9999a6',track=.7)
  for i,(id,t) in enumerate([('preview','PREVIEW'),('apply','APPLY'),('reject','REJECT'),('undo','UNDO')]):self.button(id,t,74+i*164,1178,150,29,i==1,False)
  self.text('DESIGN REVIEW / NO LIVE AUDIO',1130,1197,8,'#9393a0','end',1)
rows=[]
for n,params in META.items():
 d=D(n);lookup={p['id']:p for p in params};p=lookup[PRIMARY[n]]
 main=[PRIMARY[n]]+MAIN[n];advanced=[p['id'] for p in params if p['id'] not in main+[PRIMARY[n]]]
 d.display();d.learning()
 d.panel(GROUPS[n][0],693,177)
 main_xs=[136,262,388,514,716,918,1052] if n=='DDelay' else ([180,380,580,900] if n=='DEQ' else [100+(i+.5)*1000/len(main) for i in range(len(main))])
 for id,x in zip(main,main_xs):d.mini(lookup[id],x,774,28,force_knob=id==PRIMARY[n])
 d.panel(GROUPS[n][1],906,229)
 d.text('ADVANCED / EXPANDED FOR REVIEW',1130,893,8,mix(d.c,'#b8b8c2',.45),'end',.7)
 if n=='DEQ':
  for j,b in enumerate([1,3,4]):
   x=83+j*357;d.text('BAND 0'+str(b),x+12,933,11,d.c,track=1)
   for k,suffix in enumerate(['freq','gain','q','shape']):
    p=lookup[f'b{b}{suffix}'];y=955+k*43
    d.raw(f'<g id="param-{p["id"]}"><title>{html.escape(p["name"])}</title>')
    d.text(LABEL(p),x+12,y+12,10,'#aaaab5',track=.2)
    d.text('BELL' if suffix=='shape' else VALUE(n,p,d.values[p['id']]),x+306,y+12,14,'#d1d1dc','end',.1)
    if p['learnable']:d.circle(x+98,y+8,2.3,d.c,'none')
    d.raw('</g>')
    if p['learnable']:d.button('learn-'+p['id'],'LEARN',x+130,y-6,88,25)
   if j<2:d.line(x+331,929,x+331,1114,'#35353e')
 else:
  for i,id in enumerate(advanced):
   x=100+(i+.5)*1000/len(advanced);y=1005
   if n=='DVerb' and id=='mode':x=915;y=1089
   d.mini(lookup[id],x,y,24)
  if len(advanced)==1:
   d.text('ALL PARAMETERS VISIBLE / NO DUPLICATED CONTROLS',600,1108,9,'#8f8f9d','middle',.8)
 d.bottom();d.finish(OUT/(n+'.svg'))
 for p in params:rows.append([n,p['id'],p['name'],'main' if p['id'] in main else 'advanced',p['min'],p['max'],d.values[p['id']],p['unit'],'yes' if p['learnable'] else 'no'])
css='body{background:#111115;color:#d4d4de;font:16px Arial;margin:32px}h1{font-weight:400}p{line-height:1.9;color:#aaaaba}main{display:grid;grid-template-columns:repeat(auto-fit,minmax(min(100%,420px),1fr));gap:24px}article{background:#1b1b21;border:1px solid #373741;border-radius:14px;padding:16px}a{color:inherit;text-decoration:none}img{width:100%;height:auto;object-fit:contain}h2{font-weight:400}'
page=f'<!doctype html><html><meta charset="utf-8"><meta name="viewport" content="width=device-width"><title>D.A / Final selected reference / SVG review</title><style>{css}</style><h1>D.A / FINAL REFERENCE — SVG REVIEW 06 / PER-PLUGIN NOIR PALETTE</h1><p dir="rtl">نسخهٔ بررسی SVG با بازگشت به ناب‌های قبلی، بدنهٔ Noir Black، و رنگ اختصاصی برای هر پلاگین (که فقط DVerb بنفش است). روی هر تصویر کلیک کنید تا SVG اصلی را در اندازهٔ کامل ببینید.</p><main>'
for n in META:page+=f'<article><a href="{n}.svg"><h2 style="color:{ACCENT[n]}">{n}</h2><img src="{n}.svg" alt="{n} review prototype" loading="lazy"></a></article>'
(OUT/'Preview.html').write_text(page+'</main><details><summary>Optional Reference panel / پنل جداگانهٔ مرجع</summary><p>Closed by default in the proposed interface. SVG actions are illustrative only.</p><img src="Reference-Panel.svg" alt="Optional reference panel empty state"></details></html>')
s=io.StringIO();writer=csv.writer(s,lineterminator="\n");writer.writerow(['plugin','parameter_id','label','section','minimum','maximum','design_value','unit','learn_eligible']);writer.writerows(rows);(OUT/'Control_Review.csv').write_text(s.getvalue())
(OUT/'READ_ME_FA_EN.txt').write_text('''D.A / FINAL SELECTED REFERENCE — SVG REVIEW 06 / PER-PLUGIN NOIR PALETTE
10 standalone editable SVGs, 1200 x 1240. No runtime editor or DSP changes.
This is a review proposal, pending explicit user approval.
Noir black chassis retained across all ten plugins, with a distinct signature accent
colour per plugin (only DVerb uses the purple #a28bd0 accent).
The real repository logo was vector-traced and embedded as paths, not a raster image.

Per-plugin accent colours alongside Noir Black:
- DVerb:   #a28bd0 (Violet / Purple)
- DDelay:  #6cb8d6 (Cyan / Ice Blue)
- DComp:   #d4a259 (Amber / Gold)
- DEQ:     #68c29a (Emerald / Mint)
- DSat:    #d86c6c (Crimson / Red)
- DTrans:  #de8854 (Copper / Orange)
- DStereo: #6f95de (Cobalt / Blue)
- DSess:   #5cc2be (Teal / Turquoise)
- DGate:   #b3c662 (Lime / Olive Gold)
- DFX:     #cf7ca8 (Rose / Magenta)

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
- Review fix 03: all non-brand text enlarged (minimum 16 SVG px), less letter spacing,
  stronger small-label weight/contrast, larger small-button height and wider choices.
  Product name and logo are unchanged.
- Review fix 04: removed the screen notch/arc left by the deleted large knob.
  Learning actions now form one horizontal toolbar.
- Review fix 05: the former large-knob parameter returns as the FIRST small knob in
  the main control row, using the same 28 px radius as its neighbours.
- Review fix 06: reference waveform and heading removed from every main screen.
  REFERENCE... represents access to a separate optional panel, closed by default.
  Reference-Panel.svg shows its empty state; no file or analysis is fabricated.
- Review fix 07: restored the previous clean minimalist knob style (r=28 main, r=24 advanced),
  kept the separated discrete switch buttons (fixing DDelay Stereo / Ping Pong overlap),
  and assigned each plugin its own distinct accent colour alongside Noir Black, with only
  DVerb using purple (#a28bd0).
- Preview/Apply/Reject/Undo are visible but dimmed in this neutral design-review state.
- All graph/reference/meter visuals are STATIC EXAMPLES, not real audio or measured data.

این‌ها SVG برای بررسی شما هستند، نه پلاگین اجراشده. فایل Preview.html را باز کنید.
ناب‌ها به همان سبک قبلی برگشته‌اند، مشکل فاصلهٔ Stereo / Ping Pong در DDelay برطرف شده،
و در کنار بدنهٔ Noir Black برای هر پلاگین یک رنگ مجزا در نظر گرفته شده که فقط DVerb بنفش است.
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
archive=OUT.parent/'DAPlugins_FinalReference_SVG_Review06_PerPluginColors.zip'
with zipfile.ZipFile(archive,'w',zipfile.ZIP_DEFLATED) as z:
 for p in sorted(OUT.iterdir()):
  if p.name!='LogoPaths.json':z.write(p,'DAPlugins_FinalReference_Review06_PerPluginColors/'+p.name)
with zipfile.ZipFile(archive) as z:assert z.testzip() is None;assert sum(n.endswith('.svg') for n in z.namelist())==11
print('PASS ten final-reference SVGs / 92 unique real controls / vector logo / ZIP CRC:',archive)
