# RELATIONAL

A relational microtonal synthesis environment for VST3 and Standalone. The working name remains intentionally temporary.

## v0.1.9 build-hardening candidate

This build is focused on a clean JUCE/CMake runtime baseline before expanding the DSP.

- VST3 + Standalone
- automatic JUCE discovery at `$HOME/JUCE`
- generated `JuceHeader.h`
- explicit user VST3 installation at `~/Library/Audio/Plug-Ins/VST3`
- 2-operator FM prototype
- Scala `.scl` parser
- MIDI Tuning Standard foundation
- optional MTS-ESP client bridge using the official API signatures
- interactive Frequency Field prototype
- interactive Relational Sequencer prototype
- English hover help
- safe MIDI Note On/Off voice gate
- prebuild validation and install script

## Build

```bash
./PREBUILD_CHECK.sh
./build_and_install.sh
```

The install script validates the generated VST3 and copies it to:

`~/Library/Audio/Plug-Ins/VST3/Relational.vst3`

## UI design direction

The interface is organized as functional framed modules with deliberate spacing and no decorative dead zones. Its personality comes from data-driven visualizations, microtonal geometry, dense technical typography and interactive relationships rather than from reproducing an existing instrument.

## Current limitations

This is not yet a stable production release. Full MTS-ESP conformance, `.kbm`, complete MIDI Tuning Standard SysEx coverage, polyphony, envelopes, persistent sequencer editing, editable tuning geometry, DAW validation and release signing remain pending.
