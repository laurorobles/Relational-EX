# DSP Model

## Signal path

MIDI / Sequencer Event → Tuning Resolution → Voice Allocation → FM Oscillator → Optional Inharmonic Resonator → Voice Envelope → Soft Saturation → Output

## FM

Carrier frequency is resolved from the active tuning system. The modulator frequency is `carrier × ratio`. FM index controls deviation depth; feedback feeds the oscillator output back into the phase path. Ratios are continuous and are intentionally not restricted to integer harmonic relationships.

## Inharmonic Resonator

The resonator uses six custom mode ratios. `Inharmonicity` interpolates their spacing away from harmonic integer multiples. This is an intentionally lightweight modal abstraction, not a physical model of a named instrument.

## Tuning

The realtime audio thread consumes an immutable 128-note frequency snapshot. UI-side tuning edits are written to the inactive snapshot and published with an atomic index swap. This avoids reading partially-written tuning tables from the realtime thread.

## Modulation

The development core includes an LFO and deterministic logistic-map chaos source. These are primitives for the future relational modulation matrix.
