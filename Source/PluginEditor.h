#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "UI/TooltipSupport.h"

class FrequencyFieldComponent final : public rel::ui::TooltipComponent {
public:
    explicit FrequencyFieldComponent(RelationalEXProcessor& p) : processor(p) {
        setTooltip("Interactive tuning field. Drag a tuning point vertically to reshape its interval in cents. The field shows relative frequency relationships rather than fixed equal-tempered notes.");
        setMouseCursor(juce::MouseCursor::PointingHandCursor);
    }
    void paint(juce::Graphics&) override;
    void mouseDown(const juce::MouseEvent&) override;
    void mouseDrag(const juce::MouseEvent&) override;
    void mouseUp(const juce::MouseEvent&) override;
private:
    RelationalEXProcessor& processor;
    int selected_ = -1;
    float startY_ = 0.0f;
    double startCents_ = 0.0;
};

class SpectrumComponent final : public rel::ui::TooltipComponent {
public:
    explicit SpectrumComponent(RelationalEXProcessor& p) : processor(p) {
        setTooltip("Displays the current six-mode inharmonic resonance structure. Bar positions indicate relative mode ratios; bar movement follows the synthesis parameters.");
    }
    void paint(juce::Graphics&) override;
private:
    RelationalEXProcessor& processor;
};

class RelationalSequencerComponent final : public rel::ui::TooltipComponent {
public:
    explicit RelationalSequencerComponent(RelationalEXProcessor& p) : processor(p) {
        setTooltip("Interactive relational sequencer. Drag events in Field view, toggle cells in Grid view, or rotate events in Radial view. Each event stores pitch, velocity, probability, ratio, FM index and tuning offset.");
    }
    void paint(juce::Graphics&) override;
    void mouseDown(const juce::MouseEvent&) override;
    void mouseDrag(const juce::MouseEvent&) override;
    void mouseUp(const juce::MouseEvent&) override;
    void setViewMode(int mode) noexcept { viewMode_ = mode % 3; repaint(); }
    int viewMode() const noexcept { return viewMode_; }
private:
    RelationalEXProcessor& processor;
    int selected_ = -1;
    int viewMode_ = 0;
};

class RelationalEXEditor final : public juce::AudioProcessorEditor, private juce::Timer {
public:
    explicit RelationalEXEditor(RelationalEXProcessor&);
    void paint(juce::Graphics&) override;
    void resized() override;
private:
    void timerCallback() override { field.repaint(); spectrum.repaint(); sequencer.repaint(); }
    void showPresetMenu();
    void showTuningMenu();
    void chooseScalaFile();
    void configureSlider(juce::Slider&, juce::Label&, const juce::String&, const juce::String&, double min, double max, double step);

    RelationalEXProcessor& processor;
    juce::TooltipWindow tooltipWindow;

    FrequencyFieldComponent field;
    SpectrumComponent spectrum;
    RelationalSequencerComponent sequencer;

    rel::ui::TooltipButton presetButton { "PRESETS" };
    rel::ui::TooltipButton tuningButton { "TUNING" };
    rel::ui::TooltipButton scalaButton { "LOAD .SCL" };
    rel::ui::TooltipButton viewButton { "FIELD" };
    rel::ui::TooltipButton playButton { "PLAY" };
    rel::ui::TooltipButton euclidButton { "EUCLID" };

    rel::ui::TooltipSlider ratioSlider;
    rel::ui::TooltipSlider indexSlider;
    rel::ui::TooltipSlider feedbackSlider;
    rel::ui::TooltipSlider driftSlider;
    rel::ui::TooltipSlider inharmonicitySlider;
    rel::ui::TooltipSlider resonanceSlider;
    rel::ui::TooltipSlider bpmSlider;
    rel::ui::TooltipSlider lfoRateSlider, lfoAmountSlider, chaosAmountSlider;

    juce::Label ratioLabel, indexLabel, feedbackLabel, driftLabel;
    juce::Label inharmonicityLabel, resonanceLabel, bpmLabel;
    juce::Label lfoRateLabel, lfoAmountLabel, chaosAmountLabel;
    juce::Label statusLabel;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> ratioAtt, indexAtt, feedbackAtt, driftAtt;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> inharmonicityAtt, resonanceAtt, bpmAtt;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> lfoRateAtt, lfoAmountAtt, chaosAmountAtt;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RelationalEXEditor)
};
