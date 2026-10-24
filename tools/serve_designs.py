"""Serve Design only, plus an exact allowlist of manual downloads. Never expose .git or repo root."""
from http.server import ThreadingHTTPServer, SimpleHTTPRequestHandler
from pathlib import Path
from urllib.parse import urlsplit, unquote
import os,shutil
ROOT=Path(__file__).resolve().parents[1]
DOWNLOADS={n:ROOT/'docs/manual/pdf'/n for n in [
 'DA_Plugins_User_Manual_EN.pdf','DA_Plugins_User_Manual_FA.pdf',
 'DA_Plugins_User_Manual_EN_FA.pdf','DA_Plugins_Manuals_EN_FA.zip','SHA256SUMS.txt']}
class Handler(SimpleHTTPRequestHandler):
 def __init__(self,*args,**kwargs):super().__init__(*args,directory=str(ROOT/'Design'),**kwargs)
 def manual(self,head=False):
  path=unquote(urlsplit(self.path).path)
  if not path.startswith('/downloads/'):return False
  name=path[len('/downloads/'):];file=DOWNLOADS.get(name)
  if file is None or not file.is_file():self.send_error(404);return True
  self.send_response(200);self.send_header('Content-Type',self.guess_type(str(file)))
  self.send_header('Content-Length',str(file.stat().st_size))
  self.send_header('Content-Disposition',f'attachment; filename="{name}"')
  self.send_header('X-Content-Type-Options','nosniff');self.end_headers()
  if not head:
   with file.open('rb') as stream:shutil.copyfileobj(stream,self.wfile)
  return True
 def do_HEAD(self):
  if not self.manual(True):super().do_HEAD()
 def do_GET(self):
  if self.manual():return
  if urlsplit(self.path).path=='/':
   self.send_response(302);self.send_header('Location','/UI-Previews/');self.end_headers()
  else:super().do_GET()
if __name__=='__main__':
 ThreadingHTTPServer(('0.0.0.0',int(os.environ.get('PORT','8080'))),Handler).serve_forever()
