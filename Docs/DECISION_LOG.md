# RELATIONAL Decision Log

## 2026-08-26 — v0.1.9 candidate

### Build strategy
The project must pass a preflight check before compilation. The build script must produce both VST3 and Standalone and install the VST3 into the per-user macOS VST3 folder.

### MTS-ESP strategy
MTS-ESP is a compatibility layer, not a dependency of the core DSP. The bridge dynamically loads the official client API and safely falls back to the local tuning engine when no MTS-ESP library/master is present.

### UI direction
The interface should feel like an instrument for manipulating relationships between frequency, spectrum, rhythm and motion rather than a conventional subtractive synth. Inspiration may come from experimental modular/digital instruments and microtonal visualization, but no visual clone of Autechre, Aphex Twin, Wilsonic or a specific hardware product is intended.

### Sequencer
The sequencer remains interactive. Future views will share the same event data: Grid, Field and Radial.

## 2026-08-26 — v0.1.9 build-hardening

### Linker fix
Implemented `RelationalProcessor::createEditor()` in `PluginProcessor.cpp`, resolving the Standalone and VST3 linker failure caused by the declared-but-undefined virtual override.

### VST3 installation
The CMake target explicitly uses the per-user macOS VST3 destination `~/Library/Audio/Plug-Ins/VST3`; the build script also verifies and copies the final bundle.

### MTS-ESP
The bridge was changed from a misleading placeholder client ABI to a guarded dynamic interface with the installed libMTS tuning-table symbols. Full official `libMTSClient` source integration remains a planned conformance step.
