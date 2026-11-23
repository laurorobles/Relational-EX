# Relational EX — Parameter Reference

**Version:** 0.8.0 · *Extasis Records — coded by @laurorobles*

All parameters are host-automatable and saved with the session. **Parameter IDs are stable — never rename them.**

## Sound

| Label | ID | Range | Default | Notes |
|---|---|---|---|---|
| FM Ratio | `fmRatio` | 0.01–8 | 1.618 | Modulator/carrier ratio; non-integer = inharmonic sidebands |
| FM Index | `fmIndex` | 0–20 | 2.0 | FM depth (spectral density) |
| FM Feedback | `fmFeedback` | 0–0.95 | 0.08 | Modulator self-feedback (grit) |
| FM Continuous | `fmContinuous` | on/off | on | on: global ratio/index follow knobs live; off: latched per note |
| Inharmonicity | `inharmonicity` | 0–1 | 0.25 | Resonator partial stretch |
| Resonance | `resonance` | 0–1 | 0 | Resonator level / excitation |
| Tuning Drift | `tuningDrift` | 0–30 | 0 | Random pitch micro-drift |
| Cutoff | `filterCutoff` | 20–20000 Hz | 20000 | Low-pass filter (SVF TPT) |
| Output Level | `level` | 0–1 | 0.72 | Master level (soft-clipped with tanh) |

## Envelope

| Label | ID | Range | Default |
|---|---|---|---|
| Attack | `envAttack` | 0.001–5 s | 0.01 |
| Decay | `envDecay` | 0.001–5 s | 0.3 |
| Sustain | `envSustain` | 0–1 | 0.7 |
| Release | `envRelease` | 0.001–10 s | 0.8 |

## Voice mode & scale

| Label | ID | Range | Default | Notes |
|---|---|---|---|---|
| Mode | `synthMode` | POLY / MONO LEGATO | POLY | Mono legato reuses one voice and glides |
| Glide | `glideTime` | 0–2 s | 0.1 | Only active in MONO LEGATO |
| Scale | `scaleType` | OFF (FREE), Chromatic, Major, Minor, Harmonic Minor, Dorian, Phrygian, Lydian, Mixolydian, Pentatonic Major, Pentatonic Minor | OFF | Quantizes sequencer pitch and relational tuning drift. OFF = fully free |
| Root | `rootNote` | 0–11 (C…B) | 0 | Tonic of the selected scale |

## Modulation

| Label | ID | Range | Default | Notes |
|---|---|---|---|---|
| LFO Rate | `lfoRate` | 0.01–20 Hz | 0.35 | |
| LFO Amount | `lfoAmount` | 0–100 | 0 | Depth toward the selected target |
| LFO Target | `lfoTarget` | Relations / FM Ratio / FM Index / Cutoff | Relations | Cutoff mod spans ±3 octaves at 100 |
| Chaos | `chaosAmount` | 0–30 | 0 | Deterministic chaos: perturbs relations, event probability and timing |
| Relation Depth | `relationDepth` | 0–1 | 0.65 | Global scale of the 5×5 matrix |

## Sequencer & tuning

| Label | ID | Range | Default | Notes |
|---|---|---|---|---|
| BPM | `bpm` | 40–240 | 120 | Internal clock (16th-note steps). Host transport start resets the sequence |
| Steps | `seqSteps` | 1–32 | 16 | Euclidean length |
| Pulses | `seqPulses` | 0–32 | 8 | Euclidean pulses |
| Rhythm / Pitch / FM / Tuning / Motion Cycle | `rhythmCycle` `pitchCycle` `fmCycle` `tuningCycle` `motionCycle` | 1–32 | 13 / 7 / 5 / 11 / 17 | Independent phase lengths; coprime lengths align only every LCM steps |
| Tuning Divisions | `tuningDivisions` | 1–53 | 12 | Equal division of the octave |
| Root Frequency | `rootHz` | 20–1000 Hz | 440 | Reference frequency |

## Relation matrix (state, not automatable)

5×5 signed matrix (Pitch, Time, FM, Resonance, Tuning), each cell −1…+1, edited in the `RELATIONS` overlay and stored in plugin state.

## Sequencer events (state)

Per event: position (0–1), pitch (semitones around middle C), ratio, velocity, probability, duration, FM index, tuning offset (cents), timing offset.
