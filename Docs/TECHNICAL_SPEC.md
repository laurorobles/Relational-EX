# Relational EX — Technical Specification

**Version:** 0.8.0 · **License:** AGPL-3.0

## Product metadata

- **Product / Plugin name:** `Relational EX`
- **Company:** `Extasis Records` (*coded by @laurorobles*)
- **Bundle ID:** `com.extasisrecords.relationalex`
- **Manufacturer code:** `LAOR`
- **Plugin code:** `RLX2`
- **Formats:** VST3, Standalone
- **Platforms:** macOS 11+ (Universal: `arm64` + `x86_64`), Windows 10/11 (`x64`), Linux (`x86_64`)
- **Toolchain:** C++17 + CMake ≥ 3.22 + JUCE 9.0.1

## Realtime safety

The audio thread (`processBlock`) never allocates heap memory, acquires locks, touches the filesystem, or mutates UI-owned containers:
- **Tuning:** immutable 128-note `std::shared_ptr<const TuningSnapshot>` published via atomic store/load.
- **Sequencer & Relation Matrix:** double-buffered snapshots swapped atomically from the UI thread.
- **On-screen / computer keyboard:** note-on/off events are pushed into a lock-free `juce::AbstractFifo` on the UI thread and drained at the start of `processBlock`.
- **Scale quantization:** zero-allocation `constexpr` scale degree tables.

## State serialization

- `juce::AudioProcessorValueTreeState` (APVTS) stores all host-automatable parameters with stable IDs (see [PARAMETER_REFERENCE.md](PARAMETER_REFERENCE.md)).
- Child `ValueTree` nodes persist custom tuning points (`TUNING`), the 32-event sequence and independent cycle lengths (`SEQUENCE`), and the 5×5 coupling matrix (`RELATIONS`).

## Tuning interoperability & priority

1. **MTS-ESP master** (loaded dynamically at runtime via `MTSClientBridge`: `libMTS.dylib` on macOS, `libMTS.dll` on Windows, `libMTS.so` on Linux).
2. **MIDI Tuning Standard (MTS)** SysEx table when received locally.
3. **Internal Relational tuning** (1–53 EDO, Frequency Field edits, or locale-independent Scala `.scl` files).

Optional **Scale Quantization** (`scaleType` + `rootNote`) snaps sequencer pitches and diatonic relational tuning shifts to 10 musical scales while leaving `OFF (FREE)` completely unquantized.
