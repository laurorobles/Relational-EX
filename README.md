# Relational EX

**An experimental FM instrument where pitch, rhythm, spectrum and tuning are coupled to each other.**

`v0.8.0 (alpha)` · macOS · Windows · Linux (VST3 + Standalone) · Open source under **AGPL-3.0**

*Extasis Records — coded by [@laurorobles](https://github.com/laurorobles)*

Relational EX is not a general-purpose synth workstation. It explores one idea: instead of
treating pitch, time, FM, resonance and tuning as independent knobs, they are connected by a
signed 5×5 **relation matrix**, driven by five independent, non-aligned **cycles**
(Rhythm / Pitch / FM / Tuning / Motion). The result is slowly evolving, polymetric material that
never quite repeats.

## Features

- **8-voice FM** with continuous ratio, index and feedback, plus a six-mode inharmonic resonator
- **Relational Engine**: editable 5×5 coupling matrix between Pitch, Time, FM, Resonance, Tuning
- **Relational Sequencer**: up to 32 events, Euclidean generator, per-event pitch / ratio /
  velocity / probability / FM index / tuning offset / timing offset; Field, Grid and Radial views
- **Five independent cycles** (1–32) that phase against each other
- **Scale quantization (optional)**: 10 scales + root note, or `OFF (FREE)` for microtonal freedom
- **Mono legato + glide** mode, or 8-voice polyphony
- **ADSR, low-pass filter**, LFO with selectable target (Relations / FM Ratio / FM Index / Cutoff), deterministic chaos
- **Tuning**: 1–53 EDO, Scala `.scl` files, MIDI Tuning Standard SysEx, MTS-ESP client (loaded at runtime if installed)
- **MPE-style per-channel pitch bend**
- Factory presets, full state recall, tooltips on every control

## Install

Download the zip for your system from the [Releases](../../releases) page and copy:

| OS | VST3 → | Standalone |
|---|---|---|
| macOS (Intel + Apple Silicon) | `~/Library/Audio/Plug-Ins/VST3/` | `Relational EX.app` → `/Applications/` |
| Windows 10/11 x64 | `C:\Program Files\Common Files\VST3\` | `Standalone\Relational EX.exe` (keep the `Resources` folder beside it) |
| Linux x86_64 | `~/.vst3/` | `Standalone/Relational EX` (keep `Resources/` beside it) |

Builds are **not code-signed**:
- macOS: right-click → Open, or `xattr -dr com.apple.quarantine "Relational EX.vst3"`.
- Windows: SmartScreen may warn → *More info → Run anyway*.
- Linux needs ALSA/JACK and X11 (standard on desktop distros).

Verify downloads with the `.sha256` file published next to each zip.

## Build from source

Requirements: CMake ≥ 3.22 and a C++17 compiler (Xcode CLT / Visual Studio 2022 / GCC ≥ 9). Linux also needs the JUCE system packages listed in the [build guide](Docs/BUILD_GUIDE.md). JUCE 9.0.1 is fetched automatically;
to use a local checkout pass `-DJUCE_DIR=/path/to/JUCE` (or place it at `~/JUCE`).

```bash
./scripts/build.sh            # configure + build Release
./scripts/build.sh --install  # macOS: also copy VST3 to ~/Library/Audio/Plug-Ins/VST3/Extasis Records
```

Details: [Docs/BUILD_GUIDE.md](Docs/BUILD_GUIDE.md).

## Documentation

| | |
|---|---|
| [Manual](Docs/MANUAL.md) | How to play and what every control does |
| [Parameter reference](Docs/PARAMETER_REFERENCE.md) | IDs, ranges, defaults |
| [Architecture](Docs/ARCHITECTURE.md) · [DSP model](Docs/DSP_MODEL.md) · [Relational engine](Docs/RELATIONAL_ENGINE.md) | How it works |
| [Scope](Docs/SCOPE.md) · [Roadmap](Docs/ROADMAP.md) | What this project is (and isn't) |
| [Changelog](CHANGELOG.md) · [Contributing](CONTRIBUTING.md) · [Third-party notices](Docs/THIRD_PARTY_NOTICES.md) | Project meta |

## License

Relational EX is free software released under the **GNU Affero General Public License v3.0**
(see [LICENSE](LICENSE)). It is built with [JUCE](https://juce.com), used under its AGPLv3 option.
If you redistribute modified binaries you must publish the corresponding source under the same terms.

© 2026 Lauro Robles / Extasis Records.
