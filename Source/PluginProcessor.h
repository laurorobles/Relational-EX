#pragma once

#include <JuceHeader.h>

#include <array>
#include <cstdint>
#include <atomic>
#include <memory>
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

class RelationalEXProcessor final : public juce::AudioProcessor
{
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

    const rel::ScaleCatalog& scaleCatalog() const noexcept
    {
        return scaleCatalog_;
    }

    // --------------------------------------------------------
    // Sequencer
    // --------------------------------------------------------

    void setSequenceRunning(bool running) noexcept
    {
        sequenceRunning_.store(running, std::memory_order_release);
    }

    bool isSequenceRunning() const noexcept
    {
        return sequenceRunning_.load(std::memory_order_acquire);
    }

    void resetSequence() noexcept
    {
        sequencer.reset();
        sequenceSampleCounter_ = 0.0;
    }

    // Independent relational cycle controls.
    void setRhythmCycle(int length) noexcept;
    void setPitchCycle(int length) noexcept;
    void setFMCycle(int length) noexcept;
    void setTuningCycle(int length) noexcept;
    void setMotionCycle(int length) noexcept;

    double relationalPhase() const noexcept
    {
        return sequencer.combinedCycleProgress();
    }

    std::uint64_t relationalPeriod() const noexcept
    {
        return sequencer.combinedCycleLength();
    }

    std::uint64_t relationalStepsUntilAlignment() const noexcept
    {
        return sequencer.stepsUntilAlignment();
    }

    bool relationalIsAligned() const noexcept
    {
        return sequencer.isFullyAligned();
    }

    int currentSequenceStep() const noexcept
    {
        return sequencer.currentStep();
    }

    double bpm() const noexcept;

    // --------------------------------------------------------
    // Tuning / relations
    // --------------------------------------------------------

    std::vector<rel::TuningPoint>& mutableTuningPoints() noexcept
    {
        return tuningEditBuffer_;
    }

    void commitTuningEdit();
    void randomizeTuning();

    void resetRelations();
    void publishRelations() noexcept { relations_.publish(); }

    // --------------------------------------------------------
    // Standalone test keyboard + MIDI monitor
    // --------------------------------------------------------

    void triggerTestNote(int midiNote, float velocity = 0.8f);
    void releaseTestNote(int midiNote);

    int monitoredMidiNote() const noexcept
    {
        return monitoredMidiNote_.load(std::memory_order_relaxed);
    }

    float monitoredVelocity() const noexcept
    {
        return monitoredVelocity_.load(std::memory_order_relaxed);
    }

    double monitoredFrequency() const noexcept
    {
        return monitoredFrequency_.load(std::memory_order_relaxed);
    }

    bool monitoredGate() const noexcept
    {
        return monitoredGate_.load(std::memory_order_relaxed);
    }

    void refreshTuningSnapshot() noexcept;
    double frequencyFromSnapshot(int midiNote) const noexcept;

    // --------------------------------------------------------
    // Public engine state used by UI
    // --------------------------------------------------------
    
    juce::UndoManager undoManager;
    juce::AudioProcessorValueTreeState apvts;
    rel::TuningSystem tuning;
    rel::MidiTuningStandard midiTuning;
    rel::MTSClientBridge mts;
    rel::RelationalSequencer sequencer;
    rel::RelationEngine relations_;

private:
    struct Voice
    {
        rel::FMOscillator fm;
        rel::ResonatorBank resonator;
        juce::ADSR adsr;
        juce::dsp::StateVariableTPTFilter<float> filter;

        int note = -1;
        int channel = 1;

        bool active = false;
        bool releasing = false;
        bool isSequencer = false;

        double frequency = 440.0;
        double targetFrequency = 440.0;
        double tuningCents = 0.0;

        float envelope = 0.0f;
        float velocity = 0.0f;
        float releaseRate = 0.00035f;
        float baseRatio = 1.0f;
        float baseIndex = 2.0f;
        int samplesUntilRelease = -1;
    };

    static juce::AudioProcessorValueTreeState::ParameterLayout
    createParameterLayout();

    void handleMidiMessage(const juce::MidiMessage& message);
    void triggerVoice(int midiNote, int channel, float velocity, double forcedFrequency = 0.0, int durationSamples = -1);
    void releaseVoice(int midiNote, int channel);
    void advanceSequencer();

    struct TestEvent { int note = 0; float velocity = 0.0f; bool noteOn = false; };
    void pushTestEvent(int midiNote, float velocity, bool noteOn) noexcept;
    void drainTestEvents();

    double effectiveFrequency(int midiNote, int channel, double centsOffset) const noexcept;

    void saveSequenceToState(juce::ValueTree& state) const;
    void loadSequenceFromState(const juce::ValueTree& state);
    void saveRelationsToState(juce::ValueTree& state) const;
    void loadRelationsFromState(const juce::ValueTree& state);

    void updateMidiMonitor(int midiNote, int channel, float velocity, bool gate) noexcept;
    int snapToScale(int note, int root, int scaleIndex, int degreeShift = 0) const noexcept;

    juce::AbstractFifo testEventFifo_ { 64 };
    std::array<TestEvent, 64> testEvents_{};

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

    bool wasPlaying_ = false;
    std::array<float, 16> channelPitchBend_{};

    std::atomic<int> monitoredMidiNote_{-1};
    std::atomic<float> monitoredVelocity_{0.0f};
    std::atomic<double> monitoredFrequency_{0.0};
    std::atomic<bool> monitoredGate_{false};

    std::mt19937 audioRng_{0x52454C54u};
    std::mt19937 uiRng_{0x5549524Cu};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RelationalEXProcessor)
};