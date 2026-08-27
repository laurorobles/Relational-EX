#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "UI/TooltipSupport.h"

class FrequencyFieldComponent final : public rel::ui::TooltipComponent {
public:
    FrequencyFieldComponent(RelationalProcessor& p) : processor(p) { setRepaintsOnMouseActivity(true); }
    void paint(juce::Graphics& g) override;
    void mouseDown(const juce::MouseEvent& e) override { dragPoint_ = e.position; repaint(); }
    void mouseDrag(const juce::MouseEvent& e) override { dragPoint_ = e.position; repaint(); }
private:
    RelationalProcessor& processor;
    juce::Point<float> dragPoint_ { 0.0f, 0.0f };
};

class RelationalSequencerComponent final : public rel::ui::TooltipComponent {
public:
    RelationalSequencerComponent(RelationalProcessor& p) : processor(p) {
        setRepaintsOnMouseActivity(true);
        setTooltip("Interactive event field. Drag events horizontally to change time and vertically to change pitch. Each event can later expose probability, ratio, FM index and tuning offset.");
    }
    void paint(juce::Graphics& g) override;
    void mouseDown(const juce::MouseEvent& e) override;
    void mouseDrag(const juce::MouseEvent& e) override;
private:
    RelationalProcessor& processor;
    int selected_ = -1;
};

class RelationalEditor final : public juce::AudioProcessorEditor, private juce::Timer {
public:
    explicit RelationalEditor(RelationalProcessor&);
    void paint(juce::Graphics&) override;
    void resized() override;
private:
    void timerCallback() override { repaint(); }
    RelationalProcessor& processor;
    FrequencyFieldComponent field;
    RelationalSequencerComponent sequencer;
    juce::Slider ratioSlider, indexSlider, levelSlider, driftSlider;
    juce::Label ratioLabel, indexLabel, levelLabel, driftLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> ratioAtt, indexAtt, levelAtt, driftAtt;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RelationalEditor)
};
