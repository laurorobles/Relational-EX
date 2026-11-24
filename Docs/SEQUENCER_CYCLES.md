# Relational EX — Independent Relational Cycles

## Purpose

The sequencer treats rhythm, pitch, FM, tuning and motion as independent periodic dimensions. The goal is not generic polymeter; it is to make the interaction between musical domains audible.

Default factory cycle set:

- Rhythm: 13
- Pitch: 7
- FM: 5
- Tuning: 11
- Motion: 17

Each cycle can be changed from 1 to 32 directly from the five compact strips above the main event view.

## Interaction

Click or drag horizontally on a cycle strip. The horizontal location selects a cycle length from 1–32. The highlighted marker shows the currently active phase position.

The main event field remains the shared event data. The cycle strips do not create separate sequencers; they create independent indexing domains over the same event set.

## Realtime safety

Cycle positions are stored as atomics and UI edits are published through an immutable audio snapshot. The audio thread does not traverse UI-owned containers.

## Scope

This feature deepens the relationship between time, pitch, spectrum, tuning and motion and therefore remains inside the original Relational EX scope.

**Extasis Records**  
**coded by @laurorobles**
