# D.A Plugins

**LEARN THE SOUND. RECREATE THE FEEL.**

Development checkpoint for ten JUCE audio effects. **This is not a completed or release-ready bundle.**
The original UI assets remain in `Design/UI-Mockups/`. Runtime controls are JUCE components, not PNGs.

## Rack Collection 02 — new UI and selective learning

The user’s new full-bundle rack reference and original logo now guide all ten product-specific layouts.
Open **[the ten-design gallery](Design/UI-Previews/index.html)** for high-resolution PNG/SVG studies,
or run `python tools/serve_designs.py` for the local viewer. These are explicitly design previews, not
native screenshots. The actual JUCE editor uses custom controls, attached parameters, live signal
views, product-specific main controls and an Advanced page, rather than a screenshot. All 90
parameters remain; only 33 audio-informed controls have Learn. Details / Log is opt-in and never
opens automatically after learning. Manual/mix/output/routing controls are preserved by Learn All.

See **[UI design / implementation / reproduction notes](docs/UI-DESIGN.md)** for details and the native
snapshot test runner. Full plugin/host validation of this new UI revision remains pending.

## User manuals / راهنمای کاربر

Comprehensive documentation for all ten effects and all 90 controls:
[English PDF (51 pages)](docs/manual/pdf/DA_Plugins_User_Manual_EN.pdf) ·
[فارسی PDF (۵۱ صفحه)](docs/manual/pdf/DA_Plugins_User_Manual_FA.pdf) ·
[Combined bilingual PDF (102 pages)](docs/manual/pdf/DA_Plugins_User_Manual_EN_FA.pdf).

Includes setup, practical recipes, Learn/Reference workflows, automation, troubleshooting and
implementation limitations. These document the development build, not a certified release.
See [manual sources, checksums and rebuild instructions](docs/manual/README.md).

## Current implementation

- Separate DSP paths for DVerb, DDelay, DComp, DEQ, DSat, DTrans, DStereo, DSess, DGate and DFX.
- Shared metadata for 90 parameters; 33 eligible parameter, band/module and all-eligible scopes.
- Deterministic audio-feature estimates, with explicit uncertainty. **No trained neural model.**
- Reversible proposal lifecycle and stale-result protection.
- JUCE wiring for attached controls, input capture, asynchronous estimation/reference decoding,
  reference/description mode selection, approval controls and real peak/GR readings.
- Portable tests; plugin-format builds and host behavior still need verification.

Full acceptance gaps, per-plugin status, validated commands and next steps: [docs/STATUS.md](docs/STATUS.md).
Algorithms, policy-based versus audio-derived suggestions, and limitations: [docs/LEARNING.md](docs/LEARNING.md).

## Portable build and tests (no JUCE/system audio dependencies)

Requires CMake 3.22+ and a C++17 compiler:

```sh
cmake -S . -B build-core -DDA_BUILD_PLUGINS=OFF -DCMAKE_BUILD_TYPE=Release
cmake --build build-core --parallel 2
ctest --test-dir build-core --output-on-failure
```

## Plugin builds

JUCE 8.0.6 is fetched from an immutable, SHA-256 verified source archive. Internet access is required
on first configuration. Review JUCE's licensing terms before distribution; this repository does not
grant a JUCE commercial license. Existing `LICENSE-NOTICE.txt` is preserved.

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release --parallel 2
ctest --test-dir build -C Release --output-on-failure
```

Windows: Visual Studio 2022 C++ desktop tools and Windows SDK. macOS: Xcode command-line tools.
Linux: GCC/Clang, pkg-config and JUCE development dependencies, e.g. on Ubuntu:

```sh
sudo apt-get install build-essential cmake pkg-config libasound2-dev libjack-jackd2-dev \
  libfreetype6-dev libfontconfig1-dev libx11-dev libxcomposite-dev libxcursor-dev \
  libxext-dev libxinerama-dev libxrandr-dev libxrender-dev libgl1-mesa-dev
```

All ten targets request VST3 and Standalone; AU is requested only on macOS. IDs are distinct four-character
codes. Build products go under `build/<Plugin>_artefacts/Release/`. CI is configured for Linux, Windows and
macOS; configuration is not evidence of a successful run. No installer, signing or notarization yet.

## Development audition workflow (host integration unverified)

1. Play non-silent input. Choose **Input-aware estimate**, **Reference tonal/image guide**, or
   **Description-guided estimate**. Recognized adjectives: dark, warm, wide, smooth, cinematic.
2. Press a parameter Learn button, a band/module button, or Learn all. Recording collects 65,536 input
   samples; decoding/analysis/estimation runs off the audio callback. Capture times out in the UI after 15 s.
3. Inspect current/proposed values, reason and heuristic confidence score. Parameters are not changed yet.
4. Preview to audition; Apply to retain; Reject to restore the pre-preview values; Undo restores one
   previous learning operation. A manual/host change invalidates old proposals. Closing the editor
   restores an unaccepted preview. DAW automation and simultaneous edits still need stress testing.
5. Reference WAV/AIFF/FLAC support depends on JUCE's registered basic decoders. Only the first 30 seconds
   are analyzed; references are not copied into plugin state. Input-only analysis is never an exact match.

Do not use this checkpoint on irreplaceable sessions without backups.
