#!/usr/bin/env python3
"""Package buildable source and original before/after audition; not a signed binary release.
First compile/run tools/render_reverb_demo.cpp against 63e91b9 and current Suite.cpp to
build-audition/{old,new}.f32. Raw outputs are stereo little-endian float32, 48 kHz.
"""
from pathlib import Path
from array import array
import math, json, wave, struct, hashlib, subprocess
from zipfile import ZipFile, ZIP_DEFLATED
ROOT=Path(__file__).resolve().parents[1]
OUT=ROOT/'docs/downloads';OUT.mkdir(exist_ok=True)
WORK=ROOT/'build-audition';AUDIO=WORK/'Audio';AUDIO.mkdir(exist_ok=True)
def load(path):
 a=array('f');a.frombytes(path.read_bytes());assert len(a)==48000*8*2
 assert all(math.isfinite(x) for x in a)
 return a
old=load(WORK/'old.f32');new=load(WORK/'new.f32');dry=load(WORK/'new.f32.dry')
assert (WORK/'old.f32.dry').read_bytes()==(WORK/'new.f32.dry').read_bytes()
stats={}
def save(name,data,match):
 rms=math.sqrt(sum(x*x for x in data)/len(data));peak=max(map(abs,data))
 gain=min(10**(-27/20)/max(rms,1e-12),.94/max(peak,1e-12)) if match else 1
 with wave.open(str(AUDIO/name),'wb') as f:
  f.setnchannels(2);f.setsampwidth(3);f.setframerate(48000)
  pcm=bytearray()
  for x in data:
   value=max(-8388608,min(8388607,round(x*gain*8388607)))
   pcm.extend(value.to_bytes(3,'little',signed=True))
  f.writeframes(pcm)
 stats[name]={'raw_rms_dbfs':20*math.log10(max(rms,1e-12)),
              'gain_db':20*math.log10(gain),'matched_rms_dbfs':20*math.log10(max(rms*gain,1e-12)),
              'peak_dbfs':20*math.log10(max(peak*gain,1e-12))}
save('00_Dry_original_synthetic.wav',dry,False)
save('01_DVerb_before_63e91b9_wet.wav',old,True)
save('02_DVerb_after_0.4_wet.wav',new,True)
(AUDIO/'Level_matching.json').write_text(json.dumps(stats,indent=2)+'\n')
source=subprocess.check_output(['git','rev-parse','--short=12','HEAD'],cwd=ROOT,text=True).strip()
notes=f'''D.A Plugins 0.4 / DEVELOPMENT SOURCE + DVERB LISTENING AID
Audio implementation source checkpoint: {source}

This is NOT a plugin installer or a signed/certified commercial release.
All ten products, 50 factory programs each, DDelay sync and revised DVerb/DFX
are included as buildable source. Compiled binaries depend on platform CI;
see https://github.com/drdrillcontacts-svg/D.APlugins/actions

Build the ten VST3/Standalone targets (macOS also AU):
 cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
 cmake --build build --config Release --parallel 2
Requires CMake 3.22+, C++17 compiler, network for pinned JUCE 8.0.6,
platform JUCE development dependencies and applicable JUCE licensing.
Linux needs pkg-config, ALSA/JACK, freetype/fontconfig and X11/GL dev packages.
Portable tests without JUCE:
 cmake -S . -B build-core -DDA_BUILD_PLUGINS=OFF -DCMAKE_BUILD_TYPE=Release
 cmake --build build-core --config Release --parallel 2
 ctest --test-dir build-core -C Release --output-on-failure
The full repository includes the CI dependency list and additional documentation.

Preset catalogs: Design/Presets/FactoryPresets.json (500 complete snapshots).
Manuals are a SEPARATE ZIP:
 https://github.com/drdrillcontacts-svg/D.APlugins/blob/arena/0bdc5081-d-aplugins/docs/manual/pdf/DA_Plugins_Manuals_EN_FA.zip?raw=true

Audio: original synthetic plucked tones/noise percussion, 8 s, 48 kHz stereo,
24-bit PCM. No third-party recording. 01 and 02 are WET ONLY, identical input
and explicitly identical parameters, before=63e91b9, after=current implementation.
Both are independently RMS-matched over the full eight seconds to -27 dBFS,
with a -0.54 dBFS peak ceiling; Level_matching.json reports any ceiling limitation.
Listen on modest monitoring, then audition real sources in a host. The example
is not a subjective sound-quality approval or measurement of calibrated RT60.
Reproduce the raw data with tools/render_reverb_demo.cpp against each Suite.cpp.
The reverb algorithm changes old-session sound; keep your previous binary.
No trained neural model, activation, signed installer or commercial clearance
is included. LICENSE-NOTICE.txt and font/JUCE obligations apply.
'''
package=OUT/'DAPlugins_0.4_Development.zip'
with ZipFile(package,'w',ZIP_DEFLATED) as z:
 for directory in ['Source','Tests','Design/Brand','Design/Fonts','Design/UI','Design/Presets']:
  for p in sorted((ROOT/directory).rglob('*')):
   if p.is_file():z.write(p,str(p.relative_to(ROOT)))
 for p in sorted((ROOT/'Design/UI-Previews').glob('*.svg')):z.write(p,str(p.relative_to(ROOT)))
 for name in ['test_ui_layouts.py','export_presets.cpp','export_metadata.cpp','generate_layouts.py','render_reverb_demo.cpp']:
  z.write(ROOT/'tools'/name,'tools/'+name)
 for name in ['CMakeLists.txt','LICENSE-NOTICE.txt','README.md','docs/PRESETS-TEMPO-REVERB.md']:
  z.write(ROOT/name,name)
 for p in sorted(AUDIO.iterdir()):z.write(p,'Audio/'+p.name)
 z.writestr('READ_ME_BUILD_AND_LISTEN.txt',notes)
with ZipFile(package) as z:
 assert z.testzip() is None
 for suffix in ['Core/Suite.cpp','Core/FactoryPresets.cpp']:
  assert z.read('Source/'+suffix)==(ROOT/'Source'/suffix).read_bytes()
 assert len(json.loads(z.read('Design/Presets/FactoryPresets.json'))['banks'])==10
(OUT/'SHA256SUMS.txt').write_text(hashlib.sha256(package.read_bytes()).hexdigest()+'  '+package.name+'\n')
print(json.dumps(stats,indent=2));print(package,package.stat().st_size,'bytes; CRC/source checks PASS')
