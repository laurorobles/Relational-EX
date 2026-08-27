# RELATIONAL — Technical Architecture v0.1

## Layering

`UI -> Model/State -> DSP`

The editor can disappear. The processor remains valid and owns persistent state.

## Tuning pipeline

MIDI note -> local/MTS-ESP/MTS SysEx resolution -> tuning ratio -> frequency -> oscillator

The tuning engine must support:
- 12-TET
- N-TET
- ratio-based scales
- Scala SCL/KBM
- arbitrary user-defined frequency/ratio fields
- interpolation between tuning systems
- per-event cent offsets
- continuous retuning while notes sustain

## Spectrum pipeline

Source -> oscillator/FM -> partial/resonator layer -> modulation -> output

FM is ratio-centric. Ratios are represented as continuous doubles and are not restricted to integer harmonic relationships.

## Sequencer model

A sequencer event is not a note-only object. It contains:
- position
- pitch
- ratio
- duration
- velocity
- probability
- FM index
- tuning offset
- timing offset

Different tracks may use different cycle lengths to create polymetric relationships.

## Database/cache architecture

UI/background service -> HTTP repository -> local metadata/index/cache -> tuning engine

Never:
- perform network I/O inside `processBlock`
- parse a large scale archive inside `processBlock`
- block audio waiting for a tuning download

The audio thread consumes immutable/local snapshots.

## Dependencies

Required:
- JUCE

Optional ecosystem integration:
- ODDSound MTS-ESP client library

Reference/inspiration:
- Wilsonic-MTS-ESP source and tuning visualizations
- Scala format and scale archives
