# Build Status — v0.1.9

The current candidate is focused on the first clean JUCE runtime baseline.

## Fixed before runtime testing

- JUCE CMake discovery and C/C++ project languages
- generated JuceHeader support
- `RelationalProcessor::createEditor()` linker definition
- JUCE tooltip implementation via `SettableTooltipClient`
- safe empty tuning-field rendering
- MIDI Note On/Off voice gate
- official MTS-ESP client function signatures behind a dynamic loader
- explicit user VST3 copy location
- prebuild environment checks

## Not yet production-stable

- full `.kbm` support
- complete MIDI Tuning Standard SysEx coverage
- continuous MTS-ESP retuning during held notes
- polyphonic voice allocator and envelopes
- persistent editable sequencer state
- fully editable Frequency Field mathematics
- DAW validation and release signing
