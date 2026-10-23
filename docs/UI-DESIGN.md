# Rack Collection 02 — all ten interfaces

## Reference and scope

`Design/Reference/Bundle-rack-reference.png` is the user's new full-bundle reference. Earlier
DVerb references, original logo and mockups are preserved. This revision adopts the compact,
wide rack face, silver chassis, screws, carbon/brushed finish, recessed dark display, machined
knobs and restrained red illumination. It does not copy unsupported AI/tempo/tape/module claims
from artwork into working controls. DAmark and licensed Rajdhani fonts remain embedded assets.

## Actual editor

- **1200 × 820** logical canvas; default 1200 × 820, minimum 900 × 615, maximum 1800 × 1230.
- Four or five product-specific main controls, an analysis display and a right-hand learning strip.
- **ADVANCED / MAIN CONTROLS** switches the parameter page, preserving every attachment/value.
  All 90 original IDs/ranges/defaults remain available and host-compatible.
- **DETAILS / LOG** is opt-in. Completing learning updates its text and the status line, never opens
  or raises the report. Preview/Apply/Reject/Undo remain explicit user actions.
- **33** audio-informed controls have Learn. **57** manual controls do not. Core `canLearn` enforces
  this for single, band/module and all scopes; this is not a cosmetic removal of buttons.
- Scope policy is a product decision, not a claimed universal industry standard. Tables and rationale:
  [LEARNING.md](LEARNING.md).

| Product | Main controls | Learn-capable IDs |
|---|---|---|
| DVerb | Pre-delay / Decay / Tone / Mix | predelay, decay, tone, width, lowcut |
| DDelay | Time / Feedback / Mix / Width | filter, width |
| DComp | Threshold / Ratio / Attack / Release / Makeup | threshold, ratio, attack, release, makeup |
| DEQ | Four band gains; frequency/Q/shape in Advanced | b1gain, b2gain, b3gain, b4gain |
| DSat | Drive / Tone / Density / Mix | drive, tone |
| DTrans | Attack / Sustain / Sensitivity / Mix | attack, sensitivity |
| DStereo | Width / Center / Side / Mono | width, side |
| DSess | Frequency / Threshold / Range / Mix | frequency, threshold, range, release, focus |
| DGate | Threshold / Range / Attack / Release / Output | threshold, attack, release |
| DFX | Drive / Filter / Time / Mix | drive, filter, decay |

EQ Band Learn now preserves frequency/Q/shape. DFX has no misleading Learn Delay action; echo
timing/feedback/mixes and all enable switches remain manual. Controls present in the reference but
not implemented by the actual DSP (e.g. synced divisions, extra DFX modules) are not invented.

## Design exports versus native UI

Twenty **2400 × 1640** PNG/SVG studies show main and Advanced for each effect. The gallery includes
page selection, zoom and comparison with the new bundle reference. Its traces/meters are illustrative;
these are **not native screenshots or executed audio results**. JUCE paints real controls and uses
live processor telemetry, reference data and parameter-derived explanatory curves instead.

```sh
# Export authoritative metadata/eligibility from the real core:
g++ -std=c++17 -O2 -ISource tools/export_metadata.cpp Source/Core/Suite.cpp -o build-metadata
./build-metadata > Design/UI/Parameters.json
python tools/generate_layouts.py
python tools/render_designs.py        # requires Pillow
npm ci --prefix tools
node tools/render_designs.cjs
python tools/test_ui_layouts.py
node tools/test_gallery.cjs
python tools/serve_designs.py         # 0.0.0.0:8080, serves Design plus allowlisted manual downloads
```

`Layouts.json` is shared by native controls and design exports; eligibility must match exported
metadata. Tests check parameter coverage, control-page non-overlap, choice cardinality, the 33/57
split and no automatic report opening. Native UI tests additionally exercise attachments, numeric
formatting, actual learning/report behavior, resizing and main/Advanced snapshot generation.

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DDA_BUILD_UI_SNAPSHOTS=ON
cmake --build build --target da_ui_snapshots --parallel 2
xvfb-run -a ./build/da_ui_snapshots_artefacts/Release/da_ui_snapshots "$PWD/build/native-ui-previews"
```

The latest local native build cannot configure because system pkg-config/JUCE development packages
are missing. The five modified JUCE translation units, including the snapshot tests, are separately
strict syntax-checked against pinned JUCE 8.0.6. Syntax checks do not replace linking, native execution,
DAW usability or visual review. Current native/platform CI must pass before claiming binary readiness.
