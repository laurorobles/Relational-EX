#pragma once

#include <JuceHeader.h>

#include <array>

#include "PluginProcessor.h"
#include "UI/TooltipSupport.h"

class FrequencyFieldComponent final : public rel::ui::TooltipComponent
{
public:
    explicit FrequencyFieldComponent(RelationalEXProcessor& p)
        : processor(p)
    {
        setTooltip(
            "Interactive tuning field. Drag a point vertically to reshape its interval in cents. The field represents relative frequency relationships rather than fixed equal-tempered notes.");
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

class SpectrumComponent final : public rel::ui::TooltipComponent
{
public:
    explicit SpectrumComponent(RelationalEXProcessor& p)
        : processor(p)
    {
        setTooltip(
            "Displays the current six-mode inharmonic resonance structure. Mode height follows synthesis complexity; labels show the current relative mode ratios.");
    }

    void paint(juce::Graphics&) override;

private:
    RelationalEXProcessor& processor;
};

class RelationalSequencerComponent final : public rel::ui::TooltipComponent
{
public:
    explicit RelationalSequencerComponent(RelationalEXProcessor& p)
        : processor(p)
    {
        setTooltip(
            "Interactive relational sequencer. Drag events in Field view, toggle cells in Grid view, or rotate events in Radial view. Events contain pitch, velocity, probability, ratio, FM index and tuning offset.");
    }

    void paint(juce::Graphics&) override;
    void mouseDown(const juce::MouseEvent&) override;
    void mouseDrag(const juce::MouseEvent&) override;
    void mouseUp(const juce::MouseEvent&) override;

    void setViewMode(int mode) noexcept
    {
        viewMode_ = ((mode % 3) + 3) % 3;
        repaint();
    }

    int viewMode() const noexcept
    {
        return viewMode_;
    }

private:
    RelationalEXProcessor& processor;
    int selected_ = -1;
    int selectedCycle_ = -1;
    int viewMode_ = 0;
    bool draggingCycle_ = false;

    float dragStartX_ = 0.0f;
    float dragStartY_ = 0.0f;
    double dragStartPosition_ = 0.0;
    double dragStartPitch_ = 0.0;
    double dragStartDuration_ = 0.5;
    double dragStartVelocity_ = 1.0;
};

class RelationMatrixComponent final : public rel::ui::TooltipComponent
{
public:
    explicit RelationMatrixComponent(RelationalEXProcessor& p)
        : processor(p)
    {
        setTooltip(
            "Relational coupling matrix. Columns are sources and rows are destinations. Drag vertically to set the influence from -100% to +100%. Changes are published safely to the realtime engine.");
        setMouseCursor(juce::MouseCursor::CrosshairCursor);
    }

    void paint(juce::Graphics&) override;
    void mouseDown(const juce::MouseEvent&) override;
    void mouseDrag(const juce::MouseEvent&) override;
    void mouseUp(const juce::MouseEvent&) override;

private:
    RelationalEXProcessor& processor;
    int selectedSource_ = -1;
    int selectedDestination_ = -1;
    double startAmount_ = 0.0;
    float startY_ = 0.0f;
};

class OnScreenKeyboardComponent final : public rel::ui::TooltipComponent
{
public:
    explicit OnScreenKeyboardComponent(RelationalEXProcessor& p)
        : processor(p)
    {
        setTooltip(
            "Interactive chromatic keyboard for testing Relational EX without external MIDI hardware. Click and hold a key to play it; drag or release to stop.");
    }

    void paint(juce::Graphics&) override;
    void mouseDown(const juce::MouseEvent&) override;
    void mouseDrag(const juce::MouseEvent&) override;
    void mouseUp(const juce::MouseEvent&) override;

private:
    int noteForPosition(float x) const noexcept;
    juce::Rectangle<float> noteBounds(int note) const noexcept;

    RelationalEXProcessor& processor;
    int activeNote_ = -1;
};

class RelationalEXEditor final : public juce::AudioProcessorEditor,
                                 private juce::Timer
{
public:
    explicit RelationalEXEditor(RelationalEXProcessor&);

    void paint(juce::Graphics&) override;
    void resized() override;
    bool keyPressed(const juce::KeyPress& key) override;
    bool keyStateChanged(bool isKeyDown) override;

private:
    void timerCallback() override;

    void showPresetMenu();
    void showTuningMenu();
    void showRootMenu();
    void showScaleMenu();
    void updateScaleButtons();
    void showSynthModeMenu();
    void showLfoTargetMenu();
    void updateModeButtons();
    void chooseScalaFile();
    void showRelationEditor();

    void configureSlider(
        juce::Slider&,
        juce::Label&,
        const juce::String&,
        const juce::String&,
        double min,
        double max,
        double step);

    void pollComputerKeyboard();
    void updateMidiMonitor();
    void setMidiStatus(const juce::String& text);

    RelationalEXProcessor& processor;
    juce::TooltipWindow tooltipWindow;

    FrequencyFieldComponent field;
    SpectrumComponent spectrum;
    RelationalSequencerComponent sequencer;

    rel::ui::TooltipButton presetButton { "PRESETS" };
    rel::ui::TooltipButton tuningButton { "TUNING" };
    rel::ui::TooltipButton rootButton { "ROOT: C" };
    rel::ui::TooltipButton scaleButton { "SCALE: CHROMATIC" };
    rel::ui::TooltipButton scalaButton { "LOAD .SCL" };
    rel::ui::TooltipButton viewButton { "FIELD" };
    rel::ui::TooltipButton playButton { "PLAY" };
    rel::ui::TooltipButton euclidButton { "EUCLID" };
    rel::ui::TooltipButton relationsButton { "RELATIONS" };
    rel::ui::TooltipButton fmModeButton { "FM: LIVE" };

    rel::ui::TooltipButton lfoTargetButton { "LFO: RELATIONS" };
    rel::ui::TooltipButton synthModeButton { "MODE: POLY" };

    RelationMatrixComponent relationMatrix;
    OnScreenKeyboardComponent keyboard;

    rel::ui::TooltipSlider ratioSlider;
    rel::ui::TooltipSlider indexSlider;
    rel::ui::TooltipSlider feedbackSlider;
    rel::ui::TooltipSlider driftSlider;
    rel::ui::TooltipSlider inharmonicitySlider;
    rel::ui::TooltipSlider resonanceSlider;
    rel::ui::TooltipSlider bpmSlider;
    rel::ui::TooltipSlider lfoRateSlider;
    rel::ui::TooltipSlider lfoAmountSlider;
    rel::ui::TooltipSlider chaosAmountSlider;
    rel::ui::TooltipSlider glideSlider;

    // --- Envolventes y Filtro ---
    rel::ui::TooltipSlider attackSlider;
    rel::ui::TooltipSlider decaySlider;
    rel::ui::TooltipSlider sustainSlider;
    rel::ui::TooltipSlider releaseSlider;
    rel::ui::TooltipSlider cutoffSlider;

    juce::Label attackLabel { {}, "ATTACK" };
    juce::Label decayLabel { {}, "DECAY" };
    juce::Label sustainLabel { {}, "SUSTAIN" };
    juce::Label releaseLabel { {}, "RELEASE" };
    juce::Label cutoffLabel { {}, "CUTOFF" };
    // ----------------------------

    juce::Label ratioLabel { {}, "FM RATIO" };
    juce::Label indexLabel { {}, "FM INDEX" };
    juce::Label feedbackLabel { {}, "FM FEEDBACK" };
    juce::Label driftLabel { {}, "TUNING DRIFT" };
    juce::Label inharmonicityLabel { {}, "INHARMONICITY" };
    juce::Label resonanceLabel { {}, "RESONANCE" };
    juce::Label bpmLabel { {}, "BPM" };
    juce::Label lfoRateLabel { {}, "LFO RATE" };
    juce::Label lfoAmountLabel { {}, "LFO AMOUNT" };
    juce::Label chaosAmountLabel { {}, "CHAOS" };
    juce::Label glideLabel { {}, "GLIDE" };

    juce::Label midiNoteLabel;
    juce::Label midiVelocityLabel;
    juce::Label creditLabel;
    juce::Label midiFrequencyLabel;
    juce::Label midiInputLabel;
    juce::Label statusLabel;

    using Attachment =
        juce::AudioProcessorValueTreeState::SliderAttachment;

    std::unique_ptr<Attachment> ratioAtt;
    std::unique_ptr<Attachment> indexAtt;
    std::unique_ptr<Attachment> feedbackAtt;
    std::unique_ptr<Attachment> driftAtt;
    std::unique_ptr<Attachment> inharmonicityAtt;
    std::unique_ptr<Attachment> resonanceAtt;
    std::unique_ptr<Attachment> bpmAtt;
    std::unique_ptr<Attachment> lfoRateAtt;
    std::unique_ptr<Attachment> lfoAmountAtt;
    std::unique_ptr<Attachment> chaosAmountAtt;
    std::unique_ptr<Attachment> glideAtt;

    std::unique_ptr<Attachment> attackAtt;
    std::unique_ptr<Attachment> decayAtt;
    std::unique_ptr<Attachment> sustainAtt;
    std::unique_ptr<Attachment> releaseAtt;
    std::unique_ptr<Attachment> cutoffAtt;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> fmContinuousAtt;

    static constexpr int computerKeyCount = 17;
    std::array<int, computerKeyCount> computerKeys_ {
        'a', 'w', 's', 'e', 'd', 'f', 't', 'g', 'y',
        'h', 'u', 'j', 'k', 'o', 'l', 'p', ';'
    };

    std::array<bool, computerKeyCount> computerKeyStates_{};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RelationalEXEditor)
};
