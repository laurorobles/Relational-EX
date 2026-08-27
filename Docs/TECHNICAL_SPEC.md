# Relational EX Technical Specification

## Product

Name: Relational EX
Formats: VST3, Standalone
Platform: macOS first
Architecture: C++17 + JUCE + isolated DSP modules

## Realtime safety

The audio thread must not allocate memory, touch the filesystem, mutate UI state, or iterate over UI-owned containers. Sequencer and tuning data are published through double-buffered snapshots.

## State

APVTS stores host-automatable scalar parameters. ValueTree state stores sequence structure and custom tuning information as development support evolves.

## Tuning interoperability

Priority order:

1. MTS-ESP master when connected.
2. Local MIDI Tuning Standard table when a valid local MTS tuning has been received.
3. Internal Relational tuning snapshot.

## Plugin metadata

Company: LAO
Bundle ID: `com.lao.relationalex`
Manufacturer code: `LaRo`
Plugin code: `Rlt2`
Product name: `Relational EX`

Before release, these identifiers must be frozen and checked for global uniqueness.

## Relational Engine

The realtime synthesis path includes a compact five-domain relationship model. It is deterministic, bounded and allocation-free during processing. The current matrix is fixed in the development build; future versions may expose selected relationships through the UI without changing the underlying dimensional model.
