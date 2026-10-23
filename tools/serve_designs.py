"""Serve only the Design directory, never the repository or .git/credentials."""
from http.server import ThreadingHTTPServer, SimpleHTTPRequestHandler
from pathlib import Path
import os
root=Path(__file__).resolve().parents[1]/'Design'
class Handler(SimpleHTTPRequestHandler):
 def __init__(self,*args,**kwargs):super().__init__(*args,directory=str(root),**kwargs)
 def do_GET(self):
  if self.path=='/':
   self.send_response(302);self.send_header('Location','/UI-Previews/');self.end_headers()
  else:super().do_GET()
ThreadingHTTPServer(('0.0.0.0',int(os.environ.get('PORT','8080'))),Handler).serve_forever()
