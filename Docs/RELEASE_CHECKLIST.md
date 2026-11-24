# Release Checklist

## Build & CI
- [x] macOS Universal (`arm64` + `x86_64`) VST3 & Standalone build passes
- [x] Windows (`x64`) VST3 & Standalone build passes
- [x] Linux (`x86_64`) VST3 & Standalone build passes
- [x] Bundled `Resources/Scales/*.scl` included in all platform packages
- [x] `.zip` archives + `.sha256` checksums generated per OS

## Runtime & Audio Safety
- [x] Audio thread free of heap allocations, locks, and UI races
- [x] On-screen & computer keyboard routed via lock-free FIFO
- [x] Mono Legato glide, Scale Quantization, ADSR, Filter, and LFO routing verified
- [x] Sequencer Field / Grid / Radial views, 5 independent cycles, and 5×5 Relation Matrix persist in state
- [x] Locale-independent & UTF-8 safe Scala file loading

## Documentation & Licensing
- [x] `LICENSE` (AGPL-3.0) and `Docs/THIRD_PARTY_NOTICES.md` included
- [x] `README.md`, `CHANGELOG.md`, `Docs/MANUAL.md`, `Docs/PARAMETER_REFERENCE.md`, `Docs/BUILD_GUIDE.md`, and `Docs/TECHNICAL_SPEC.md` updated for v0.8.0

## GitHub Release
- [x] Version set in `CMakeLists.txt` and `CHANGELOG.md`
- [x] Tag `vX.Y.Z` pushed to `origin` so GitHub Actions publishes platform zips + SHA-256 checksums
