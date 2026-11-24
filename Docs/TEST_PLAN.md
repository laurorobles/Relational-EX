# Relational EX — Test Plan

## Static & core checks
- C++17 clean build (`-Wall -Wextra` on Clang/GCC, `/W3 /utf-8` on MSVC)
- Locale-independent Scala (`.scl`) and KBM (`.kbm`) parsing
- Lock-free snapshot & FIFO safety (no allocations or locks in `processBlock`)
- Mono, stereo, and multi-bus channel safety

## Cross-platform CI build matrix
- **macOS:** Universal binary (`arm64` + `x86_64`), VST3 + Standalone `.app`, `Info.plist` lint
- **Windows:** `x64` MSVC Release, VST3 + Standalone `.exe` + bundled `Resources/Scales`
- **Linux:** `x86_64` GCC Release (Ubuntu 22.04 LTS), VST3 + Standalone + bundled `Resources/Scales`

## Runtime smoke tests
- Standalone opens without crash and outputs audio
- On-screen keyboard & computer keyboard trigger/release voices cleanly via lock-free FIFO
- `POLY` (8-voice) and `MONO LEGATO` (single-voice portamento without envelope retrigger) behave as expected
- Scale quantization (`SCALE` + `ROOT`) snaps sequencer and relational tuning shifts; `OFF (FREE)` preserves microtonal pitch
- ADSR, SVF low-pass filter, and `LFO >` target routing (`RELATIONS`, `FM RATIO`, `FM INDEX`, `CUTOFF`) work smoothly
- `FIELD` / `GRID` / `RADIAL` sequencer views edit the same underlying event set; horizontal drags preserve event values
- Preset switching and DAW state save/reload restore parameters, tuning, sequence, cycles, and 5×5 relation matrix
- Malformed `.scl` files report a clean error message without crashing

## Tuning & DAW interoperability
- Verify VST3 scan and playback in at least one host (Ableton Live, Reaper, or Bitwig) and `pluginval` strictness level 5
- Verify MIDI Note On/Off, CC123/CC120 All Notes Off, MPE pitch bend, and MTS SysEx / MTS-ESP fallback
