# Build Guide

## Requirements
- macOS 11+ (Apple Silicon or Intel), Xcode Command Line Tools
- CMake ≥ 3.22, Git
- JUCE 9.0.1 — **fetched automatically** by CMake. To use a local copy: `-DJUCE_DIR=/path/to/JUCE` or put it at `~/JUCE`.

## Build
```bash
./scripts/build.sh              # Release build into ./build
./scripts/build.sh --install    # + copy VST3 to ~/Library/Audio/Plug-Ins/VST3/Extasis Records
./scripts/build.sh --clean --launch
```
Manual equivalent:
```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DRELATIONAL_COPY_PLUGIN=OFF
cmake --build build --config Release --parallel
```
Artefacts: `build/RelationalEX_artefacts/Release/{VST3,Standalone}/`.

## Package a release
```bash
./scripts/package_release.sh    # → dist/RelationalEX-v<version>-macOS.zip (+ .sha256)
```
Then create a GitHub release (see `RELEASE_CHECKLIST.md`). Pushing a tag `vX.Y.Z` makes CI build and attach the zip automatically.

## Platform status
macOS is the only tested platform. Windows/Linux are untested and unsupported for now (contributions welcome).
