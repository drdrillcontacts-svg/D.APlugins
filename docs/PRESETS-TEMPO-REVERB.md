# Development 0.4 — factory programs, tempo and DVerb

## Implemented

- Every plugin exposes **50 named factory programs** through the host program API and header browser
  with previous/next. Each bank contains Init plus 49 musical variants across ten source/character
  profiles. All 500 snapshots are complete, bounded and unique within each bank; no runtime randomness.
  `Design/Presets/FactoryPresets.json` exports the exact compiled banks, not a second runtime source.
- Loading a preset replaces the whole plugin state, including Mix/Output and enable decisions. It
  cancels capture and invalidates old analysis/preview/undo. A `*` indicates changes relative to the last
  factory selection; save the host session to retain them. Factory names are read-only; no proprietary
  preset-file manager or activation is implied. Selected index and actual modified values persist in XML.
- DDelay appends stable `sync` and `division` IDs to the old eight parameters. FREE MS preserves the
  millisecond control; SYNC selects 15 straight/dotted/triplet divisions. Host tempo is queried once per
  process block. Supported tempo is 10–400 BPM; a whole note at 10 BPM is **24 seconds**, not clamped to
  the 2-second free range. Buffers are prepared for 25 seconds with no audio-callback resizing.
- In sync mode both channels use the exact same time (the old 1.011 right-channel factor is free-only),
  and time modulation is disabled. Tempo/division changes change tap position and can produce a pitch
  transition; this is not a click-free dual-head crossfade or transport-position retrigger effect.
- With missing/invalid/out-of-range tempo, the last valid tempo is held; before any valid host tempo,
  fallback is 120 BPM. The UI explicitly shows HOST/HOLD/FALLBACK and the resolved milliseconds. A
  standalone instance has no DAW project tempo. Sync/division/milliseconds/preset choice persist;
  host BPM itself belongs to the project, not the preset. Legacy states missing new IDs get defaults.

## DVerb engine changes and rationale

Previous code repeatedly high-passed/cut midrange inside each feedback circulation, mixed an
orthogonal matrix with identity (changing decay energy), saturated every tank sample and applied
an extra hidden 0.9 feedback factor. These paths could dull/shorten a tail and emphasize sparse echoes.

The new input is high-passed and broad-mid-shaped **once**, then passes four stereo allpass diffusers.
The late eight-line FDN always uses a normalized Hadamard scatter; Density controls normalized input
excitation, while Diffusion adjusts the input allpasses. Tone damps recirculating high frequencies,
but the output decodes raw line returns to preserve early detail. Stereo decoding uses two orthogonal
all-line sign patterns. Per-line nominal decay gains no longer include the extra 0.9 multiplier.
Normal-level tank processing is linear; only exceptional internal excursions above ±4 use a continuous,
slope-matched soft overload guard. Shimmer reserves feedback margin and remains a two-grain octave effect.
Tone default is now 10 kHz and Mid Cut −1 dB. Old projects retain their stored values but will sound
different because the algorithm changed. Keep the old plugin build for exact old-project recall.

Control-rate coefficients update every 32 samples using a persistent counter, independent of host block
splits. This avoids repeatedly computing multiple exponentials/powers per line on every sample. DFX uses
the same improved ambience network. RT60, aliasing and perceived quality are not independently certified.

## Other concrete fixes

- Shared DGate open/hold state now advances once per stereo sample, not twice through channel loops.
- GR reads the maximum channel reduction, not whichever channel happened to be processed last.
- Delay/DFX tail hints now account for current delay time/feedback instead of always returning 30 s.
  This is an approximate −60 dB decay hint, not a measured loudness/true-peak guarantee.
- The log drawer is explicitly hidden initially; native tests identify the actual log by component ID
  rather than accidentally selecting an unrelated read-only editor.

## Executed checks

Release CTest **6/6** and ASan/UBSan **6/6** (45.80 s). Tests include all 500 banks and finite audio
smoke renders; note mathematics and real impulse timing at 60/120/240 BPM; no synced stereo timing drift;
capture concurrency; isolated selective learning; linear small-signal reverb; dense stereo impulse
response; decay to silence; identical output across 64/257-sample block splits; and higher late HF
retention with brighter Tone. The latter measures the late tail (1–2 s), not unfiltered first arrivals.

Updated native tests also exercise all 50 host programs per product, XML roundtrip, legacy missing sync
IDs, mock host-tempo loss/hold, initial/optional logs, attachments, preview/manual changes and resize.
**The actual Linux JUCE native runner passed on CI run 39982254703**, including all these checks
and all ten main/Advanced editor captures. Native previews were successfully uploaded. The portable
sanitizer job also passed on that run. Full cross-platform format builds are still running at this
checkpoint. Their sources also strict-syntax-check against JUCE 8.0.6. Local linked native setup is blocked by missing
pkg-config/system development dependencies. Native execution was therefore performed on CI, not locally.

Previous revision 63e91b9 built on Windows/macOS but failed the Ubuntu native UI runner (exit 10).
That is historical evidence, not validation of this revision; the report initialization/identification
fix is now explicit. No claims of commercial certification, bug-free operation, signed installers or
host/pluginval/listening acceptance are made. Test musical material at matched level before replacing
a production reverb. The original synthetic before/after listening aid is reproducible with
`tools/render_reverb_demo.cpp`; it is not an external recording or a subjective listening approval.
