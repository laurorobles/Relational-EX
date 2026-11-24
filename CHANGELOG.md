# Changelog

All notable changes to Relational EX. Format based on [Keep a Changelog](https://keepachangelog.com/);
versions follow [SemVer](https://semver.org/) (pre-1.0: minor bumps may change behavior).

## [0.8.0] — 2026-11-23

First public release.

### Added
- **Windows and Linux builds** (VST3 + Standalone) alongside macOS (universal arm64 + x86_64), built and packaged by CI.
- **Scale quantization** (optional): 10 scales (chromatic, major, minor, harmonic minor, dorian,
  phrygian, lydian, mixolydian, pentatonic major/minor) + root note. `OFF (FREE)` keeps full microtonal behavior.
  Applies to sequencer pitches and to relational tuning drift (diatonic stepping).
- **Mono legato mode** with exponential glide (`glideTime`); legato transitions do not retrigger the envelope, FM phase or resonator.
- **ADSR envelope** and **low-pass filter** (`filterCutoff`).
- **LFO routing**: Relations (default), FM Ratio, FM Index or Cutoff.
- Tooltips on every control, including detailed sequencer help.
- Discreet credit in the UI. All-notes-off / all-sound-off MIDI handling.
- `LICENSE` (AGPL-3.0), `scripts/build.sh`, `scripts/package_release.sh`.
- Bundled scales are now found in every platform layout; MTS-ESP library lookup for Windows/Linux.

### Changed
- Sequencer: dragging events horizontally no longer resets them; only vertical value matters.
- UI layout reorganized so the keyboard never overlaps the sequencer; FM controls grouped under the Frequency Field.
- CMake fetches JUCE 9.0.1 automatically when no local JUCE is found; plugin copy-after-build is now the
  option `RELATIONAL_COPY_PLUGIN` (default ON on macOS, OFF in CI).
- Scale snapping uses constant tables (no heap access on the audio thread); block-constant parameters are
  read once per block instead of per sample/per voice.

### Fixed
- Glide had no audible effect: voice frequency was overwritten with the target at every block.
- Per-event tuning offsets (cents) from the sequencer were lost after the first audio block.
- **Thread safety:** on-screen / computer-keyboard notes were triggering voices from the UI thread. They are now
  queued through a lock-free FIFO and consumed on the audio thread.
- **Cross-platform & host safety:** mono/multi-channel output buffer safety, locale-independent Scala/KBM decimal parsing, and UTF-8 file path handling on Windows/Linux/macOS.

### Removed
- Obsolete versioned status docs, installer/sync helper scripts, build artefacts previously tracked in git,
  and the unused MTS-ESP placeholder sources (MTS-ESP is loaded dynamically at runtime).

## 0.7.x — internal
Relational phase, five automatable cycles, phase-aware modulation, sequencer interaction fixes.

## 0.4–0.6 — internal
Relational Engine matrix, independent cycles, factory presets, Scala/MTS tuning, UI iterations.
