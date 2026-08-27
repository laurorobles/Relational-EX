# Changelog

## 0.3.2-dev — 2026-08-27

### Added
- Relational Engine with five-domain coupling model.
- `relationDepth` parameter.
- Explicit LFO/chaos APVTS parameters required by the UI.
- Build/install path normalized to `Relational EX.vst3`.
- Scope and Relational Engine documentation.

### Fixed
- Scala loading now refreshes the realtime tuning snapshot.
- Public bundle identifier updated to `com.lao.relationalex`.
- CI and local installer now agree on the VST3 bundle name.
- Realtime modulation sources are advanced once per sample instead of once per voice.

## 0.3.1-dev

## 0.3.1-dev

- Replaced unsafe realtime snapshot patterns with immutable atomic shared snapshots.
- Separated audio-thread and UI-thread random generators.
- Added pinned JUCE CI matrix for macOS ARM64 and Intel builds.
- Added stronger state restoration for custom tuning.

## 0.3.0-dev

- Renamed public product identity to Relational EX.
- Reworked tuning and sequencer snapshots to use double buffering for realtime safety.
- Added LFO and deterministic chaos modulation primitives.
- Added macOS ARM64 GitHub Actions build workflow.
- Added safe GitHub synchronization script with no force-push behavior.
- Improved build/install documentation and scope protection.

## 0.2.0-dev

- Renamed public plugin identity to Relational EX.
- Formalized VST3 + Standalone targets.
- Expanded tuning architecture and scale catalog planning.
- Expanded relational sequencer model.
- Added development documentation set.
- Added comprehensive user manual.
- Added references and third-party notices.
- Established repository metadata and design principles.

## 0.1.x

- Initial JUCE/CMake prototype iterations.
- Added Frequency Field experiments.
- Added FM and inharmonic resonator prototype.
- Added Scala and MTS-related experiments.
