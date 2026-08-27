#pragma once
#include <JuceHeader.h>
#include <array>
#include <memory>
#include <atomic>
#include <random>
#include <vector>
#include "Core/Tuning/TuningSystem.h"
#include "Core/Tuning/MidiTuningStandard.h"
#include "Core/Tuning/MTSClientBridge.h"
#include "Core/Tuning/ScalaParser.h"
#include "Core/Sequencer/RelationalSequencer.h"
#include "Core/Relations/RelationEngine.h"
#include "Core/DSP/FMOscillator.h"
#include "Core/DSP/ResonatorBank.h"
#include "Core/Modulation/LFO.h"
#include "Core/Modulation/ChaosDrift.h"
#include "Core/Presets/PresetLibrary.h"
#include "Core/Database/ScaleCatalog.h"

class RelationalEXProcessor final : public juce::AudioProcessor {
public:
    RelationalEXProcessor();
    ~RelationalEXProcessor() override;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }
    const juce::String getName() const override { return JucePlugin_Name; }
    bool acceptsMidi() const override { return true; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }
    int getNumPrograms() override;
    int getCurrentProgram() override { return presetIndex_; }
    void setCurrentProgram(int index) override;
    const juce::String getProgramName(int index) override;
    void changeProgramName(int, const juce::String&) override {}
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;

    void getStateInformation(juce::MemoryBlock&) override;
    void setStateInformation(const void*, int) override;

    void applyFactoryPreset(int index);
    void setTuningDivisions(int divisions);
    bool loadScalaFile(const juce::File& file, juce::String& error);
    void scanScaleDatabase();
    const rel::ScaleCatalog& scaleCatalog() const noexcept { return scaleCatalog_; }

    void setSequenceRunning(bool running) noexcept { sequenceRunning_.store(running); }
    bool isSequenceRunning() const noexcept { return sequenceRunning_.load(); }
    void resetSequence() noexcept { sequencer.reset(); sequenceSampleCounter_ = 0.0; }

    int currentSequenceStep() const noexcept { return sequencer.currentStep(); }
    double bpm() const noexcept { return apvts.getRawParameterValue("bpm")->load(); }

    // GUI-facing tuning state.
    std::vector<rel::TuningPoint>& mutableTuningPoints() noexcept { return tuningEditBuffer_; }
    void commitTuningEdit();
    void randomizeTuning();
    void refreshTuningSnapshot() noexcept;
    double frequencyFromSnapshot(int midiNote) const noexcept;

    juce::AudioProcessorValueTreeState apvts;
    rel::TuningSystem tuning;
    rel::MidiTuningStandard midiTuning;
    rel::MTSClientBridge mts;
    rel::RelationalSequencer sequencer;
    rel::RelationEngine relations_;

private:
    struct Voice {
        rel::FMOscillator fm;
        rel::ResonatorBank resonator;
        int note = -1;
        int channel = 1;
        bool active = false;
        bool releasing = false;
        double frequency = 440.0;
        float envelope = 0.0f;
        float velocity = 0.0f;
        float releaseRate = 0.00035f;
    };

    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    void handleMidiMessage(const juce::MidiMessage& message);
    void triggerVoice(int midiNote, int channel, float velocity, double forcedFrequency = 0.0);
    void releaseVoice(int midiNote, int channel);
    void advanceSequencer();
    double effectiveFrequency(int midiNote, int channel, double centsOffset) const noexcept;
    void saveSequenceToState(juce::ValueTree& state) const;
    void loadSequenceFromState(const juce::ValueTree& state);

    std::array<Voice, 8> voices_{};
    std::vector<rel::TuningPoint> tuningEditBuffer_;
    rel::ScaleCatalog scaleCatalog_;
    std::atomic<bool> sequenceRunning_{false};
    int presetIndex_ = 0;
    double sampleRate_ = 48000.0;
    double sequenceSampleCounter_ = 0.0;
    using TuningSnapshot = std::array<double, 128>;
    std::shared_ptr<const TuningSnapshot> tuningSnapshot_;
    rel::LFO lfo_;
    rel::ChaosDrift chaos_;
    std::atomic<float> visualPhase_{0.0f};
    std::mt19937 audioRng_{0x52454C54u};
    std::mt19937 uiRng_{0x5549524Cu};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RelationalEXProcessor)
};
