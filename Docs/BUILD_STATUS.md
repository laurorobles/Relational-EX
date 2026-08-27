# Relational EX — Build Status

## Version

**0.3.1-dev**

## Validation completed in the development environment

- Core modules compile under C++17 with `-Wall -Wextra -Wpedantic`.
- Core smoke test covers tuning, Scala parsing, KBM mapping, MTS bridge construction, sequencing, FM, resonator and modulation primitives.
- Shell scripts pass `bash -n`.
- Git working tree passes `git diff --check`.
- CMake structure has been reviewed against JUCE's documented CMake target conventions.

## Validation still required on a real macOS JUCE checkout

- CMake configure using the developer's JUCE checkout.
- Full Xcode/Clang compilation of VST3 and Standalone.
- Standalone launch and audio I/O.
- VST3 loading in Ableton Live.
- MTS-ESP with an installed current provider/library.
- macOS code signing and library validation behavior.

GitHub Actions is configured to build macOS ARM64 on `macos-14` and macOS Intel on `macos-15-intel`. GitHub documents both runner families as standard hosted runners. See the official GitHub-hosted runner reference for current availability.

## Release rule

The project must not be labeled stable or release-ready until all real-macOS and DAW tests above pass.

## 2026-08-27 — v0.3.2-dev audit

Static/core checks passed in the development environment with C++17, `-Wall -Wextra -Wpedantic`:

- DSP core
- tuning system
- Scala parser
- KBM parser
- MTS bridge compilation
- sequencer
- presets
- scale catalog
- relation engine

A core smoke test completed successfully. Full JUCE/VST3/Standalone linking still requires the developer Mac/CI JUCE checkout and is therefore not claimed as locally verified here.
