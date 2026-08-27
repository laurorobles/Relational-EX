#pragma once
#include <JuceHeader.h>
#include "Core/Tuning/TuningSystem.h"
#include "Core/Tuning/MidiTuningStandard.h"
#include "Core/Tuning/MTSClientBridge.h"
#include "Core/Sequencer/RelationalSequencer.h"
#include "Core/DSP/FMOscillator.h"

class RelationalProcessor final : public juce::AudioProcessor {
public:
    RelationalProcessor();
    ~RelationalProcessor() override = default;

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
    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int) override {}
    const juce::String getProgramName(int) override { return {}; }
    void changeProgramName(int, const juce::String&) override {}
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;

    void getStateInformation(juce::MemoryBlock&) override;
    void setStateInformation(const void*, int) override;

    juce::AudioProcessorValueTreeState apvts;
    rel::TuningSystem tuning;
    rel::MidiTuningStandard midiTuning;
    rel::MTSClientBridge mts;
    rel::RelationalSequencer sequencer;

private:
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    rel::FMOscillator fm_;
    double sampleRate_ = 48000.0;
    bool voiceActive_ = false;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RelationalProcessor)
};
