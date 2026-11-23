#include "PluginProcessor.h"
#include "PluginEditor.h"

#include <algorithm>
#include <cmath>
#include <filesystem>
#include <limits>
#include <random>

namespace
{
float param(RelationalEXProcessor& p, const char* id, float fallback)
{
    if (auto* v = p.apvts.getRawParameterValue(id))
        return v->load();

    return fallback;
}

double equalTemperamentFrequency(int midiNote)
{
    return 440.0 * std::pow(2.0,
        (static_cast<double>(midiNote) - 69.0) / 12.0);
}
}

RelationalEXProcessor::RelationalEXProcessor()
    : AudioProcessor(
        BusesProperties()
            .withOutput(
                "Output",
                juce::AudioChannelSet::stereo(),
                true)),
      apvts(
          *this,
          &undoManager,
          "APVTS",
          createParameterLayout())
{
    tuning.setEqualTemperament(12);
    tuning.setRootFrequency(440.0);
    tuningEditBuffer_ = tuning.points();
    refreshTuningSnapshot();

    mts.registerClient();

    sequencer.setLength(16);
    sequencer.applyEuclidean(8, 0);
    sequencer.publishToAudio();

    scanScaleDatabase();
    applyFactoryPreset(0);
}

RelationalEXProcessor::~RelationalEXProcessor()
{
    mts.deregisterClient();
}

juce::AudioProcessorValueTreeState::ParameterLayout
RelationalEXProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> p;

    p.push_back(std::make_unique<juce::AudioParameterInt>(
        "rootNote", "Root Note", 0, 11, 0));

    juce::StringArray scales {
        "OFF (FREE)", "CHROMATIC", "MAJOR", "MINOR", "HARMONIC MINOR", 
        "DORIAN", "PHRYGIAN", "LYDIAN", "MIXOLYDIAN", 
        "PENTATONIC MAJOR", "PENTATONIC MINOR"
    };
    p.push_back(std::make_unique<juce::AudioParameterChoice>(
        "scaleType", "Scale Type", scales, 0));

    juce::StringArray modes { "POLY", "MONO LEGATO" };
    p.push_back(std::make_unique<juce::AudioParameterChoice>(
        "synthMode", "Synth Mode", modes, 0));

    p.push_back(std::make_unique<juce::AudioParameterFloat>(
        "glideTime", "Glide Time",
        juce::NormalisableRange<float>(0.0f, 2.0f, 0.001f, 0.3f), 0.1f));

    juce::StringArray lfoTargets { "RELATIONS", "FM RATIO", "FM INDEX", "CUTOFF" };
    p.push_back(std::make_unique<juce::AudioParameterChoice>(
        "lfoTarget", "LFO Target", lfoTargets, 0));

    p.push_back(std::make_unique<juce::AudioParameterFloat>(
        "envAttack", "Attack",
        juce::NormalisableRange<float>(0.001f, 5.0f, 0.001f, 0.3f), 0.01f));
        
    p.push_back(std::make_unique<juce::AudioParameterFloat>(
        "envDecay", "Decay",
        juce::NormalisableRange<float>(0.001f, 5.0f, 0.001f, 0.3f), 0.3f));
        
    p.push_back(std::make_unique<juce::AudioParameterFloat>(
        "envSustain", "Sustain",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.7f));
        
    p.push_back(std::make_unique<juce::AudioParameterFloat>(
        "envRelease", "Release",
        juce::NormalisableRange<float>(0.001f, 10.0f, 0.001f, 0.3f), 0.8f));
        
    p.push_back(std::make_unique<juce::AudioParameterFloat>(
        "filterCutoff", "Cutoff",
        juce::NormalisableRange<float>(20.0f, 20000.0f, 1.0f, 0.3f), 20000.0f));

    p.push_back(std::make_unique<juce::AudioParameterFloat>(
        "level", "Output Level",
        juce::NormalisableRange<float>(0.0f, 1.0f), 0.72f));

    p.push_back(std::make_unique<juce::AudioParameterFloat>(
        "fmRatio", "FM Ratio",
        juce::NormalisableRange<float>(0.01f, 8.0f, 0.001f, 0.45f), 1.618f));

    p.push_back(std::make_unique<juce::AudioParameterFloat>(
        "fmIndex", "FM Index",
        juce::NormalisableRange<float>(0.0f, 20.0f, 0.01f, 0.6f), 2.0f));

    p.push_back(std::make_unique<juce::AudioParameterFloat>(
        "fmFeedback", "FM Feedback",
        juce::NormalisableRange<float>(0.0f, 0.95f, 0.001f, 0.6f), 0.08f));

    p.push_back(std::make_unique<juce::AudioParameterFloat>(
        "tuningDrift", "Tuning Drift",
        juce::NormalisableRange<float>(0.0f, 30.0f, 0.01f, 0.7f), 0.0f));

    p.push_back(std::make_unique<juce::AudioParameterFloat>(
        "inharmonicity", "Inharmonicity",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.001f, 0.7f), 0.25f));

    p.push_back(std::make_unique<juce::AudioParameterFloat>(
        "resonance", "Resonance",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.001f, 0.6f), 0.0f));

    p.push_back(std::make_unique<juce::AudioParameterFloat>(
        "bpm", "BPM",
        juce::NormalisableRange<float>(40.0f, 240.0f, 0.1f), 120.0f));

    p.push_back(std::make_unique<juce::AudioParameterBool>(
        "fmContinuous", "FM Continuous", true));

    p.push_back(std::make_unique<juce::AudioParameterInt>(
        "seqSteps", "Sequence Steps", 1, 32, 16));

    p.push_back(std::make_unique<juce::AudioParameterInt>(
        "seqPulses", "Sequence Pulses", 0, 32, 8));

    p.push_back(std::make_unique<juce::AudioParameterInt>(
        "rhythmCycle", "Rhythm Cycle", 1, 32, 13));

    p.push_back(std::make_unique<juce::AudioParameterInt>(
        "pitchCycle", "Pitch Cycle", 1, 32, 7));

    p.push_back(std::make_unique<juce::AudioParameterInt>(
        "fmCycle", "FM Cycle", 1, 32, 5));

    p.push_back(std::make_unique<juce::AudioParameterInt>(
        "tuningCycle", "Tuning Cycle", 1, 32, 11));

    p.push_back(std::make_unique<juce::AudioParameterInt>(
        "motionCycle", "Motion Cycle", 1, 32, 17));

    p.push_back(std::make_unique<juce::AudioParameterInt>(
        "tuningDivisions", "Tuning Divisions", 1, 53, 12));

    p.push_back(std::make_unique<juce::AudioParameterFloat>(
        "rootHz", "Root Frequency",
        juce::NormalisableRange<float>(20.0f, 1000.0f, 0.01f, 0.5f), 440.0f));

    p.push_back(std::make_unique<juce::AudioParameterFloat>(
        "lfoRate", "LFO Rate",
        juce::NormalisableRange<float>(0.01f, 20.0f, 0.001f, 0.5f), 0.35f));

    p.push_back(std::make_unique<juce::AudioParameterFloat>(
        "lfoAmount", "LFO Amount",
        juce::NormalisableRange<float>(0.0f, 100.0f, 0.01f, 0.7f), 0.0f));

    p.push_back(std::make_unique<juce::AudioParameterFloat>(
        "chaosAmount", "Chaos",
        juce::NormalisableRange<float>(0.0f, 30.0f, 0.01f, 0.7f), 0.0f));

    p.push_back(std::make_unique<juce::AudioParameterFloat>(
        "relationDepth", "Relation Depth",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.001f, 0.7f), 0.65f));

    return { p.begin(), p.end() };
}

void RelationalEXProcessor::prepareToPlay(double sr, int)
{
    sampleRate_ = std::max(1000.0, sr);
    sequenceSampleCounter_ = 0.0;

    for (auto& voice : voices_)
    {
        voice.fm.prepare(sampleRate_);
        voice.resonator.prepare(sampleRate_);
        voice.adsr.setSampleRate(sampleRate_);
        
        juce::dsp::ProcessSpec spec { sampleRate_, 1024, 1 }; // Rough max block size
        voice.filter.prepare(spec);
        voice.filter.setType(juce::dsp::StateVariableTPTFilterType::lowpass);
        
        voice.note = -1;
        voice.active = false;
        voice.releasing = false;
        voice.envelope = 0.0f;
        voice.velocity = 0.0f;
    }

    lfo_.prepare(sampleRate_);
    chaos_.reset();
    
    for (int i = 0; i < 16; ++i)
        channelPitchBend_[i] = 0.0f;
}

void RelationalEXProcessor::releaseResources()
{
}

bool RelationalEXProcessor::isBusesLayoutSupported(
    const BusesLayout& layouts) const
{
    return layouts.getMainOutputChannelSet()
        == juce::AudioChannelSet::stereo();
}

double RelationalEXProcessor::bpm() const noexcept
{
    if (auto* v = apvts.getRawParameterValue("bpm"))
        return v->load();

    return 120.0;
}

void RelationalEXProcessor::updateMidiMonitor(
    int midiNote,
    int channel,
    float velocity,
    bool gate) noexcept
{
    monitoredMidiNote_.store(
        midiNote,
        std::memory_order_relaxed);

    monitoredVelocity_.store(
        velocity,
        std::memory_order_relaxed);

    monitoredFrequency_.store(
        effectiveFrequency(midiNote, channel, 0.0),
        std::memory_order_relaxed);

    monitoredGate_.store(
        gate,
        std::memory_order_relaxed);
}

void RelationalEXProcessor::handleMidiMessage(
    const juce::MidiMessage& message)
{
    if (message.isSysEx())
    {
        const auto* raw = message.getSysExData();
        const auto size =
            static_cast<std::size_t>(
                message.getSysExDataSize());

        if (raw != nullptr && size > 0)
        {
            mts.parseMidiData(
                reinterpret_cast<const unsigned char*>(raw),
                static_cast<int>(size));

            midiTuning.parseSysEx(raw, size);
            refreshTuningSnapshot();
        }

        return;
    }

    if (message.isNoteOn())
    {
        const int note = message.getNoteNumber();
        const int channel = message.getChannel();
        const float velocity = message.getFloatVelocity();

        if (mts.available()
            && mts.shouldFilterNote(note, channel - 1))
        {
            updateMidiMonitor(note, channel, velocity, true);
            return;
        }

        triggerVoice(note, channel, velocity);
        updateMidiMonitor(note, channel, velocity, true);
        return;
    }

    if (message.isNoteOff())
    {
        const int note = message.getNoteNumber();
        const int channel = message.getChannel();

        releaseVoice(note, channel);

        monitoredMidiNote_.store(
            note,
            std::memory_order_relaxed);

        monitoredGate_.store(
            false,
            std::memory_order_relaxed);
            
        return;
    }

    if (message.isAllNotesOff() || message.isAllSoundOff())
    {
        for (auto& voice : voices_)
        {
            if (voice.active && !voice.releasing)
            {
                voice.releasing = true;
                voice.adsr.noteOff();
            }
        }
        monitoredGate_.store(false, std::memory_order_relaxed);
        return;
    }

    if (message.isPitchWheel())
    {
        const int channel = message.getChannel();
        if (channel >= 1 && channel <= 16) {
            // map 0-16383 to -1.0 to 1.0
            channelPitchBend_[channel - 1] = (message.getPitchWheelValue() - 8192) / 8192.0f;
        }
    }
}

double RelationalEXProcessor::frequencyFromSnapshot(
    int midiNote) const noexcept
{
    midiNote = std::clamp(midiNote, 0, 127);

    const auto snapshot =
        std::atomic_load_explicit(
            &tuningSnapshot_,
            std::memory_order_acquire);

    if (!snapshot)
        return equalTemperamentFrequency(midiNote);

    return (*snapshot)[
        static_cast<std::size_t>(midiNote)];
}

void RelationalEXProcessor::refreshTuningSnapshot() noexcept
{
    auto snapshot =
        std::make_shared<TuningSnapshot>();

    for (int note = 0; note < 128; ++note)
    {
        (*snapshot)[static_cast<std::size_t>(note)] =
            tuning.frequencyForMidi(note);
    }

    std::shared_ptr<const TuningSnapshot> immutable =
        std::move(snapshot);

    std::atomic_store_explicit(
        &tuningSnapshot_,
        std::move(immutable),
        std::memory_order_release);
}

int RelationalEXProcessor::snapToScale(int note, int root, int scaleIndex, int degreeShift) const noexcept
{
    struct ScaleDef { int size; std::array<int, 12> degrees; };
    static constexpr std::array<ScaleDef, 10> scales {{
        { 12, { 0,1,2,3,4,5,6,7,8,9,10,11 } }, // CHROMATIC
        {  7, { 0,2,4,5,7,9,11 } },            // MAJOR
        {  7, { 0,2,3,5,7,8,10 } },            // MINOR
        {  7, { 0,2,3,5,7,8,11 } },            // HARMONIC MINOR
        {  7, { 0,2,3,5,7,9,10 } },            // DORIAN
        {  7, { 0,1,3,5,7,8,10 } },            // PHRYGIAN
        {  7, { 0,2,4,6,7,9,11 } },            // LYDIAN
        {  7, { 0,2,4,5,7,9,10 } },            // MIXOLYDIAN
        {  5, { 0,2,4,7,9 } },                 // PENTA MAJOR
        {  5, { 0,3,5,7,10 } }                 // PENTA MINOR
    }};

    if (scaleIndex <= 0 || scaleIndex > static_cast<int>(scales.size()))
        return std::clamp(note + degreeShift, 0, 127); // OFF mode or out of bounds

    const auto& def = scales[static_cast<std::size_t>(scaleIndex - 1)];
    const int size = def.size;
    const auto& scale = def.degrees;
    int n = note - root;
    int octave = (n >= 0) ? (n / 12) : ((n - 11) / 12);
    int pc = n - octave * 12;

    int closestIndex = 0;
    int minDiff = 12;
    for (int i = 0; i < size; ++i) {
        int diff = std::abs(scale[i] - pc);
        if (diff < minDiff) {
            minDiff = diff;
            closestIndex = i;
        }
    }

    int finalIndex = closestIndex + degreeShift;
    int shiftOctaves = (finalIndex >= 0) ? (finalIndex / size) : ((finalIndex - (size - 1)) / size);
    int wrappedIndex = finalIndex - shiftOctaves * size;

    int finalPc = scale[static_cast<std::size_t>(wrappedIndex)];
    int finalNote = root + (octave + shiftOctaves) * 12 + finalPc;
    
    return std::clamp(finalNote, 0, 127);
}

double RelationalEXProcessor::effectiveFrequency(
    int midiNote,
    int channel,
    double centsOffset) const noexcept
{
    double hz = frequencyFromSnapshot(midiNote);

    if (mts.available() && mts.hasMaster())
    {
        hz = mts.noteFrequency(
            midiNote,
            channel - 1);
    }
    else if (midiTuning.hasTuning())
    {
        hz = midiTuning.frequencyForNote(midiNote);
    }

    hz *= std::pow(
        2.0,
        centsOffset / 1200.0);

    return std::clamp(hz, 1.0, 20000.0);
}

void RelationalEXProcessor::triggerVoice(
    int midiNote,
    int channel,
    float velocity,
    double forcedFrequency,
    int durationSamples)
{
    Voice* target = nullptr;
    const int synthMode = static_cast<int>(param(*this, "synthMode", 0));
    bool isLegatoTransition = false;

    if (synthMode == 1) // MONO LEGATO
    {
        target = &voices_[0];
        if (target->active && !target->releasing && target->envelope > 0.001f)
            isLegatoTransition = true;
    }
    else
    {
        for (auto& voice : voices_)
        {
            if (voice.active && voice.note == midiNote && voice.channel == channel)
            {
                target = &voice;
                break;
            }
        }

        if (target == nullptr)
        {
            for (auto& voice : voices_)
            {
                if (!voice.active)
                {
                    target = &voice;
                    break;
                }
            }
        }

        if (target == nullptr)
        {
            target = &*std::min_element(voices_.begin(), voices_.end(),
                [](const Voice& a, const Voice& b) { return a.envelope < b.envelope; });
        }
    }

    target->note = std::clamp(midiNote, 0, 127);
    target->channel = juce::jlimit(1, 16, channel);
    target->velocity = juce::jlimit(0.0f, 1.0f, velocity);
    target->active = true;
    target->releasing = false;
    target->isSequencer = false;
    target->tuningCents = 0.0;
    target->samplesUntilRelease = durationSamples;
    juce::ADSR::Parameters adsrParams;
    adsrParams.attack = param(*this, "envAttack", 0.01f);
    adsrParams.decay = param(*this, "envDecay", 0.3f);
    adsrParams.sustain = param(*this, "envSustain", 0.7f);
    adsrParams.release = param(*this, "envRelease", 0.8f);
    target->adsr.setParameters(adsrParams);
    
    const double newFreq = forcedFrequency > 0.0
            ? forcedFrequency
            : effectiveFrequency(target->note, target->channel, 0.0);

    target->targetFrequency = newFreq;

    if (!isLegatoTransition)
    {
        target->adsr.noteOn();
        target->frequency = newFreq;
        target->fm.setCarrier(target->frequency);

        const bool continuous = param(*this, "fmContinuous", 1.0f) > 0.5f;
        target->baseRatio = continuous ? 1.0f : static_cast<float>(std::max(0.001, forcedFrequency > 0.0 ? 1.0 : param(*this, "fmRatio", 1.618f)));
        target->baseIndex = continuous ? 1.0f : param(*this, "fmIndex", 2.0f);

        target->fm.setRatio(target->baseRatio);
        target->fm.setIndex(target->baseIndex);
        target->fm.setFeedback(param(*this, "fmFeedback", 0.08f));
        target->fm.reset();

        target->resonator.setFundamental(target->frequency);
        target->resonator.setInharmonicity(param(*this, "inharmonicity", 0.25f));
        target->resonator.setDamping(0.9935);
        target->resonator.reset();

        const float resonance = param(*this, "resonance", 0.0f);
        if (resonance > 0.001f)
            target->resonator.excite(resonance * target->velocity);
    }
}

void RelationalEXProcessor::releaseVoice(
    int midiNote,
    int channel)
{
    for (auto& voice : voices_)
    {
        if (voice.active
            && voice.note == midiNote
            && voice.channel == channel)
        {
            voice.releasing = true;
            voice.adsr.noteOff();
        }
    }
}

void RelationalEXProcessor::triggerTestNote(
    int midiNote,
    float velocity)
{
    // Called from the UI thread: never touch voices_ here. The event is queued
    // lock-free and consumed at the start of the next processBlock().
    pushTestEvent(std::clamp(midiNote, 0, 127), juce::jlimit(0.0f, 1.0f, velocity), true);
}

void RelationalEXProcessor::releaseTestNote(
    int midiNote)
{
    pushTestEvent(std::clamp(midiNote, 0, 127), 0.0f, false);
}

void RelationalEXProcessor::pushTestEvent(int midiNote, float velocity, bool noteOn) noexcept
{
    const auto scope = testEventFifo_.write(1);

    if (scope.blockSize1 > 0)
        testEvents_[static_cast<std::size_t>(scope.startIndex1)] = { midiNote, velocity, noteOn };
    else if (scope.blockSize2 > 0)
        testEvents_[static_cast<std::size_t>(scope.startIndex2)] = { midiNote, velocity, noteOn };
}

void RelationalEXProcessor::drainTestEvents()
{
    const auto scope = testEventFifo_.read(testEventFifo_.getNumReady());

    auto handle = [this](const TestEvent& e)
    {
        if (e.noteOn)
        {
            triggerVoice(e.note, 1, e.velocity);
            updateMidiMonitor(e.note, 1, e.velocity, true);
        }
        else
        {
            releaseVoice(e.note, 1);
            monitoredMidiNote_.store(e.note, std::memory_order_relaxed);
            monitoredGate_.store(false, std::memory_order_relaxed);
        }
    };

    for (int i = 0; i < scope.blockSize1; ++i)
        handle(testEvents_[static_cast<std::size_t>(scope.startIndex1 + i)]);
    for (int i = 0; i < scope.blockSize2; ++i)
        handle(testEvents_[static_cast<std::size_t>(scope.startIndex2 + i)]);
}

void RelationalEXProcessor::advanceSequencer()
{
    const auto phaseBefore = sequencer.cyclePositions();
    const int index = sequencer.advance();

    if (index < 0 || index >= sequencer.length())
        return;

    rel::RelationalEvent event;

    if (!sequencer.copyEventForAudio(index, phaseBefore, event))
        return;

    // Relational Chaos Jitter: Use the deterministic chaos state to perturb sequencer events
    const float chaosJitter = chaos_.value();
    if (std::abs(chaosJitter) > 0.001f) {
        event.probability = std::clamp(event.probability + chaosJitter * 0.35, 0.0, 1.0);
        event.timingOffset = std::clamp(event.timingOffset + chaosJitter * 0.03, -0.05, 0.05);
    }

    if (!sequencer.shouldTrigger(event))
        return;

    const int root = static_cast<int>(param(*this, "rootNote", 0));
    const int scaleType = static_cast<int>(param(*this, "scaleType", 0));
    
    int rawNote = 60 + static_cast<int>(std::lround(event.pitch));
    const int note = snapToScale(rawNote, root, scaleType, 0);

    const double frequency =
        effectiveFrequency(
            note,
            1,
            event.tuningOffsetCents);

    const double samplesPerStep = sampleRate_ * 60.0 / std::max(1.0, bpm()) / 4.0;
    const int durationSamples = static_cast<int>(samplesPerStep * event.duration);

    triggerVoice(
        note,
        1,
        static_cast<float>(
            juce::jlimit(
                0.0,
                1.0,
                event.velocity)),
        frequency,
        durationSamples);

    for (auto& voice : voices_)
    {
        if (voice.active
            && voice.note == note
            && voice.channel == 1)
        {
            voice.isSequencer = true;
            voice.tuningCents = event.tuningOffsetCents;
            if (event.ratio > 0.0)
            {
                voice.baseRatio = static_cast<float>(event.ratio);
                voice.fm.setRatio(voice.baseRatio);
            }

            voice.baseIndex = static_cast<float>(std::max(0.0, event.fmIndex));
            voice.fm.setIndex(voice.baseIndex);
            break;
        }
    }
}

void RelationalEXProcessor::processBlock(
    juce::AudioBuffer<float>& buffer,
    juce::MidiBuffer& midi)
{
    juce::ScopedNoDenormals noDenormals;

    buffer.clear();

    // Host Transport Sync
    if (auto* playHead = getPlayHead()) {
        if (auto pos = playHead->getPosition()) {
            const bool isPlaying = pos->getIsPlaying();
            if (isPlaying && !wasPlaying_) {
                // Host transport just started, sync sequencer to grid
                resetSequence();
            }
            wasPlaying_ = isPlaying;
        }
    }

    drainTestEvents();

    for (const auto metadata : midi)
        handleMidiMessage(metadata.getMessage());

    const double samplesPerStep =
        sampleRate_
        * 60.0
        / std::max(1.0, bpm())
        / 4.0;

    if (sequenceRunning_.load(
            std::memory_order_acquire))
    {
        sequenceSampleCounter_ +=
            static_cast<double>(
                buffer.getNumSamples());

        while (sequenceSampleCounter_ >= samplesPerStep)
        {
            sequenceSampleCounter_ -= samplesPerStep;
            advanceSequencer();
        }
    }

    const float level =
        param(*this, "level", 0.72f);

    const float drift =
        param(*this, "tuningDrift", 0.0f);

    const float resonance =
        param(*this, "resonance", 0.0f);

    lfo_.setRate(
        param(*this, "lfoRate", 0.35f));

    chaos_.setAmount(
        param(*this, "chaosAmount", 0.0f)
            / 30.0);

    const double relationDepth =
        param(*this, "relationDepth", 0.65f);

    const double lfoAmount =
        param(*this, "lfoAmount", 0.0f)
            / 100.0;
            
    const float globalFmRatio = param(*this, "fmRatio", 1.618f);
    const float globalFmIndex = param(*this, "fmIndex", 2.0f);
    const bool isFmContinuous = param(*this, "fmContinuous", 1.0f) > 0.5f;

    const auto tuningSnap =
        std::atomic_load_explicit(
            &tuningSnapshot_,
            std::memory_order_acquire);

    // Per-block target pitch (tuning + per-event cents + MPE bend). The audio-rate
    // glide below moves voice.frequency toward this target, so MONO LEGATO works.
    for (auto& voice : voices_)
    {
        if (voice.active && voice.note >= 0)
        {
            double base = voice.targetFrequency;

            if (mts.available() && mts.hasMaster())
                base = mts.noteFrequency(voice.note, voice.channel - 1);
            else if (tuningSnap)
                base = (*tuningSnap)[static_cast<std::size_t>(voice.note)];

            if (voice.tuningCents != 0.0)
                base *= std::pow(2.0, voice.tuningCents / 1200.0);

            // MPE-style pitch bend (+/- 48 semitones for microtonal expressiveness)
            const float bend = channelPitchBend_[static_cast<std::size_t>(std::clamp(voice.channel - 1, 0, 15))];
            if (std::abs(bend) > 0.001f)
                base *= std::pow(2.0, static_cast<double>(bend) * 48.0 / 12.0);

            voice.targetFrequency = std::clamp(base, 1.0, 20000.0);
        }
    }

    std::uniform_real_distribution<float>
        random(-1.0f, 1.0f);

    // Block-constant parameters (hoisted out of the per-sample / per-voice loops).
    const int lfoTarget = static_cast<int>(param(*this, "lfoTarget", 0));
    const float glideTime = param(*this, "glideTime", 0.1f);
    const bool monoLegato = static_cast<int>(param(*this, "synthMode", 0)) == 1;
    const double glideCoeff = (monoLegato && glideTime > 0.001f)
        ? (1.0 - std::exp(-1.0 / (static_cast<double>(glideTime) * sampleRate_)))
        : 1.0;
    const int scaleRoot = static_cast<int>(param(*this, "rootNote", 0));
    const int scaleKind = static_cast<int>(param(*this, "scaleType", 0));
    const float baseCutoff = param(*this, "filterCutoff", 20000.0f);

    rel::RelationEngine::Matrix relationMatrix{};
    relations_.copyMatrixForAudio(
        relationMatrix);

    for (int sample = 0;
         sample < buffer.getNumSamples();
         ++sample)
    {
        float output = 0.0f;
        float display = 0.0f;

        const double lfo =
            static_cast<double>(
                lfo_.process());

        const double chaos =
            static_cast<double>(
                chaos_.process());

        const double globalPhase =
            sequencer.combinedCycleProgress();

        const auto phase =
            sequencer.cyclePositions();

        const double timePhase =
            sequencer.cycleProgress(
                sequencer.rhythmCycle(),
                phase.rhythm);

        const double tuningPhase =
            sequencer.cycleProgress(
                sequencer.tuningCycle(),
                phase.tuning);

        const double pitchPhase =
            sequencer.cycleProgress(
                sequencer.pitchCycle(),
                phase.pitch);

        const rel::RelationEngine::Vector
            relationInput
        {
            pitchPhase * 2.0 - 1.0,
            ((globalPhase * 0.7) + (timePhase * 0.3)) * 2.0 - 1.0,
            (lfoTarget == 0) ? (lfo * 0.35 * lfoAmount) : 0.0,
            static_cast<double>(resonance) * 2.0 - 1.0,
            (tuningPhase * 2.0 - 1.0) * 0.75 + chaos * 0.25
        };

        const auto relation =
            relations_.transformSlewed(
                relationInput,
                relationDepth,
                relationMatrix);

        for (auto& voice : voices_)
        {
            if (!voice.active)
                continue;
                
            // Glide / Portamento
            voice.frequency += (voice.targetFrequency - voice.frequency) * glideCoeff;

            const float driftValue = random(audioRng_) * drift * 0.0000015f;

            // Harmonic Constraints: Map tuning modulation directly to scale degrees (Quantize to Scale)
            const float relationTuning = static_cast<float>(relation[rel::RelationEngine::Tuning]);
            const int degreeShift = static_cast<int>(std::round(relationTuning * 12.0f));

            if (std::isfinite(voice.frequency))
            {
                double retuned = voice.frequency;
                
                if (degreeShift != 0) {
                    const int shiftedNote = snapToScale(voice.note, scaleRoot, scaleKind, degreeShift);
                    
                    double shiftedTarget = (mts.available() && mts.hasMaster())
                        ? mts.noteFrequency(shiftedNote, voice.channel - 1)
                        : (tuningSnap ? (*tuningSnap)[static_cast<std::size_t>(shiftedNote)] : voice.targetFrequency);
                        
                    // Reapply the current glide progress factor to the shifted target
                    double glideProgress = (voice.targetFrequency > 0.1) ? (voice.frequency / voice.targetFrequency) : 1.0;
                    retuned = shiftedTarget * glideProgress;
                }

                voice.fm.setCarrier(std::clamp(retuned, 0.1, 20000.0));
            }

            const float pitchMod = static_cast<float>(relation[rel::RelationEngine::Pitch] * 0.0005);
            const float relationFM = static_cast<float>(relation[rel::RelationEngine::FM]);

            const float currentBaseRatio = voice.isSequencer ? voice.baseRatio : (isFmContinuous ? globalFmRatio : voice.baseRatio);
            const float currentBaseIndex = voice.isSequencer ? voice.baseIndex : (isFmContinuous ? globalFmIndex : voice.baseIndex);
            
            float lfoRatioMod = (lfoTarget == 1) ? (lfo * lfoAmount) : 0.0f;
            float lfoIndexMod = (lfoTarget == 2) ? (lfo * lfoAmount) : 0.0f;
            float lfoCutoffMod = (lfoTarget == 3) ? (lfo * lfoAmount) : 0.0f;

            const float dynamicRatio =
                std::clamp(
                    currentBaseRatio
                        * std::pow(
                            2.0f,
                            relationFM * 0.20f + lfoRatioMod * 0.20f),
                    0.001f,
                    32.0f);

            const float dynamicIndex =
                std::clamp(
                    currentBaseIndex
                        * (1.0f + relationFM * 0.50f + lfoIndexMod * 0.50f),
                    0.0f,
                    50.0f);

            voice.fm.setRatio(dynamicRatio);
            voice.fm.setIndex(dynamicIndex);

            const float fm =
                voice.fm.process(
                    driftValue
                    + pitchMod);

            float signal =
                fm
                * voice.envelope
                * voice.velocity;

            if (resonance > 0.001f)
            {
                const float resonanceAmount =
                    static_cast<float>(
                        juce::jlimit(
                            0.0,
                            1.0,
                            static_cast<double>(resonance)
                            + relation[
                                rel::RelationEngine::Resonance]
                                * 0.15));

                signal +=
                    voice.resonator.process()
                    * resonanceAmount
                    * voice.envelope;
            }

            if (voice.samplesUntilRelease > 0) {
                voice.samplesUntilRelease--;
                if (voice.samplesUntilRelease == 0) {
                    voice.releasing = true;
                    voice.adsr.noteOff();
                }
            }

            voice.envelope = voice.adsr.getNextSample();
            
            if (!voice.adsr.isActive())
            {
                voice.active = false;
                voice.note = -1;
                voice.releasing = false;
                voice.velocity = 0.0f;
                voice.envelope = 0.0f;
            }
            
            // Apply Low-Pass Filter
            float cutoff = baseCutoff;
            if (lfoTarget == 3)
                cutoff *= std::pow(2.0f, lfoCutoffMod * 3.0f); // Up to 3 octaves mod

            voice.filter.setCutoffFrequency(std::clamp(cutoff, 20.0f, 20000.0f));
            signal = voice.filter.processSample(0, signal);

            output += signal;
            display += std::abs(signal);
        }

        output =
            std::tanh(output * 0.85f)
            * level
            * 0.7f;

        buffer.setSample(0, sample, output);
        buffer.setSample(1, sample, output);

        const float visual =
            juce::jlimit(
                0.0f,
                1.0f,
                display);

        const float previous =
            visualPhase_.load(
                std::memory_order_relaxed);

        visualPhase_.store(
            0.98f * previous
                + 0.02f * visual,
            std::memory_order_relaxed);
    }
}

int RelationalEXProcessor::getNumPrograms()
{
    return static_cast<int>(
        rel::PresetLibrary::factoryPresets().size());
}

void RelationalEXProcessor::setCurrentProgram(int index)
{
    applyFactoryPreset(index);
}

const juce::String
RelationalEXProcessor::getProgramName(int index)
{
    const auto& presets =
        rel::PresetLibrary::factoryPresets();

    if (index < 0
        || index >= static_cast<int>(presets.size()))
    {
        return {};
    }

    return presets[
        static_cast<std::size_t>(index)].name;
}

void RelationalEXProcessor::applyFactoryPreset(int index)
{
    const auto& presets =
        rel::PresetLibrary::factoryPresets();

    if (presets.empty())
        return;

    index =
        std::clamp(
            index,
            0,
            static_cast<int>(presets.size()) - 1);

    const auto& preset =
        presets[static_cast<std::size_t>(index)];

    auto setFloat =
        [this](const char* id, float value)
        {
            if (auto* p = apvts.getParameter(id))
            {
                p->setValueNotifyingHost(
                    p->convertTo0to1(value));
            }
        };

    auto setInt =
        [this](const char* id, int value)
        {
            if (auto* p = apvts.getParameter(id))
            {
                p->setValueNotifyingHost(
                    p->convertTo0to1(
                        static_cast<float>(value)));
            }
        };

    setFloat("fmRatio", preset.fmRatio);
    setFloat("fmIndex", preset.fmIndex);
    setFloat("fmFeedback", preset.feedback);
    setFloat("level", preset.output);
    setFloat("tuningDrift", preset.drift);
    setFloat("inharmonicity", preset.inharmonicity);
    setFloat("resonance", preset.resonance);
    setFloat("bpm", preset.bpm);

    setFloat("lfoRate", 0.35f);
    setFloat("lfoAmount", 0.0f);
    setFloat("chaosAmount", 0.0f);
    setFloat("relationDepth", 0.65f);

    setInt("tuningDivisions", preset.tuningDivisions);
    setInt("seqSteps", preset.steps);
    setInt("seqPulses", preset.pulses);

    const int defaultRhythm =
        juce::jlimit(1, 32, preset.rhythmCycle);
    const int defaultPitch =
        juce::jlimit(1, 32, preset.pitchCycle);
    const int defaultFM =
        juce::jlimit(1, 32, preset.fmCycle);
    const int defaultTuning =
        juce::jlimit(1, 32, preset.tuningCycle);
    const int defaultMotion =
        juce::jlimit(1, 32, preset.motionCycle);

    setInt("rhythmCycle", defaultRhythm);
    setInt("pitchCycle", defaultPitch);
    setInt("fmCycle", defaultFM);
    setInt("tuningCycle", defaultTuning);
    setInt("motionCycle", defaultMotion);

    tuning.setEqualTemperament(
        preset.tuningDivisions);

    tuningEditBuffer_ =
        tuning.points();

    refreshTuningSnapshot();

    sequencer.setLength(
        preset.steps);

    sequencer.applyEuclidean(
        preset.pulses,
        0);

    sequencer.setCycleLengths({
        defaultRhythm,
        defaultPitch,
        defaultFM,
        defaultTuning,
        defaultMotion
    });

    resetSequence();

    // Factory presets are also demonstrations of the relational engine.
    // Each category starts from the base coupling graph and emphasizes a
    // different set of cross-domain relationships without introducing a
    // second modulation architecture.
    relations_.reset();

    const std::string category = preset.category;

    if (category == "TUNING")
    {
        relations_.set(rel::RelationEngine::Tuning, rel::RelationEngine::Pitch, 0.48);
        relations_.set(rel::RelationEngine::Tuning, rel::RelationEngine::FM, 0.32);
    }
    else if (category == "FM")
    {
        relations_.set(rel::RelationEngine::Pitch, rel::RelationEngine::FM, 0.52);
        relations_.set(rel::RelationEngine::Time, rel::RelationEngine::FM, 0.28);
        relations_.set(rel::RelationEngine::FM, rel::RelationEngine::Resonance, 0.42);
    }
    else if (category == "MOTION")
    {
        relations_.set(rel::RelationEngine::Time, rel::RelationEngine::FM, 0.42);
        relations_.set(rel::RelationEngine::Time, rel::RelationEngine::Resonance, 0.35);
        relations_.set(rel::RelationEngine::Tuning, rel::RelationEngine::Pitch, 0.28);
    }
    else if (category == "RATIO")
    {
        relations_.set(rel::RelationEngine::Pitch, rel::RelationEngine::FM, 0.58);
        relations_.set(rel::RelationEngine::Tuning, rel::RelationEngine::FM, 0.38);
    }
    else if (category == "RESONANT")
    {
        relations_.set(rel::RelationEngine::Pitch, rel::RelationEngine::Resonance, 0.38);
        relations_.set(rel::RelationEngine::FM, rel::RelationEngine::Resonance, 0.54);
        relations_.set(rel::RelationEngine::Time, rel::RelationEngine::Resonance, 0.30);
    }
    else if (category == "CHAOS")
    {
        relations_.set(rel::RelationEngine::Time, rel::RelationEngine::Tuning, 0.46);
        relations_.set(rel::RelationEngine::Tuning, rel::RelationEngine::Pitch, 0.40);
        relations_.set(rel::RelationEngine::FM, rel::RelationEngine::Resonance, -0.35);
    }
    else if (category == "MODAL")
    {
        relations_.set(rel::RelationEngine::Tuning, rel::RelationEngine::Pitch, 0.35);
        relations_.set(rel::RelationEngine::FM, rel::RelationEngine::Resonance, 0.48);
        relations_.set(rel::RelationEngine::Pitch, rel::RelationEngine::Resonance, 0.22);
    }
    else if (category == "PERCUSSIVE")
    {
        relations_.set(rel::RelationEngine::Time, rel::RelationEngine::Resonance, 0.48);
        relations_.set(rel::RelationEngine::FM, rel::RelationEngine::Resonance, 0.35);
    }
    else if (category == "ORGANIC")
    {
        relations_.set(rel::RelationEngine::Time, rel::RelationEngine::Tuning, 0.24);
        relations_.set(rel::RelationEngine::Tuning, rel::RelationEngine::Pitch, 0.20);
    }
    else if (category == "MINIMAL")
    {
        relations_.set(rel::RelationEngine::Pitch, rel::RelationEngine::Tuning, 0.16);
        relations_.set(rel::RelationEngine::Time, rel::RelationEngine::FM, 0.14);
    }
    else if (category == "EXTREME")
    {
        relations_.set(rel::RelationEngine::Pitch, rel::RelationEngine::FM, 0.78);
        relations_.set(rel::RelationEngine::Time, rel::RelationEngine::Resonance, 0.62);
        relations_.set(rel::RelationEngine::Tuning, rel::RelationEngine::Pitch, 0.55);
        relations_.set(rel::RelationEngine::FM, rel::RelationEngine::Tuning, 0.36);
    }

    relations_.publish();

    presetIndex_ = index;

    sequencer.publishToAudio();
}

void RelationalEXProcessor::setRhythmCycle(int length) noexcept
{
    length = juce::jlimit(1, 32, length);
    sequencer.setRhythmCycle(length);
    if (auto* p = apvts.getParameter("rhythmCycle"))
        p->setValueNotifyingHost(p->convertTo0to1(static_cast<float>(length)));
    sequencer.publishToAudio();
}

void RelationalEXProcessor::setPitchCycle(int length) noexcept
{
    length = juce::jlimit(1, 32, length);
    sequencer.setPitchCycle(length);
    if (auto* p = apvts.getParameter("pitchCycle"))
        p->setValueNotifyingHost(p->convertTo0to1(static_cast<float>(length)));
    sequencer.publishToAudio();
}

void RelationalEXProcessor::setFMCycle(int length) noexcept
{
    length = juce::jlimit(1, 32, length);
    sequencer.setFMCycle(length);
    if (auto* p = apvts.getParameter("fmCycle"))
        p->setValueNotifyingHost(p->convertTo0to1(static_cast<float>(length)));
    sequencer.publishToAudio();
}

void RelationalEXProcessor::setTuningCycle(int length) noexcept
{
    length = juce::jlimit(1, 32, length);
    sequencer.setTuningCycle(length);
    if (auto* p = apvts.getParameter("tuningCycle"))
        p->setValueNotifyingHost(p->convertTo0to1(static_cast<float>(length)));
    sequencer.publishToAudio();
}

void RelationalEXProcessor::setMotionCycle(int length) noexcept
{
    length = juce::jlimit(1, 32, length);
    sequencer.setMotionCycle(length);
    if (auto* p = apvts.getParameter("motionCycle"))
        p->setValueNotifyingHost(p->convertTo0to1(static_cast<float>(length)));
    sequencer.publishToAudio();
}

void RelationalEXProcessor::setTuningDivisions(
    int divisions)
{
    divisions =
        std::clamp(
            divisions,
            1,
            53);

    tuning.setEqualTemperament(
        divisions);

    tuningEditBuffer_ =
        tuning.points();

    refreshTuningSnapshot();

    if (auto* p =
            apvts.getParameter(
                "tuningDivisions"))
    {
        p->setValueNotifyingHost(
            p->convertTo0to1(
                static_cast<float>(divisions)));
    }
}

void RelationalEXProcessor::scanScaleDatabase()
{
    std::vector<std::filesystem::path> roots;

    const auto exeDir =
        juce::File::getSpecialLocation(
            juce::File::currentExecutableFile)
            .getParentDirectory();

    // Bundled scales, per platform layout:
    //  macOS bundle:   <bundle>/Contents/Resources/Scales   (binary in Contents/MacOS)
    //  Win/Linux VST3: <bundle>/Contents/Resources/Scales   (binary in Contents/<arch>)
    //  Standalone:     <exe dir>/Resources/Scales
    for (const char* rel : { "../Resources/Scales", "Resources/Scales", "Scales" })
        roots.emplace_back(
            exeDir.getChildFile(rel).getFullPathName().toStdString());

    roots.emplace_back(
        juce::File::getSpecialLocation(
            juce::File::userApplicationDataDirectory)
            .getChildFile(
                "RELATIONAL EX/Scales")
            .getFullPathName()
            .toStdString());

    scaleCatalog_.scan(roots);
}

bool RelationalEXProcessor::loadScalaFile(
    const juce::File& file,
    juce::String& error)
{
    std::string parserError;

    if (!rel::ScalaParser::loadSclFile(
            file.getFullPathName().toStdString(),
            tuning,
            parserError))
    {
        error = juce::String(parserError);
        return false;
    }

    tuningEditBuffer_ =
        tuning.points();

    refreshTuningSnapshot();

    if (auto* p =
            apvts.getParameter(
                "tuningDivisions"))
    {
        p->setValueNotifyingHost(
            p->convertTo0to1(
                static_cast<float>(
                    tuning.degreesPerPeriod())));
    }

    return true;
}

void RelationalEXProcessor::commitTuningEdit()
{
    tuning.setPoints(
        tuningEditBuffer_);

    refreshTuningSnapshot();
}

void RelationalEXProcessor::randomizeTuning()
{
    tuningEditBuffer_ =
        tuning.points();

    if (tuningEditBuffer_.empty())
        return;

    std::uniform_real_distribution<double>
        cents(-35.0, 35.0);

    for (std::size_t i = 1;
         i < tuningEditBuffer_.size();
         ++i)
    {
        tuningEditBuffer_[i].ratio *=
            std::pow(
                2.0,
                cents(uiRng_) / 1200.0);
    }

    tuning.setPoints(
        tuningEditBuffer_);

    refreshTuningSnapshot();
}

void RelationalEXProcessor::resetRelations()
{
    relations_.reset();
}

void RelationalEXProcessor::saveSequenceToState(
    juce::ValueTree& state) const
{
    auto seq =
        state.getOrCreateChildWithName(
            "SEQUENCER",
            nullptr);
            
    seq.setProperty("version", 2, nullptr); // Version 2: duration model and MPE

    seq.removeAllProperties(nullptr);
    seq.removeAllChildren(nullptr);

    seq.setProperty(
        "length",
        sequencer.length(),
        nullptr);

    const auto cycles =
        sequencer.cycleLengths();

    seq.setProperty("rhythmCycle", cycles.rhythm, nullptr);
    seq.setProperty("pitchCycle", cycles.pitch, nullptr);
    seq.setProperty("fmCycle", cycles.fm, nullptr);
    seq.setProperty("tuningCycle", cycles.tuning, nullptr);
    seq.setProperty("motionCycle", cycles.motion, nullptr);

    seq.setProperty(
        "combinedCycleLength",
        static_cast<juce::int64>(
            sequencer.combinedCycleLength()),
        nullptr);

    const auto& events =
        sequencer.events();

    for (int i = 0;
         i < sequencer.length()
             && i < static_cast<int>(events.size());
         ++i)
    {
        const auto& e =
            events[static_cast<std::size_t>(i)];

        auto node =
            juce::ValueTree("EVENT");

        node.setProperty("position", e.position, nullptr);
        node.setProperty("pitch", e.pitch, nullptr);
        node.setProperty("ratio", e.ratio, nullptr);
        node.setProperty("velocity", e.velocity, nullptr);
        node.setProperty("probability", e.probability, nullptr);
        node.setProperty("fmIndex", e.fmIndex, nullptr);
        node.setProperty("tuningOffsetCents", e.tuningOffsetCents, nullptr);
        node.setProperty("timingOffset", e.timingOffset, nullptr);
        node.setProperty("duration", e.duration, nullptr);
        node.setProperty("active", e.active, nullptr);

        seq.addChild(node, -1, nullptr);
    }
}

void RelationalEXProcessor::loadSequenceFromState(
    const juce::ValueTree& state)
{
    auto seq =
        state.getChildWithName(
            "SEQUENCER");

    if (!seq.isValid())
        return;

    // State Migration System
    const int version = static_cast<int>(seq.getProperty("version", 1));
    juce::ignoreUnused(version); // for future migration branches

    const int length =
        juce::jlimit(
            1,
            32,
            static_cast<int>(
                seq.getProperty(
                    "length",
                    16)));

    sequencer.setLength(length);

    rel::RelationalSequencer::CycleLengths cycles;

    cycles.rhythm =
        juce::jlimit(
            1,
            32,
            static_cast<int>(
                seq.getProperty(
                    "rhythmCycle",
                    length)));

    cycles.pitch =
        juce::jlimit(
            1,
            32,
            static_cast<int>(
                seq.getProperty(
                    "pitchCycle",
                    7)));

    cycles.fm =
        juce::jlimit(
            1,
            32,
            static_cast<int>(
                seq.getProperty(
                    "fmCycle",
                    5)));

    cycles.tuning =
        juce::jlimit(
            1,
            32,
            static_cast<int>(
                seq.getProperty(
                    "tuningCycle",
                    11)));

    cycles.motion =
        juce::jlimit(
            1,
            32,
            static_cast<int>(
                seq.getProperty(
                    "motionCycle",
                    17)));

    sequencer.setCycleLengths(cycles);

    if (auto* p = apvts.getParameter("rhythmCycle"))
        p->setValueNotifyingHost(p->convertTo0to1(static_cast<float>(cycles.rhythm)));
    if (auto* p = apvts.getParameter("pitchCycle"))
        p->setValueNotifyingHost(p->convertTo0to1(static_cast<float>(cycles.pitch)));
    if (auto* p = apvts.getParameter("fmCycle"))
        p->setValueNotifyingHost(p->convertTo0to1(static_cast<float>(cycles.fm)));
    if (auto* p = apvts.getParameter("tuningCycle"))
        p->setValueNotifyingHost(p->convertTo0to1(static_cast<float>(cycles.tuning)));
    if (auto* p = apvts.getParameter("motionCycle"))
        p->setValueNotifyingHost(p->convertTo0to1(static_cast<float>(cycles.motion)));

    auto& events =
        sequencer.events();

    for (int i = 0;
         i < length
             && i < seq.getNumChildren()
             && i < static_cast<int>(events.size());
         ++i)
    {
        const auto node =
            seq.getChild(i);

        auto& e =
            events[static_cast<std::size_t>(i)];

        e.position = static_cast<double>(
            node.getProperty("position", e.position));
        e.pitch = static_cast<double>(
            node.getProperty("pitch", e.pitch));
        e.ratio = static_cast<double>(
            node.getProperty("ratio", e.ratio));
        e.velocity = static_cast<double>(
            node.getProperty("velocity", e.velocity));
        e.probability = static_cast<double>(
            node.getProperty("probability", e.probability));
        e.fmIndex = static_cast<double>(
            node.getProperty("fmIndex", e.fmIndex));
        e.tuningOffsetCents = static_cast<double>(
            node.getProperty("tuningOffsetCents", e.tuningOffsetCents));
        e.timingOffset = static_cast<double>(
            node.getProperty("timingOffset", e.timingOffset));
        e.duration = static_cast<double>(
            node.getProperty("duration", e.duration));
        e.active = static_cast<bool>(
            node.getProperty("active", e.active));
    }

    sequencer.publishToAudio();
}

void RelationalEXProcessor::saveRelationsToState(
    juce::ValueTree& state) const
{
    auto relationState =
        state.getOrCreateChildWithName(
            "RELATIONS",
            nullptr);

    relationState.removeAllProperties(nullptr);
    relationState.removeAllChildren(nullptr);

    for (int destination = 0;
         destination < rel::RelationEngine::dimensions;
         ++destination)
    {
        for (int source = 0;
             source < rel::RelationEngine::dimensions;
             ++source)
        {
            if (source == destination)
                continue;

            const double amount =
                relations_.get(
                    source,
                    destination);

            if (std::abs(amount) < 0.000001)
                continue;

            auto node =
                juce::ValueTree("LINK");

            node.setProperty(
                "source",
                source,
                nullptr);

            node.setProperty(
                "destination",
                destination,
                nullptr);

            node.setProperty(
                "amount",
                amount,
                nullptr);

            relationState.addChild(
                node,
                -1,
                nullptr);
        }
    }
}

void RelationalEXProcessor::loadRelationsFromState(
    const juce::ValueTree& state)
{
    auto relationState =
        state.getChildWithName(
            "RELATIONS");

    if (!relationState.isValid())
    {
        relations_.reset();
        return;
    }

    relations_.reset();

    for (int i = 0;
         i < relationState.getNumChildren();
         ++i)
    {
        const auto node =
            relationState.getChild(i);

        relations_.set(
            static_cast<int>(
                node.getProperty("source", -1)),
            static_cast<int>(
                node.getProperty("destination", -1)),
            static_cast<double>(
                node.getProperty("amount", 0.0)));
    }

    relations_.publish();
}

void RelationalEXProcessor::getStateInformation(
    juce::MemoryBlock& dest)
{
    auto state =
        apvts.copyState();

    state.setProperty(
        "presetIndex",
        presetIndex_,
        nullptr);

    auto tuningState =
        state.getOrCreateChildWithName(
            "TUNING",
            nullptr);

    tuningState.removeAllChildren(nullptr);

    tuningState.setProperty(
        "periodRatio",
        tuning.periodRatio(),
        nullptr);

    tuningState.setProperty(
        "rootFrequency",
        tuning.rootFrequency(),
        nullptr);

    for (const auto& point : tuning.points())
    {
        auto node =
            juce::ValueTree("POINT");

        node.setProperty(
            "ratio",
            point.ratio,
            nullptr);

        node.setProperty(
            "cents",
            point.cents,
            nullptr);

        tuningState.addChild(
            node,
            -1,
            nullptr);
    }

    saveSequenceToState(state);
    saveRelationsToState(state);

    if (auto xml = state.createXml())
        copyXmlToBinary(*xml, dest);
}

void RelationalEXProcessor::setStateInformation(
    const void* data,
    int sizeInBytes)
{
    if (auto xml =
            getXmlFromBinary(
                data,
                sizeInBytes))
    {
        auto state =
            juce::ValueTree::fromXml(*xml);

        if (!state.isValid()
            || state.getType()
                != apvts.state.getType())
        {
            return;
        }

        apvts.replaceState(state);

        presetIndex_ =
            juce::jlimit(
                0,
                std::max(
                    0,
                    getNumPrograms() - 1),
                static_cast<int>(
                    state.getProperty(
                        "presetIndex",
                        0)));

        auto tuningState =
            state.getChildWithName(
                "TUNING");

        if (tuningState.isValid()
            && tuningState.getNumChildren() > 0)
        {
            std::vector<rel::TuningPoint>
                points;

            points.reserve(
                static_cast<std::size_t>(
                    tuningState.getNumChildren()));

            for (int i = 0;
                 i < tuningState.getNumChildren();
                 ++i)
            {
                const auto node =
                    tuningState.getChild(i);

                points.push_back({
                    static_cast<double>(
                        node.getProperty("ratio", 1.0)),
                    static_cast<double>(
                        node.getProperty("cents", 0.0))});
            }

            tuning.setPeriod(
                static_cast<double>(
                    tuningState.getProperty(
                        "periodRatio",
                        2.0)));

            tuning.setPoints(
                std::move(points));

            tuning.setRootFrequency(
                static_cast<double>(
                    tuningState.getProperty(
                        "rootFrequency",
                        440.0)));

            tuningEditBuffer_ =
                tuning.points();

            refreshTuningSnapshot();
        }
        else
        {
            setTuningDivisions(
                static_cast<int>(
                    param(
                        *this,
                        "tuningDivisions",
                        12.0f)));
        }

        loadRelationsFromState(state);
        loadSequenceFromState(state);
    }
}

juce::AudioProcessorEditor*
RelationalEXProcessor::createEditor()
{
    return new RelationalEXEditor(*this);
}

juce::AudioProcessor*
JUCE_CALLTYPE createPluginFilter()
{
    return new RelationalEXProcessor();
}