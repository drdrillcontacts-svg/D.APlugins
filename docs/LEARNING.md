# Learning architecture — Rack Collection 02

## Eligibility is enforced, not merely hidden

All 90 parameters remain manual/automatable. `canLearn(Kind,id)` selects **33** controls whose current
strategies actually use source features. The other **57** never appear in a proposal, including
Learn All and module/band scopes. Unsupported/manual-only scopes fail explicitly. The metadata
export, layout generator and manuals share this core-derived eligibility flag.

This is an evidence-based product policy, not an asserted universal plugin-making standard. Input
metrics do not uniquely determine an artistic setting. Even eligible results are bounded starting
points, not guaranteed sound improvements or original hardware settings.

| Plugin | Eligible controls | Current method and limits |
|---|---|---|
| DVerb | Pre-delay, Decay, Tone, Width, Low Cut | Crest-based ambience/separation; coarse low/high energy; side/correlation target. Decay is not measured reference RT60 |
| DDelay | Filter, Width | High-band energy and side/correlation guidance. No inferred rhythm, echo timing or feedback optimum |
| DComp | Threshold, Ratio, Attack, Release, Makeup | RMS/crest; smooth extends recovery; target/input RMS makeup, not measured post-compression compensation |
| DEQ | Four band gains | RMS-normalized source/target broad energy; ±6 dB limit. Frequency/Q/shape are preserved, never reset to policy values |
| DSat | Drive, Tone | Input peak/crest headroom and high-band energy/dark/warm target. No harmonic classifier or automatic output loudness match |
| DTrans | Attack, Sensitivity | Crest-factor onset/sensitivity starting points; Sustain/Shape remain manual |
| DStereo | Width, Side | Relative side energy and correlation; negative correlation narrows Width and reduces Side. Cannot widen identical dual mono |
| DSess | Frequency, Threshold, Range, Release, Focus | Coarse high-band energies/crest; does not reliably classify vocal sibilance versus cymbals |
| DGate | Threshold, Attack, Release | RMS−18 dB threshold; crest-based timing, smooth extends release. No measured noise floor or SNR separation |
| DFX | Drive, Filter, Decay | Saturation/Filter/Ambience scopes only. Delay and module enables/mixes/output are manual; no coupled chain optimization |

## Features, modes and limits

`analyze` is offline on copied audio. It measures RMS/peak/crest, correlation, side energy, four coarse
overlapping one-pole bands and normalized sample differences. **Band powers and differences are now
computed per channel and averaged**, so anti-phase stereo no longer cancels the learning spectrum.
These are not a calibrated FFT, loudness/noise/tempo model or a trained classifier. At least 1024
samples and RMS > 1e-7 are required; nonfinite samples are sanitized.

- Input mode uses measured features plus conservative goals.
- Reference requires a valid local file. It informs DEQ gains, Width in DVerb/DDelay/DStereo and DComp
  Makeup only. Other eligible controls use source/objective features, not reference matching.
- Description accepts whole English keywords `dark`, `warm`, `wide`, `smooth`, `cinematic`.
  Dark/warm affect supported tonal targets; wide affects Width; smooth extends eligible Release;
  cinematic affects eligible Decay. A keyword need not affect every chosen control. Other words
  may be ignored. Unknown-only text and common negation words fail explicitly; `not dark` no longer
  means dark, and `darkness`/`smoothie` do not match. No Persian/general-language interpretation.
- Scores are heuristic confidence, not calibrated probabilities. No trained neural model, model
  weights, inference service, exact-reference reconstruction or output-error optimization is shipped.

## Capture, workers and lifecycle

`InputCapture` owns fixed 65,536-sample stereo storage, with a lock-free state handshake. The audio
producer alone owns its write position. Message-thread cancellation never resets that memory.
Cancelled captures can be restarted even while transport is stopped; old completions cannot publish
as a new request. Prepare cancels an old capture and stores the sample rate separately from the
completed buffer. Mono is duplicated. The editor times out after 15 seconds and cancels on close.

Reference decode is local and bounded to 30 seconds; analysis and I/O run on workers, not the audio
thread. Future tasks capture values/buffers, not editor/processor pointers; the file chooser callback
uses a SafePointer. Cancellation invalidates in-flight results rather than interrupting a running
worker. `std::async` future destruction can still wait for bounded work when closing the editor;
shutdown responsiveness under slow storage remains a release qualification item.

Completion updates status/report text **without opening the log**. Only DETAILS / LOG opens it.
PREVIEW temporarily writes the proposal; APPLY keeps it; REJECT restores it. There is no automatic
parameter application. A manual/host edit cancels stale learning, restores untouched previewed
controls and retains the genuine edit. That edit expires old full-state learning Undo so Undo
cannot overwrite subsequent manual settings. Closing an editor restores an unaccepted preview.
Session parameters persist through APVTS/XML; decoded references, descriptions, pending results
and undo history do not. Writes notify hosts only for parameters that actually changed.

## Evidence and outstanding qualification

Portable tests cover every eligible scope, explicit rejection of manual-only IDs, module/all
isolation, preserved manual controls, reference direction, amplitude responsiveness, reversible
lifecycle, stale results and Undo expiration. Regression tests compare in-phase versus anti-phase
band/transient features and reject negated/subword goals. Capture tests exercise cancellation,
restart without transport, partial/ready cancellation, sample-rate changes, mono and 100 concurrent
stereo captures with every sample checked. ASan/UBSan supplement those tests, not thread-race proof.

Native tests request actual audio-driven learning and assert the report remains hidden until clicked;
these require linked JUCE/display execution. Cross-platform/host automation, slow/corrupt reference
files, spectral/temporal estimate quality, simultaneous changes and listening tests remain required.
The suite must not be labelled commercial-release-ready based on portable tests or design artwork.
