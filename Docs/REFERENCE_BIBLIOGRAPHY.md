# Relational EX — Technical & Conceptual References

This bibliography is the canonical record of the external ideas, standards, software and research used to shape Relational EX.

## Standards and interoperability

### MIDI Association — MIDI Tuning Standard
MIDI Polyphonic Expression and MIDI Tuning Standard specifications are the normative references for MIDI-based retuning. Relational EX uses MTS concepts for local SysEx handling and for interoperability with external tuning masters.

Primary reference: https://midi.org/specifications-old/item/the-midi-1-0-specification

### ODDSound — MTS-ESP
The MTS-ESP client API is the interoperability reference for master/client tuning relationships. The official client guidance requires registration/deregistration, note-frequency queries, channel-aware retuning where possible, optional note filtering for unmapped notes and MTS SysEx parsing. It also recommends continuous retuning while notes are held when the instrument architecture permits it.

Primary references:
- https://github.com/ODDSound/MTS-ESP
- https://github.com/ODDSound/MTS-ESP/blob/main/Client/libMTSClient.h

### Scala tuning ecosystem
Scala `.scl` files are the principal human-readable tuning interchange format used in Relational EX. `.kbm` files describe MIDI keyboard mapping for Scala tunings.

Primary reference: https://www.huygens-fokker.org/scala/

## Tuning theory and composition

### Wilsonic
Wilsonic is used as a conceptual and visualization reference for multidimensional tuning exploration. Relational EX does not embed Wilsonic and is not a clone. Its geometric thinking informs our Frequency Field and tuning laboratory.

Primary reference: https://github.com/wilsonic/ios

### Harry Partch
Partch's just-intonation theory and ratio-oriented thinking are important conceptual references for the idea that pitch can be represented as relationships rather than twelve fixed chromatic classes.

Reference: Harry Partch, *Genesis of a Music*, Da Capo Press.

### Ben Johnston / extended just intonation
Extended just intonation is a reference for thinking about interval ratios beyond conventional twelve-tone notation.

Reference: https://www.kylegann.com/tuning.html

## Digital synthesis

### John Chowning — FM synthesis
Frequency modulation synthesis is foundational to Relational EX. The project emphasizes non-integer carrier/modulator ratios because they change sideband relationships and perceived spectral identity.

Reference: John M. Chowning, “The Synthesis of Complex Audio Spectra by Means of Frequency Modulation,” *Journal of the Audio Engineering Society*, 1973.

### Yamaha FM architecture
The DX family is a historical reference for ratio-driven digital spectra, operator feedback and compact FM parameterization.

Reference: Yamaha, *FM Synthesis* / DX Series manuals.

### Physical/modal resonance
The resonator layer is informed by modal synthesis concepts and by the idea that inharmonic partial relationships can imply the identity of physical or imagined objects without reproducing a specific physical model.

Reference: Julius O. Smith, *Physical Audio Signal Processing*.

Primary reference: https://ccrma.stanford.edu/~jos/pasp/

## Algorithmic rhythm and sequencing

### Euclidean rhythms / Bjorklund algorithm
The Euclidean sequencer is inspired by the even-distribution principle commonly associated with the Bjorklund algorithm. Relational EX treats it as one source of event activation rather than the complete definition of the sequencer.

Reference: Erik D. Bjorklund, “The Theory of Time Division By the Euclidean Algorithm,” Los Alamos National Laboratory.

### Autechre
Autechre is a conceptual reference for procedural composition, custom sequencing systems, algorithmic transformation, unusual timing relationships and the use of software environments as instruments. Relational EX does not attempt to reproduce Autechre's sound or instruments.

Reference archive: https://autechre.neocities.org/

## Historical microtuning practice in electronic music

### Richard D. James / Aphex Twin
Richard D. James is an important reference for practical use of custom tunings, Scala and hardware microtuning. This informs Relational EX's emphasis on tunings as an active musical material rather than a technical afterthought.

Reference: Korg / Tatsuya Takahashi interviews and public documentation surrounding the Monologue's microtuning implementation.

## Software architecture

### JUCE
JUCE is the application and plugin framework for Relational EX. The project uses its VST3 and Standalone targets and its DSP/UI abstractions.

Primary reference: https://juce.com/

### CMake
CMake is used to make the build reproducible on the development Mac and in GitHub Actions.

Primary reference: https://cmake.org/documentation/

### GitHub Actions
GitHub Actions provides the repository's macOS ARM64 build verification. GitHub currently documents standard ARM64 macOS hosted runners including `macos-14`, `macos-15` and `macos-latest`; Relational EX pins CI to `macos-14` for a conservative Apple Silicon baseline.

Primary reference: https://docs.github.com/en/actions/reference/runners/github-hosted-runners

## Scale database sources

### scale-library
A public collection of Scala scales used as a source for research/indexing. Relational EX must audit provenance and licenses before redistributing third-party scale files in a commercial release.

Primary reference: https://github.com/narenratan/scale-library

## Reference policy

References are divided into four roles:

1. **Normative** — standards/APIs that the implementation must follow.
2. **Technical** — algorithms and DSP literature that inform implementation.
3. **Conceptual** — instruments, artists and systems that influence design direction.
4. **Research corpus** — public data collections used for exploration and indexing.

A reference never becomes an automatic implementation requirement. Relational EX's scope is defined by the project's own architecture and decision log.
