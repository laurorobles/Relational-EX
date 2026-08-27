#pragma once
#include <JuceHeader.h>

namespace rel::ui {

// JUCE tooltips belong to SettableTooltipClient, not to Component itself.
// This small helper component lets custom-drawn modules participate in the
// same tooltip system as native JUCE widgets.
class TooltipComponent : public juce::Component,
                         public juce::SettableTooltipClient
{
public:
    explicit TooltipComponent(const juce::String& text = {})
    {
        setTooltip(text);
    }

    ~TooltipComponent() override = default;
};

class TooltipSlider : public juce::Slider
{
public:
    explicit TooltipSlider(const juce::String& text = {})
    {
        setTooltip(text);
    }
};

inline void setHelp(juce::SettableTooltipClient& component,
                    const juce::String& englishDescription)
{
    component.setTooltip(englishDescription);
}

} // namespace rel::ui
