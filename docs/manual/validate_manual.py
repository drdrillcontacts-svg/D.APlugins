#!/usr/bin/env python3
"""PDF structure/content sanity checks. Visual RTL review remains a separate human check."""
from pathlib import Path
import json,re,hashlib
from pypdf import PdfReader
import pymupdf
HERE=Path(__file__).resolve().parent;ROOT=HERE.parents[1];OUT=HERE/'pdf'
metadata=json.loads((ROOT/'Design/UI/Parameters.json').read_text())
counts=[]
for lang in ['EN','FA']:
 path=OUT/f'DA_Plugins_User_Manual_{lang}.pdf';reader=PdfReader(path);doc=pymupdf.open(path)
 assert not reader.is_encrypted and len(doc)>=40
 assert reader.trailer['/Root']['/Lang']==('fa-IR' if lang=='FA' else 'en-GB')
 alltext=' '.join(' '.join(p.get_text() for p in doc).split())
 assert '\ufffd' not in alltext,'Replacement glyph found'
 assert len(doc.get_toc())>=40,'Missing PDF outline'
 assert sum(len(p.get_links()) for p in doc)>=40,'Missing linked navigation'
 for name,params in metadata.items():
  assert name in alltext,name
  for p in params:assert 'ID: '+p['id'] in alltext,(lang,name,p['id'])
 assert len(re.findall(r'ID: ',alltext))==92,'All parameter reference rows must be present exactly once'
 # No content should be physically outside the sheet, and no accidental near-empty pages.
 for number,page in enumerate(doc,1):
  text=page.get_text();assert len(text)>200,(lang,'near-empty page',number)
  for block in page.get_text('dict')['blocks']:
   if block['type']!=0:continue
   for line in block['lines']:
    for span in line['spans']:
     x0,y0,x1,y1=span['bbox'];assert x0>=-1 and y0>=-1 and x1<=page.rect.width+1 and y1<=page.rect.height+1,(lang,number,span)
 # Verify all fonts used in page resources are embedded, including Unicode fonts.
 fonts=set()
 for page in doc:
  for font in page.get_fonts():fonts.add(font[0])
 for xref in fonts:assert doc.extract_font(xref)[3],('Non-embedded font',lang,xref)
 toc=doc.get_toc();top=[x for x in toc if x[0]==1];assert len(top)==22,(lang,len(top))
 for level,title,page in toc:assert 1<=page<=len(doc)
 for link in (link for page in doc for link in page.get_links()):
  if link['kind']==pymupdf.LINK_GOTO:assert 0<=link['page']<len(doc)
 # Guard the two most error-prone documented distinctions.
 if lang=='EN':
  assert 'first 30 seconds' in alltext and '65,536' in alltext
  assert 'No trained neural network' in alltext
  assert alltext.count('MANUAL ONLY:')==59 and alltext.count('LEARN: audio-informed')==33
  assert 'DETAILS / LOG' in alltext and 'never opens automatically' in alltext
  assert alltext.count('50 factory presets')>=10 and 'SYNC' in alltext and 'FALLBACK' in alltext
  assert 'Mix to 100%' in alltext and 'Q affects Bell only' in alltext
 else:assert 'راهنمای کاربر' in alltext and 'راهنمای' in alltext
 counts.append(len(doc));print('PASS',lang,len(doc),'pages / 92 IDs / 22 chapters / embedded fonts / links / bounds')
combined=pymupdf.open(OUT/'DA_Plugins_User_Manual_EN_FA.pdf')
assert len(combined)==sum(counts)
assert len([e for e in combined.get_toc() if e[0]==1])==2
for line in (OUT/'SHA256SUMS.txt').read_text().splitlines():
 checksum,name=line.split('  ');assert hashlib.sha256((OUT/name).read_bytes()).hexdigest()==checksum
print('PASS bilingual:',len(combined),'pages / both language outlines / SHA-256 checksums')

from zipfile import ZipFile
bundle=OUT/'DA_Plugins_Manuals_EN_FA.zip'
with ZipFile(bundle) as z:
 assert z.testzip() is None
 assert {n for n in z.namelist() if n.endswith('.pdf')}=={p.name for p in OUT.glob('*.pdf')}
 for p in OUT.glob('*.pdf'):assert z.read(p.name)==p.read_bytes()
 assert z.read('SHA256SUMS.txt')==(OUT/'SHA256SUMS.txt').read_bytes()
print('PASS standalone manuals ZIP / all three PDFs byte-identical / checksums')
