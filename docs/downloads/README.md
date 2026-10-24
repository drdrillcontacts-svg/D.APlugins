# D.A Plugins 0.4 development package

- [Download buildable source + DVerb before/after listening WAVs](DAPlugins_0.4_Development.zip)
- [Checksum](SHA256SUMS.txt)
- [Separate Persian/English PDF manuals ZIP](../manual/pdf/DA_Plugins_Manuals_EN_FA.zip)
- [Compiled platform artifacts / CI](https://github.com/drdrillcontacts-svg/D.APlugins/actions/runs/39982254703)

The development ZIP is **not a plugin installer** or commercial release certification. It contains
all ten plugin sources, complete 500-state factory bank manifest, required build-time UI assets,
portable tests, build instructions and an original synthetic 48 kHz/24-bit stereo listening aid.
Both wet comparison files use identical source/explicit controls and are independently RMS-matched
at −27 dBFS. They do not establish subjective sound quality; audition real material too.
The old/new raw RMS differs by ~3.88 dB, which is compensated in the supplied wet files.

Code checkpoint: **1aaf67d9d271** (printed in the PDFs and source package). Its Release and ASan/UBSan
suites pass 6/6 each. The ZIP was extracted into a clean directory, compiled with plugins disabled,
and passed 6/6 again. The actual ten-editor/program/state/tempo/Linux native CI runner also passed.
Full Windows/macOS/Linux format builds are tracked separately in the run above. Download artifacts
only once their jobs complete successfully; GitHub may require sign-in, and CI artifacts expire.

The PDFs are in their own ZIP (63 English pages, 63 Persian, 126 combined). They include all 92
controls, 33 Learn / 59 manual policy, complete 50-name product catalogs and Sync/compatibility notes.
Neither ZIP includes commercial-use clearance, signing, notarization or a trained neural model.
