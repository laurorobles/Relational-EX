# Relational EX — Architecture

## Layers

```text
Host / Standalone
        ↓
PluginProcessor / PluginEditor
        ↓
Application state + APVTS
        ↓
Relational control model
        ↓
Tuning / Sequencer / Modulation
        ↓
Voice / FM / Resonator
        ↓
Audio output
```

## Core rule

Realtime DSP must never depend on UI-owned mutable containers or filesystem operations.

## Tuning

UI-side tuning edits modify the edit model, then publish an immutable 128-note frequency snapshot by atomically switching between two completed buffers.

## Sequencer

UI-side event edits modify the edit vector, then publish a complete event-array snapshot through the same double-buffer pattern.

## MTS

MTS-ESP is isolated behind `MTSClientBridge`. When present, it can override local pitch resolution. Without an MTS-ESP master, internal Relational tuning remains the main source; a valid local MTS SysEx table can override it when present.

## Presets

Factory presets are code-defined development assets. Host state serialization is separate from the conceptual preset catalogue.

## Database

Scale discovery and synchronization are non-realtime operations. Runtime lookup is local.

## UI

Components consume snapshots or UI-safe models. They do not directly mutate realtime state without an explicit publish/commit boundary.

## Relation Engine

The Relation Engine is a bounded five-domain coupling stage between control interpretation and voice rendering. It consumes normalized pitch/time/FM/resonance/tuning states and returns bounded coupled values. It contains no filesystem, UI or allocation work on the realtime path.
