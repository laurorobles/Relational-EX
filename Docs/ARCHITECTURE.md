# Relational EX — Architecture

## Layers

```text
Host DAW (VST3) / Standalone App (macOS · Windows · Linux)
        ↓
PluginProcessor / PluginEditor
        ↓
APVTS Parameters + ValueTree State (Tuning, Sequence, Relations)
        ↓
Lock-Free Bridges (Atomic Snapshots + Double Buffers + AbstractFifo)
        ↓
Sequencer Cycles / Relational Matrix / Scale & Tuning Resolution
        ↓
8 Voices (FM + 6-Mode Resonator + ADSR + SVF TPT Filter)
        ↓
Soft-Clipped Stereo / Mono-Safe Output
```

## Core realtime rules

Realtime DSP (`processBlock`) never depends on UI-owned mutable containers, locks, heap allocations, or filesystem I/O:
- **Tuning:** UI edits write into `tuningEditBuffer_`, update `TuningSystem`, and publish an immutable 128-note `TuningSnapshot` via `std::atomic_store_explicit`.
- **Sequencer & Relations:** UI edits modify staging structures and publish complete snapshots to the audio thread.
- **Test keyboard:** UI mouse/key events enqueue `TestEvent` structs into a 64-slot `juce::AbstractFifo` consumed on the audio thread.

## Cross-platform isolation

- `Source/Core/` is pure standard C++17 (no JUCE headers), keeping DSP, Scala/KBM parsing (classic `"C"` locale), tuning math, and relational sequencing portable across Clang, GCC, and MSVC.
- `MTSClientBridge` dynamically loads the optional MTS-ESP shared library per OS (`dlopen` on macOS/Linux, `LoadLibraryA` on Windows) and falls back cleanly when absent.
- `ScaleCatalog` scans bundled `Resources/Scales` and user application data directories using UTF-8 paths (`u8path` / `u8string`).
