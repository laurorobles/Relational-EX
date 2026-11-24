# Relational EX — DSP Model

## Signal path

```text
MIDI / Sequencer Event
   ↓
Scale Quantization (optional) & Tuning Resolution (MTS-ESP → MTS SysEx → Internal Snapshot)
   ↓
Voice Allocation (8-voice Poly or single-voice Mono Legato with exponential Glide)
   ↓
2-Op Continuous FM Oscillator (Ratio, Index, Feedback + Relational & LFO modulation)
   ↓
Optional 6-Mode Inharmonic Resonator
   ↓
Per-Voice ADSR Envelope
   ↓
Per-Voice SVF TPT Low-Pass Filter (20 Hz – 20 kHz, optional ±3 oct LFO mod)
   ↓
Voice Sum → Soft Saturation (tanh) → Master Level
```

## FM Oscillator

Carrier frequency is resolved from the active tuning system (plus per-event cents offset, MPE pitch bend, and optional exponential portamento in `MONO LEGATO` mode). Modulator frequency is `carrier × ratio`, smoothed per sample with one-pole filters. Ratios are continuous and intentionally unrestricted to integer multiples; feedback routes the oscillator output back into phase.

## Inharmonic Resonator

Six modal bandpass/ringing stages whose partial ratios interpolate away from harmonic multiples according to `inharmonicity` (`0.0–1.0`), adding metallic or object-like body without imitating a fixed acoustic instrument.

## Envelope & Filter

Each voice runs an independent `juce::ADSR` (`envAttack`, `envDecay`, `envSustain`, `envRelease`) and a zero-delay feedback `juce::dsp::StateVariableTPTFilter` configured as a low-pass filter (`filterCutoff`). In `MONO LEGATO` mode, overlapping notes update the glide target without retriggering the ADSR, FM phase, resonator, or filter state.

## Modulation & Relational Coupling

- **LFO:** periodic modulator routable via `lfoTarget` to `RELATIONS` (default), `FM RATIO`, `FM INDEX`, or `CUTOFF`.
- **Chaos:** deterministic nonlinear drift perturbing tuning phase, sequencer probability, and microtiming.
- **Relational Engine:** slewed 5×5 signed coupling across Pitch, Time, FM, Resonance, and Tuning scaled by `relationDepth`.
