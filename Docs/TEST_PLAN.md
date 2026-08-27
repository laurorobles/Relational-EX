# Relational EX — Test Plan

## Static/core tests

Required for every development milestone:

- C++17
- `-Wall -Wextra -Wpedantic`
- tuning frequency sanity
- Scala parser sanity
- KBM parser sanity
- sequencer snapshot sanity
- Euclidean pattern sanity
- preset catalogue sanity

## macOS build tests

- CMake configure
- CMake build
- Standalone bundle exists
- VST3 bundle exists
- `Info.plist` is valid
- executable architecture is correct
- user VST3 install succeeds

## Runtime smoke tests

- Standalone opens without crash
- audio device can be selected
- MIDI Note On produces audio
- Note Off releases voices
- polyphony works without obvious zippering/clicking
- Frequency Field edits remain stable
- sequencer view switching does not alter event data
- Grid toggling publishes changes
- Radial dragging publishes changes
- preset switching changes DSP state
- state save/reload restores scalar parameters and sequence
- loading a malformed `.scl` produces a safe error

## MTS tests

With a current MTS-ESP provider installed:

- client registers
- master is detected
- Note On resolves provider tuning
- channel-aware tuning works when supplied
- unmapped note filtering is respected
- MTS SysEx received by the plugin is parsed
- continuous retuning is stable while a note is sustained

## DAW tests

At minimum validate in one current VST3 host before any release candidate. Ableton Live should be part of the release test matrix because it is a primary target for the project's use case.
