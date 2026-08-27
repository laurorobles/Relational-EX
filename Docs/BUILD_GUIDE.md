# Relational EX Build Guide

## Requirements

- macOS
- Xcode Command Line Tools
- CMake 3.22 or newer
- JUCE 9.0.1 recommended for the current CI target
- Git

The local default JUCE location is `~/JUCE`. Override it with `JUCE_DIR` when necessary.

## Preflight

```bash
./PREBUILD_CHECK.sh
```

The preflight verifies the compiler, CMake, JUCE, source tree, VST3 destination and CMake configuration.

## Build

```bash
./BUILD_RELATIONAL_EX.sh
```

Or manually:

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release --parallel "$(sysctl -n hw.logicalcpu)"
```

## Install

The developer VST3 is copied to:

`~/Library/Audio/Plug-Ins/VST3/Relational EX.vst3`

The Standalone is produced under:

`build/RelationalEX_artefacts/Release/Standalone/Relational EX.app`

## GitHub

The canonical repository is:

`git@github.com:laurorobles/Relational-EX.git`

Use:

```bash
./SYNC_GITHUB.sh
```

The script never force-pushes. Divergent histories stop with an error.
