# Relational EX

Relational EX is an experimental software instrument for exploring non-standard relationships between **frequency, rhythm, spectrum and motion**.

The public plugin identity is **Relational EX**. The final product identity can still evolve, but the repository and plugin metadata use this name consistently.

## Status

**Development / alpha — v0.3.2-dev.** The project is actively evolving. The architecture is intended to become a production-quality macOS VST3 + Standalone instrument, but the current branch is not a release build.

## Formats

- VST3
- Standalone
- macOS ARM64 CI target

GitHub provides standard hosted ARM64 macOS runners including `macos-14`, `macos-15`, and `macos-latest`; the repository CI uses `macos-14` for a stable Apple Silicon target.

## Core concept

Relational EX does not treat microtuning as a cosmetic note-scale feature. Frequency relationships are part of the synthesis engine, and the sequencer stores events as relational objects containing pitch, timing, ratio, probability, velocity, FM behavior and tuning offsets.

The same sequence can be inspected through multiple representations:

- **FIELD** — continuous event space
- **GRID** — temporal/event matrix
- **RADIAL** — cyclic phase geometry

## Current systems

### Synthesis

- 8-voice polyphonic FM engine
- continuous carrier/modulator ratios
- FM index and feedback
- six-mode inharmonic resonator
- deterministic chaos modulation core
- LFO modulation core
- tuning drift
- 5-domain Relational Engine coupling pitch, time, FM, resonance and tuning

### Tuning

- 1–53 TET
- Scala `.scl`
- initial KBM parser
- MIDI Tuning Standard parsing
- MTS-ESP client interoperability layer
- continuous MTS-ESP retuning path
- internal tuning snapshot for realtime-safe access

MTS-ESP integration follows the official client model: register a client, query note frequencies using MIDI note/channel, optionally filter unmapped notes, parse incoming MTS SysEx, and deregister on shutdown.

### Sequencing

- up to 32 events
- Euclidean pattern generation
- per-event pitch
- ratio
- velocity
- probability
- FM index
- tuning offset
- timing offset
- interactive Field / Grid / Radial views
- double-buffered realtime snapshot

### Presets

Factory presets are intentionally exploratory starting hypotheses rather than generic genre presets. The current set includes:

`PRIME LATTICE`, `FRACTURED GLASS`, `PHASE RAIN`, `RATIONAL GHOST`, `TIDAL METAL`, `PAPER CIRCUIT`, `NULL MACHINE`, `ALIEN MARIMBA`, `MICRO POLAROID`, `BROKEN GENERATOR` and others.

## Build

Requirements:

- macOS
- Xcode Command Line Tools
- CMake 3.22+
- JUCE checkout at `~/JUCE` by default

```bash
./PREBUILD_CHECK.sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release --parallel "$(sysctl -n hw.logicalcpu)"
./build_and_install.sh
```

The intended user VST3 location is:

```text
~/Library/Audio/Plug-Ins/VST3/Relational EX.vst3
```

## GitHub synchronization

The repository includes a safe sync script that never force-pushes:

```bash
./BUILD_RELATIONAL_EX.sh
./SYNC_GITHUB.sh
```

The default remote is:

```text
git@github.com:laurorobles/Relational-EX.git
```

If the remote history has diverged, the script stops rather than overwriting commits.

## Documentation

See `Docs/` for:

- technical specification
- user manual
- parameter reference
- DSP model
- architecture
- UI specification
- research notes
- references and bibliography
- database plan
- implementation status
- build status
- test plan
- release checklist
- decision log
- third-party notices
- repository metadata

## Relational Engine

The current engine uses a compact 5×5 relationship model rather than a large generic modulation matrix. Its purpose is to make existing domains interact: tuning can influence pitch/spectrum, time can influence spectral state, and resonance can feed back into FM behavior. The user-facing matrix editor is planned, but the coupling model already exists in the audio path. See `Docs/RELATIONAL_ENGINE.md`.

## Scope protection

Relational EX is not intended to become a general-purpose subtractive synthesizer or a copy of Wilsonic, Buchla hardware, Autechre, Aphex Twin or any other existing instrument. Those are references for concepts and techniques, not a specification to imitate.
