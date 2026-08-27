# Relational EX — User Manual

## 1. Overview

Relational EX is an experimental instrument built around relationships between **frequency, rhythm, spectrum and motion**. It is not designed as a conventional subtractive synthesizer. The important question is not only “what note is this?” but “what relationships define this sound right now?”

The interface is intentionally modular. Every major area is bounded, aligned to a common grid, and given a functional role. Empty space is minimized without crowding controls. All interface text and help is in English.

---

## 2. Quick start

1. Open the Standalone or load the VST3.
2. Play a MIDI note.
3. Move **FM Ratio** from `1.000` to a non-integer value such as `1.414` or `1.618`.
4. Increase **FM Index** slowly.
5. Move a point in the **Frequency Field**.
6. Switch the sequencer between **FIELD**, **GRID** and **RADIAL**.
7. Press **EUCLID** and experiment with Steps/Pulses.
8. Open **PRESETS** and use a factory patch as a starting hypothesis.

A useful way to learn the instrument is to change one relational dimension at a time: tuning first, then FM ratio, then FM index, then temporal structure.

---

## 3. Header and global controls

### RELATIONAL EX

The product identity. The secondary line describes the four core domains: frequency, rhythm, spectrum and motion.

### PRESETS

Opens the factory preset browser. Presets are grouped conceptually rather than by conventional categories such as bass/lead/pad. Each preset is a starting point for exploring a relationship.

### TUNING

Opens built-in equal temperament choices and entries discovered by the local scale catalog. A tuning changes the frequency field that the synthesis engine uses to resolve MIDI notes.

### LOAD .SCL

Loads a Scala `.scl` file from disk. The loaded scale becomes the current internal tuning. The project deliberately treats Scala as an exchange format, not as a hard-coded list of “exotic” scales.

### FIELD / GRID / RADIAL

Cycles the three sequencer representations. The data are shared: changing the view never creates a second sequence.

### PLAY / STOP

Starts or stops the internal sequencer clock. Incoming MIDI notes can still be played while the sequencer is stopped.

### EUCLID

Rebuilds the current event activation pattern using the current Steps and Pulses relationship. It preserves the event data structure around the activation mask.

### STATUS

Shows the active preset, tuning source or error state. It is intentionally textual rather than decorative.

---

## 4. Frequency Field

The Frequency Field is the main tuning laboratory. Horizontal placement corresponds to scale degree. Vertical placement represents the interval in cents relative to the current tuning system.

A point can be selected and dragged vertically. The current prototype maps vertical movement to approximately four cents per screen pixel, with the actual ratio stored continuously.

### Why it is not a piano roll

The field does not assume 12 semitones. A 12-TET tuning produces a familiar ladder, while 31-TET, just-intonation-like data or a custom Scala scale can create different geometries.

### Editing a point

1. Move the pointer near a point.
2. Press and hold.
3. Drag vertically.
4. Release to keep the edit.

The frequency snapshot used by the audio thread is updated through a double-buffered publish operation, so the audio path does not read a partially edited tuning table.

### Tuning terminology

- **Ratio**: frequency divided by the reference frequency.
- **Cents**: logarithmic interval where 1200 cents represent the configured period of 2:1.
- **Period**: the ratio that repeats the tuning system. Usually `2/1`, but Scala scales may define other periods.

---

## 5. Tuning systems

### TET / equal divisions

Built-in equal divisions currently cover one to 53 steps over the configured period. More divisions produce a denser field of possible pitches.

The frequency relationship is:

`frequency = rootFrequency × period^(degree / divisions)`

### Scala `.scl`

Scala files may specify intervals in cents or ratios. Relational EX reads the interval list and uses the final interval as the period.

### KBM

Keyboard Mapping (`.kbm`) describes how MIDI keys correspond to scale degrees. KBM support is currently a development subset and should be treated as partial until the full mapping semantics are validated against a larger corpus.

### MIDI Tuning Standard

Incoming supported MTS SysEx messages can update the local tuning table. The local table is used when no MTS-ESP master is connected.

### MTS-ESP

When an MTS-ESP master is present, the engine can query retuned frequencies by MIDI note and channel. The official client model recommends checking note filtering at Note On and, where practical, querying continuously while a note is held.

The current project isolates MTS-ESP from the rest of the tuning engine so the instrument remains functional without the external library.

---

## 6. FM Engine

### FM RATIO

Defines the relationship between modulator and carrier.

- `1.000`: same frequency.
- `2.000`: simple harmonic relationship.
- `1.5`: harmonic relationship with a different overtone alignment.
- `1.414`: irrational relationship approximating square-root-of-two behavior.
- `1.618`: golden-ratio relationship.
- values that are not simple ratios generally produce increasingly complex sideband structures.

The parameter is deliberately continuous. There is no “musical ratio whitelist.”

### FM INDEX

Controls modulation depth. Raising the index usually increases the number and strength of sidebands. Small changes can move a sound from a recognizable tone toward a dense or metallic spectrum.

### FM FEEDBACK

Feeds a portion of the oscillator output back into its own phase. Low values add edge and complexity. High values can make the oscillator aggressive or unstable.

---

## 7. Inharmonic Resonator

Relational EX contains a lightweight six-mode resonator. Its mode ratios are intentionally synthetic rather than a physical model of one named instrument.

### INHARMONICITY

Interpolates the modal ratios away from simple harmonic multiples. Low values keep the body relatively coherent; high values emphasize metallic and object-like relationships.

### RESONANCE

Controls the amount of resonator excitation mixed with the FM voice. A high value can turn a simple FM tone into something closer to an abstract struck or ringing object.

---

## 8. Motion layer

### TUNING DRIFT

Adds tiny random movement around the resolved tuning. The range is intentionally small because the desired effect is a changing frequency relationship rather than a traditional vibrato.

### LFO RATE

Sets the periodic motion rate.

### LFO AMOUNT

Controls how strongly the LFO influences the phase/frequency path. The current scale is intentionally wider than a conventional fine-vibrato control because the same source can be pushed into experimental territories.

### CHAOS

Uses a deterministic logistic-map source. Unlike unconstrained random noise, the source has internal structure and is repeatable from its initial condition. Higher amounts reduce obvious periodicity while preserving a bounded output.

---

## 9. Relational Sequencer

The sequencer is interactive. An event is a relational object, not merely an on/off step.

Each event can contain:

- **Position** — normalized cycle position.
- **Pitch** — displacement from the sequence reference.
- **Ratio** — event-level spectral/FM relationship.
- **Velocity** — event amplitude.
- **Probability** — chance to trigger each cycle.
- **FM Index** — event-specific FM depth.
- **Tuning Offset** — additional cents after tuning resolution.
- **Timing Offset** — displacement from nominal event timing.
- **Active** — event participation.

### Steps

Defines the total cycle length. The current implementation supports one to 32 events. The original design deliberately avoids assuming that 32 steps are always musically necessary.

### Pulses

Defines how many active hits a Euclidean pattern distributes over the current cycle.

### Probability

At `1.0`, the event always triggers. At `0.5`, each cycle has an independent 50% chance of triggering.

### Timing Offset

Shifts the event around its nominal position. This is intended for microtiming and future groove/phase studies.

---

## 11. Relational Engine

The Relational Engine is what makes the instrument more than a synthesizer with microtuning. It links five domains: **Pitch, Time, FM, Resonance and Tuning**.

### Relation Depth

Controls how strongly the built-in relationship network influences the synthesis path. At `0`, the domains behave mostly independently. At higher values, changes in motion, tuning and spectral state create secondary consequences.

The current development build uses a sparse fixed relationship topology. A future editor will expose selected connections visually.

### Interpreting the relationships

- **Pitch → FM**: pitch movement can alter perceived spectral complexity.
- **Pitch → Resonance**: pitch movement can alter the amount of modal emphasis.
- **Time → FM**: temporal position can slightly change spectral behavior.
- **FM → Resonance**: denser FM can excite more body-like energy.
- **Tuning → Pitch**: changing the tuning field can feed back into pitch movement.
- **Tuning → FM**: microtonal changes can alter spectral relationships.

The goal is not randomization. The relationships are bounded and deterministic so that complex behavior remains musically inspectable.

## 12. Preset design principles

Factory presets are demonstrations of relationships. Start with a preset, identify the dominant relationship, then change one domain. For example:

- `PRIME LATTICE` — tuning relationships and moderate resonant coupling.
- `FRACTURED GLASS` — non-integer FM relationships and dense sidebands.
- `PHASE RAIN` — motion and temporal instability.
- `RATIONAL GHOST` — simple ratios with subtle tuning structure.
- `ALIEN MARIMBA` — inharmonic resonance and modal color.
- `BROKEN GENERATOR` — extreme tuning, FM and deterministic chaos.

Presets are intentionally not organized as “bass / lead / pad”. They are conceptual experiments.

## 10. Sequencer views

### FIELD

Horizontal axis: time. Vertical axis: relative pitch. Event position, pitch and velocity become immediately visible as geometry. This is the primary “musical analysis” view.

### GRID

A discrete view for quickly enabling/disabling events. It is useful when working rhythmically and when a clear step relationship matters more than continuous geometry.

### RADIAL

Maps time onto angle. Events orbit around the cycle. This is designed to make cyclic and rotational relationships legible rather than simply decorate a familiar step grid.

---

## 13. Presets

Factory presets are deliberately named as conceptual prompts. They should encourage the user to interrogate the synthesis system.

### INIT / PURE

A clean baseline. Best for learning individual controls.

### PRIME LATTICE

31-TET, moderate/high FM complexity and an altered ratio field. Start here for pitch relationships that feel coherent but not chromatically familiar.

### FRACTURED GLASS

Higher FM index, feedback and inharmonicity. A gateway into brittle and metallic timbres.

### PHASE RAIN

Motion-led. Listen to the pitch relationships changing rather than treating the sound as a static oscillator.

### RATIONAL GHOST

A restrained ratio-focused patch. Use it to hear how simple relationships can still create a characteristic FM spectrum.

### TIDAL METAL

Resonator-forward. Long notes reveal the modal decay structure.

### PAPER CIRCUIT

A middle-ground percussive/digital patch.

### NULL MACHINE

Extreme. Useful for exposing how dense ratios and high modulation can destabilize pitch perception.

### SOFT COLLISION

Low-intensity movement and relatively stable spectrum.

### ALIEN MARIMBA

A deliberately synthetic modal prompt inspired by the idea of wooden resonance; it is not a physical marimba model.

### MICRO POLAROID

A narrow microtonal environment for exploring small interval changes.

### BROKEN GENERATOR

High-complexity FM and chaos-oriented exploration.

---

## 14. Recommended workflows

### Workflow A — Build a tuning

Start at `12-TET`. Change to `31-TET`. Move one Frequency Field point. Compare the sound before and after. Load a Scala scale and repeat.

### Workflow B — Build a timbre from a ratio

Hold one note. Set FM Ratio to `1.000`. Raise FM Index until the spectrum becomes audible. Then try `1.414`, `1.618`, `2.173` and `3.141`. Listen for the point where pitch identity weakens and spectral identity becomes dominant.

### Workflow C — Make a relational rhythm

Choose 13 Steps and 5 Pulses. Press EUCLID. Switch to Radial. Then change event positions manually in Field view. Return to Grid and disable one event. The sequence remains the same underlying dataset.

### Workflow D — Combine domains

Choose 31-TET, an irrational FM ratio and a 13/5 rhythm. This produces a clear separation between familiar rhythmic periodicity and unusual pitch/spectral relationships.

---

## 15. Tooltips

Every interactive control provides English mouse-over help. Tooltips are designed to explain effect and musical consequence. They are part of the product design system, not an optional documentation layer.

---

## 16. Saving and state

The plugin stores APVTS parameters and development-state information for the relational sequence. State serialization will continue to evolve as the tuning database and multi-lane sequencing model become more sophisticated.

---

## 17. Current limitations

- Full KBM semantics are still being validated.
- MTS SysEx local support is not yet a complete implementation of every MTS message family.
- MTS-ESP requires validation against a current installed library.
- Multi-lane polymetric sequencing is part of the intended architecture but not yet the full UI.
- Distribution signing/notarization is not yet part of the development build.

These limitations are documented rather than hidden so presets and future work remain traceable to the actual implementation.

## 18. Detailed control reference

### Output Level
Controls the final output gain after voice summing and soft saturation. Keep lower values when using many simultaneous voices or high FM feedback.

### Tuning Divisions
Selects the number of equal divisions of the current period for internal TET tuning. This is a fast exploration control; loading Scala replaces the current interval list with the imported scale.

### Root Frequency
Defines the reference frequency of the internal tuning system. MIDI note 69 maps to this reference in the default 12-TET interpretation.

### Sequence Steps
Sets the number of events in the cycle, from 1 to 32. Short sequences are useful for strong phase repetition; longer sequences expose slower relationships between pitch and rhythm.

### Sequence Pulses
Controls the number of active events when EUCLID is pressed. Pulses are constrained to the current number of steps.

### Relation Depth
Controls the strength of the internal five-domain coupling network. At zero, the network contributes nothing. Increasing it makes changes in tuning, motion and spectral state influence other domains.

### LFO Rate
Sets the frequency of the periodic motion source. Slow values produce gradual evolution; faster values can become audible as vibrato-like movement.

### LFO Amount
Scales the LFO contribution to the relational motion state. At zero the LFO is running internally but contributes no modulation.

### Chaos
Controls the amount of deterministic nonlinear motion. The source is bounded and repeatable; it is intentionally different from sample-by-sample random noise.

### Event Position
In Field view, drag an event horizontally to change its normalized cycle position. The current prototype stores the position continuously.

### Event Pitch
In Field view, drag an event vertically to change its pitch displacement. The event remains a relational pitch offset rather than becoming a fixed named note.

### Event Probability
Controls whether an event is allowed to trigger on each cycle. Probability is evaluated independently per cycle using the sequencer's deterministic random source.

### Event Ratio / Event FM Index
These override the voice's base FM relationship for that event. They are intended to create spectral patterns that differ from the pitch pattern.

### Event Tuning Offset
Adds cents after the current tuning resolution. This allows small microtonal deviations without changing the underlying scale.

### Event Timing Offset
Stored as part of the event model for microtiming and future sample-accurate event scheduling. The current development build does not yet apply it as a separate scheduling offset.
