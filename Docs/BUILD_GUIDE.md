# Build Guide

## Requirements
- macOS 11+ (Xcode Command Line Tools), Windows 10/11 (Visual Studio 2022), or Linux (GCC ≥ 9 / Clang)
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

## Linux dependencies (Debian/Ubuntu)
```bash
sudo apt-get install -y cmake build-essential libasound2-dev libjack-jackd2-dev ladspa-sdk \
  libfreetype-dev libfontconfig1-dev libx11-dev libxcomposite-dev libxcursor-dev \
  libxext-dev libxinerama-dev libxrandr-dev libxrender-dev libxi-dev \
  libgl1-mesa-dev libglu1-mesa-dev mesa-common-dev libegl-dev
```

## Windows
```powershell
cmake -S . -B build -G "Visual Studio 17 2022" -A x64
cmake --build build --config Release --parallel
```
(Git Bash can run `scripts/package_release.sh`.)

## Releasing
1. Update `CHANGELOG.md` and the version in `CMakeLists.txt`.
2. `git tag vX.Y.Z && git push origin main --tags`.
3. CI builds macOS (universal), Windows and Linux, and publishes the GitHub Release with zips + SHA-256 files.

## Platform status
CI builds all three platforms on every push. Runtime testing is most thorough on macOS; please report
Windows/Linux issues on GitHub. MTS-ESP and user scale folders:

| OS | MTS-ESP library | User scales folder |
|---|---|---|
| macOS | `/Library/Application Support/MTS-ESP/libMTS.dylib` | `~/Library/RELATIONAL EX/Scales` |
| Windows | `C:\ProgramData\MTS-ESP\libMTS.dll` | `%APPDATA%\RELATIONAL EX\Scales` |
| Linux | `/usr/local/lib/libMTS.so` | `~/.config/RELATIONAL EX/Scales` |
