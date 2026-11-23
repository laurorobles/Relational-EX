# Relational EX — Relational Engine

## Purpose

The Relational Engine is the identity layer of Relational EX. It is not a general-purpose modulation matrix. It models bounded coupling between five domains:

- Pitch
- Time
- FM
- Resonance
- Tuning

## Matrix

The development build exposes a 5×5 matrix. Columns are sources; rows are destinations. Each off-diagonal cell is a signed coupling from -1.0 to +1.0.

Positive coupling pushes the destination in the same normalized direction as the source. Negative coupling inverts the relationship. `Relation Depth` scales the complete matrix response at runtime.

## Realtime model

The editor owns an editable matrix. A published immutable snapshot is atomically made available to the audio thread. The audio thread copies the matrix once per audio block and performs all sample-level transforms against that local copy. No audio-thread allocation or UI-container access is required.

## Default relationships

The initial matrix contains deliberately sparse relationships between pitch, FM, resonance and tuning, with smaller time relationships. This makes the engine audible without requiring an enormous control surface.

## UI

The `RELATIONS` control opens an overlay editor. Drag vertically inside a cell to change its amount. The display is intentionally compact: coupling strength is encoded by fill magnitude and sign, while the numeric percentage remains visible for precision.

## Persistence

The complete 5×5 matrix is serialized into the plugin state under `RELATIONS/CELL` nodes. Future releases may move selected relationships into automatable parameters; that change must preserve the 5-domain model.
