# Release Checklist

## Build

- [ ] macOS ARM64 build passes
- [ ] macOS Intel build passes
- [ ] VST3 bundle exists
- [ ] Standalone bundle exists
- [ ] Info.plist validates
- [ ] plugin architecture matches target
- [ ] VST3 installed to user directory

## Runtime

- [ ] Standalone launches
- [ ] MIDI Note On/Off works
- [ ] audio output works
- [ ] Frequency Field edits work
- [ ] sequencer Field/Grid/Radial views work
- [ ] presets load correctly
- [ ] state restores correctly
- [ ] Scala loading works
- [ ] MTS SysEx path works
- [ ] MTS-ESP validated with current library

## Documentation

- [ ] README updated
- [ ] MANUAL updated
- [ ] PARAMETER_REFERENCE updated
- [ ] TECHNICAL_SPEC updated
- [ ] IMPLEMENTATION_STATUS updated
- [ ] CHANGELOG updated
- [ ] DECISION_LOG updated
- [ ] third-party notices checked

## Distribution

- [ ] version frozen
- [ ] signing/notarization verified
- [ ] clean-machine installation tested
