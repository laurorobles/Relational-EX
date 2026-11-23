# Relational EX v0.7 — Relational Motion

## Purpose

v0.7 turns the five independent sequencer periods into a continuous relational motion layer.

### Five domains
- RHYTHM — event occurrence / base time
- PITCH — pitch source phase
- FM — spectral relationship phase
- TUNING — microtonal offset phase
- MOTION — timing displacement phase

## Global relational phase

The sequencer maintains a monotonic `cycleTick` and derives a normalized global phase from the least-common-multiple period of the five cycle lengths.

`0.0` means the five phase domains are aligned. `1.0` is never displayed; phase wraps back to `0.0`.

## Alignment

`combinedCycleLength()` is the mathematical least common multiple of the five cycle lengths. `stepsUntilAlignment()` reports how many phase advances remain until all domains coincide again.

## Complexity

`normalizedComplexity()` compresses the logarithmic size of the five cycle lengths into a 0..1 display value. It is a descriptive metric, not a DSP control.

## Audio relationship

The audio engine samples global/time/pitch/tuning phases into the Relational Engine. The FM result modulates ratio and index around the event's base values. Tuning relation produces a bounded continuous retuning offset.

## Event correctness

A sequencer advance captures the phase frame before advancing. The event resolver uses that captured frame, so the event played at a boundary corresponds to the phase visible for that boundary rather than the already-incremented frame.

## Scope decision

v0.7 does not introduce a new oscillator family, sampler, granular engine, or effect suite. The goal is deeper interaction among existing domains.

**Extasis Records**  
*coded by @laurorobles*
