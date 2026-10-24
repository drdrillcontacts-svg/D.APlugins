#!/usr/bin/env python3
"""User-selected portrait reference: neutral matte, red actions, numeric focus, no large knob."""
from pathlib import Path
import math,json,re,html,zipfile,xml.etree.ElementTree as ET
ROOT=Path(__file__).resolve().parents[1]
ns={'__file__':str(ROOT/'tools/design_noir_concepts.py')}
exec((ROOT/'tools/design_noir_concepts.py').read_text().split('for name,params in META.items():')[0],ns)
Drawing=ns['Drawing'];META=ns['META'];PRIMARY=ns['PRIMARY'];VALUES=ns['OVERRIDE'];LABEL=ns['label'];VALUE=ns['value'];GRAPH=ns['graph']
OUT=ROOT/'Design/Concepts/Noir-Reference-Red';OUT.mkdir(parents=True,exist_ok=True)
RED='#df4b4f'
# Remove violet/amber from the source primitives: highlights neutral, action accents red.
ACTIONS={'#d19a50','#bb8a4d','#d39c55','#b38a52','#ce9d62','#9983ce','#79747e'}
WHITE_GRAPHS={'#9a82e5','#9d88eb','#796794','#b9a3f1','#b29aee','#b19aed','#9e83d8','#aa8de6','#c0aecf'}
def neutralize(svg):
 def color(m):
  c=m.group().lower()
  if c==RED:return RED
  if c in ACTIONS:return RED
  if c in WHITE_GRAPHS:return '#c5c5c5'
  rgb=[int(c[i:i+2],16) for i in (1,3,5)];g=round(sum(rgb)/3)
  return '#'+f'{g:02x}'*3+c[7:]
 return re.sub(r'#[0-9a-fA-F]{8}\b|#[0-9a-fA-F]{6}\b',color,svg)
def small_knob(d,p,v,x,y):
 r=31;d.group('param-'+p['id']);d.raw('<title>'+html.escape(p['name'])+'</title>')
 d.text(LABEL(p),x,y-r-22,11,'#aaaaaa','middle',1.1)
 d.circle(x,y+4,r+5,'#070707','#111111');d.circle(x,y,r+3,'url(#bevel)','#3a3a3a');d.circle(x,y,r,'url(#dial)','#404040');d.circle(x,y,r-3,'none','#ffffff06')
 f=(v-p['min'])/(p['max']-p['min'])
 if p['unit']=='Hz':f=math.log(v/p['min'])/math.log(p['max']/p['min'])
 a=math.radians(-225+270*f);d.line(x+math.cos(a)*r*.63,y+math.sin(a)*r*.63,x+math.cos(a)*r*.91,y+math.sin(a)*r*.91,'#d2d2d2',2.5)
 d.text(VALUE(d.name,p,v),x,y+r+27,16,'#d2d2d2','middle',.1)
 if p['learnable']:d.circle(x+len(LABEL(p))*3.3+11,y-r-26,2.4,RED)
 d.end()
for name,params in META.items():
 d=Drawing(name);vals={p['id']:VALUES.get(name,{}).get(p['id'],p['initial']) for p in params}
 d.group('preset-header');d.text('D.A',78,100,27,'#d5d5d5',spacing=-1,weight=600)
 d.rect(295,65,381,46,'#101010','#363636',7);d.text('Factory presets',313,94,14,'#cacaca',spacing=.4)
 d.line(580,65,580,111);d.line(628,65,628,111)
 d.path('M607 81 l-7 7 7 7','#bcbcbc',1.8);d.path('M649 81 l7 7 -7 7','#bcbcbc',1.8)
 d.rect(691,65,47,46,'#111111','#363636',7);d.path('M707 79 h14 v19 h-14 z m4 5 h6 m-6 5 h6','#a6a6a6',1.3)
 for y in [80,88,96]:d.line(890,y,912,y,'#b9b9b9',1.8)
 d.end()
 d.rect(57,137,886,379,'url(#bevel)','#292929',27)
 d.rect(70,150,860,351,'#0d0d0d','#232323',18)
 d.text(name.upper(),91,236,28,'#d5d5d5',spacing=3,weight=400);d.text('RESET',887,237,10,'#939393','end',2)
 GRAPH(d,name)
 # The focused parameter is a large, readable numeric field; NEVER a giant centre knob.
 p=next(p for p in params if p['id']==PRIMARY[name]);d.group('param-'+p['id'])
 d.text(LABEL(p),91,596,19,'#d1d1d1',spacing=3)
 d.text(VALUE(name,p,vals[p['id']]),90,650,43,'#eeeeee',spacing=.1)
 d.text('SELECTED / BAND 02' if name=='DEQ' else 'SYNC / 120 BPM / 375 ms' if name=='DDelay' else 'PRECISION CONTROL',91,682,10,'#999999',spacing=2)
 if p['learnable']:d.text('LEARN',887,622,12,RED,'end',2)
 d.end()
 if name=='DEQ':
  for b in range(1,5):
   x=84+(b-1)*214;d.rect(x,770,190,267,'#151515','#5a5a5a' if b==2 else '#303030',10)
   d.text('BAND '+str(b),x+16,800,12,'#cccccc' if b==2 else '#a0a0a0',spacing=1.5)
   for j,(suffix,cap) in enumerate([('freq','FREQ'),('gain','GAIN'),('q','Q'),('shape','SHAPE')]):
    p=next(p for p in params if p['id']==f'b{b}{suffix}');d.group(('mirror-' if p['id']==PRIMARY[name] else 'param-')+p['id'])
    d.text(cap,x+16,841+j*51,10,'#a2a2a2',spacing=.5)
    d.text('BELL' if suffix=='shape' else VALUE(name,p,vals[p['id']]),x+174,862+j*51,14,'#d0d0d0','end',.1)
    if p['learnable']:d.circle(x+60,837+j*51,2.4,RED)
    d.end()
 else:
  rest=[p for p in params if p['id']!=PRIMARY[name]];rows=2 if len(rest)>6 else 1;cols=math.ceil(len(rest)/rows)
  for i,p in enumerate(rest):
   row=i//cols;col=i%cols;n=min(cols,len(rest)-row*cols);x=90+(col+.5)*820/n;y=805+row*165 if rows==2 else 887
   if p['discrete']:
    d.group('param-'+p['id']);d.text(LABEL(p),x,y-53,11,'#aaaaaa','middle',1.1)
    d.rect(x-58,y-26,116,43,'#141414','#3a3a3a',7);d.text(VALUE(name,p,vals[p['id']]),x,y+1,13,'#c9c9c9','middle',.2);d.end()
   else:
    if name=='DDelay' and p['id'] in ['time','modulation']:d.raw('<g opacity=".45">')
    small_knob(d,p,vals[p['id']],x,y)
    if name=='DDelay' and p['id'] in ['time','modulation']:d.end()
 d.line(76,1098,924,1098,'#333333')
 for x,t,c in [(165,'CAPTURE INPUT','#b3b3b3'),(382,'REFERENCE','#b3b3b3'),(602,'LEARN ALL',RED),(817,'DETAILS / LOG','#b3b3b3')]:d.text(t,x,1150,11,c,'middle',1.2)
 d.text('D.A PLUGINS',76,1212,9,'#969696',spacing=1.6);d.text('DESIGN STUDY / NO LIVE AUDIO',924,1212,8,'#929292','end',1)
 d.raw('</svg>');text=neutralize('\n'.join(d.parts)).replace('Noir 01 concept','Selected Reference / Red').replace('restrained violet and amber','neutral highlights and red accents');r=ET.fromstring(text)
 ids=[a.get('id') for a in r.iter() if a.get('id')]
 assert len(ids)==len(set(ids)),name
 assert {i[6:] for i in ids if i.startswith('param-')}=={p['id'] for p in params},name
 for c in re.findall(r'#[0-9a-f]{6}\b',text):assert c==RED or c[1:3]==c[3:5]==c[5:7],(name,c)
 assert 'r="155"' not in text and 'giant' not in text
 (OUT/(name+'.svg')).write_text(text)
css='body{margin:32px;background:#101010;color:#d2d2d2;font:16px Arial}h1{font-weight:400}p{color:#aaa;line-height:1.8}main{display:grid;grid-template-columns:repeat(auto-fit,minmax(min(100%,390px),1fr));gap:24px}article{background:#181818;border:1px solid #333;border-radius:16px;padding:15px}a{color:inherit;text-decoration:none}img{width:100%;height:650px;object-fit:contain}h2{font-weight:400}strong{color:#df4b4f}'
page=f'<!doctype html><html><meta charset="utf-8"><meta name="viewport" content="width=device-width"><title>D.A / Selected Reference / Red</title><style>{css}</style><h1>D.A / SELECTED REFERENCE — RED</h1><p dir="rtl">هر ده پلاگین با استایل تصویر انتخاب‌شده: مشکی مات، ناب‌های کوچک، مقدار اصلی عددی و <strong>تأکید قرمز به‌جای زرد</strong>. برای بازکردن SVG اصلی روی طرح کلیک کنید. این‌ها کانسپت هستند، نه رابط اجرایی.</p><main>'
for n in META:page+=f'<article><a href="{n}.svg"><h2>{n}</h2><img src="{n}.svg" alt="{n}" loading="lazy"></a></article>'
(OUT/'Preview.html').write_text(page+'</main></html>')
(OUT/'READ_ME.txt').write_text('''D.A / USER-SELECTED REFERENCE / RED EDITION
Ten standalone editable SVG concepts; 1000 x 1280 each.
The supplied DVerb reference guides material, spacing, numeric focus and small knob styling.
All former yellow/amber action accents are RED (#df4b4f). No violet or yellow remains.
Other colours are strictly neutral grayscale. The primary parameter is a numeric readout,
not a giant centre knob. Each product retains its actual controls and purpose-specific visual.
DEQ has four complete band cards. DFX depicts its fixed four-stage signal path.
All 92 parameter IDs are represented. Only eligible controls carry Learn marks/actions.
Logs remain explicit and closed. DDelay example uses Sync / dotted eighth / 120 BPM;
free time and modulation are dimmed. Graphs are illustrative, not measured or live.
Main values are design examples, not a claim of a matching factory preset.
No runtime UI, DSP, automation or program-state changes. Awaiting user approval.
Open Preview.html after extracting the complete ZIP; click any SVG to view or edit it.

همان استایل تصویر انتخاب‌شده برای ده پلاگین، با تأکیدهای قرمز به‌جای زرد.
هیچ ناب بزرگ مرکزی وجود ندارد. فایل‌ها مستقل و وکتور قابل ویرایش‌اند.
فعلاً فقط طرح برای تأیید شما؛ پس از تأیید، پیاده‌سازی و تست رابط واقعی انجام می‌شود.
''')
archive=OUT.parent/'DAPlugins_SelectedReference_Red_10_SVG.zip'
with zipfile.ZipFile(archive,'w',zipfile.ZIP_DEFLATED) as z:
 for p in sorted(OUT.iterdir()):z.write(p,'DAPlugins_SelectedReference_Red/'+p.name)
with zipfile.ZipFile(archive) as z:assert z.testzip() is None;assert sum(n.endswith('.svg') for n in z.namelist())==10
print('PASS 10 SVGs / 92 controls / grayscale + red only / no giant knobs / ZIP CRC:',archive)
