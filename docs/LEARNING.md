# Learning architecture and actual strategies

## Boundaries

`Source/Core/Suite.h/.cpp` is independent of JUCE. `Parameter` metadata declares stable ID, module,
unit, range, default and discreteness. Every result is constrained and carries an explanation and a
**heuristic confidence score, not a calibrated probability**. Dispatch currently uses plugin kind and
parameter ID; richer declarative analysis requirements/strategy registration remain future work.

`Engine` owns prepared DSP state. `analyze` operates offline on copied audio. `learn` produces proposals
without changing state. `LearningSession` implements revision-gated offer/preview/apply/reject/undo.
The JUCE layer captures 65,536 samples in fixed memory, transfers only when ready, runs estimates with
`std::async`, polls futures on the message thread, and applies APVTS changes only on user approval.
Reference decode uses a separate worker and reads at most 30 seconds. Futures never capture UI/processor
pointers; chooser callback uses a JUCE SafePointer. A running worker is not forcibly stopped, but stale
results cannot apply. A timeout/cancelled recording may finish and be discarded later.

## Feature extraction

- Channel-combined RMS and peak, crest factor, channel correlation and side/RMS ratio.
- Four coarse **overlapping one-pole-derived bands**, not an FFT or perceptual loudness model.
- Normalized absolute sample differences, not an independently validated transient/onset classifier.
- At least 1024 samples, RMS > 1e-7 and finite analysis are required. NaN/Inf samples are sanitized.
- No LUFS, true peak, noise-floor distribution, tempo, harmonic classifier or decay-time estimator yet.
- Spectrum uses the mono sum: anti-phase source material needs a more robust per-channel power spectrum.

## Policies implemented (all proposals bounded to the parameter metadata)

| Parameters | Features and objective / actual estimate | Constraints, ambiguity and test approach |
|---|---|---|
| EQ band gain | Ratio of target/source band energy after RMS normalization; input-only broad balance target 0.42; dark/warm alter target | ±6 dB proposal limit. Synthesized darker target must reduce high band. This is tonal guidance, not inverse filtering |
| EQ frequency/Q/shape | Fixed 100/400/2500/10000 Hz, Q 0.707, bell | Low confidence 0.2. These are a broad tonal objective, NOT measured optimum frequencies or masking-aware learning |
| Compressor threshold | RMS dBFS + 0.25 × crest dB | Louder input test requires higher threshold. No level percentiles yet |
| Ratio | crest/3 clamped 1.5–6 | Intended crest reduction; needs output-target optimization |
| Dynamics attack/release | Compressor attack 20 ms for crest>12 dB, else 5 ms; de-esser attack 1 ms; release 100 + 5×crest ms | Transient-preservation policy, not inferred original hardware envelope. Bounds tested |
| Knee/makeup | smooth goal → 12 dB knee, otherwise 6; makeup uses target/input RMS difference | Reference target level used only for compressor makeup. Not loudness normalization or measured GR compensation |
| Gate threshold | RMS −18 dB | Conservative starting point only; no noise-floor detector. A constant source/noise can be misclassified |
| Gate range/hold/hysteresis | 40 dB / 50 ms / 6 dB policy | No audio-identifiable unique answer; must audition. Add distribution/noise objectives |
| De-esser frequency/threshold/range/focus | Upper-band energy comparison chooses 4.5/7 kHz; high-band RMS−4 dB threshold; relative high energy controls range/focus | Current signal driven, but cannot distinguish sibilance from cymbals. Need voiced/noise test corpus |
| Stereo width | Target side ratio / source side ratio, limited 0.5–1.5; negative correlation forces 0.5 | Reference side ratio or wide goal. Cannot make stereo from dual mono; add output correlation verification |
| Stereo side/center/mono/depth | Negative correlation → −6 dB side; center/monitor neutral; cinematic → 0.15 depth | Neutral monitor/center are user decisions. No measured depth inference |
| Saturation drive | 12 − input peak dB −0.4×crest, limited 0–18 dB | Input-headroom heuristic, not harmonic reference matching |
| Tone/filter | Source high-band energy reduces cutoff; dark/warm request 3.5/6 kHz | Spectral objective, bounded 2–14 kHz before parameter constraint. Reference tone fitting not implemented |
| Transient attack/sustain/sensitivity/shape | Attack 6 −0.4×crest; sensitivity 1−crest/30; smooth gives +2 dB sustain and 0.7 shape; otherwise −2 dB /1.2 | Actual envelope DSP; learner still needs temporal feature objectives beyond crest |
| Reverb pre-delay/decay/size/lowcut | Pre-delay 2×crest ms; decay 1+0.05×crest s or cinematic 4 s; size 0.5/0.85; low band heavy →200 Hz lowcut, otherwise 80 | Decay is a source/goal ambience policy, NOT measured reference RT60. Long reference spaces cannot be promised |
| Reverb midcut/mode/density/diffusion/modulation | warm midcut −1 vs −3 dB; cinematic Hall vs Room; smooth density/diffusion .85 vs .6; cinematic modulation .2 vs .05 | Objective/discrete policy only. No unique mode reconstruction, trained room recognition or impulse-response inference |
| Delay time/feedback/ping-pong | Crest>12 →250 ms else375 ms; cinematic feedback .5 vs .25; wide enables ping-pong | Explicit low-confidence timing policy (0.15), NOT tempo detection/reference echo matching |
| Mix and DFX module mixes | Dynamics/image full wet; cinematic .35 otherwise .2 | Objective starting points, not independently learned optimal wetness |
| Output | Input peak headroom to −3 dBFS, limited −12..0 dB change | Does not account for processed peaks; output verify loop remains required |
| DFX enable controls | Retained current value with confidence .1 | User routing decision, not fabricated classification |

Common default score is 0.45; parameter-specific score/explanation overrides exist. These numbers are
not validated confidence intervals. Coordinated Learn all currently assembles policies without a coupled
optimizer. Module isolation is tested, but inter-module optimality is not.

## Truthful mode separation

- **Input:** depends on source features plus declared conservative policies; no target is implied.
- **Description:** five adjectives set tonal/width/smoothing/ambience goals, while still requiring input.
  Unknown-only descriptions fail. Mixed text currently ignores unrecognized words and does not handle
  negation; general natural-language interpretation is not implemented.
- **Reference:** requires a valid decoded target; only EQ gains, width and compressor makeup currently
  depend on the reference. The proposal explicitly explains this restriction. It does not claim to recreate
  a room, infer delay tempo or optimize arbitrary effects to a reference. These are outstanding tasks.

## Future trained-model adapter

`ModelInference` declares availability, model version and a proposal-producing interface. **It is not
instantiated, configurable or wired to runtime selection yet.** To enable a real trained model:

1. Specify feature version, sample-rate normalization, windowing, plugin parameter schema and goal encoding.
2. Provide legally distributable weights, model card, input/output schema, validated per-effect dataset and
   objective metrics (including unsuitable-source and impossible-target behavior).
3. Implement an inference-runtime adapter (e.g. ONNX Runtime), CMake/runtime dependencies, checksum/version
   checks and availability/error reporting. Run it only on analysis workers, never the audio callback.
4. Validate/constrain model outputs through parameter metadata and revision gating; expose provenance,
   explanations and confidence calibration. Use the explicitly labeled deterministic path on unavailable models.
5. Add backend selection, failure/cancellation tests, reproducible model fixtures and offline/no-network tests.

No network inference, model download, hidden preset selection or automatic parameter application occurs now.
