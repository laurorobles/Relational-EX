# RELATIONAL — Implementation Status

## Implemented in prototype

- VST3 and Standalone CMake targets.
- Interactive Frequency Field editing prototype.
- Interactive Relational Sequencer event-field prototype.
- 2-operator FM oscillator with continuous ratio.
- First-class tuning model.
- Scala `.scl` parser.
- Local MIDI Tuning Standard table + SysEx handling foundation.
- Dynamic MTS-ESP client bridge architecture.
- English hover-help policy wired into prototype controls.

## Important limitation

The current `MIDI Tuning Standard` parser intentionally starts with the real-time Single Note Tuning Change path and establishes the local tuning-table architecture. Full updated MTS variants (banked bulk dumps and scale/octave 1-byte/2-byte messages) still need conformance tests against the official MMA specification before shipping.

The MTS-ESP bridge dynamically loads the installed ODDSound library rather than bundling its client source in this snapshot. This avoids making network/download availability part of the build. The official client API supports real-time retuning, local MTS SysEx fallback and multi-channel tuning. See the project decision log for the production plan.

## Next implementation slice

1. Harden MTS parser with test vectors.
2. Complete MTS-ESP client querying, including multi-channel/MPE policy.
3. Finish Scala KBM mapping.
4. Add local scale database index/cache and background synchronization.
5. Turn Wilson-inspired tuning geometry into the editable Frequency Field.
6. Make the sequencer event model APVTS/state-tree backed so UI edits are automatable and undoable.
