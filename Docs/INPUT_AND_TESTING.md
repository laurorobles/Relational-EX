# Relational EX — Input and Testing

## On-screen keyboard
The built-in keyboard covers MIDI notes 48–72. Click and hold to trigger a note. Dragging changes the active note; releasing stops it.

## Computer keyboard
The editor maps a chromatic row to notes starting at MIDI 60:

`A W S E D F T G Y H U J K O L P ;`

The mapping is polled at 60 Hz on the UI timer and forwarded through a lock-free FIFO to the audio thread so note-off transitions are deterministic across macOS, Windows, and Linux.

## MIDI monitor
The header displays:
- MIDI / NOTE ON or MIDI / READY
- note name
- velocity
- assigned frequency in Hz

The frequency is derived from the active tuning path for monitored events.

## Regression checklist
1. Open Standalone.
2. Click the on-screen keyboard and verify audio.
3. Release the key and verify the voice enters release.
4. Use the computer keyboard and verify note-on/note-off behavior.
5. Change 12-TET to 31-TET and confirm the monitored frequency changes.
6. Load a Scala file and repeat the frequency test.
7. Open RELATIONS, modify a non-diagonal link, close/reopen the editor, and save/restore plugin state.
8. Cycle FIELD / GRID / RADIAL and verify all views manipulate the same event set.
9. Load factory presets and verify the monitored tuning/audio state changes.
10. Inspect the installed VST3 (`~/Library/Audio/Plug-Ins/VST3/` on macOS, `C:\Program Files\Common Files\VST3\` on Windows, `~/.vst3/` on Linux).
