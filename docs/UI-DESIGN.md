# Final Approved UI — Noir Black & Per-Plugin Signature Palette

## Reference and scope

The runtime JUCE interfaces implement the user-approved Final Reference UI (`Review 06 — Per-Plugin Noir Colors`):
- **1200 × 1240** logical canvas (`750 × 775` minimum, `1800 × 1860` maximum, fixed `1200:1240` aspect ratio).
- Matte **Noir Black** chassis (`#131315` / `#17171b` / `#0e0e11`) across all ten plugins, paired with a distinct signature accent colour per plugin (only **DVerb** uses the purple `#a28bd0` accent):
  - **DVerb:** `#a28bd0` (Purple / Violet)
  - **DDelay:** `#6cb8d6` (Cyan / Ice Blue)
  - **DComp:** `#d4a259` (Amber / Gold)
  - **DEQ:** `#68c29a` (Emerald / Mint)
  - **DSat:** `#d86c6c` (Crimson / Red)
  - **DTrans:** `#de8854` (Copper / Orange)
  - **DStereo:** `#6f95de` (Cobalt / Sky Blue)
  - **DSess:** `#5cc2be` (Teal / Turquoise)
  - **DGate:** `#b3c662` (Lime / Olive Gold)
  - **DFX:** `#cf7ca8` (Rose / Magenta)
- Original `DAmark` silhouette and product title placement (`x=278, y=123`) are preserved.
- All **92** actual parameters are visible on the unified two-tier layout without page-switching or duplicated controls:
  - The former large primary parameter is the **first/leftmost small rotary knob** (`r=28`) in the main row, matching the size of its neighbours.
  - No giant centered knob, no curved display notch, and no optional English-keywords box are present on the main interface.
  - Discrete choice buttons (such as `STEREO` / `PING PONG` in `DDelay`) have explicit horizontal separation so labels never collide.
- **Separate Optional Reference Workspace (`REFERENCE...`):**
  - The main analysis screen shows only the `IN`/`OUT` stereo meters and the wide live/parameter-aware graph (`900 × 209`).
  - Reference waveform and file controls live in a dedicated overlay panel (`reference-panel`) that is **closed by default** and opened only when the user clicks `REFERENCE...` in the horizontal `LEARNING` toolbar.
- **Selective Learning & Opt-In Details / Log:**
  - **33** analysis-eligible controls expose `LEARN`; **59** manual/taste/routing/output controls do not.
  - `DETAILS / LOG` (`learning-log`) is closed by default and never opens automatically when learning completes.
  - `PREVIEW`, `APPLY`, `REJECT`, and `UNDO` remain explicit user actions.

| Product | Main row controls (`r=28`) | Lower section controls (`r=24` / EQ band rows) | Learn-capable IDs |
|---|---|---|---|
| DVerb | Decay / Pre-delay / Size / Tone / Mix / Width / Density | Diffusion / Mod Depth / Low Cut / Mid Cut / Character | predelay, decay, tone, width, lowcut |
| DDelay | Division / Time / Feedback / Filter / Routing / Mix / Width | Mod Depth / Output / Tempo Sync | filter, width |
| DComp | Threshold / Ratio / Attack / Release / Makeup | Knee / Mix / Output | threshold, ratio, attack, release, makeup |
| DEQ | Band 2 Gain / Freq / Q / Shape | Bands 01 / 03 / 04 (Freq, Gain, Q, Shape) | b1gain, b2gain, b3gain, b4gain |
| DSat | Drive / Tone / Density / Mix | Character / Output | drive, tone |
| DTrans | Attack / Sustain / Sensitivity / Shape / Mix | Output | attack, sensitivity |
| DStereo | Width / Center / Side / Depth / Mix | Mono Check | width, side |
| DSess | Detect Freq / Threshold / Range / Focus / Mix | Attack / Release / Output | frequency, threshold, range, release, focus |
| DGate | Threshold / Attack / Hold / Release / Range | Hysteresis / Output | threshold, attack, release |
| DFX | Mix / Drive On / Drive / Filter On / Low Pass / Delay On / Time | Feedback / Delay Mix / Reverb On / Decay / Space Mix / Output | drive, filter, decay |

## Validation and reproduction

```sh
python3 tools/generate_layouts.py
python3 tools/render_designs.py
node tools/render_designs.cjs
python3 tools/test_ui_layouts.py
node tools/test_gallery.cjs
```

Native CMake + JUCE 8.0.6 build and snapshot runner (executed in multi-platform CI on Ubuntu, Windows, and macOS):

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DDA_BUILD_UI_SNAPSHOTS=ON
cmake --build build --target da_ui_snapshots --parallel 2
xvfb-run -a ./build/da_ui_snapshots_artefacts/Release/da_ui_snapshots "$PWD/build/native-ui-previews"
```
