# RELATIONAL EX — MANUAL
## v0.8 Manual

**Extasis Records**  
*coded by @laurorobles*

Relational EX is an experimental instrument built around relationships between pitch, tuning, rhythm, spectrum and time.

### 0. QUICK START

1. Play with the on-screen keyboard, your computer keyboard (Standalone) or MIDI.
2. Press play on the sequencer; drag event dots **up/down** to change their pitch (horizontal movement does not matter).
3. Hover any control: every control has a tooltip describing exactly what it does.
4. Turn up `RELATION DEPTH`, open `RELATIONS` and edit the 5×5 matrix to couple Pitch, Time, FM, Resonance and Tuning.
5. Pick a `SCALE` + `ROOT` to quantize to a musical scale, or leave it on `OFF (FREE)` for microtonal/chaotic pitch.
6. `MODE` switches POLY (8 voices) / MONO LEGATO (single voice with `GLIDE`). `LFO >` chooses what the LFO modulates.

Parameter ranges and defaults: see [PARAMETER_REFERENCE.md](PARAMETER_REFERENCE.md).

### 1. RELATIONAL SEQUENCER

The sequencer is an event system rather than a conventional row of fixed notes. Each event can contain:

- position
- pitch
- velocity
- probability
- FM ratio
- FM index
- tuning offset
- timing offset

The base event pattern and the five relational phase cycles are separate concepts.

### 2. FIVE RELATIONAL CYCLES

The sequencer exposes five independent phase lengths:

| Domain | Meaning |
|---|---|
| RHYTHM | Determines which rhythmic event slot is active |
| PITCH | Selects the pitch source position |
| FM | Selects the spectral/FM source position |
| TUNING | Selects the tuning-offset source position |
| MOTION | Selects timing/motion source position |

Example:

`RHYTHM 13 / PITCH 7 / FM 5 / TUNING 11 / MOTION 17`

The system does not collapse these values to the event-list length. A cycle length is a phase period.

### 3. RELATIONAL PERIOD

The UI reports the least common multiple of the five cycle lengths.

For the example above:

`LCM(13, 7, 5, 11, 17) = 85085`

This is the number of phase advances required before all five cycle phases return to their initial alignment.

The displayed period is therefore a structural measurement of the sequence system, not a claim about the literal number of unique audio events.

### 4. CYCLE STRIPS

The five compact horizontal tracks in the sequencer are both visualization and controls.

The horizontal position selects a cycle length from 1 to 32.

Each track shows:

- its name;
- its current length;
- discrete reference ticks;
- a length marker;
- the current phase.

Drag horizontally to change a cycle.

### 5. FIELD / GRID / RADIAL

**FIELD** shows time horizontally and pitch vertically.

**GRID** emphasizes discrete event positions and active/inactive state.

**RADIAL** represents event position as phase around a cycle.

The three modes operate on the same event data.

### 6. EUCLID

EUCLID distributes active pulses across the current base sequence.

It is a pattern generator, not the identity of the instrument.

After generation, the events can still be shaped by the five relational cycles.

### 7. GENERATIVE PRESET

Holding Shift while using the sequence generation control invokes the experimental generative preset.

The preset demonstrates:

- 11 pulses over 16 steps;
- non-integer FM ratios;
- moving FM index;
- probability;
- tuning offsets;
- timing offsets;
- divergent relational cycles.

### 8. TUNING

Relational EX separates tuning structure from keyboard mapping.

Supported architecture includes:

- equal temperaments;
- Scala;
- KBM;
- MIDI Tuning Standard;
- MTS-ESP.

The Frequency Field represents interval relationships visually and permits direct editing.

### 9. RELATIONAL MATRIX

The Relational Coupling Matrix contains five domains:

`PITCH / TIME / FM / RESONANCE / TUNING`

Columns are sources. Rows are destinations.

A link can have negative or positive influence.

The matrix is published to the realtime engine through an immutable snapshot.

### 10. SOUND ENGINE

The core synthesis path combines:

`TUNING → FM → INHARMONIC RESONANCE`

with modulation from:

`LFO + CHAOS + RELATIONAL ENGINE`

The intent is not to emulate a specific conventional synthesizer architecture. The goal is to make relationships between domains audible.

### 11. MIDI / TEST INPUT

The Standalone provides:

- external MIDI;
- on-screen keyboard;
- computer keyboard entry;
- MIDI monitor.

The monitor reports note, velocity, gate and frequency.

### 12. TOOLtips

Interactive controls provide English explanations on mouse-over.

The explanations should describe:

1. what the control changes;
2. its role in the larger system;
3. the musical consequence of increasing or decreasing it.

### 13. SAVED STATE

Plugin state stores the synthesis parameters, tuning state, event sequence and relational matrix.

Relational cycle lengths are stored with the sequencer state.

Older states without cycle metadata fall back to:

`RHYTHM = sequence length`

`PITCH = 7`

`FM = 5`

`TUNING = 11`

`MOTION = 17`

### 14. DESIGN PRINCIPLE

The primary scope guardrail remains:

> Does this feature deepen the relationship between pitch, tuning, rhythm, spectrum or time?

Features that do not strengthen that relationship should normally remain outside Relational EX.

### 15. CREDITS

Relational EX  
Extasis Records  
coded by @laurorobles


## RELATIONAL MOTION & PHASE ALIGNMENT

The sequencer now treats RHYTHM, PITCH, FM, TUNING and MOTION as independent phase domains. Their lengths can differ intentionally.

### RELATIONAL PERIOD

The RELATIONAL PERIOD is the least common multiple of the five cycle lengths. It describes the full structural recurrence of the phase system. A very large value is expected when the selected periods are mutually prime. It does not mean that the plugin literally stores that many events.

### PHASE

The PHASE indicator shows the normalized position inside the current combined relational period. It wraps from 99% back to 0%.

### TO ALIGN

TO ALIGN reports the number of phase advances remaining until all five cycle phases return to their initial alignment.

### COMPLEXITY

COMPLEXITY is a descriptive logarithmic metric of the selected cycle lengths. It is intended as a visual indication of how much periodic structure has been introduced, not as an audio parameter.

### CYCLE STRIPS

Each strip is both a phase-length control and a live phase display. Click/drag horizontally to select a value from 1 to 32. The illuminated phase marker shows the current position of that domain.

### RELATIONAL AUDIO BEHAVIOR

The global sequencer phase now enters the Relational Engine's TIME domain. PITCH and TUNING phase also become normalized relation sources. The FM destination can continuously bend the active voice's ratio and index, while TUNING can create a bounded continuous pitch displacement.

This is the intended design philosophy of Relational EX: a control can alter the relationship between domains rather than simply acting as an isolated knob.

## Sequencer interaction

The Relational Sequencer is divided into two interactive layers and one status layer.

### Phase tracks

The five phase tracks are editable directly. Click or drag horizontally over RHYTHM, PITCH, FM, TUNING, or MOTION to choose a cycle length from 1 to 32. These are cycle lengths, not necessarily the number of stored events.

### Event canvas

The large lower canvas is always interactive. There is no requirement to click directly on a dot. In FIELD and RADIAL views, clicking anywhere in the canvas selects the nearest event so the user can immediately drag it. In GRID view, clicking a cell toggles its active state.

FIELD: horizontal movement changes event time; vertical movement changes pitch.

GRID: clicking toggles the selected step; vertical dragging changes pitch.

RADIAL: angular movement changes event phase around the cycle.

Shift-click toggles an event in FIELD and RADIAL views.

The bottom status strip is intentionally non-editable. It reports interaction instructions and the currently selected event so it cannot be confused with the event canvas.

