# Relational EX — Roadmap

## Done (≤ v0.8.0)
- JUCE/CMake project, VST3 + Standalone, GitHub CI
- Editable 5×5 relation matrix with persistence and realtime-safe snapshot
- Five independent relational cycles, relational phase, 32-event sequencer (Field/Grid/Radial)
- Scala / MTS SysEx tuning, 1–53 EDO, runtime MTS-ESP client
- ADSR, filter, LFO routing, mono legato + glide, optional scale quantization
- Host transport start sync, MPE-style per-channel pitch bend, tooltips

## Next (toward 1.0)
- Validation: pluginval at high strictness, Intel + Apple Silicon (universal binary)
- Code signing + notarization, installer
- MTS-ESP validation against the official library; KBM integration; continuous retuning option
- State migration between versions
- Windows / Linux builds (contributions welcome)
- Unit tests for sequencer, scale snapping and tuning

## Ideas (must respect [SCOPE.md](SCOPE.md))
- Spectral morphing and partial-bank experiments
- Transformation operators and reversible randomization for the sequencer
- Event trajectory / duration model
- Host-synced sequencer position (PPQ-locked)
