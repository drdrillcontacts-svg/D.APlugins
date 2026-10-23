# D.A Plugins — bilingual user manual

Original user documentation for the complete ten-effect development bundle. Manual revision **1.2**,
product development series **0.4**, dated **23 October 2026**. This documents a source checkpoint,
not a certified commercial release. The exact source snapshot is printed in each PDF.

## Download / read

- [Separate manuals ZIP — English, Persian and combined PDFs](pdf/DA_Plugins_Manuals_EN_FA.zip)

- [English PDF — 63 pages](pdf/DA_Plugins_User_Manual_EN.pdf)
- [Persian PDF — 63 pages](pdf/DA_Plugins_User_Manual_FA.pdf)
- [English + Persian PDF — 126 pages](pdf/DA_Plugins_User_Manual_EN_FA.pdf)
- [SHA-256 checksums](pdf/SHA256SUMS.txt)
- Text editions: [English](Manual_EN.md) · [فارسی](Manual_FA.md)

The combined edition places English first and Persian second. Printed page numbers restart for
both languages; language-root bookmarks identify each edition. The PDFs include linked contents,
hierarchical bookmarks, control locator links, embedded fonts and shaped right-to-left Persian.
The text exports contain the editorial and parameter reference material; PDF-only front matter,
installation-path supplement, pictures and interactive navigation are added by the builder.

## Coverage and editorial boundaries

22 chapters cover installation/build/host setup, safe routing, interface and metering, capture and
reference files, learning scopes and reversible approval, all ten effects, all **92** controls with
source-derived ranges/defaults, practical starting recipes, automation/session recall,
troubleshooting, technical limitations, glossary, privacy, licensing and support.

The 0.4 edition adds 50 named complete factory presets per plugin, program/project recall, DDelay host-tempo sync with 15 divisions, and revised DVerb/DFX diffusion/decay. Each product has a full 50-name catalog. The Rack Collection 02 edition includes both main and Advanced illustrations for every product,
33 Learn-eligible controls, 59 explicitly manual controls and the opt-in DETAILS / LOG workflow.

Parameter ranges/defaults come directly from `Design/UI/Parameters.json`. Behavioral explanations
were checked against `Source/Core/Suite.cpp`, processor/editor code and the project status notes.
The manual distinguishes eligible audio-informed starting points from deliberately manual controls.
Those manual values, including enable states, are preserved by every learning scope. It does not invent neural models, certified hosts, commercial installers, activation,
undocumented features, licensing grants or support guarantees. Commercial vendors' manuals were
not copied. The named controls and their actual implementation take precedence over conventions
from other vendors' products.

Pictures are explicitly labelled **design studies, not native screenshots**. Plugin/host tests
and release qualification remain separate from documentation validation.

## Edit and rebuild

Python 3.10+ and a virtual environment are recommended. From the repository root:

```sh
python3 -m venv .venv
.venv/bin/python -m pip install -r docs/manual/requirements.txt
.venv/bin/python docs/manual/build_manual.py
.venv/bin/python docs/manual/validate_manual.py
```

On Windows use `.venv\Scripts\python.exe` instead. Keep the virtual environment outside
version control (or use the already ignored `.venv/` directory). Dependency installation requires
network access; PDF generation itself is offline.

- Edit `content.py` for bilingual editorial copy, explanations and recipes.
- Edit `build_manual.py` for layout, front matter, navigation, supplements and export logic.
- Update the authoritative parameter metadata only when changing the actual product, not to make
  a documentation-only correction.
- The build reads the current Git HEAD for its source snapshot and writes the three PDFs,
  checksums, both Markdown exports and the separate manuals-only ZIP. The document date/revision are explicit editorial values
  in the builder; update them when issuing a new manual revision.
- `build-manual/` is an ignored cache of font instances, optimized illustrations and page manifests.
  It can be removed and regenerated. Original PNG/SVG studies remain in `Design/UI-Previews/`.

Validation checks both editions for 92 parameter rows, 22 top-level chapters, valid navigation,
embedded fonts, replacement characters, accidental near-empty pages, sheet-boundary overflow,
selected implementation caveats, the 33/59 learning split, merged page/outline counts and checksum integrity.
The builder also rejects unmapped editorial glyphs and substitutes ASCII signal-path arrows for
Unicode arrows absent from Vazirmatn. It is a
structural/content smoke test, not proof of absence of every editorial or visual defect. Rendered
samples of covers, contents and English/Persian control pages were also reviewed visually. Any
future font/layout edits should receive a fresh visual RTL and pagination review.

## Fonts and distribution

Vazirmatn is bundled under its [SIL Open Font License](fonts/OFL.txt). Rajdhani branding fonts and
their OFL are in `Design/Fonts/`. PDF font subsets are embedded. Original illustrations and the
DA mark are repository assets; font licenses do not license those assets or the plugin software.
The repository's `LICENSE-NOTICE.txt` and separate JUCE obligations still apply. PDF generation
is not authorization to publish a software release.

The separate manuals ZIP is a documentation deliverable, not a plugin installer. It contains all
three PDFs and their checksums. The gallery server exposes it and the PDFs under an exact
`/downloads/` allowlist; it does not serve the repository root or `.git`.
