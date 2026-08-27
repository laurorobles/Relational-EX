#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <algorithm>
#include <cmath>
#include <random>
#include <filesystem>

namespace {
float param(RelationalEXProcessor& p, const char* id, float fallback)
{
    if (auto* v = p.apvts.getRawParameterValue(id)) return v->load();
    return fallback;
}
}

RelationalEXProcessor::RelationalEXProcessor()
    : AudioProcessor(BusesProperties().withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      apvts(*this, nullptr, "APVTS", createParameterLayout())
{
    tuning.setEqualTemperament(12);
    tuning.setRootFrequency(440.0);
    tuningEditBuffer_ = tuning.points();
    refreshTuningSnapshot();
    mts.registerClient();
    sequencer.setLength(16);
    sequencer.applyEuclidean(8, 0);
    scanScaleDatabase();
    applyFactoryPreset(0);
}

RelationalEXProcessor::~RelationalEXProcessor()
{
    mts.deregisterClient();
}

juce::AudioProcessorValueTreeState::ParameterLayout RelationalEXProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> p;
    p.push_back(std::make_unique<juce::AudioParameterFloat>("level", "Output Level", juce::NormalisableRange<float>(0.0f, 1.0f), 0.72f));
    p.push_back(std::make_unique<juce::AudioParameterFloat>("fmRatio", "FM Ratio", juce::NormalisableRange<float>(0.01f, 8.0f, 0.001f, 0.45f), 1.618f));
    p.push_back(std::make_unique<juce::AudioParameterFloat>("fmIndex", "FM Index", juce::NormalisableRange<float>(0.0f, 20.0f, 0.01f, 0.6f), 2.0f));
    p.push_back(std::make_unique<juce::AudioParameterFloat>("fmFeedback", "FM Feedback", juce::NormalisableRange<float>(0.0f, 0.95f, 0.001f, 0.6f), 0.08f));
    p.push_back(std::make_unique<juce::AudioParameterFloat>("tuningDrift", "Tuning Drift", juce::NormalisableRange<float>(0.0f, 30.0f, 0.01f, 0.7f), 0.0f));
    p.push_back(std::make_unique<juce::AudioParameterFloat>("inharmonicity", "Inharmonicity", juce::NormalisableRange<float>(0.0f, 1.0f, 0.001f, 0.7f), 0.25f));
    p.push_back(std::make_unique<juce::AudioParameterFloat>("resonance", "Resonance", juce::NormalisableRange<float>(0.0f, 1.0f, 0.001f, 0.6f), 0.0f));
    p.push_back(std::make_unique<juce::AudioParameterFloat>("bpm", "BPM", juce::NormalisableRange<float>(40.0f, 240.0f, 0.1f), 120.0f));
    p.push_back(std::make_unique<juce::AudioParameterInt>("seqSteps", "Sequence Steps", 1, 32, 16));
    p.push_back(std::make_unique<juce::AudioParameterInt>("seqPulses", "Sequence Pulses", 0, 32, 8));
    p.push_back(std::make_unique<juce::AudioParameterInt>("tuningDivisions", "Tuning Divisions", 1, 53, 12));
    p.push_back(std::make_unique<juce::AudioParameterFloat>("rootHz", "Root Frequency", juce::NormalisableRange<float>(20.0f, 1000.0f, 0.01f, 0.5f), 440.0f));
    p.push_back(std::make_unique<juce::AudioParameterFloat>("lfoRate", "LFO Rate", juce::NormalisableRange<float>(0.01f, 20.0f, 0.001f, 0.5f), 0.35f));
    p.push_back(std::make_unique<juce::AudioParameterFloat>("lfoAmount", "LFO Amount", juce::NormalisableRange<float>(0.0f, 100.0f, 0.01f, 0.7f), 0.0f));
    p.push_back(std::make_unique<juce::AudioParameterFloat>("chaosAmount", "Chaos", juce::NormalisableRange<float>(0.0f, 30.0f, 0.01f, 0.7f), 0.0f));
    p.push_back(std::make_unique<juce::AudioParameterFloat>("relationDepth", "Relation Depth", juce::NormalisableRange<float>(0.0f, 1.0f, 0.001f, 0.7f), 0.65f));
    return { p.begin(), p.end() };
}

void RelationalEXProcessor::prepareToPlay(double sr, int)
{
    sampleRate_ = std::max(1000.0, sr);
    sequenceSampleCounter_ = 0.0;
    for (auto& v : voices_) {
        v.fm.prepare(sampleRate_);
        v.resonator.prepare(sampleRate_);
    }
    lfo_.prepare(sampleRate_);
    chaos_.reset();
    relations_.reset();
}

void RelationalEXProcessor::releaseResources() {}

bool RelationalEXProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    return layouts.getMainOutputChannelSet() == juce::AudioChannelSet::stereo();
}

void RelationalEXProcessor::handleMidiMessage(const juce::MidiMessage& message)
{
    if (message.isSysEx()) {
        const auto* raw = message.getSysExData();
        const auto size = static_cast<size_t>(message.getSysExDataSize());
        if (raw && size > 0) {
            mts.parseMidiData(reinterpret_cast<const unsigned char*>(raw), static_cast<int>(size));
            midiTuning.parseSysEx(raw, size);
        }
        return;
    }
    if (message.isNoteOn()) {
        const int note = message.getNoteNumber();
        const int channel = message.getChannel();
        if (mts.available() && mts.shouldFilterNote(note, channel - 1)) return;
        triggerVoice(note, channel, message.getFloatVelocity());
    } else if (message.isNoteOff()) {
        releaseVoice(message.getNoteNumber(), message.getChannel());
    }
}

double RelationalEXProcessor::frequencyFromSnapshot(int midiNote) const noexcept
{
    midiNote = std::clamp(midiNote, 0, 127);
    const auto snapshot = std::atomic_load_explicit(&tuningSnapshot_, std::memory_order_acquire);
    if (!snapshot) return 440.0 * std::pow(2.0, (static_cast<double>(midiNote) - 69.0) / 12.0);
    return (*snapshot)[static_cast<std::size_t>(midiNote)];
}

void RelationalEXProcessor::refreshTuningSnapshot() noexcept
{
    auto snapshot = std::make_shared<TuningSnapshot>();
    for (int note = 0; note < 128; ++note)
        (*snapshot)[static_cast<std::size_t>(note)] = tuning.frequencyForMidi(note);
    std::atomic_store_explicit(&tuningSnapshot_, std::shared_ptr<const TuningSnapshot>(std::move(snapshot)), std::memory_order_release);
}

double RelationalEXProcessor::effectiveFrequency(int midiNote, int channel, double centsOffset) const noexcept
{
    double hz = frequencyFromSnapshot(midiNote);
    if (mts.available() && mts.hasMaster()) {
        hz = mts.noteFrequency(midiNote, channel - 1);
    } else if (midiTuning.hasTuning()) {
        hz = midiTuning.frequencyForNote(midiNote);
    }
    hz *= std::pow(2.0, centsOffset / 1200.0);
    return std::clamp(hz, 1.0, 20000.0);
}

void RelationalEXProcessor::triggerVoice(int midiNote, int channel, float velocity, double forcedFrequency)
{
    Voice* target = nullptr;
    for (auto& v : voices_) {
        if (v.active && v.note == midiNote && v.channel == channel) { target = &v; break; }
    }
    if (!target) {
        for (auto& v : voices_) if (!v.active) { target = &v; break; }
    }
    if (!target) target = &*std::min_element(voices_.begin(), voices_.end(), [](const Voice& a, const Voice& b) { return a.envelope < b.envelope; });

    target->note = midiNote;
    target->channel = channel;
    target->velocity = velocity;
    target->active = true;
    target->releasing = false;
    target->envelope = 0.0f;
    target->releaseRate = 1.0f / static_cast<float>(std::max(1.0, sampleRate_ * 0.18));
    target->frequency = forcedFrequency > 0.0 ? forcedFrequency : effectiveFrequency(midiNote, channel, 0.0);
    target->fm.setCarrier(target->frequency);
    target->fm.setRatio(param(*this, "fmRatio", 1.618f));
    target->fm.setIndex(param(*this, "fmIndex", 2.0f));
    target->fm.setFeedback(param(*this, "fmFeedback", 0.08f));
    target->fm.reset();
    target->resonator.setFundamental(target->frequency);
    target->resonator.setInharmonicity(param(*this, "inharmonicity", 0.25f));
    target->resonator.setDamping(0.9935);
    target->resonator.reset();
    if (param(*this, "resonance", 0.0f) > 0.01f) target->resonator.excite(param(*this, "resonance", 0.0f) * velocity);
}

void RelationalEXProcessor::releaseVoice(int midiNote, int channel)
{
    for (auto& v : voices_) {
        if (v.active && v.note == midiNote && v.channel == channel) {
            v.releasing = true;
            break;
        }
    }
}

void RelationalEXProcessor::advanceSequencer()
{
    const int index = sequencer.advance();
    if (index < 0 || index >= sequencer.length()) return;
    rel::RelationalEvent event;
    if (!sequencer.copyEventForAudio(index, event)) return;
    if (!sequencer.shouldTrigger(event)) return;
    const int note = std::clamp(60 + static_cast<int>(std::lround(event.pitch)), 0, 127);
    const double frequency = effectiveFrequency(note, 1, event.tuningOffsetCents);
    triggerVoice(note, 1, static_cast<float>(event.velocity), frequency);
    if (event.ratio > 0.0) {
        if (!voices_.empty()) {
            // The most recently triggered voice is the one whose note/channel match this event.
            for (auto& v : voices_)
                if (v.active && v.note == note && v.channel == 1) {
                    v.fm.setRatio(event.ratio);
                    v.fm.setIndex(event.fmIndex);
                    break;
                }
        }
    }
}

void RelationalEXProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midi)
{
    juce::ScopedNoDenormals noDenormals;
    buffer.clear();

    for (const auto metadata : midi) handleMidiMessage(metadata.getMessage());

    const double bpmValue = bpm();
    const double samplesPerStep = sampleRate_ * 60.0 / std::max(1.0, bpmValue) / 4.0;
    if (sequenceRunning_.load()) {
        sequenceSampleCounter_ += buffer.getNumSamples();
        while (sequenceSampleCounter_ >= samplesPerStep) {
            sequenceSampleCounter_ -= samplesPerStep;
            advanceSequencer();
        }
    }

    const float level = param(*this, "level", 0.72f);
    const float drift = param(*this, "tuningDrift", 0.0f);
    const float resonance = param(*this, "resonance", 0.0f);
    lfo_.setRate(param(*this, "lfoRate", 0.35f));
    chaos_.setAmount(param(*this, "chaosAmount", 0.0f) / 30.0);
    const double relationDepth = param(*this, "relationDepth", 0.65f);
    const double lfoAmount = param(*this, "lfoAmount", 0.0f) / 100.0;
    if (mts.available() && mts.hasMaster()) {
        for (auto& v : voices_) if (v.active && v.note >= 0)
            v.frequency = mts.noteFrequency(v.note, v.channel - 1);
    }

    std::uniform_real_distribution<float> random(-1.0f, 1.0f);
    for (int sample = 0; sample < buffer.getNumSamples(); ++sample) {
        float out = 0.0f;
        float display = 0.0f;
        const double lfo = static_cast<double>(lfo_.process());
        const double chaos = static_cast<double>(chaos_.process());
        const rel::RelationEngine::Vector relationInput {
                0.0,
                static_cast<double>(sequenceSampleCounter_ > 0.0 ? 0.15 : -0.15),
                lfo * 0.35 * lfoAmount,
                static_cast<double>(resonance) * 2.0 - 1.0,
                chaos
        };
        const auto relation = relations_.transform(relationInput, relationDepth);
        for (auto& v : voices_) {
            if (!v.active) continue;
            const float d = static_cast<float>(random(audioRng_) * drift * 0.0000015 + relation[rel::RelationEngine::Tuning] * 0.00003);
            const float fm = v.fm.process(d + static_cast<float>(relation[rel::RelationEngine::Pitch] * 0.0005));
            float x = fm * v.envelope * v.velocity;
            if (resonance > 0.001f) x += v.resonator.process() * static_cast<float>(juce::jlimit(0.0, 1.0, resonance + relation[rel::RelationEngine::Resonance] * 0.15)) * v.envelope;
            out += x;
            display += std::abs(x);

            if (!v.releasing) {
                v.envelope = std::min(1.0f, v.envelope + 0.0025f);
            } else {
                v.envelope = std::max(0.0f, v.envelope - v.releaseRate);
                if (v.envelope <= 0.0001f) v.active = false;
            }
        }
        out = std::tanh(out * 0.85f) * level * 0.7f;
        buffer.setSample(0, sample, out);
        buffer.setSample(1, sample, out);
        visualPhase_.store(0.98f * visualPhase_.load() + 0.02f * juce::jlimit(0.0f, 1.0f, display));
    }
}

int RelationalEXProcessor::getNumPrograms() { return static_cast<int>(rel::PresetLibrary::factoryPresets().size()); }

void RelationalEXProcessor::setCurrentProgram(int index)
{
    applyFactoryPreset(index);
}

juce::String RelationalEXProcessor::getProgramName(int index)
{
    const auto& p = rel::PresetLibrary::factoryPresets();
    if (index < 0 || index >= static_cast<int>(p.size())) return {};
    return p[static_cast<std::size_t>(index)].name;
}

void RelationalEXProcessor::applyFactoryPreset(int index)
{
    const auto& presets = rel::PresetLibrary::factoryPresets();
    if (presets.empty()) return;
    index = std::clamp(index, 0, static_cast<int>(presets.size()) - 1);
    const auto& p = presets[static_cast<std::size_t>(index)];
    auto set = [this](const char* id, float value) {
        if (auto* ap = apvts.getParameter(id)) ap->setValueNotifyingHost(ap->convertTo0to1(value));
    };
    auto setInt = [this](const char* id, int value) {
        if (auto* ap = apvts.getParameter(id)) ap->setValueNotifyingHost(ap->convertTo0to1(static_cast<float>(value)));
    };
    set("fmRatio", p.fmRatio); set("fmIndex", p.fmIndex); set("fmFeedback", p.feedback);
    set("level", p.output); set("tuningDrift", p.drift); set("inharmonicity", p.inharmonicity);
    set("resonance", p.resonance); set("bpm", p.bpm);
    set("lfoRate", 0.35f); set("lfoAmount", 0.0f); set("chaosAmount", 0.0f);
    setInt("tuningDivisions", p.tuningDivisions); setInt("seqSteps", p.steps); setInt("seqPulses", p.pulses);
    tuning.setEqualTemperament(p.tuningDivisions);
    tuningEditBuffer_ = tuning.points();
    refreshTuningSnapshot();
    sequencer.setLength(p.steps); sequencer.applyEuclidean(p.pulses, 0); resetSequence();
    presetIndex_ = index;
    sequencer.publishToAudio();
}

void RelationalEXProcessor::setTuningDivisions(int divisions)
{
    divisions = std::clamp(divisions, 1, 53);
    tuning.setEqualTemperament(divisions);
    tuningEditBuffer_ = tuning.points();
    refreshTuningSnapshot();
    if (auto* p = apvts.getParameter("tuningDivisions")) p->setValueNotifyingHost(p->convertTo0to1(static_cast<float>(divisions)));
}


void RelationalEXProcessor::scanScaleDatabase()
{
    std::vector<std::filesystem::path> roots;
    const auto exeDir = juce::File::getSpecialLocation(juce::File::currentExecutableFile).getParentDirectory();
    roots.emplace_back(exeDir.getChildFile("Resources/Scales").getFullPathName().toStdString());
    roots.emplace_back(juce::File::getSpecialLocation(juce::File::userApplicationDataDirectory).getChildFile("RELATIONAL/Scales").getFullPathName().toStdString());
    scaleCatalog_.scan(roots);
}

bool RelationalEXProcessor::loadScalaFile(const juce::File& file, juce::String& error)
{
    std::string e;
    if (!rel::ScalaParser::loadSclFile(file.getFullPathName().toStdString(), tuning, e)) {
        error = juce::String(e);
        return false;
    }
    tuningEditBuffer_ = tuning.points();
    refreshTuningSnapshot();
    if (auto* p = apvts.getParameter("tuningDivisions")) p->setValueNotifyingHost(p->convertTo0to1(static_cast<float>(tuning.degreesPerPeriod())));
    return true;
}

void RelationalEXProcessor::commitTuningEdit()
{
    tuning.setPoints(tuningEditBuffer_);
    refreshTuningSnapshot();
}

void RelationalEXProcessor::randomizeTuning()
{
    tuningEditBuffer_ = tuning.points();
    if (tuningEditBuffer_.empty()) return;
    std::uniform_real_distribution<double> cents(-35.0, 35.0);
    for (std::size_t i = 1; i < tuningEditBuffer_.size(); ++i)
        tuningEditBuffer_[i].ratio *= std::pow(2.0, cents(uiRng_) / 1200.0);
    tuning.setPoints(tuningEditBuffer_);
    refreshTuningSnapshot();
}

void RelationalEXProcessor::saveSequenceToState(juce::ValueTree& state) const
{
    auto seq = state.getOrCreateChildWithName("SEQUENCER", nullptr);
    seq.removeAllProperties(nullptr);
    seq.setProperty("length", sequencer.length(), nullptr);
    for (int i = 0; i < sequencer.length(); ++i) {
        const auto& e = sequencer.events()[static_cast<std::size_t>(i)];
        auto ev = juce::ValueTree("EVENT");
        ev.setProperty("position", e.position, nullptr);
        ev.setProperty("pitch", e.pitch, nullptr);
        ev.setProperty("ratio", e.ratio, nullptr);
        ev.setProperty("velocity", e.velocity, nullptr);
        ev.setProperty("probability", e.probability, nullptr);
        ev.setProperty("fmIndex", e.fmIndex, nullptr);
        ev.setProperty("tuningOffsetCents", e.tuningOffsetCents, nullptr);
        ev.setProperty("timingOffset", e.timingOffset, nullptr);
        ev.setProperty("active", e.active, nullptr);
        seq.addChild(ev, -1, nullptr);
    }
}

void RelationalEXProcessor::loadSequenceFromState(const juce::ValueTree& state)
{
    auto seq = state.getChildWithName("SEQUENCER");
    if (!seq.isValid()) return;
    const int n = juce::jlimit(1, 32, static_cast<int>(seq.getProperty("length", 16)));
    sequencer.setLength(n);
    for (int i = 0; i < n && i < seq.getNumChildren(); ++i) {
        auto ev = seq.getChild(i);
        auto& e = sequencer.events()[static_cast<std::size_t>(i)];
        e.position = static_cast<double>(ev.getProperty("position", e.position));
        e.pitch = static_cast<double>(ev.getProperty("pitch", e.pitch));
        e.ratio = static_cast<double>(ev.getProperty("ratio", e.ratio));
        e.velocity = static_cast<double>(ev.getProperty("velocity", e.velocity));
        e.probability = static_cast<double>(ev.getProperty("probability", e.probability));
        e.fmIndex = static_cast<double>(ev.getProperty("fmIndex", e.fmIndex));
        e.tuningOffsetCents = static_cast<double>(ev.getProperty("tuningOffsetCents", e.tuningOffsetCents));
        e.timingOffset = static_cast<double>(ev.getProperty("timingOffset", e.timingOffset));
        e.active = static_cast<bool>(ev.getProperty("active", e.active));
    }
    sequencer.publishToAudio();
}

void RelationalEXProcessor::getStateInformation(juce::MemoryBlock& dest)
{
    auto state = apvts.copyState();
    state.setProperty("presetIndex", presetIndex_, nullptr);

    auto tuningState = state.getOrCreateChildWithName("TUNING", nullptr);
    tuningState.removeAllChildren(nullptr);
    tuningState.setProperty("periodRatio", tuning.periodRatio(), nullptr);
    tuningState.setProperty("rootFrequency", tuning.rootFrequency(), nullptr);
    for (const auto& point : tuning.points()) {
        auto node = juce::ValueTree("POINT");
        node.setProperty("ratio", point.ratio, nullptr);
        node.setProperty("cents", point.cents, nullptr);
        tuningState.addChild(node, -1, nullptr);
    }

    saveSequenceToState(state);
    if (auto xml = state.createXml())
        copyXmlToBinary(*xml, dest);
}

void RelationalEXProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    if (auto xml = getXmlFromBinary(data, sizeInBytes)) {
        auto state = juce::ValueTree::fromXml(*xml);
        if (!state.isValid() || state.getType() != apvts.state.getType())
            return;

        apvts.replaceState(state);
        presetIndex_ = juce::jlimit(0, getNumPrograms() - 1,
                                    static_cast<int>(state.getProperty("presetIndex", 0)));

        auto tuningState = state.getChildWithName("TUNING");
        if (tuningState.isValid() && tuningState.getNumChildren() > 0) {
            std::vector<rel::TuningPoint> points;
            points.reserve(static_cast<std::size_t>(tuningState.getNumChildren()));
            for (int i = 0; i < tuningState.getNumChildren(); ++i) {
                const auto node = tuningState.getChild(i);
                points.push_back({ static_cast<double>(node.getProperty("ratio", 1.0)),
                                   static_cast<double>(node.getProperty("cents", 0.0)) });
            }
            tuning.setPeriod(static_cast<double>(tuningState.getProperty("periodRatio", 2.0)));
            tuning.setPoints(std::move(points));
            tuning.setRootFrequency(static_cast<double>(tuningState.getProperty("rootFrequency", 440.0)));
            tuningEditBuffer_ = tuning.points();
            refreshTuningSnapshot();
        } else {
            setTuningDivisions(static_cast<int>(param(*this, "tuningDivisions", 12.0f)));
        }

        loadSequenceFromState(state);
    }
}

juce::AudioProcessorEditor* RelationalEXProcessor::createEditor()
{
    return new RelationalEXEditor(*this);
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new RelationalEXProcessor();
}
