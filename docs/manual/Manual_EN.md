# D.A Plugins — User Manual

Development version 0.3 / 2026-10-23 / Source baa25a22674c

## Welcome & bundle overview

### What this bundle does

D.A Plugins is a ten-effect suite for space, dynamics, tone, harmonics and stereo design. Its learning workflow analyzes audio and offers parameter proposals for inspection before approval. Manual operation remains available. This edition documents the current development implementation, not a finished commercial release.

Read the quick start first, then the chapter for your chosen plugin. Parameter references include the actual ID, manual range, factory default and a description. Example settings are starting points, not factory presets or guaranteed solutions.

### Before listening

Turn monitoring down before changing modes, feedback, drive or large gains. Keep headroom in the host and compare at matched level. The suite is not a hearing-protection system, true-peak limiter or substitute for an independent safety limiter. Use project backups and test copies for development builds.

### How to read this guide

A parameter label in English matches the current interface. A percentage describes an amplitude/control proportion, not necessarily perceived loudness. Some normalized controls, including Feedback, may display decimal values: 0.35 means 35%. A module scope is not always the same as a visual panel.

## Installation, compatibility & updates

### Obtain the correct build

Use a trusted build artifact for the correct operating system and CPU architecture. A ZIP of UI designs is not a plugin installer. The repository does not currently provide a signed commercial installer, activation manager or notarized release. CI output is not a blanket guarantee of DAW compatibility.

VST3 and Standalone are build targets on Windows, macOS and Linux; AU is a macOS target only. VST2, AAX and CLAP are not supplied. A host that requires AAX cannot load this bundle natively as an AAX plugin. No complete host/version certification matrix is published.

### Manual installation

Close your DAW. Extract the matching plugin bundles without changing their internal structure. Copy VST3 bundles into a standard location accepted by your host; the next page lists common paths. On macOS, copy .component bundles for AU. Then reopen the host and rescan its plugin database.

If a platform rejects an unsigned build, do not disable system security globally or run an untrusted binary. Use the platform’s supported per-item approval process only after verifying its origin, or ask the supplier for a properly signed build.

### Host setup and standalone use

Insert the chosen effect on an audio track or return bus. The processors accept matching mono-in/mono-out or stereo-in/stereo-out layouts; they are not instruments, MIDI effects, surround processors or mono-to-stereo layout converters. A silent track provides no usable learning capture.

A Standalone target uses the JUCE wrapper rather than a DAW session. Select an appropriate audio device and routing in the wrapper if available. Avoid feeding loudspeaker output back into a microphone input. A DAW is the recommended environment for routing, automation and session recall.

### Update or remove

Back up the session and keep the previous build before replacing files. Close the host, replace the matching bundle, rescan and test a copy of the session. Early four-parameter prototype states are not guaranteed to migrate. To remove a build, close the host and remove only its installed bundles, not shared host folders.

## Quick start & safe signal flow

### A first audition in six steps

1. Save a copy of the session and lower the monitor level. Choose the plugin that solves one identified problem. 2. Route a representative passage to the instance and confirm input/output activity. 3. Begin with small manual changes and sufficient host headroom.

4. Choose INPUT-AWARE LEARN, REFERENCE GUIDANCE or DESCRIPTION GUIDANCE. 5. Press a parameter Learn, a band/module Learn or LEARN ALL while audio plays. 6. Audition PREVIEW, then APPLY or REJECT; open DETAILS / LOG only if you need the analysis. Compare at matched level and save again after deciding.

### Insert versus auxiliary return

On an insert, Mix sets the dry/processed balance before any final Output control. On an auxiliary reverb or delay return, use 100% wet and control effect level with the host send/return. DEQ and DGate have no global Mix parameter. DFX has both local blends and a global Mix.

There is no universal plugin power switch in the current editor. Use host bypass for a whole-instance comparison. Mix 0% is not necessarily bypass: Output gain can still apply, and time-effect buffers may keep running. Module OFF in DFX is not a promise of buffer reset.

### An example processing order

A practical starting chain is editing → DGate if needed → corrective DEQ → DComp → DSess if needed → DSat sparingly → ambience on sends. This is a production suggestion, not a fixed internal bundle order. Move stages when the material benefits; avoid stacking level boosts without checking peaks.

## Interface, controls & meters

### Knobs and choices

Drag a rotary control to adjust it; double-click a rotary control to restore its declared default. Click its value field to enter a supported numeric value. Values are bounded to the parameter range. Frequency entry supports Hz and kHz notation; do not assume every host accepts the same text syntax in its automation list.

Room/Hall/Plate/Shimmer, filter shape, saturation character, Stereo/Mono and DFX OFF/ON controls are discrete choices. They are host-attached parameters, not decorative buttons. Mode changes are not guaranteed click-free crossfades; lower the level and audition transitions.

### Learning panel and workbench

The learning-mode menu selects how objectives are formed. LOAD FILE selects a local reference. DESCRIBE THE SOUND accepts the supported English keywords. DETAILS / LOG opens the optional report; it never opens automatically after Learn. BACK TO CONTROLS closes it. ADVANCED and MAIN CONTROLS switch control pages without changing parameter values. PREVIEW, APPLY, REJECT and UNDO control the proposal lifecycle, not a full session history.

### What the displays mean

Reference waveforms and audio analysis views show available data in the native editor. Frequency views use a 2048-sample windowed analysis and 128 displayed points. DVerb spectral history is a sequence of spectra, not a measured decay-time plot. DStereo’s vectorscope plots actual left/right samples. No signal can leave the display idle or empty.

DEQ draws a parameter-derived filter response. DComp, DGate and DSat draw explanatory transfer curves; a drawing is not an independently calibrated measurement of every dynamic state. The input/output readings are sample-peak dBFS-style displays, not LUFS, RMS loudness or inter-sample true peak. GR is an implementation readout with no certified meter-ballistics specification.

### Window and artwork

The hardware interface scales uniformly and preserves its aspect ratio. Hover tooltips identify many controls. The illustrations in this manual are design studies, not native screenshots; example traces/meters in those images are not analysis results or performance claims.

## Learning modes, scopes & approval

### Input-aware learn

The engine measures level, crest, coarse band energies and stereo features. Only 33 audio-informed controls have Learn. The other 57, including mixes, output trims, timing/routing and EQ frequency/Q/shape, stay manual in every scope. No trained neural network or model inference is shipped. Eligible values are still estimates, not uniquely identifiable settings.

### Reference guidance

A valid reference is required before reference mode can succeed. In this revision it informs normalized DEQ band gains, Width parameters in DVerb/DDelay/DStereo, and DComp Makeup level. Other suggestions remain source/objective policies. There is no rendered-output comparison loop or optimization that recreates an arbitrary reference.

### Description guidance

Enter English keywords: dark, warm, wide, smooth, cinematic. They request tonal, stereo, envelope or ambience objectives, while input audio is still required. Persian prose is not interpreted by the current keyword parser. Unknown-only descriptions fail; mixed text can silently use only recognized words.

Common negation words are rejected: “not dark” fails rather than activating dark. Only whole keywords match; “darkness” and “smoothie” do not match dark or smooth. Avoid complex instructions, exact emulation promises or unsupported adjectives. The phrase “dark, warm, wide, smooth, cinematic” is valid, but its result is an estimate to audition rather than a guarantee.

### Choose the smallest useful scope

LEARN under one parameter proposes only that ID. A band/module action proposes the declared section. Each EQ band action learns only that band’s gain; frequency, Q and shape stay manual. LEARN ALL assembles only eligible controls for the plugin. It is not a coupled optimizer; an individual proposal does not secretly authorize changes elsewhere.

### Preview, apply, reject, undo

Suggestions leave parameters unchanged until a deliberate action. PREVIEW temporarily writes the proposal so it can be heard. APPLY keeps it. REJECT restores the pre-preview values. UNDO restores one previously applied learning operation until a manual/host edit expires it; it is not a multilevel preset history. Closing the editor restores an unaccepted preview in the current implementation.

A manual/host edit invalidates older proposals and expires learning Undo; during Preview, untouched controls are restored while the real edit is kept. Avoid automating learned controls or changing settings while analysis is pending until that interaction is fully validated. Re-run after editing the source. Confidence is a heuristic score, not a calibrated probability or an audio-quality percentage.

## Reference files & capture workflow

### Load and validate

Choose LOAD FILE and a readable WAV, AIFF/AIF or FLAC supported by the bundled JUCE basic decoders. The picker filters these extensions; not every possible codec variant is guaranteed. Files are decoded locally on a worker. The first 30 seconds only are analyzed; select a reference whose relevant passage is at the beginning.

The reader accepts sample rates from 8 kHz to 384 kHz and at least 1024 samples. Silent or invalid features fail validation. Mono references are duplicated to the stereo analysis buffer. Surround-specific channel mapping and perceptual downmix are not provided.

### Capture the source

Keep the host passing non-silent audio while requesting a scope. The capture length is 65,536 input samples: about 1.49 s at 44.1 kHz, 1.37 s at 48 kHz or 0.68 s at 96 kHz. It is not the entire song. If playback is stopped or the host suspends the track, capture may time out after 15 seconds.

Use a representative phrase with transients and sustain, not a fade-in or an unusually loud isolated peak. Source analysis is of the input, not the already-processed output. New proposals replace values; they are not guaranteed incremental corrections to your current sound.

### Cancellation and failures

REJECT/cancel invalidates pending results; a bounded worker can still finish in the background. A stale result must not be treated as a new approval. If Capture busy persists after stopping, let audio run so an older capture can complete and be discarded, then request a fresh analysis.

Ready means idle; Analyzing means capture/analysis is underway; Ready to audition means a proposal exists; Previewing means temporary audition; Applied means retained; Cancelled means discarded/restored. Failed or Reference unavailable is not a success. No installed neural-model status should be inferred from the hardware artwork.

## DVerb — Reverb & ambience

DVerb places a source in an algorithmic ambience using an eight-line feedback-delay network. Pre-delay separates the dry event from the reverb onset; damping, diffusion and modulation shape the return. It is an effect-design tool, not a convolution player or an exact reconstruction of a sampled room.

Input → Pre-delay → Damped / diffused FDN → Stereo width → Dry/wet mix

### Control reference

#### Pre-delay / `predelay`

Range: 0 ms to 200 ms | Default: 20 ms

Time before the reverb excitation. Increase it to separate the dry attack from the ambience; it does not delay the dry path.

Learn: audio-informed

#### Size / `size`

Range: 0% to 100% | Default: 60%

Scales network delay lengths. Larger values change spacing and perceived scale; they are not room dimensions in metres.

Manual only; no Learn

#### Decay / `decay`

Range: 0.1 s to 12 s | Default: 2 s

Controls feedback decay design. Longer values sustain the ambience, but actual decay also depends on damping, diffusion, size and mode.

Learn: audio-informed

#### Tone / `tone`

Range: 500 Hz to 18000 Hz | Default: 6000 Hz

Low-pass damping cutoff in the reverb network. Lower values produce a darker return; this is a frequency control, not a dB tone tilt.

Learn: audio-informed

#### Mix / `mix`

Range: 0% to 100% | Default: 25%

Linear blend of original and reverberant signal. Use 100% for a conventional fully wet auxiliary return.

Manual only; no Learn

#### Width / `width`

Range: 0% to 200% | Default: 100%

Scales the side component of the wet reverb: 0% mono wet, 100% nominal, 200% double side. It does not widen the dry component.

Learn: audio-informed

#### Density / `density`

Range: 0% to 100% | Default: 70%

Changes excitation of additional network lines. Lower values emphasize fewer initial paths; higher values feed a denser initial network.

Manual only; no Learn

#### Diffusion / `diffusion`

Range: 0% to 100% | Default: 70%

Blends direct line feedback with a normalized scattering matrix. Higher settings disperse energy between delay paths.

Manual only; no Learn

#### Modulation / `modulation`

Range: 0% to 100% | Default: 10%

Depth of slow delay-length motion. It adds movement and can introduce pitch texture; no separate rate parameter is exposed.

Manual only; no Learn

#### Low cut / `lowcut`

Range: 20 Hz to 1000 Hz | Default: 120 Hz

High-pass-related damping of low-frequency tank content. Raise it to reduce low-end accumulation in the reverb.

Learn: audio-informed

#### Mid cut / `midcut`

Range: -12 dB to 0 dB | Default: -2 dB

Attenuates a broad, fixed midrange component in the tank. The band is not a freely tunable parametric EQ.

Manual only; no Learn

#### Room/Hall/Plate/Shimmer / `mode`

Range: Room / Hall / Plate / Shimmer | Default: Hall

Room, Hall and Plate use different delay scaling; Shimmer adds octave-up granular feedback. This is a discrete choice, not a wet level.

Manual only; no Learn

### In practice

1. For a vocal send, start with Hall, 20–40 ms Pre-delay and 1.5–2.5 s Decay.
2. Set Mix to 100% on the auxiliary return; adjust the send level in the host.
3. Try Low Cut around 120–200 Hz, then lower Tone if the return obscures consonants or cymbals.
4. Adjust Density and Diffusion for the desired build-up. Use modest Modulation before trying Shimmer.
5. Listen to the tail after the source stops; shorten Decay or reduce the send if successive phrases blur.

Learn is available for Pre-delay, Decay, Tone, Width and Low Cut only. Crest and coarse spectral features inform starting points; Width can use the reference. Size, Mix, mode, Density, Diffusion, Modulation and Mid Cut stay manual. No reference RT60 or room impulse response is inferred.

Decay is a design control, not a calibrated RT60 readout. Room, Hall and Plate change the network timing; they do not identify a particular physical venue or hardware model. Shimmer is a two-grain pitch-feedback effect whose artifacts need auditioning.

Use the Space or Colour scope to keep a change focused. Width above 100% increases the side component of the wet return; check mono. Do not judge a large reverb only in solo.

## DDelay — Stereo delay

DDelay combines fractional delay lines, a low-pass feedback path, stereo or cross-feedback routing, slow time modulation and a wet-width control. Time is entered in milliseconds. The two channels use a small timing difference; this is not a sample-identical dual-mono echo.

Input → Stereo delay ↔ Filtered feedback → Wet width → Mix → Output

### Control reference

#### Time / `time`

Range: 1 ms to 2000 ms | Default: 375 ms

Nominal left delay time in milliseconds; the right line has a small additional offset. No note-value sync is available.

Manual only; no Learn

#### Feedback / `feedback`

Range: 0 (0%) to 0.95 (95%) | Default: 0.35 (35%)

Amount returned to the delay loop. 0 means no recursive repeats; 0.95 is 95%, not a guarantee of a particular repeat count.

Manual only; no Learn

#### Filter / `filter`

Range: 200 Hz to 18000 Hz | Default: 6000 Hz

Low-pass cutoff applied to delayed/feedback content. Lower values darken the repeats.

Learn: audio-informed

#### Ping-pong / `pingpong`

Range: Stereo / Ping-pong | Default: Ping-pong

Stereo keeps feedback within each channel; Ping-pong cross-feeds the opposite channel. Both lines still receive input.

Manual only; no Learn

#### Modulation / `modulation`

Range: 0% to 100% | Default: 10%

Depth of slow delay-time modulation. Higher values create stronger motion and pitch variation.

Manual only; no Learn

#### Mix / `mix`

Range: 0% to 100% | Default: 25%

Linear dry/wet blend before Output gain. At 0%, the wet return is inaudible but the delay memory is not reset.

Manual only; no Learn

#### Width / `width`

Range: 0% to 200% | Default: 100%

Side scaling of the wet echo only. Check the mono sum after widening.

Learn: audio-informed

#### Output / `output`

Range: -24 dB to 12 dB | Default: 0 dB

Post-mix level in dB. Reduce it if input plus repeats increases peaks.

Manual only; no Learn

### In practice

1. For a tempo-related quarter-note echo, calculate Time = 60,000 / BPM in milliseconds. At 120 BPM, use 500 ms.
2. Use 250 ms for eighth notes or 375 ms for dotted eighth notes at 120 BPM. Enter the value manually.
3. Begin around 20–35% Feedback and 10–25% Mix on an insert.
4. Lower Filter for darker repeats and enable Ping-pong if alternating feedback supports the arrangement.
5. Keep Modulation small for stable pitch; allow the final repeat to decay before rendering the region end.

Only Filter and Width have Learn. Filter follows upper-band energy; Width can use reference side energy. Time, Feedback, Modulation, routing, Mix and Output are manual and are not reset by LEARN ALL. There is no tempo detection or echo-timing match.

There is no host-tempo synchronization, tap tempo or detected-BPM control in this revision. Feedback is limited, but high feedback and bright material can still create loud, persistent repeats. Soft bounding in the loop can add coloration.

Stereo keeps feedback in its own channel; Ping-pong crosses it. Both channels receive the input, so a centered mono source does not necessarily produce a hard left/right first-repeat pattern. Mix 0% silences the wet return, not its internal buffer.

## DComp — Dynamics compressor

DComp reduces level above a threshold using peak-envelope detection, adjustable ratio, attack/release and a soft knee. Makeup acts inside the processed path; Output acts after parallel mixing. The detector is not a loudness meter and the processor is not a brick-wall limiter.

Input → Peak envelope / gain curve → Makeup → Parallel mix → Output

### Control reference

#### Threshold / `threshold`

Range: -60 dB to 0 dB | Default: -18 dB

Detector level where compression begins, modified by Knee. Lower settings generally produce more reduction.

Learn: audio-informed

#### Ratio / `ratio`

Range: 1:1 to 20:1 | Default: 4:1

Compression ratio above the knee. 4:1 means an additional 4 dB at the detector maps to approximately 1 dB after the static curve.

Learn: audio-informed

#### Attack / `attack`

Range: 0.1 ms to 200 ms | Default: 15 ms

Envelope response time for rising level. Shorter values catch attacks more quickly; longer values can preserve initial impact.

Learn: audio-informed

#### Release / `release`

Range: 10 ms to 2000 ms | Default: 150 ms

Envelope recovery time as level falls. Too-short recovery may pump; too-long recovery can suppress the following phrase.

Learn: audio-informed

#### Knee / `knee`

Range: 0 dB to 24 dB | Default: 6 dB

Width of the transition around Threshold. 0 dB is a hard knee; larger values soften the onset.

Manual only; no Learn

#### Makeup / `makeup`

Range: 0 dB to 24 dB | Default: 0 dB

Gain applied to the compressed branch before Mix. It is manual, not an automatic loudness-compensation system.

Learn: audio-informed

#### Mix / `mix`

Range: 0% to 100% | Default: 100%

Blends original and compressed-plus-makeup branches. It does not change the detector threshold.

Manual only; no Learn

#### Output / `output`

Range: -24 dB to 12 dB | Default: 0 dB

Final gain after parallel blending. Use for level matching and headroom.

Manual only; no Learn

### In practice

1. For a vocal starting point, set Ratio near 3:1, Attack around 15–25 ms and Release around 100–200 ms.
2. Lower Threshold until normal phrases receive a few dB of reduction; listen to quiet and loud sections.
3. Use a wider Knee for a softer transition into compression.
4. Raise Makeup only enough for a level-matched comparison. Use Mix for parallel blending.
5. If the result pumps, revisit Release and Threshold before increasing makeup or ratio.

Threshold, Ratio, Attack, Release and Makeup have Learn; Knee, Mix and Output remain manual. Level/crest features guide dynamics; smooth extends release. Makeup uses the target/input RMS difference, not measured processed gain reduction. No output optimization or automatic loudness matching is performed.

Left and right envelopes are independent; there is no stereo-link control. Unequal channel transients may shift the stereo image. The GR readout is an implementation reading, not a certified stereo-maximum reduction meter.

At 1:1, the compression curve is neutral, but Makeup, Mix and Output can still change level. Attack and Release are smoothing times, not look-ahead or hold controls. Compare at matched loudness instead of choosing the louder version.

## DEQ — Four-band equalizer

DEQ provides four serial RBJ biquad bands. Each band has its own frequency, gain, Q and shape, and can be learned independently. The Low, Low Mid, Presence and Air panel names describe a working arrangement, not fixed frequency limits. Every band spans the declared frequency range.

Input → Band 1 → Band 2 → Band 3 → Band 4 → Output

### Control reference

#### Frequency / `b1freq`

Range: 20 Hz to 20000 Hz | Default: 100 Hz

Center frequency for Bell, transition frequency for a shelf. Each band can use the complete manual range; the panel name does not restrict it.

Manual only; no Learn

#### Gain / `b1gain`

Range: -18 dB to 18 dB | Default: 0 dB

Boost or cut for the selected shape. 0 dB is neutral. Learned gain is limited to ±6 dB, narrower than the manual range.

Learn: audio-informed

#### Q / `b1q`

Range: 0.2 to 8 | Default: 0.707

Bell bandwidth: larger Q is narrower. The current shelf implementation uses fixed slope and does not use this Q value.

Manual only; no Learn

#### Bell/Low shelf/High shelf / `b1shape`

Range: Bell / Low Shelf / High Shelf | Default: Bell

Bell, Low Shelf or High Shelf. Learning preserves this manual choice; L-SHELF/H-SHELF mean Low/High Shelf.

Manual only; no Learn

#### Frequency / `b2freq`

Range: 20 Hz to 20000 Hz | Default: 400 Hz

Center frequency for Bell, transition frequency for a shelf. Each band can use the complete manual range; the panel name does not restrict it.

Manual only; no Learn

#### Gain / `b2gain`

Range: -18 dB to 18 dB | Default: 0 dB

Boost or cut for the selected shape. 0 dB is neutral. Learned gain is limited to ±6 dB, narrower than the manual range.

Learn: audio-informed

#### Q / `b2q`

Range: 0.2 to 8 | Default: 0.707

Bell bandwidth: larger Q is narrower. The current shelf implementation uses fixed slope and does not use this Q value.

Manual only; no Learn

#### Bell/Low shelf/High shelf / `b2shape`

Range: Bell / Low Shelf / High Shelf | Default: Bell

Bell, Low Shelf or High Shelf. Learning preserves this manual choice; L-SHELF/H-SHELF mean Low/High Shelf.

Manual only; no Learn

#### Frequency / `b3freq`

Range: 20 Hz to 20000 Hz | Default: 2500 Hz

Center frequency for Bell, transition frequency for a shelf. Each band can use the complete manual range; the panel name does not restrict it.

Manual only; no Learn

#### Gain / `b3gain`

Range: -18 dB to 18 dB | Default: 0 dB

Boost or cut for the selected shape. 0 dB is neutral. Learned gain is limited to ±6 dB, narrower than the manual range.

Learn: audio-informed

#### Q / `b3q`

Range: 0.2 to 8 | Default: 0.707

Bell bandwidth: larger Q is narrower. The current shelf implementation uses fixed slope and does not use this Q value.

Manual only; no Learn

#### Bell/Low shelf/High shelf / `b3shape`

Range: Bell / Low Shelf / High Shelf | Default: Bell

Bell, Low Shelf or High Shelf. Learning preserves this manual choice; L-SHELF/H-SHELF mean Low/High Shelf.

Manual only; no Learn

#### Frequency / `b4freq`

Range: 20 Hz to 20000 Hz | Default: 10000 Hz

Center frequency for Bell, transition frequency for a shelf. Each band can use the complete manual range; the panel name does not restrict it.

Manual only; no Learn

#### Gain / `b4gain`

Range: -18 dB to 18 dB | Default: 0 dB

Boost or cut for the selected shape. 0 dB is neutral. Learned gain is limited to ±6 dB, narrower than the manual range.

Learn: audio-informed

#### Q / `b4q`

Range: 0.2 to 8 | Default: 0.707

Bell bandwidth: larger Q is narrower. The current shelf implementation uses fixed slope and does not use this Q value.

Manual only; no Learn

#### Bell/Low shelf/High shelf / `b4shape`

Range: Bell / Low Shelf / High Shelf | Default: Bell

Bell, Low Shelf or High Shelf. Learning preserves this manual choice; L-SHELF/H-SHELF mean Low/High Shelf.

Manual only; no Learn

### In practice

1. Start from all gains at 0 dB. Identify a tonal issue in the full mix before boosting.
2. Try a broad low-mid cut of 1–3 dB with Q around 0.7–1.2 if the source is muddy.
3. For a reference comparison, load comparable material and choose REFERENCE GUIDANCE.
4. Use LEARN BAND 2 or its GAIN Learn button to propose only Band 2 gain; both preserve its frequency, Q and shape.
5. Review the response and audition at matched level. Large boosts can overload later processors.

Only the four band gains have Learn, limited to ±6 dB. Normalized coarse band energy guides the gains. Band Learn changes that band’s gain only; LEARN ALL covers all four gains. Frequency, Q and shape stay at your manual settings. Find them on ADVANCED; the main page holds the four gains.

The current learner uses four coarse source/target bands; it does not detect exact resonances, masking or an inverse room response. Frequency, Q and shape are manual-only and preserved by all learning scopes. Q affects Bell only; the shelf slope is fixed.

DEQ has no global Mix, Output or band bypass parameter in this build. Use 0 dB gain for a neutral band and host bypass for the whole instance. It is not linear-phase EQ; do not assume its dry and filtered phases match in parallel routing.

## DSat — Harmonic saturation

DSat applies one of three nonlinear transfer functions, then blends direct and low-pass-filtered nonlinear sound with Density. Drive changes the level entering the nonlinear stage; Tone and Density jointly determine its brightness. Output is a separate post-mix gain control.

Input → Drive / nonlinear character → Tone-density blend → Mix → Output

### Control reference

#### Drive / `drive`

Range: 0 dB to 30 dB | Default: 6 dB

Gain into the nonlinear function. Increasing it changes both harmonics and level; it is not a transparent input trim.

Learn: audio-informed

#### Tone / `tone`

Range: 500 Hz to 18000 Hz | Default: 10000 Hz

Low-pass cutoff for the filtered nonlinear branch. Its audibility depends on Density.

Learn: audio-informed

#### Soft/Hard/Asymmetric / `mode`

Range: Soft / Hard / Asymmetric | Default: Soft

Soft uses tanh, Hard clips the driven waveform, and Asymmetric offsets a tanh curve with static offset compensation.

Manual only; no Learn

#### Density / `density`

Range: 0% to 100% | Default: 50%

Blend toward the tone-filtered nonlinear branch. This is not an independent harmonic-count or compression-density control.

Manual only; no Learn

#### Mix / `mix`

Range: 0% to 100% | Default: 50%

Parallel blend of original and nonlinear signal before Output.

Manual only; no Learn

#### Output / `output`

Range: -24 dB to 12 dB | Default: 0 dB

Post-mix gain; no automatic drive compensation is applied.

Manual only; no Learn

### In practice

1. Start with Soft, Drive around 3–6 dB and a moderate Mix.
2. Increase Drive while reducing Output to keep the comparison level consistent.
3. Use Tone with Density above 0% to darken the nonlinear component.
4. Switch to Hard only if the sharper texture supports the source; try Asymmetric for a different curve balance.
5. Recheck cymbals and upper harmonics, then use Mix for a restrained parallel result.

Only Drive and Tone have Learn: measured peak/crest and high-band energy inform their starting points. Mode, Density, Mix and Output stay manual. This is not a trained tape/tube classifier, harmonic reference match or automatic loudness match.

Oversampling is not implemented. Strong drive and bright/high-frequency sources can alias. The asymmetric curve subtracts a static offset; this is not a general-purpose DC-blocking filter. There is no automatic output-level compensation.

With Density at 0%, the tone-filtered branch is excluded, so Tone has little practical role. With Density at 100%, the wet nonlinear branch is fully low-pass filtered. Hard clipping is a sound-design option, not a mastering safety limiter.

## DTrans — Transient designer

DTrans compares fast and slow amplitude envelopes to estimate onset strength. Attack and Sustain apply gain according to that estimate; Sensitivity and Shape change the separation. Unlike compressor attack, its Attack parameter is a gain in dB, not a duration in milliseconds.

Input → Fast / slow envelope comparison → Attack-sustain gain → Mix → Output

### Control reference

#### Attack / `attack`

Range: -18 dB to 18 dB | Default: 0 dB

Gain applied where the onset estimate is strong. Positive values emphasize attacks; negative values soften them. Unit: dB.

Learn: audio-informed

#### Sustain / `sustain`

Range: -18 dB to 18 dB | Default: 0 dB

Gain weighted toward the non-onset part of the signal. Positive settings can lift room and noise as well as musical sustain.

Manual only; no Learn

#### Sensitivity / `sensitivity`

Range: 0% to 100% | Default: 50%

Scales the normalized difference between fast and slow envelopes. It is not a dB threshold or a time control.

Learn: audio-informed

#### Shape / `shape`

Range: 0.2 to 3 | Default: 1

Exponent applied to the onset weight. Higher settings concentrate attack action on stronger detected onsets.

Manual only; no Learn

#### Mix / `mix`

Range: 0% to 100% | Default: 100%

Parallel blend of original and envelope-shaped signal.

Manual only; no Learn

#### Output / `output`

Range: -24 dB to 12 dB | Default: 0 dB

Final gain after the envelope effect and parallel blend.

Manual only; no Learn

### In practice

1. For tighter drums, begin with Attack at +2 to +4 dB and Sustain at −2 to −4 dB.
2. Keep Sensitivity around the middle while comparing short and sustained hits.
3. Raise Shape if you need a more selective action on strongly detected onsets.
4. Reduce Mix if the processing sounds detached from the body of the instrument.
5. Check quiet hits and cymbal spill, then trim Output to protect downstream headroom.

Learn is available for Attack and Sensitivity using source crest factor. Sustain, Shape, Mix and Output stay manual. It is a conservative starting-point estimate, not a reconstructed reference envelope or trained drum classifier.

Sustain is an envelope-weighted gain, not a separate decay-time control or source separator. Strong positive settings can emphasize noise and room tone. Independent channel envelopes may affect stereo balance.

Start with small dB changes. On material without clear onsets, Sustain may dominate. The central envelope view reflects live signal magnitude; it is not a calibrated time-constant or transient-confidence graph.

## DStereo — Stereo designer

DStereo forms Mid = (L + R)/2 and Side = (L − R)/2, scales the two components and reconstructs stereo. Depth blends a low-passed side signal, changing side brightness rather than adding a distance or Haas delay. A vectorscope and correlation readout help inspect the result.

L/R → Mid/Side → Center / side / width / depth → Stereo or mono wet path → Mix → L/R output

### Control reference

#### Width / `width`

Range: 0% to 200% | Default: 100%

Multiplies the wet Side component. 100% is unity before Side gain; 200% doubles Side amplitude.

Learn: audio-informed

#### Depth / `depth`

Range: 0% to 100% | Default: 0%

Blends Side toward a smoothed low-pass version. Higher values darken side information rather than moving the source in a measured space.

Manual only; no Learn

#### Center / `center`

Range: -18 dB to 6 dB | Default: 0 dB

Gain of the Mid component. It is not a center-pan position or a center-extraction algorithm.

Manual only; no Learn

#### Side / `side`

Range: -18 dB to 6 dB | Default: 0 dB

Gain of the Side component, combined with Width. Both controls can increase side peaks.

Learn: audio-informed

#### Mix / `mix`

Range: 0% to 100% | Default: 100%

Blends original stereo with the processed stereo/mono branch. Dry stereo remains below 100% even with Mono enabled.

Manual only; no Learn

#### Mono monitor / `mono`

Range: Stereo / Mono | Default: Stereo

Stereo retains the wet Side; Mono sets wet Side to zero. Set Mix to 100% for a complete in-plugin mono audition.

Manual only; no Learn

### In practice

1. Begin at Width 100%, Center 0 dB, Side 0 dB and Depth 0%.
2. Try Width around 110–125% on an already-stereo source; listen to the center image.
3. Watch correlation and compare mono at Mix 100%. Negative correlation is a warning, not a complete quality score.
4. Use modest Depth if the side content feels too bright; restore Width if the mix loses focus.
5. Check on headphones and loudspeakers, and verify the host mono sum as a second check.

Only Width and Side have Learn. Side/RMS ratio and correlation guide Width; reference side ratio can set its target. Negative correlation narrows Width and reduces Side. Depth, Center, Mix and Mono remain manual. Processed-output mono compatibility must still be checked.

The Mono control sums the wet path only. For a fully mono audition inside this plugin, set Mix to 100%; at lower Mix, dry stereo remains. Identical L/R or mono input contains no side signal, so Width cannot create stereo detail from it.

Width 0% removes wet Side; Width 100% is nominal unity before the other controls; 200% doubles it. Center and Side gains can increase peaks. The vectorscope shows actual samples; a static display may simply mean there is no new audio.

## DSess — Frequency-dependent de-esser

DSess derives a high-frequency component from a low-pass split, detects its envelope and attenuates that component above Threshold. Range caps the computed reduction and Focus scales its contribution. It is not a trained speech recognizer or a surgical adjustable band-pass processor.

Input → High-frequency split / detector → Range-limited attenuation → Mix → Output

### Control reference

#### Detection frequency / `frequency`

Range: 2000 Hz to 12000 Hz | Default: 6000 Hz

Low-pass-split frequency used to derive the high-frequency detector and processed component. It is not a narrow band center.

Learn: audio-informed

#### Threshold / `threshold`

Range: -60 dB to 0 dB | Default: -24 dB

High-component envelope level above which computed attenuation rises. This is not full-band input RMS.

Learn: audio-informed

#### Range / `range`

Range: 0 dB to 18 dB | Default: 6 dB

Maximum computed high-band reduction before Focus and Mix scaling. 0 dB disables the reduction amount.

Learn: audio-informed

#### Attack / `attack`

Range: 0.1 ms to 30 ms | Default: 1 ms

Detector envelope response to increasing high-frequency energy.

Manual only; no Learn

#### Release / `release`

Range: 10 ms to 500 ms | Default: 80 ms

Recovery of the high-frequency detector envelope after the burst.

Learn: audio-informed

#### Focus / `focus`

Range: 20% to 100% | Default: 60%

Scales the high-frequency attenuation contribution. It is a strength control, not Q or detector bandwidth.

Learn: audio-informed

#### Mix / `mix`

Range: 0% to 100% | Default: 100%

Blends dry signal with the frequency-reduced branch.

Manual only; no Learn

#### Output / `output`

Range: -24 dB to 12 dB | Default: 0 dB

Final gain after the de-essing blend.

Manual only; no Learn

### In practice

1. Loop a phrase with both vowels and problematic sibilants. Start near 5–7 kHz.
2. Use Range around 3–6 dB and lower Threshold until the strongest sibilants are controlled.
3. Start around 1 ms Attack and 60–120 ms Release; listen for dullness or lisping.
4. Reduce Focus or Mix if the whole phrase loses air rather than only the offending sounds.
5. Compare bypass at the same level, including quiet breaths and consonant endings.

Frequency, Threshold, Range, Release and Focus have Learn. Upper-band energies guide detection/reduction; crest guides release and smooth extends it. Attack, Mix and Output remain manual. Learning cannot reliably separate every bright non-vocal transient from sibilance and does not match a reference vocal.

Detection Frequency is a crossover-related control, not the center of a narrow isolated sibilance band. Cymbals, breath and bright instruments may trigger it. The GR number is not an exact full-band level loss because Focus and Mix also affect the result.

There is no detector-listen button or external sidechain in this build. Work in the mix after finding the issue in solo. Independent left/right detection requires care on stereo material.

## DGate — Gate & level control

DGate opens above Threshold, holds open for a chosen time and closes smoothly toward a Range-limited floor. Hysteresis creates a lower closing threshold to reduce chatter. The current processor behaves as a gate; there is no variable expansion-ratio control.

Stereo-linked peak detection → Threshold / hysteresis / hold → Smoothed gate gain → Output

### Control reference

#### Threshold / `threshold`

Range: -80 dB to 0 dB | Default: -45 dB

Opening threshold for the stereo-linked instantaneous magnitude detector. Raising it rejects more quiet material.

Learn: audio-informed

#### Attack / `attack`

Range: 0.1 ms to 100 ms | Default: 2 ms

Smoothing time as gate gain moves toward open. Extremely short settings can sound abrupt.

Learn: audio-informed

#### Hold / `hold`

Range: 0 ms to 500 ms | Default: 50 ms

Time retained open after level drops below the closing region. It helps avoid multiple closures inside one event.

Manual only; no Learn

#### Release / `release`

Range: 5 ms to 2000 ms | Default: 150 ms

Smoothing time as gain approaches the closed floor. Longer settings preserve more of a tail.

Learn: audio-informed

#### Range / `range`

Range: 0 dB to 90 dB | Default: 60 dB

Closed-state attenuation. 0 dB means no attenuation; large values strongly suppress but do not classify wanted sound.

Manual only; no Learn

#### Hysteresis / `hysteresis`

Range: 0 dB to 18 dB | Default: 6 dB

Difference between opening and closing thresholds. Closing threshold = Threshold − Hysteresis.

Manual only; no Learn

#### Output / `output`

Range: -24 dB to 12 dB | Default: 0 dB

Gain after the gate, including when the gate is open.

Manual only; no Learn

### In practice

1. Start with a moderate Range, such as 12–24 dB, instead of maximum attenuation.
2. Set Threshold just below the quietest wanted events and above the spill you intend to suppress.
3. Try 1–5 ms Attack for percussion, then adjust Hold so one event does not reopen repeatedly.
4. Increase Release until the tail fades naturally; add Hysteresis if the gate chatters.
5. Check every quiet passage. For very exposed vocals, manual editing may be safer than aggressive gating.

Threshold, Attack and Release have Learn. Threshold uses input RMS minus 18 dB, not a measured noise floor; timing uses crest factor and smooth extends release. Range, Hold, Hysteresis and Output stay manual. Verify quiet desired sounds and tails before using the result.

A gate cannot remove noise under wanted audio. The learner does not measure a separate noise-floor distribution. Quiet words, ghost notes and tails may be cut; audition the quietest intended material before applying a suggestion.

The detector follows the larger left/right instantaneous magnitude, so either channel can open the gate. A large Range is not the same as digital silence. There is no external sidechain, key filter or look-ahead control.

## DFX — Modular multi-effects

DFX connects saturation, low-pass filtering, delay and an independent ambience network in a fixed order. Each module has its own enable control; only Saturation, Filter and Ambience have learning scopes. Delay Mix and Space Mix set local blends; the master Mix blends the whole processed chain with the original input.

Input → Saturation → Filter → Delay → Ambience → Master mix → Output

### Control reference

#### Enable saturation / `driveOn`

Range: Off / On | Default: On

Includes or bypasses the tanh saturation stage. This is manual-only; learning never changes enable decisions.

Manual only; no Learn

#### Drive / `drive`

Range: 0 dB to 24 dB | Default: 3 dB

Gain into the saturation stage when enabled. This module has no separate character or oversampling selector.

Learn: audio-informed

#### Enable filter / `filterOn`

Range: Off / On | Default: On

Enables the input low-pass stage. Disabling it does not remove the separate delay feedback filtering.

Manual only; no Learn

#### Low pass / `filter`

Range: 200 Hz to 18000 Hz | Default: 12000 Hz

Low-pass cutoff used by the filter module and delay feedback path.

Learn: audio-informed

#### Enable delay / `delayOn`

Range: Off / On | Default: On

Includes the local echo blend. Delay memory can continue running while its contribution is bypassed.

Manual only; no Learn

#### Time / `time`

Range: 1 ms to 2000 ms | Default: 250 ms

Nominal delay in milliseconds; no host-BPM sync is provided.

Manual only; no Learn

#### Feedback / `feedback`

Range: 0 (0%) to 0.9 (90%) | Default: 0.3 (30%)

Recirculation amount, limited to 0.90. High settings can sustain a loud tail.

Manual only; no Learn

#### Delay mix / `delayMix`

Range: 0% to 100% | Default: 20%

Blend between the incoming module signal and delayed signal, before ambience.

Manual only; no Learn

#### Enable ambience / `reverbOn`

Range: Off / On | Default: On

Includes the independent ambience contribution. It is not a master bypass.

Manual only; no Learn

#### Decay / `decay`

Range: 0.1 s to 8 s | Default: 1.5 s

Feedback-decay design for the fixed-character ambience tank. Other reverb controls are not exposed in DFX.

Learn: audio-informed

#### Ambience mix / `reverbMix`

Range: 0% to 100% | Default: 20%

Local dry/ambience blend following delay. The dry side here is the signal reaching the ambience module, not necessarily the original input.

Manual only; no Learn

#### Mix / `mix`

Range: 0% to 100% | Default: 50%

Global blend between original input and the complete effects chain, before Output.

Manual only; no Learn

#### Output / `output`

Range: -24 dB to 12 dB | Default: 0 dB

Final gain applied after the global blend, even at Mix 0%.

Manual only; no Learn

### In practice

1. Start with low master Mix and modest Drive; keep Output near unity while establishing the tone.
2. Enable Filter and choose a cutoff that leaves the source intelligible.
3. Set Time manually; keep Feedback and Delay Mix moderate before adding ambience.
4. Enable Ambience and use a short Decay first. Increase Space Mix only as needed.
5. Use a module Learn action for focused changes, then level-match the full chain and check its tail.

Only Drive, Filter and Decay have Learn, grouped as Saturation, Filter and Ambience. The Delay module is manual-only. Enable switches, Time, Feedback, local/global mixes and Output are preserved by every learning scope. LEARN ALL is not joint chain, loudness or reference-error optimization.

Routing cannot be reordered in this build. Enabling/bypassing a module does not necessarily clear its internal buffers. The Filter frequency also participates in the delay feedback filtering, even when the separate input-filter module is disabled. There is no additional modulation module.

At master Mix 0%, the original input remains subject to Output gain. At 100%, the result can still contain dry components from the local delay/reverb blends. Use the host bypass when you need to remove the entire instance.

## Sessions, automation & recall

### What is stored

The processor serializes parameter state through JUCE APVTS/XML for host recall. Save your DAW project after accepting settings. Roundtrip and legacy-state migration still require host validation; keep the previous binary when archiving a project. No built-in preset browser, factory preset pack, A/B memory slots or preset-file import/export is provided.

Reference audio, descriptions, proposal lists and one-level learning undo are not embedded in the processor state. Reopen the reference after closing the editor or reloading the project. Keep an authorized copy in your project asset folder if needed; a filename shown in the UI is not proof that the file was saved with the session.

### Automation discipline

Parameters are exposed to the host with stable IDs listed in the control reference. Host automation is separate from proposal undo. Learned changes notify the host, so host write/read modes can affect the outcome. Stop automation writing before learning and test the result on a duplicate session. Discrete modes should be changed with care, not swept as continuous values.

### Freeze and export

Allow reverb and delay tails at the end of an offline render. The processor reports a 30-second tail for DVerb, DDelay and DFX; this is a host hint, not a measured guarantee for every setting. Verify fades and boundaries. No extra host-compensation latency is explicitly declared; audible pre-delay/echo timing is intentional, not the same as plugin delay compensation.

## Troubleshooting & frequently asked questions

### Plugin missing from the host

Check OS/CPU architecture, format and scan folder. Confirm that a real plugin binary—not the design ZIP or source folder—was installed. Rescan with the host closed/reopened as its manual recommends. AAX-only hosts need a supported AAX product, not these VST3/AU targets. Do not rename a bundle’s internal executable.

### No signal or no learning result

Check routing, track mute, host bypass, device selection, output gain and whether playback actually reaches the instance. Reduce gate threshold/range if wanted audio is being closed. For Learn, keep playback running through capture, avoid silence and choose a valid mode/reference. A label change alone is not a valid proposal.

### Reference rejected or lost

Try a readable local PCM WAV or supported AIFF/FLAC, a valid sample rate and a non-silent beginning. Convert only from audio you are authorized to use. The reader analyzes the first 30 seconds, not a selected later region. Reloading is required after editor/project recall because references are not serialized.

### Suggestion disappeared or cannot be applied

Manual/host edits can invalidate proposals. Restart learning after finishing those edits. Avoid overlapping analysis requests; a busy or timed-out capture may need audio playback to clear. UNDO affects one applied learning operation and does not recover an entire host edit history.

### Clicks, overload or unstable loudness

Lower monitoring and output gain. Check Drive, EQ boosts, makeup and feedback. Compare matched levels, leave headroom and use a separate safety limiter when appropriate. Increase the host buffer if overloaded; reduce unnecessary high sample rates or effect instances. Continuous parameter smoothing does not guarantee artifact-free mode switches or extreme automation.

### Why does mono still sound stereo?

In DStereo, Mono affects the processed branch. Set Mix to 100% or use the host mono monitor for a complete check. Width cannot synthesize Side from identical channels. Negative correlation calls for listening and diagnosis, not simply increasing Center or trusting a number.

### Does “AI” mean a trained model?

Not in this implementation. The shipped path uses deterministic feature analysis and heuristic objectives. No model weights, remote inference or neural matching service is enabled. An abstract future model interface is not a trained product feature. Read the proposal explanation and the limitation notes for the specific parameter.

## Technical reference & known limits

### Audio architecture

The JUCE AudioBuffer path processes float samples, with double-state EQ biquads. Layouts are matching mono or stereo; MIDI input/output and external sidechain buses are not implemented. The core clamps its preparation sample rate to 8–384 kHz; this implementation bound is not a complete tested host-rate certification. Some filter cutoffs are limited below Nyquist internally, so a high displayed setting is not always the effective cutoff at a low rate. Choose the rate appropriate to your project.

Continuous values are smoothed with approximately 20 ms coefficient smoothing in the core; discrete choices are immediate. Delay/FDN buffers are allocated during preparation. Analysis and reference decoding are worker tasks, not work intentionally placed in the audio callback. Visualization uses a bounded single-producer/single-consumer queue and may drop display frames without dropping audio by design.

### Practical limitations

No oversampling, look-ahead, true-peak limiter, tempo detection/sync, arbitrary convolution, general language interpretation or iterative output-reference optimizer is provided. DEQ is not linear phase; DSess is not a full parametric dynamic EQ; DGate has no expansion ratio; DFX order is fixed. DSP quality and automation must still be auditioned on real material.

CPU and memory depend on sample rate, platform, instance count and settings. No measured minimum RAM/CPU or certified buffer-size guarantee is published. Internal finite-value safeguards and output bounding are not transparent overload prevention. Use host meters and sensible input levels.

### Verification status

Portable DSP/learning tests and layout/FIFO checks exist; they are not listening tests or blanket DAW certification. A previous checkpoint passed Linux/Windows/macOS CI builds; the current UI has its own pending native snapshot/host validation. Consult the repository checks and STATUS.md for evidence tied to the exact commit. This manual does not turn development code into a release certification.

## Glossary

**Attack** — Envelope rise response; in DTrans, attack gain rather than time.

**Auxiliary / send** — A parallel host bus receiving a chosen amount of signal.

**Crest factor** — Ratio of peak to RMS, commonly expressed in dB.

**dBFS** — Digital level relative to nominal full scale; not a loudness unit.

**Dry / wet** — Original signal versus the processed branch.

**Feedback** — Returning part of a delayed or reverberant signal to its input path.

**FDN** — Interconnected feedback delay lines used for algorithmic ambience.

**Gain reduction / GR** — Attenuation computed by a dynamics process.

**Headroom** — Margin between current peaks and an intended overload boundary.

**Hysteresis** — Different opening and closing thresholds to reduce rapid switching.

**Knee** — Transition region between uncompressed and compressed behavior.

**Mid / Side** — Sum and difference components of a stereo signal.

**Mono compatibility** — How the signal behaves when channels are summed.

**PDC** — Host compensation for reported processing latency.

**Pre-delay** — Gap before the onset of a time-effect return.

**Q** — Bell-filter selectivity; higher Q means narrower bandwidth.

**Release** — Recovery or closing smoothing after a level event.

**RMS** — A signal-energy amplitude measure, not LUFS loudness.

**RT60** — A measured 60 dB decay time; not a certified DVerb readout.

**Sibilance** — Strong high-frequency speech energy such as s/sh sounds.

**Stereo correlation** — Relationship between channel waveforms; negative values merit mono checks.

**True peak** — Estimate of reconstructed peaks between samples; not measured by these meters.

## Support, privacy & legal notices

### Report a reproducible problem

Use the repository issue tracker or the support channel supplied with your build. Include plugin name, build/commit, format, OS/CPU, DAW version, sample rate, buffer size, mono/stereo layout, steps, expected/actual behavior and whether host bypass changes it. Include crash logs only after checking them for private paths or identifiers.

Do not publish private sessions, licensed reference music, credentials or personal data. Prefer a short, authorized synthetic or anonymized test file and a minimal project. An issue tracker is not a published service-level agreement, response-time promise or warranty.

### Local processing and rights

The described code decodes references and analyzes input locally; no remote learning/inference upload path is implemented. This statement is not an independent privacy/security audit or a policy for your DAW, OS, cloud backup or third-party plugins. Respect source/reference licensing and do not assume reference analysis grants copying or distribution rights.

### Licensing and document scope

The repository notice identifies prototype source for evaluation and further development. This guide does not replace a software license, provide commercial distribution rights or invent a refund/warranty policy. Review JUCE licensing separately before distribution. Bundled fonts carry their own SIL Open Font License notices.

D.A Plugins branding and supplied artwork identify this project. Product/host names mentioned for compatibility remain the names of their respective owners; no third-party endorsement is implied. This is original documentation, not a copy of another manufacturer’s manual. English and Persian editions describe the same development feature set.
