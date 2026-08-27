# Relational EX Parameter Reference

This is the canonical parameter reference for the development build. Parameter IDs are stable only within the current development series and may be versioned before the first public release.

## Global / voice parameters

| Parameter | Range | Function | Musical interpretation |
|---|---:|---|---|
| Output Level | 0–1 | Final gain after summing/saturation | Overall level and headroom |
| FM Ratio | 0.01–8 | Modulator frequency divided by carrier frequency | Controls harmonic vs inharmonic sideband relationships |
| FM Index | 0–20 | FM deviation depth | Controls spectral density |
| FM Feedback | 0–0.95 | Feeds output into phase | Adds density, grit and instability |
| Tuning Drift | 0–30 | Adds tiny continuously changing pitch displacement | From stable to unstable pitch field |
| Inharmonicity | 0–1 | Expands resonator modal ratios away from harmonic multiples | From harmonic body to metallic/object-like resonance |
| Resonance | 0–1 | Mixes six-mode resonator | Adds ringing body and decay |
| LFO Rate | 0.01–20 Hz | Motion source rate | Slow drift through rapid vibrato-like motion |
| LFO Amount | 0–100 | LFO modulation depth | Amount of periodic phase movement |
| Chaos | 0–30 | Deterministic nonlinear modulation depth | From periodic to less predictable motion |
| Relation Depth | 0–1 | Global depth of the five-domain coupling model | How strongly pitch/time/FM/resonance/tuning influence one another |
| BPM | 40–240 | Internal clock | Sets the sequence tempo |
| Sequence Steps | 1–32 | Current cycle length | Short cycles or long phase relationships |
| Sequence Pulses | 0–32 | Euclidean hit count | Controls density of active events |
| Tuning Divisions | 1–53 | Equal divisions of the configured period | Pitch-field resolution |
| Root Frequency | 20–1000 Hz | Reference frequency | Base point for the internal tuning |

## Event parameters

Each relational event stores `position`, `pitch`, `ratio`, `velocity`, `probability`, `fmIndex`, `tuningOffsetCents`, `timingOffset`, and `active`.

The defining principle is that an event is not merely a note. It is a bundle of temporal, spectral and tuning relationships.

## Current control IDs

`level`, `fmRatio`, `fmIndex`, `fmFeedback`, `tuningDrift`, `inharmonicity`, `resonance`, `lfoRate`, `lfoAmount`, `chaosAmount`, `relationDepth`, `bpm`, `seqSteps`, `seqPulses`, `tuningDivisions`, `rootHz`.

## Tooltip standard

Every interactive control must have an English tooltip that explains behavior rather than repeating the label. For complex parameters it should also explain the musical consequence of increasing or decreasing the value.
