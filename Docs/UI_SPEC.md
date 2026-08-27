# Relational EX — UI Specification

## Design intent

Relational EX should feel like an experimental instrument panel rather than a generic software synthesizer. References include scientific visualization, early computer music interfaces, modular hardware, tuning maps and contemporary experimental electronic instruments, but the visual identity must remain original.

## Layout

The interface uses a functional modular grid:

```text
┌──────────────────────────────────────────────────────────────┐
│ IDENTITY / PRESETS / TUNING / TRANSPORT / STATUS            │
├──────────────────────────────┬───────────────────────────────┤
│                              │                               │
│ FREQUENCY FIELD              │ SPECTRAL / FM MAP             │
│                              │                               │
├──────────────────────────────┴───────────────────────────────┤
│ CONTROL STRIP: FM / MOTION / RESONANCE / CLOCK              │
├──────────────────────────────────────────────────────────────┤
│ RELATIONAL SEQUENCER                                         │
│ FIELD / GRID / RADIAL                                        │
└──────────────────────────────────────────────────────────────┘
```

## Visual language

- dark technical substrate
- fine alignment lines
- restrained warm/cool accent hierarchy
- compact typography
- hard-edged or lightly squared module frames
- numerical values remain legible
- activity is represented by motion, traces and geometric state rather than decorative gradients

## Empty-space rule

No region exists only to look expensive. Unused area should either provide touch/mouse room, preserve hierarchy, expose state, or support future controls that are already architecturally defined.

## Tooltips

Every interactive parameter and action must have English hover help. Descriptions should explain the behavior and the musical consequence.

## Frequency Field

Primary tuning visualization. X = degree order, Y = interval/cents displacement. Points are draggable and commit changes to the realtime tuning snapshot.

## Spectrum / FM map

The map emphasizes modal ratios and FM relationships rather than presenting a generic FFT analyzer.

## Sequencer views

### FIELD
Continuous event space. X = time, Y = relative pitch.

### GRID
Discrete temporal matrix. Best for enabling/disabling events.

### RADIAL
Angle = cycle position. Radius can encode pitch/energy relationship. Intended for cyclic patterns and later polymetric visualization.

## Preset browser

Presets should be grouped by conceptual behavior: Tuning, FM, Motion, Resonant, Percussive, Extreme. Names are prompts, not genre labels.

## Accessibility

Values must remain readable at the minimum supported window size. Mouse-over explanations must not be the only way to understand a control; labels and values stay visible.
