#pragma once
#include <JuceHeader.h>

namespace rel::ui {

// JUCE Slider/Button/ComboBox already implement SettableTooltipClient.
class TooltipComponent : public juce::Component, public juce::SettableTooltipClient {
public:
    explicit TooltipComponent(const juce::String& text = {}) { setTooltip(text); }
    ~TooltipComponent() override = default;
};

class TooltipSlider : public juce::Slider {
public:
    explicit TooltipSlider(const juce::String& text = {}) { setTooltip(text); }
};

class TooltipButton : public juce::TextButton {
public:
    TooltipButton(const juce::String& text = {}, const juce::String& help = {}) : juce::TextButton(text) { setTooltip(help); }
};

inline void setHelp(juce::SettableTooltipClient& component, const juce::String& englishDescription)
{
    component.setTooltip(englishDescription);
}

} // namespace rel::ui
