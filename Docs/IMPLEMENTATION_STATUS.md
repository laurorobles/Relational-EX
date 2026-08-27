# Implementation Status

## Implemented / development-ready

- Relational EX public product name and JUCE metadata.
- VST3 + Standalone CMake targets.
- 8-voice FM engine.
- Inharmonic six-mode resonator.
- Tuning snapshot with double buffering for realtime-safe reads.
- Interactive tuning field.
- Relational event model and double-buffered sequencer snapshot.
- Euclidean pattern generation.
- Factory exploration presets.
- Scala `.scl` import.
- Initial KBM parser.
- MTS SysEx local parser for supported Single Note Tuning Change.
- MTS-ESP bridge with dynamic loading and optional operation.
- LFO and deterministic chaos modulation primitives.
- Relational Engine with pitch/time/FM/resonance/tuning coupling in the audio path.
- Explicit lfoRate, lfoAmount, chaosAmount and relationDepth APVTS parameters.
- English contextual hover help.
- macOS VST3 installation workflow.
- GitHub Actions macOS ARM64 build workflow.

## Partial

- Full KBM semantics and validation.
- Complete MTS local message family coverage beyond the implemented subset.
- MTS-ESP validation against an installed current `libMTS.dylib` on the developer Mac.
- Full multi-lane polyrhythmic sequencer UI.
- User-editable modulation/relationship matrix UI.
- Preset browser search/tagging.
- Distribution signing/notarization.
- Complete multi-lane polymetric UI, event timing-offset playback, and user-editable relationship-matrix routing.
- Event timingOffset is stored and visualized but is not yet applied as sample-accurate scheduling.

## Next

The next development milestone should deepen the existing relational architecture rather than broaden into a generic synthesizer: complete the tuning field model, multi-lane relational sequencing, modulation routing, database browser and MTS validation.
