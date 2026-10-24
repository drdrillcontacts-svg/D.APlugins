#!/usr/bin/env python3
"""Final approved UI layout generator (1200 x 1240, Noir Black + per-plugin signature accent).
All 92 parameter IDs and 33 learnable parameters are preserved without overlap.
"""
import json
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
META = json.loads((ROOT / 'Design/UI/Parameters.json').read_text())

ACCENT = {
    'DVerb': '#a28bd0',   # Purple (only DVerb is purple)
    'DDelay': '#6cb8d6',  # Cyan / Ice Blue
    'DComp': '#d4a259',   # Warm Amber / Gold
    'DEQ': '#68c29a',     # Emerald / Studio Mint
    'DSat': '#d86c6c',    # Crimson / Tube Red
    'DTrans': '#de8854',  # Copper / Tangerine Orange
    'DStereo': '#6f95de', # Cobalt / Sky Blue
    'DSess': '#5cc2be',   # Teal / Aquamarine
    'DGate': '#b3c662',   # Lime / Olive Gold
    'DFX': '#cf7ca8',     # Rose / Magenta
}

ROLE = {
    'DVerb': 'REVERBERATION',
    'DDelay': 'STEREO DELAY',
    'DComp': 'DYNAMICS',
    'DEQ': 'FOUR-BAND EQ',
    'DSat': 'SATURATION',
    'DTrans': 'TRANSIENT SHAPING',
    'DStereo': 'STEREO IMAGE',
    'DSess': 'SIBILANCE CONTROL',
    'DGate': 'NOISE GATE',
    'DFX': 'MULTI EFFECT',
}

PRIMARY = {
    'DVerb': 'decay',
    'DDelay': 'division',
    'DComp': 'threshold',
    'DEQ': 'b2gain',
    'DSat': 'drive',
    'DTrans': 'attack',
    'DStereo': 'width',
    'DSess': 'frequency',
    'DGate': 'threshold',
    'DFX': 'mix',
}

MAIN = {
    'DVerb': ['predelay', 'size', 'tone', 'mix', 'width', 'density'],
    'DDelay': ['time', 'feedback', 'filter', 'pingpong', 'mix', 'width'],
    'DComp': ['ratio', 'attack', 'release', 'makeup'],
    'DEQ': ['b2freq', 'b2q', 'b2shape'],
    'DSat': ['tone', 'density', 'mix'],
    'DTrans': ['sustain', 'sensitivity', 'shape', 'mix'],
    'DStereo': ['center', 'side', 'depth', 'mix'],
    'DSess': ['threshold', 'range', 'focus', 'mix'],
    'DGate': ['attack', 'hold', 'release', 'range'],
    'DFX': ['driveOn', 'drive', 'filterOn', 'filter', 'delayOn', 'time'],
}

GROUPS = {
    'DVerb': ('SPACE & CHARACTER', 'DIFFUSION & COLOUR'),
    'DDelay': ('ECHO & COLOUR', 'SYNC / MOTION / OUTPUT'),
    'DComp': ('DYNAMICS', 'KNEE / BLEND / OUTPUT'),
    'DEQ': ('BAND 02 / SELECTED', 'BANDS 01 / 03 / 04'),
    'DSat': ('HARMONIC COLOUR', 'CHARACTER / OUTPUT'),
    'DTrans': ('ENVELOPE', 'OUTPUT'),
    'DStereo': ('MID / SIDE IMAGE', 'MONO CHECK'),
    'DSess': ('SIBILANCE CONTROL', 'TIMING / OUTPUT'),
    'DGate': ('OPEN / HOLD / RELEASE', 'HYSTERESIS / OUTPUT'),
    'DFX': ('DRIVE / FILTER / DELAY', 'FEEDBACK / AMBIENCE / OUTPUT'),
}

LABEL_MAP = {
    'predelay': 'PRE-DELAY',
    'modulation': 'MOD DEPTH',
    'frequency': 'DETECT FREQ',
    'pingpong': 'ROUTING',
    'mode': 'CHARACTER',
    'lowcut': 'LOW CUT',
    'midcut': 'MID CUT',
    'driveOn': 'DRIVE ON',
    'filterOn': 'FILTER ON',
    'delayOn': 'DELAY ON',
    'reverbOn': 'REVERB ON',
    'delayMix': 'DELAY MIX',
    'reverbMix': 'SPACE MIX',
    'sync': 'TEMPO SYNC',
    'division': 'DIVISION',
    'mono': 'MONO CHECK',
    'sensitivity': 'SENSITIVITY',
}

OPTIONS = {
    'DVerb:mode': ['ROOM', 'HALL', 'PLATE', 'SHIMMER'],
    'DDelay:pingpong': ['STEREO', 'PING PONG'],
    'DDelay:sync': ['FREE MS', 'SYNC'],
    'DDelay:division': ['1/32', '1/32 D', '1/32 T', '1/16', '1/16 D', '1/16 T', '1/8', '1/8 D', '1/8 T', '1/4', '1/4 D', '1/4 T', '1/2', '1/2 D', '1/1'],
    'DSat:mode': ['SOFT', 'HARD', 'ASYMMETRIC'],
    'DStereo:mono': ['STEREO', 'MONO'],
}

def label_for(p):
    i = p['id']
    if i.startswith('b') and i[1:2].isdigit():
        return 'B' + i[1] + ' ' + {'freq': 'FREQ', 'gain': 'GAIN', 'q': 'Q', 'shape': 'SHAPE'}[i[2:]]
    return LABEL_MAP.get(i, p['name'].upper())

def options_for(name, p):
    i = p['id']
    if not p['discrete']:
        return []
    if name == 'DEQ' and i.endswith('shape'):
        return ['BELL', 'LOW SHELF', 'HIGH SHELF']
    return OPTIONS.get(f'{name}:{i}', ['OFF', 'ON'])

def control_box(name, p, cx, cy, r, force_knob=False):
    opts = options_for(name, p)
    if p['discrete'] and not force_knob:
        if opts == ['OFF', 'ON']:
            w = 62 * 2 + 10
        elif len(opts) == 2:
            w = 114 * 2 + 12
        elif len(opts) == 3:
            w = 128 * 3 + 20
        else:
            w = 96 * 4 + 24
        top = round(cy - r - 36)
        h = (r + 36) + 24 + (26 if p['learnable'] else 0)
        return [round(cx - w / 2), top, int(w), int(h)]
    w = 120 if r >= 28 else 116
    top = round(cy - r - 36)
    h = (r + 36) + r + 32 + (24 if p['learnable'] else 0)
    return [round(cx - w / 2), top, int(w), int(h)]

layouts = []
for name, metadata in META.items():
    lookup = {m['id']: m for m in metadata}
    main_ids = [PRIMARY[name]] + MAIN[name]
    adv_ids = [m['id'] for m in metadata if m['id'] not in main_ids]
    spec = {
        'name': name,
        'subtitle': ROLE[name] + ' / LEARNING',
        'role': ROLE[name],
        'accent': ACCENT[name],
        'slogan': GROUPS[name][0],
        'advancedTitle': GROUPS[name][1],
        'canvas': [1200, 1240],
        'analysisRect': [70, 207, 1060, 342],
        'learningRect': [55, 590, 1090, 72],
        'mainPanelRect': [55, 693, 1090, 177],
        'advancedPanelRect': [55, 906, 1090, 229],
        'controls': [],
        'modules': [],
    }
    if name == 'DDelay':
        main_xs = [136, 262, 388, 514, 716, 918, 1052]
    elif name == 'DEQ':
        main_xs = [180, 380, 580, 900]
    else:
        main_xs = [100 + (i + 0.5) * 1000 / len(main_ids) for i in range(len(main_ids))]

    for i, cid in enumerate(main_ids):
        m = lookup[cid]
        fk = (cid == PRIMARY[name])
        opts = options_for(name, m)
        rect = control_box(name, m, main_xs[i], 774, 28, force_knob=fk)
        style = 'rotary_discrete' if (m['discrete'] and fk) else ('choice' if opts else 'rotary')
        spec['controls'].append({
            'id': cid,
            'label': label_for(m),
            'rect': rect,
            'center': [round(main_xs[i], 1), 774],
            'radius': 28,
            'style': style,
            'options': opts,
            'advanced': False,
            'learnable': m['learnable'],
        })

    if name == 'DEQ':
        for j, b in enumerate([1, 3, 4]):
            bx = 83 + j * 357
            for k, suffix in enumerate(['freq', 'gain', 'q', 'shape']):
                cid = f'b{b}{suffix}'
                m = lookup[cid]
                opts = options_for(name, m)
                ry = 945 + k * 43
                spec['controls'].append({
                    'id': cid,
                    'label': label_for(m),
                    'rect': [bx + 8, ry, 306, 36],
                    'center': [bx + 161, ry + 18],
                    'radius': 0,
                    'style': 'band_row',
                    'options': opts,
                    'advanced': True,
                    'learnable': m['learnable'],
                })
    else:
        for i, cid in enumerate(adv_ids):
            m = lookup[cid]
            opts = options_for(name, m)
            cx = 100 + (i + 0.5) * 1000 / len(adv_ids)
            cy = 1005
            if name == 'DVerb' and cid == 'mode':
                cx, cy = 915, 1089
            rect = control_box(name, m, cx, cy, 24, force_knob=False)
            if name == 'DVerb' and cid == 'midcut':
                rect = [round(cx - 58), 945, 116, 92]
            if name == 'DVerb' and cid == 'mode':
                rect = [round(cx - 204), 1044, 408, 76]
            spec['controls'].append({
                'id': cid,
                'label': label_for(m),
                'rect': rect,
                'center': [round(cx, 1), cy],
                'radius': 24,
                'style': 'choice' if opts else 'rotary',
                'options': opts,
                'advanced': True,
                'learnable': m['learnable'],
            })

    scopes = list(dict.fromkeys(m['module'] for m in metadata if m['learnable']))
    for i, scope in enumerate(scopes):
        w = 136
        x = 1145 - len(scopes) * (w + 8) + i * (w + 8)
        spec['modules'].append({
            'title': 'LEARN ' + scope.upper(),
            'scope': scope,
            'rect': [int(x), 662, int(w), 25],
        })
    layouts.append(spec)

(ROOT / 'Design/UI/Layouts.json').write_text(json.dumps(layouts, indent=2) + '\n')
print('Final approved layouts:', len(layouts), 'products /',
      sum(len(s['controls']) for s in layouts), 'controls /',
      sum(c['learnable'] for s in layouts for c in s['controls']), 'Learn buttons')
