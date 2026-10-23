#!/usr/bin/env python3
"""Build original EN/FA manuals with shaping, embedded fonts, live TOC and bookmarks.
Requires the pinned packages in requirements.txt. No network use during document generation.
"""
from pathlib import Path
import sys,json,math,subprocess,hashlib
from datetime import datetime,timezone
from io import BytesIO
from fontTools.ttLib import TTFont
from fontTools.varLib.instancer import instantiateVariableFont
from PIL import Image
from fpdf import FPDF
from fpdf.enums import XPos,YPos,MethodReturnValue
from pypdf import PdfReader,PdfWriter
sys.path.insert(0,str(Path(__file__).parent))
from content import PRODUCTS,CONTROLS,BEFORE,AFTER,GLOSSARY,learn_class
ROOT=Path(__file__).resolve().parents[2];HERE=Path(__file__).resolve().parent;OUT=HERE/'pdf';WORK=ROOT/'build-manual'
OUT.mkdir(exist_ok=True);WORK.mkdir(exist_ok=True)
META=json.loads((ROOT/'Design/UI/Parameters.json').read_text())
SOURCE=subprocess.check_output(['git','rev-parse','--short=12','HEAD'],cwd=ROOT,text=True).strip()
DATE='2026-10-23';VERSION='0.3';REVISION='1.1 / Rack Collection 02 / Development'
RED=(197,22,53);INK=(25,32,38);GRAY=(90,104,112);PALE=(239,243,245)
for weight,name in [(400,'Regular'),(700,'Bold')]:
 target=WORK/f'Vazirmatn-{name}.ttf'
 if not target.exists():
  font=TTFont(HERE/'fonts/Vazirmatn-Variable.ttf');instantiateVariableFont(font,{'wght':weight},inplace=True).save(target)
for name in PRODUCTS:
 assert set(CONTROLS[name])=={p['id'] for p in META[name]},name
assert sum(map(len,META.values()))==90

class Manual(FPDF):
 def __init__(self,lang):
  super().__init__(format='A4');self.code=lang;self.fa=lang=='FA';self.idx=1 if self.fa else 0
  self.entries=[];self.chapter_number=0;self.running='';self.toc_page=0;self.parameter_pages={};self.body_pages=[]
  self.set_margins(18,25,18);self.set_auto_page_break(True,19)
  self.add_font('Body','',WORK/'Vazirmatn-Regular.ttf');self.add_font('Body','B',WORK/'Vazirmatn-Bold.ttf')
  self.add_font('Display','',ROOT/'Design/Fonts/RajdhaniMedium.ttf');self.add_font('Display','B',ROOT/'Design/Fonts/RajdhaniSemiBold.ttf')
  self.set_title('D.A Plugins — '+('راهنمای کاربر باندل' if self.fa else 'Complete Bundle User Manual'))
  self.set_author('D.A Plugins');self.set_subject('Development version 0.3: ten effects, controls and learning workflow')
  self.set_creator('D.A Plugins bilingual manual builder / fpdf2 + HarfBuzz');self.set_lang('fa-IR' if self.fa else 'en-GB')
  self.set_creation_date(datetime(2026,10,23,12,tzinfo=timezone.utc));self.alias_nb_pages();self.set_display_mode('fullwidth','continuous')
 def choose(self,pair):return pair[self.idx]
 def shaping(self,rtl=None):
  rtl=self.fa if rtl is None else rtl
  self.set_text_shaping(True,direction='rtl' if rtl else 'ltr',language='fa' if rtl else 'en')
 def font(self,size=10.2,bold=False,rtl=None):
  self.set_font('Body','B' if bold else '',size);self.shaping(rtl)
 def header(self):
  if self.page_no()==1:return
  self.set_fill_color(*INK);self.rect(0,0,210,14,'F');self.set_fill_color(*RED);self.rect(0,14,210,.7,'F')
  self.set_text_shaping(False);self.set_font('Display','B',10);self.set_text_color(227,231,234)
  self.set_xy(18,4);self.cell(174,5,'D.A PLUGINS  /  USER MANUAL',align='L')
  self.set_xy(18,4);self.cell(174,5,f'{self.code}  /  DEVELOPMENT 0.3',align='R')
  self.set_y(24);self.set_text_color(*INK)
 def footer(self):
  if self.page_no()==1:return
  self.set_draw_color(211,219,223);self.line(18,281,192,281)
  self.set_text_shaping(False);self.set_font('Body','',7.2);self.set_text_color(*GRAY)
  self.set_xy(18,284);self.cell(140,4,f'D.A Plugins  |  v{VERSION}  |  {DATE}  |  {self.code}')
  self.set_xy(158,284);self.cell(34,4,f'{self.page_no():02d} / {{nb}}',align='R')
 def ensure(self,h):
  if self.get_y()+h>277:self.add_page()
 def text(self,t,size=10.2,bold=False,colour=INK,rtl=None,space=3):
  self.font(size,bold,rtl);self.set_text_color(*colour);self.set_x(18)
  self.multi_cell(174,5.6 if size<=11 else size*.52,t,align='R' if (self.fa if rtl is None else rtl) else 'L',new_x=XPos.LMARGIN,new_y=YPos.NEXT)
  self.ln(space)
 def heading(self,t,level=2):
  self.ensure(20);self.ln(2);self.text(t,size=14 if level==2 else 11.5,bold=True,colour=RED if level==2 else INK,space=2)
 def callout(self,title,body):
  self.font(9.5);h=self.multi_cell(162,5.2,body,dry_run=True,output=MethodReturnValue.HEIGHT,align='R' if self.fa else 'L')+21
  self.ensure(h+3);x,y=18,self.get_y();self.set_fill_color(*PALE);self.rect(x,y,174,h,'F')
  self.set_fill_color(*RED);self.rect(x+172 if self.fa else x,y,2,h,'F')
  self.set_xy(x+6,y+5);self.font(10,True);self.set_text_color(*RED);self.multi_cell(162,5.5,title,align='R' if self.fa else 'L',new_x=XPos.LEFT,new_y=YPos.NEXT)
  self.set_xy(x+6,y+13);self.font(9.5);self.set_text_color(*INK);self.multi_cell(162,5.2,body,align='R' if self.fa else 'L')
  self.set_xy(18,y+h+4)
 def chapter(self,pair):
  self.add_page();self.chapter_number+=1;t=self.choose(pair);self.running=t
  self.start_section(t,level=0);page=self.page_no();link=self.add_link(page=page)
  self.entries.append((self.chapter_number,t,page,link))
  self.font(10,True,False);self.set_text_color(*RED);self.cell(174,7,f'{self.chapter_number:02d}   /   D.A PLUGINS',new_x=XPos.LMARGIN,new_y=YPos.NEXT)
  self.text(t,22,True,space=3)
  self.set_draw_color(*RED);self.line(18,self.get_y(),192,self.get_y());self.ln(5)
 def continuation(self,title):
  self.add_page();self.start_section(title,level=1);self.text(title,18,True,space=4)
 def cover(self):
  self.add_page();self.set_auto_page_break(False);self.set_fill_color(8,11,14);self.rect(0,0,210,297,'F')
  self.set_fill_color(*RED);self.rect(0,0,210,2,'F');self.rect(18,32,32,1.1,'F')
  self.image(ROOT/'Design/Brand/DAmark.png',x=18,y=17,w=24)
  self.set_font('Display','',12);self.set_text_shaping(False);self.set_text_color(169,182,189);self.set_xy(53,18);self.cell(139,9,'LEARN THE SOUND. RECREATE THE FEEL.',align='R')
  self.set_y(43);self.set_font('Display','B',40);self.set_text_color(235,239,242);self.cell(0,18,'D.A PLUGINS',new_x=XPos.LMARGIN,new_y=YPos.NEXT,align='C')
  self.font(28,True);self.set_text_color(235,239,242);self.cell(0,16,'راهنمای کاربر' if self.fa else 'USER MANUAL',new_x=XPos.LMARGIN,new_y=YPos.NEXT,align='C')
  self.font(11);self.set_text_color(165,179,188);self.cell(0,9,'راهنمای کامل باندل ده‌پلاگینی' if self.fa else 'THE COMPLETE TEN-PLUGIN BUNDLE',new_x=XPos.LMARGIN,new_y=YPos.NEXT,align='C')
  self.preview('DVerb',x=15,y=100,w=180)
  self.set_xy(18,233);self.font(9);self.set_text_color(165,179,188)
  self.cell(174,7,'۹۰ پارامتر / ۳۳ کنترل با Learn / گزارش اختیاری' if self.fa else '90 PARAMETERS / 33 LEARN CONTROLS / OPTIONAL DETAILS',align='C')
  self.set_y(250);self.font(8.5,rtl=False);self.set_text_color(166,181,190)
  self.multi_cell(174,5,'DVerb  /  DDelay  /  DComp  /  DEQ  /  DSat\nDTrans  /  DStereo  /  DSess  /  DGate  /  DFX',align='C')
  self.set_y(267);self.font(10,True);self.set_text_color(239,62,93)
  self.cell(0,7,'نسخهٔ توسعه‌ای محصول ۰٫۳ — ویرایش راهنما ۱٫۱' if self.fa else 'DEVELOPMENT VERSION 0.3  /  MANUAL REVISION 1.1',new_x=XPos.LMARGIN,new_y=YPos.NEXT,align='C')
  self.set_text_shaping(False);self.set_font('Body','',8);self.set_text_color(148,163,173);self.cell(0,6,DATE+'  /  '+('PERSIAN' if self.fa else 'ENGLISH'),align='C')
  self.set_auto_page_break(True,19)
 def preview(self,name,x,y,w):
  source=ROOT/f'Design/UI-Previews/{name}.png'
  target=WORK/f"{name}-{hashlib.sha256(source.read_bytes()).hexdigest()[:10]}-manual.jpg"
  if not target.exists():
   im=Image.open(ROOT/f'Design/UI-Previews/{name}.png').convert('RGB');im.thumbnail((1400,1400));im.save(target,quality=90,optimize=True)
  self.image(target,x=x,y=y,w=w)
 def about(self):
  self.add_page();self.text('پیش از استفاده بخوانید' if self.fa else 'Read this before you begin',23,True)
  self.callout(self.choose(('Development edition, not a release certification','نسخهٔ توسعه‌ای؛ نه گواهی انتشار')),
   self.choose(('This manual documents the current source implementation. The complete bundle acceptance criteria are not yet met. Do not interpret the professional artwork or the word “AI” as evidence of a trained model, a certified reference match, a signed installer or validated operation in every DAW.','این راهنما پیاده‌سازی فعلی سورس را شرح می‌دهد. معیار پذیرش کامل باندل هنوز برآورده نشده است. ظاهر حرفه‌ای یا واژهٔ AI را مدرک مدل آموزش‌دیده، تطبیق تأییدشدهٔ مرجع، نصب‌کنندهٔ امضاشده یا عملکرد معتبر در همهٔ DAWها ندانید.')))
  facts=[(('Product version','نسخهٔ محصول'),VERSION), (('Manual revision','ویرایش راهنما'),REVISION), (('Document date','تاریخ سند'),DATE), (('Source snapshot','مبنای سورس'),SOURCE), (('Language','زبان'), 'Persian / فارسی' if self.fa else 'English'), (('Parameter coverage','پوشش پارامتر'), '90 parameters / 10 plugins')]
  for k,v in facts:
   self.text(self.choose(k),9.5,True,space=0);self.text(v,10,rtl=False,space=2)
  self.heading(self.choose(('Illustrations and example settings','تصاویر و تنظیمات نمونه')))
  self.text(self.choose(('Hardware images are supplied design studies, not screenshots of the running plugin. Their example traces and meters are illustrative. All numerical recipes are suggested starting points, not included presets, measurements, guarantees or settings that suit every source.','تصاویر سخت‌افزاری، طرح گرافیکی‌اند نه اسکرین‌شات پلاگین اجراشده. نمودار و متر نمونه در آن‌ها نمایشی است. همهٔ دستورهای عددی، نقطهٔ شروع پیشنهادی‌اند؛ پریست همراه، اندازه‌گیری، تضمین یا تنظیم مناسب همهٔ منابع نیستند.')))
  self.text(self.choose(('In the combined PDF, English comes first and Persian second; printed page numbers restart for each edition.','در PDF ترکیبی ابتدا انگلیسی و سپس فارسی آمده و شمارهٔ چاپی صفحات در هر نسخه از نو آغاز می‌شود.')),9.2,colour=GRAY)
  self.text(self.choose(('English interface names are retained in the Persian edition so you can find the exact controls. PDF navigation includes a linked contents page and bookmarks. Parameter ranges/defaults are generated from the source registry rather than inferred from artwork.','نام انگلیسی رابط در نسخهٔ فارسی حفظ شده تا کنترل دقیق را پیدا کنید. PDF فهرست پیونددار و نشانک دارد. دامنه و پیش‌فرض پارامترها از رجیستری سورس تولید شده‌اند، نه از روی تصویر.')))
 def reserve_toc(self):
  self.add_page();self.toc_page=self.page_no()
 def fill_toc(self):
  last=self.page;self.page=self.toc_page;self.set_auto_page_break(False);self.set_xy(18,27)
  self.text('فهرست مطالب' if self.fa else 'Contents',25,True,space=6)
  self.text(self.choose(('Select a chapter or use the PDF bookmarks. Page references refer to this language edition.','یک فصل را انتخاب یا از نشانک PDF استفاده کنید. شماره‌ها به همین نسخهٔ زبانی ارجاع می‌دهند.')),9.5,colour=GRAY,space=6)
  for number,title,page,link in self.entries:
   y=self.get_y();self.set_fill_color(*(PALE if number%2 else (255,255,255)));self.rect(18,y,174,8.6,'F')
   self.font(10);self.set_text_color(*INK)
   self.set_xy(22,y+1.2);self.cell(164,6,f'{number:02d}   {title}',align='R' if self.fa else 'L',link=link)
   self.font(10,True,False);self.set_xy(22,y+1.2);self.cell(164,6,str(page),align='L' if self.fa else 'R',link=link)
   self.set_xy(18,y+8.6)
  self.ln(7);self.text(self.choose(('Need a particular control? Search its English name or parameter ID in the PDF.','برای کنترل خاص، نام انگلیسی یا شناسهٔ آن را در PDF جست‌وجو کنید.')),9,colour=GRAY)
  self.page=last;self.set_auto_page_break(True,19)
 def sections(self,sections):
  for heading,paragraphs in sections:
   self.heading(self.choose(heading))
   for paragraph in paragraphs:self.text(self.choose(paragraph))
 def install_paths(self):
  self.continuation(self.choose(('Installation paths & build notes','مسیرهای نصب و نکات بیلد')))
  self.text(self.choose(('These are common platform locations, not a promise that every host scans them automatically. Keep each .vst3 or .component bundle intact and follow the host’s scan instructions. Administrative permission may be required for shared system folders.','این‌ها مسیرهای رایج‌اند و اسکن خودکار همهٔ میزبان‌ها را تضمین نمی‌کنند. ساختار هر بستهٔ .vst3 یا .component را حفظ و دستور اسکن میزبان را دنبال کنید. پوشهٔ مشترک سیستم ممکن است دسترسی مدیر بخواهد.')))
  paths=[('Windows / VST3',r'C:\Program Files\Common Files\VST3'),('macOS / VST3','/Library/Audio/Plug-Ins/VST3\n~/Library/Audio/Plug-Ins/VST3'),('macOS / Audio Unit (AU)','/Library/Audio/Plug-Ins/Components\n~/Library/Audio/Plug-Ins/Components'),('Linux / VST3','~/.vst3\n/usr/lib/vst3  or  /usr/local/lib/vst3 (host dependent)')]
  for title,path in paths:
   self.heading(title,3);self.text(path,9.4,rtl=False,colour=GRAY,space=4)
  self.heading(self.choose(('If you are building from source','اگر از سورس بیلد می‌کنید')))
  self.text(self.choose(('Advanced users need CMake 3.22+, a C++17 compiler and platform JUCE dependencies. JUCE 8.0.6 is fetched from a pinned, verified archive. Full setup and dependency lists are in README.md. Building does not install, sign, notarize or activate the product.','کاربر پیشرفته به CMake نسخهٔ ۳٫۲۲ به بالا، کامپایلر C++17 و وابستگی‌های پلتفرم JUCE نیاز دارد. JUCE نسخهٔ ۸٫۰٫۶ از آرشیو ثابت و بررسی‌شده دریافت می‌شود. جزئیات در README.md است. بیلد، نصب، امضا، نوتاریز یا فعال‌سازی انجام نمی‌دهد.')))
  self.text('cmake -S . -B build -DCMAKE_BUILD_TYPE=Release\ncmake --build build --config Release --parallel 2\nctest --test-dir build -C Release --output-on-failure',8.8,rtl=False)
  self.text(self.choose(('Build products: build/<Plugin>_artefacts/Release/. Use only formats/platforms whose build and host checks have actually passed for your revision.','محصول بیلد در مسیر زیر است. فقط فرمت و پلتفرمی را استفاده کنید که بیلد و بررسی میزبانِ همان ویرایش واقعاً پاس شده باشد.')),9.4)
  self.text('build/<Plugin>_artefacts/Release/',9.2,rtl=False)
 def product_open(self,name):
  product=PRODUCTS[name];self.chapter((name+' — '+product['title'][0],name+' — '+product['title'][1]))
  self.text(self.choose(product['intro']),10.2,space=4)
  self.text(product['signal'],8.5,rtl=False,colour=GRAY,space=4)
  self.ensure(154);y=self.get_y();w=100;x=(210-w)/2
  for file,titles in [(name,('Main controls — design study, not a native screenshot.','کنترل‌های اصلی — طرح گرافیکی، نه اسکرین‌شات اجرای بومی.')),(name+'-Advanced',('Advanced controls — design study, not a native screenshot.','کنترل‌های تکمیلی — طرح گرافیکی، نه اسکرین‌شات اجرای بومی.'))]:
   self.preview(file,x,y,w);self.set_xy(18,y+w*820/1200+1)
   self.text(self.choose(titles),8,colour=GRAY,space=2);y=self.get_y()
  self.heading(self.choose(('Typical uses','کاربردهای معمول')),3)
  self.text(' / '.join(self.choose(t) for t in product['uses']),9.5,space=2)
 def parameter_reference(self,name):
  self.continuation(self.choose((name+' / Parameter reference',name+' / مرجع پارامترها')))
  self.parameter_pages[name]=self.page_no()
  self.text(self.choose(('Ranges are manual limits; learning may propose a narrower interval. Defaults are source factory values, not the example image values. The learning badge identifies a strategy type, not guaranteed estimation accuracy.','دامنه‌ها حد دستی‌اند و پیشنهاد یادگیری ممکن است محدودتر باشد. پیش‌فرض از سورس است، نه عدد تصویر نمونه. برچسب یادگیری نوع راهبرد را نشان می‌دهد، نه دقت تضمینی.')),9.4,colour=GRAY,space=4)
  chunk={'DVerb':6,'DEQ':8,'DFX':7}.get(name,8)
  for i,p in enumerate(META[name]):
   if i and i%chunk==0:self.continuation(self.choose((name+' / Controls continued',name+' / ادامهٔ کنترل‌ها')))
   self.parameter(name,p)
 def parameter(self,name,p):
  label,explain_en,explain_fa=CONTROLS[name][p['id']];body=explain_fa if self.fa else explain_en
  self.font(9.2);h=self.multi_cell(166,4.9,body,dry_run=True,output=MethodReturnValue.HEIGHT,align='R' if self.fa else 'L')
  total=h+15.5;self.ensure(total+2);x,y=18,self.get_y();self.set_fill_color(*PALE);self.rect(x,y,174,6,'F')
  title=(label+'  /  '+p['name']) if self.fa else p['name']
  if name=='DEQ':title=(label+'  /  Band '+p['id'][1]+' '+p['name']) if self.fa else 'Band '+p['id'][1]+' / '+p['name']
  self.font(9.6,True);self.set_text_color(*INK);self.set_xy(21,y+.4);self.cell(168,5.2,title,align='R' if self.fa else 'L')
  self.set_xy(20,y+6.5);self.font(7.8,False,False);self.set_text_color(*GRAY)
  info=f"Range: {format_range(name,p)}   |   Default: {format_value(name,p,p['initial'])}   |   ID: {p['id']}"
  self.multi_cell(170,4.4,info,new_x=XPos.LMARGIN,new_y=YPos.NEXT)
  self.set_xy(21,y+11.5);self.font(9.2);self.set_text_color(*INK)
  self.multi_cell(166,4.9,body,align='R' if self.fa else 'L',new_x=XPos.LMARGIN,new_y=YPos.NEXT)
  klass=learn_class(name,p);badge={'audio':('LEARN: audio-informed estimate','یادگیری: برآورد با کمک ویژگی صوتی'),'manual':('MANUAL ONLY: preserved by every Learn scope','فقط دستی: در تمام دامنه‌های Learn حفظ می‌شود')}[klass]
  self.font(7.3);self.set_text_color(*(RED if klass=='audio' else GRAY));self.set_xy(21,self.get_y()+.5);self.cell(166,3.5,self.choose(badge),align='R' if self.fa else 'L')
  self.set_xy(18,y+total+2)
 def product_practice(self,name):
  p=PRODUCTS[name];self.continuation(self.choose((name+' / In practice',name+' / استفادهٔ عملی')))
  self.heading(self.choose(('Starting recipe — adjust by ear','دستور شروع — با گوش اصلاح کنید')))
  for i,pair in enumerate(p['recipe']):self.text(f'{i+1}. '+self.choose(pair),10,space=3)
  self.heading(self.choose(('How learning behaves here','رفتار یادگیری در این پلاگین')))
  self.text(self.choose(p['learning']))
  self.callout(self.choose(('Important implementation limit','محدودیت مهم پیاده‌سازی')),self.choose(p['caution']))
  self.heading(self.choose(('Listening and workflow tip','نکتهٔ شنیداری و روند کار')),3);self.text(self.choose(p['tips']),10)
 def glossary(self):
  self.chapter(('Glossary & parameter locator','واژه‌نامه و راهنمای یافتن پارامتر'))
  for en,fa,de,df in GLOSSARY:
   self.ensure(20);self.heading((fa+' / '+en) if self.fa else en,3);self.text(df if self.fa else de,9.8,space=2)
  self.continuation(self.choose(('Find a parameter quickly','یافتن سریع پارامتر')))
  self.text(self.choose(('All 90 parameter IDs are documented in the control pages. The links below open each product’s reference. Searchable IDs also identify host automation parameters; the displayed host name can include its module prefix.','هر ۹۰ شناسه در صفحات کنترل توضیح داده شده‌اند. پیوندهای زیر مرجع هر محصول را باز می‌کنند. شناسهٔ قابل‌جست‌وجو پارامتر اتومیشن را هم معرفی می‌کند؛ نام میزبان ممکن است پیشوند ماژول داشته باشد.')))
  for name in PRODUCTS:
   self.ensure(23);page=self.parameter_pages[name];link=self.add_link(page=page)
   self.font(10,True,False);self.set_text_color(*RED);self.set_x(18)
   self.cell(174,6,f'{name}  /  {len(META[name])} parameters  /  Page {page}',link=link,new_x=XPos.LMARGIN,new_y=YPos.NEXT)
   self.text('  ·  '.join(p['id'] for p in META[name]),8.2,rtl=False,space=3)
 def project_links(self):
  self.heading(self.choose(('Project resources','منابع پروژه')),3)
  resources=[('Repository','https://github.com/drdrillcontacts-svg/D.APlugins'),('Issue tracker','https://github.com/drdrillcontacts-svg/D.APlugins/issues'),('Source snapshot','https://github.com/drdrillcontacts-svg/D.APlugins/tree/'+SOURCE)]
  for name,url in resources:
   self.font(8.3,False,False);self.set_text_color(*RED);self.set_x(18);self.multi_cell(174,5,name+' / '+url,link=url,new_x=XPos.LMARGIN,new_y=YPos.NEXT)
  self.ln(5);self.text(self.choose(('Document maintenance: update the parameter registry, the behavioral notes and both language editions together when a feature changes. This edition intentionally distinguishes implemented controls, heuristic policies, planned work and verification limits.','نگه‌داری سند: هنگام تغییر قابلیت، رجیستری پارامتر، توضیح رفتار و هر دو زبان را هم‌زمان به‌روز کنید. این ویرایش عمداً کنترل پیاده‌شده، سیاست ابتکاری، کار آینده و محدودیت اعتبارسنجی را جدا می‌کند.')),9)

def format_value(name,p,v):
 id=p['id'];opts=[]
 if p['discrete']:
  if name=='DVerb':opts=['Room','Hall','Plate','Shimmer']
  elif name=='DDelay':opts=['Stereo','Ping-pong']
  elif name=='DSat':opts=['Soft','Hard','Asymmetric']
  elif name=='DStereo':opts=['Stereo','Mono']
  elif name=='DEQ':opts=['Bell','Low Shelf','High Shelf']
  else:opts=['Off','On']
  return opts[int(v)]
 percent=p['unit']=='%' or id in {'mix','width','density','diffusion','modulation','sensitivity','depth','focus','delayMix','reverbMix'}
 if percent:return f'{v*100:g}%'
 if id=='feedback':return f'{v:g} ({v*100:g}%)'
 unit=p['unit'];s=f'{v:g}'
 return s+(':1' if unit==':1' else (' '+unit if unit else ''))
def format_range(name,p):
 if p['discrete']:
  if name=='DVerb':return 'Room / Hall / Plate / Shimmer'
  if name=='DDelay':return 'Stereo / Ping-pong'
  if name=='DSat':return 'Soft / Hard / Asymmetric'
  if name=='DStereo':return 'Stereo / Mono'
  if name=='DEQ':return 'Bell / Low Shelf / High Shelf'
  return 'Off / On'
 return format_value(name,p,p['min'])+' to '+format_value(name,p,p['max'])

def build(lang):
 pdf=Manual(lang);pdf.cover();pdf.about();pdf.reserve_toc()
 for title,sections in BEFORE:
  pdf.chapter(title);pdf.sections(sections)
  if title==BEFORE[1][0]:pdf.install_paths()
  if title==BEFORE[0][0]:
   pdf.heading(pdf.choose(('Choose your tool','انتخاب ابزار')))
   for name,p in PRODUCTS.items():pdf.text(name+' — '+pdf.choose(p['title']),9.5,space=1)
 for name in PRODUCTS:pdf.product_open(name);pdf.parameter_reference(name);pdf.product_practice(name)
 for title,sections in AFTER[:-1]:
  pdf.chapter(title)
  if title==AFTER[1][0]:
   pdf.sections(sections[:3]);pdf.continuation(pdf.choose(('Troubleshooting / continued','ادامهٔ عیب‌یابی')));pdf.sections(sections[3:])
  else:pdf.sections(sections)
 pdf.glossary();pdf.chapter(AFTER[-1][0]);pdf.sections(AFTER[-1][1]);pdf.project_links()
 pdf.fill_toc();path=OUT/f'DA_Plugins_User_Manual_{lang}.pdf';pdf.output(path)
 manifest={'language':lang,'pages':len(pdf.pages),'source':SOURCE,'chapters':[{'number':n,'title':t,'page':p} for n,t,p,l in pdf.entries],'parameter_reference_pages':pdf.parameter_pages,'parameters':90}
 (WORK/f'manifest-{lang}.json').write_text(json.dumps(manifest,ensure_ascii=False,indent=2))
 print(lang,len(pdf.pages),'pages',path.stat().st_size,'bytes');return path

def export_text(lang):
 index=1 if lang=='FA' else 0;lines=['# D.A Plugins — '+('راهنمای کاربر' if lang=='FA' else 'User Manual'),'','Development version 0.3 / 2026-10-23 / Source '+SOURCE,'']
 for title,sections in BEFORE:
  lines+=['## '+title[index],'']
  for heading,paragraphs in sections:lines+=['### '+heading[index],'']+[p[index]+'\n' for p in paragraphs]
 for name,p in PRODUCTS.items():
  lines+=['## '+name+' — '+p['title'][index],'',p['intro'][index],'',p['signal'],'','### '+('مرجع کنترل‌ها' if index else 'Control reference'),'']
  for m in META[name]:
   control=CONTROLS[name][m['id']];lines+=['#### '+m['name']+' / `'+m['id']+'`','',f"Range: {format_range(name,m)} | Default: {format_value(name,m,m['initial'])}",'',control[2 if index else 1],'',('Learn: موجود' if m['learnable'] else 'فقط دستی؛ بدون Learn') if index else ('Learn: audio-informed' if m['learnable'] else 'Manual only; no Learn'),'']
  lines+=['### '+('استفادهٔ عملی' if index else 'In practice'),'']+[f'{i+1}. '+t[index] for i,t in enumerate(p['recipe'])]+['',p['learning'][index],'',p['caution'][index],'',p['tips'][index],'']
 for title,sections in AFTER[:-1]:
  lines+=['## '+title[index],'']
  for heading,paragraphs in sections:lines+=['### '+heading[index],'']+[p[index]+'\n' for p in paragraphs]
 lines+=['## '+('واژه‌نامه' if index else 'Glossary'),'']+[f'**{g[1] if index else g[0]}** — {g[3] if index else g[2]}\n' for g in GLOSSARY]
 for title,sections in AFTER[-1:]:
  lines+=['## '+title[index],'']
  for heading,paragraphs in sections:lines+=['### '+heading[index],'']+[p[index]+'\n' for p in paragraphs]
 (HERE/f'Manual_{lang}.md').write_text('\n'.join(lines),encoding='utf-8')

if __name__=='__main__':
 files=[]
 for lang in ['EN','FA']:files.append(build(lang));export_text(lang)
 writer=PdfWriter()
 for lang,file in zip(['ENGLISH / EN','فارسی / FA'],files):writer.append(str(file),outline_item=lang,import_outline=True)
 writer.page_mode='/UseOutlines'
 writer.add_metadata({'/Title':'D.A Plugins — English & Persian User Manual','/Author':'D.A Plugins','/Subject':'Development version 0.3 / bilingual manual / 90 controls'})
 writer.compress_identical_objects(remove_duplicates=True,remove_unreferenced=True)
 combined=OUT/'DA_Plugins_User_Manual_EN_FA.pdf'
 with combined.open('wb') as f:writer.write(f)
 print('Bilingual',len(writer.pages),'pages',combined.stat().st_size,'bytes')
 checksums={p.name:hashlib.sha256(p.read_bytes()).hexdigest() for p in OUT.glob('*.pdf')}
 (OUT/'SHA256SUMS.txt').write_text('\n'.join(f'{v}  {k}' for k,v in sorted(checksums.items()))+'\n')
