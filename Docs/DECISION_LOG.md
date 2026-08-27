# Relational EX — Decision Log

## 2026-08-26 — Product concept

The instrument is defined as a relational synthesizer: frequency, rhythm, spectrum and motion are first-class interacting systems.

## 2026-08-26 — Microtuning

Microtuning is part of the synthesis architecture, not a secondary MIDI feature.

## 2026-08-26 — Wilsonic

Wilsonic is a conceptual/technical reference for tuning visualization, not a runtime dependency.

## 2026-08-26 — MTS

Support MTS-ESP, MIDI Tuning Standard and Scala, with local fallbacks and no network dependency in the audio path.

## 2026-08-26 — Sequencer

Events are relational objects. Field, Grid and Radial are views onto the same event model.

## 2026-08-26 — UI

Every interactive control gets English hover help. The interface uses bounded functional modules and a strict spacing/alignment system.

## 2026-08-27 — Realtime safety

Tuning and sequencer snapshots use double buffering instead of a C++ seqlock over non-atomic objects. This avoids a data race in which the audio thread could read while the UI writes.

## 2026-08-27 — GitHub

Repository name and public plugin identity are `Relational EX`. Synchronization tooling never force-pushes and stops on divergent remote history.

## 2026-08-27 — Relational Engine enters the audio path

Decision: implement a bounded five-domain relationship model before adding more synthesis modules.

Rationale: the product thesis depends on relationships between pitch, time, spectrum and tuning. A small coupling model makes that concept operational without turning the plugin into a generic modulation matrix.

Constraints: deterministic, bounded, allocation-free in the audio callback, sparse defaults, and documented as development behavior until the relationship UI is finalized.
