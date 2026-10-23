# Carbon / Crimson — ten-interface redesign

## Authoritative visual reference

The user-supplied original logo and DVerb hardware design are preserved without modification:

- `Design/Brand/DA-original.png`
- `Design/Reference/DVerb-user-reference.png`

`Design/Brand/DAmark.png` is a cropped, transparent derivative for use on the dark enclosure. No invented
replacement logo was generated. Rajdhani Medium/SemiBold are embedded with their SIL OFL license in
`Design/Fonts/OFL.txt`. Old mockups in `Design/UI-Mockups/` remain untouched.

## Deliverables and an important distinction

- `Design/UI-Previews/index.html`: Persian-language collection viewer, with all ten designs, zoom,
  downloadable PNG/SVG and comparison against the original reference.
- Ten **2400 × 2240** PNG design studies, plus scalable SVG originals in the same directory.
- Native JUCE hardware components and ten declarative product layouts, **not a screenshot used as a UI**.
- CI snapshot runner `da_ui_snapshots` to produce actual JUCE component captures and exercise attachment wiring.

**The checked-in PNG/SVG studies are design previews, not native plugin screenshots.** Their sample
waveforms, traces, reference and meter readings are explicitly illustrative. Their footer and the gallery
identify this. The native editor does not use these images as controls or fabricate match scores. It draws
its own enclosure and controls and uses actual telemetry/parameters for the displays. Native screenshot
quality, detailed visual parity, automation and platform font rendering remain to be validated by CI/hosts.

## Visual system

1200 × 1120 logical canvas, uniformly scaled from 900 × 840 to 1800 × 1680; initial size 1080 × 1008.
Angular machined enclosure, subtle diagonal carbon weave, recessed dark screens, fine steel highlights,
restrained crimson rings and strokes, metal knobs with shadows/brushed highlights, embedded brand mark.

The hardware reference's fake window controls and unsupported “neural engine”/percentage labels are
not copied. Buttons in the native editor have actual actions. Learn All is the illuminated central action
on the six-control products; parameter-dense EQ/DFX put it in the learning panel. Every parameter, including
choices/enable switches, retains its own Learn action. A Suggestions drawer shows explanations and the
current/proposed values; approval controls stay accessible below the drawer.

| Product | Purpose-specific arrangement and view |
|---|---|
| DVerb | Six space/colour controls around Learn All; Room/Hall/Plate/Shimmer selector; 3+2 advanced controls; actual spectral history |
| DDelay | Time/feedback/filter/mix/width/output; dedicated stereo/ping-pong selection and modulation; frequency view |
| DComp | Six dynamics controls; knee/parallel mix; parameter-derived transfer curve and actual GR readout |
| DEQ | Four independent band strips, each with frequency/gain/Q/shape and its own Learn Band; parameter-derived RBJ response |
| DSat | Drive/tone/density/mix/output; Soft/Hard/Asymmetric selector; parameter-derived nonlinear transfer curve |
| DTrans | Attack/sustain/sensitivity/shape/mix/output; actual waveform envelope display |
| DStereo | Width/depth/center/side/mix and mono choice; actual L/R vectorscope and channel-correlation reading |
| DSess | Detection/threshold/range/focus/mix/output; attack/release section; actual spectrum with crossover highlight and GR |
| DGate | Threshold/attack/hold/release/range/output; hysteresis and gate transfer/GR section |
| DFX | Four separately enabled module strips (Saturation, Filter, Delay, Ambience), per-module learning; master mix/output |

## Runtime architecture

`Source/UI/HardwareLook.*` paints reusable knobs, bevelled buttons, enclosure, branding and parameter
controls. Host parameter IDs/ranges are preserved. Choice buttons share the attached slider's value,
so host automation updates their selected state too. Unit/value formatting is installed **after** JUCE's
SliderAttachment, because the attachment overwrites slider text/parser callbacks during construction.

`Design/UI/Layouts.json` is consumed by the native editor and by the design exporter. It covers every
one of the 90 current parameter IDs exactly once. Geometry tests verify unique coverage, choice counts,
band/module scopes and non-overlapping parameter rectangles. `Parameters.json` is generated from the
actual C++ metadata registry by `tools/export_metadata.cpp`, not an independently maintained parameter list.

`Source/Core/ScopeFifo.h` is a fixed-capacity, lock-free SPSC queue owned by the processor. The audio
thread copies dry/wet frames in bounded stack chunks; full queues drop visualization frames without
blocking. The GUI drains the queue and computes windowed spectra off the audio thread. No fake
waveform is painted when no signal/reference exists. Spectral history is temporal FFT history, **not**
a measured RT60 or a reference-match score. Drawn transfer curves are visualizations, not claims of
oversampling or independently verified model accuracy.

Full/empty/wrap and 200,000 concurrent ordered frames are tested. Allocation/performance profiling,
thread sanitization, signal/GUI lifetime stress, exact meter calibration and full accessibility still need work.
Only one editor consumer may drain the queue. Reference loading continues to be asynchronous and
bounded to 30 seconds. Existing learning-engine limitations in `LEARNING.md` remain; a new appearance
does not turn heuristic policies into trained inference.

## Reproduce the design studies

```sh
python -m venv .venv
. .venv/bin/activate
pip install -r tools/requirements.txt
g++ -std=c++17 -O2 -ISource tools/export_metadata.cpp Source/Core/Suite.cpp -o /tmp/da-metadata
/tmp/da-metadata > Design/UI/Parameters.json
python tools/generate_layouts.py
python tools/render_designs.py
npm ci --prefix tools
npm run render --prefix tools
python tools/test_ui_layouts.py
python tools/serve_designs.py  # 0.0.0.0:8080; only Design/ is exposed
```

No generative-image service is required. SVG/text/layout geometry remains editable. PNGs use a pinned
resvg renderer and bundled fonts. Raster output is not expected to be pixel-identical to JUCE's renderer.

## Render actual native screenshots (Linux with dependencies)

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DDA_BUILD_UI_SNAPSHOTS=ON
cmake --build build --target da_ui_snapshots --parallel 2
xvfb-run -a ./build/da_ui_snapshots_artefacts/Release/da_ui_snapshots "$PWD/build/native-ui-previews"
```

The runner creates each real editor, checks control count and parameter attachment isolation/value units,
processes an actual synthesized fixture, renders 2× PNG captures, and exercises the minimum window size.
CI uploads those separately as **native-juce-ui-previews**. Until that run passes, these native snapshots
are a configured verification path, not a claimed successful result.

## Verified in this session

- All ten PNG/SVG studies exported; DVerb and DFX reviewed at full resolution, collection reviewed as a sheet.
- All 90 native layout controls: IDs, option ranges, uniqueness and non-overlap tests passed.
- Core, FIFO concurrency and layout CTests: **3/3 passed** in Release and ASan/UBSan Debug.
- Gallery navigation, all ten download links, reference comparison and zoom/Escape wiring tests passed.
- Processor/editor/hardware/display/snapshot-runner sources: GCC C++17 warnings-as-errors syntax check passed
  against the pinned JUCE 8.0.6 headers. This does **not** claim a linked or visually audited native UI.
- Local full JUCE configuration remains blocked by missing pkg-config/system development packages.
- Native snapshot and new cross-platform plugin CI runs are pending; no new native captures claimed here.
