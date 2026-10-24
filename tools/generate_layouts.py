#!/usr/bin/env python3
"""Final approved UI layout generator (1120 x 860 edge-to-edge Noir Black + per-plugin accent).
Eliminates outer side margins, increases parameter label legibility, and preserves all 92 controls
and 33 selective Learn buttons without overlap.
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


def control_box(name, p, cx, top, h, r, force_knob=False):
    opts = options_for(name, p)
    if p['discrete'] and not force_knob:
        if opts == ['OFF', 'ON']:
            w = 134
        elif len(opts) == 2:
            w = 224
        elif len(opts) == 3:
            w = 340
        else:
            w = 368
        return [round(cx - w / 2), int(top), int(w), int(h)]
    w = 132 if r >= 28 else 128
    return [round(cx - w / 2), int(top), int(w), int(h)]


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
        'canvas': [1120, 860],
        'analysisRect': [22, 82, 1076, 204],
        'learningRect': [14, 302, 1092, 48],
        'mainPanelRect': [14, 382, 1092, 192],
        'advancedPanelRect': [14, 608, 1092, 194],
        'controls': [],
        'modules': [],
    }
    if name == 'DDelay':
        main_xs = [86, 224, 362, 500, 696, 892, 1030]
    elif name == 'DEQ':
        main_xs = [150, 360, 570, 875]
    else:
        span = 1068
        main_xs = [26 + (i + 0.5) * span / len(main_ids) for i in range(len(main_ids))]

    for i, cid in enumerate(main_ids):
        m = lookup[cid]
        fk = (cid == PRIMARY[name])
        opts = options_for(name, m)
        rect = control_box(name, m, main_xs[i], 388, 180, 28, force_knob=fk)
        style = 'rotary_discrete' if (m['discrete'] and fk) else ('choice' if opts else 'rotary')
        spec['controls'].append({
            'id': cid,
            'label': label_for(m),
            'rect': rect,
            'center': [round(main_xs[i], 1), 466],
            'radius': 28,
            'style': style,
            'options': opts,
            'advanced': False,
            'learnable': m['learnable'],
        })

    if name == 'DEQ':
        for j, b in enumerate([1, 3, 4]):
            bx = 22 + j * 362
            sub_specs = [
                (f'b{b}freq',  'FREQ',  [bx + 6,   638, 108, 118], 21, 'rotary'),
                (f'b{b}gain',  'GAIN',  [bx + 120, 638, 108, 118], 21, 'rotary'),
                (f'b{b}q',     'Q',     [bx + 234, 638, 108, 118], 21, 'rotary'),
                (f'b{b}shape', '',      [bx + 10,  760, 332, 32],  0,  'choice'),
            ]
            for cid, lbl, rbox, rad, st in sub_specs:
                m = lookup[cid]
                opts = options_for(name, m)
                spec['controls'].append({
                    'id': cid,
                    'label': lbl,
                    'rect': rbox,
                    'center': [rbox[0] + rbox[2] / 2, rbox[1] + rbox[3] / 2],
                    'radius': rad,
                    'style': st,
                    'options': opts,
                    'advanced': True,
                    'learnable': m['learnable'],
                })
    else:
        if name == 'DVerb':
            adv_xs = [106, 266, 426, 586, 886]
        else:
            span = 1068
            adv_xs = [26 + (i + 0.5) * span / len(adv_ids) for i in range(len(adv_ids))]
        for i, cid in enumerate(adv_ids):
            m = lookup[cid]
            opts = options_for(name, m)
            cx = adv_xs[i]
            rect = control_box(name, m, cx, 614, 180, 25, force_knob=False)
            spec['controls'].append({
                'id': cid,
                'label': label_for(m),
                'rect': rect,
                'center': [round(cx, 1), 692],
                'radius': 25,
                'style': 'choice' if opts else 'rotary',
                'options': opts,
                'advanced': True,
                'learnable': m['learnable'],
            })

    scopes = list(dict.fromkeys(m['module'] for m in metadata if m['learnable']))
    for i, scope in enumerate(scopes):
        w = 148
        x = 1106 - len(scopes) * (w + 8) + i * (w + 8)
        spec['modules'].append({
            'title': 'LEARN ' + scope.upper(),
            'scope': scope,
            'rect': [int(x), 354, int(w), 25],
        })
    layouts.append(spec)

(ROOT / 'Design/UI/Layouts.json').write_text(json.dumps(layouts, indent=2) + '\n')
print('Final approved layouts:', len(layouts), 'products /',
      sum(len(s['controls']) for s in layouts), 'controls /',
      sum(c['learnable'] for s in layouts for c in s['controls']), 'Learn buttons')
