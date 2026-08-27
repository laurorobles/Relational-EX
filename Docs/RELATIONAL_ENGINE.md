# Relational Engine

The Relational Engine is the conceptual center of Relational EX. It couples five normalized domains:

| Dimension | Meaning |
|---|---|
| Pitch | pitch displacement / pitch topology |
| Time | temporal phase / event position |
| FM | spectral modulation state |
| Resonance | inharmonic body / modal energy |
| Tuning | microtonal displacement / tuning motion |

The current implementation is a deliberately small 5×5 matrix with sparse default couplings. It is not exposed as a full user-editable modulation matrix yet.

## Why a matrix?

A conventional synthesizer often treats controls independently. Relational EX instead asks what happens when a change in one domain produces a controlled consequence in another domain.

For example:

`Tuning → Pitch → FM → Resonance`

can produce a timbral change without requiring a separate envelope or effect.

## Current default couplings

The development defaults include weak relationships such as:

- Pitch → FM
- Pitch → Resonance
- Pitch → Tuning
- Time → FM
- Time → Resonance
- FM → Resonance
- FM → Tuning
- Resonance → FM
- Tuning → Pitch
- Tuning → FM

The depth parameter scales these relationships globally.

## Future UI

The intended UI is a compact relationship matrix/graph rather than a large conventional modulation table. It should make the topology visible without turning the instrument into a spreadsheet.
