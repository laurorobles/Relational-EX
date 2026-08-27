#include "PluginEditor.h"

void FrequencyFieldComponent::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xff101114));
    auto area = getLocalBounds().toFloat().reduced(24.0f);
    g.setColour(juce::Colour(0xff34383d));
    g.drawRect(area, 1.0f);

    const auto& pts = processor.tuning.points();
    if (pts.empty()) {
        g.setColour(juce::Colour(0xff68717a));
        g.setFont(juce::FontOptions(11.0f));
        g.drawText("No tuning points loaded", area, juce::Justification::centred);
        return;
    }
    const int n = static_cast<int>(pts.size());
    for (int i = 0; i < n; ++i) {
        const float x = area.getX() + area.getWidth() * ((i + 0.5f) / static_cast<float>(n));
        const float y = area.getCentreY() - static_cast<float>(std::log2(std::max(pts[static_cast<size_t>(i)].ratio, 1e-9)) * 22.0);
        g.setColour(juce::Colour(0xffd8e0e8));
        g.fillEllipse(x - 3.0f, y - 3.0f, 6.0f, 6.0f);
    }

    g.setColour(juce::Colour(0xffb7c1ca));
    g.setFont(juce::FontOptions(12.0f));
    g.drawText("FREQUENCY FIELD", area.getX(), 6, 180, 18, juce::Justification::left);
    g.setColour(juce::Colour(0xff68717a));
    g.drawText("Tuning points • drag interaction prototype", area.getX(), getHeight() - 24, 320, 16, juce::Justification::left);
}

void RelationalSequencerComponent::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xff0d0e10));
    auto area = getLocalBounds().toFloat().reduced(24.0f);
    g.setColour(juce::Colour(0xff34383d)); g.drawRect(area, 1.0f);
    g.setColour(juce::Colour(0xffd8e0e8)); g.setFont(juce::FontOptions(12.0f));
    g.drawText("RELATIONAL SEQUENCER", area.getX(), 6, 220, 18, juce::Justification::left);

    const auto& events = processor.sequencer.events();
    const int n = static_cast<int>(events.size());
    for (int i = 0; i < n; ++i) {
        const auto& ev = events[static_cast<size_t>(i)];
        const float x = area.getX() + static_cast<float>(ev.position) * area.getWidth();
        const float y = area.getBottom() - 32.0f - static_cast<float>(ev.pitch) * 18.0f;
        const float r = 5.0f + static_cast<float>(ev.velocity) * 5.0f;
        g.setColour(i == selected_ ? juce::Colour(0xffffc857) : juce::Colour(0xffc7d0d8));
        g.fillEllipse(x - r, y - r, r * 2.0f, r * 2.0f);
        g.setColour(juce::Colour(0xff464d54));
        g.drawLine(x, area.getCentreY(), x, y, 1.0f);
    }
    g.setColour(juce::Colour(0xff50585f));
    g.drawHorizontalLine(static_cast<int>(area.getCentreY()), area.getX(), area.getRight());
}

void RelationalSequencerComponent::mouseDown(const juce::MouseEvent& e)
{
    const auto area = getLocalBounds().toFloat().reduced(24.0f);
    const int n = static_cast<int>(processor.sequencer.events().size());
    selected_ = n > 0 ? juce::jlimit(0, n - 1, static_cast<int>(((e.position.x - area.getX()) / area.getWidth()) * n)) : -1;
    repaint();
}

void RelationalSequencerComponent::mouseDrag(const juce::MouseEvent& e)
{
    if (selected_ < 0) return;
    auto& ev = processor.sequencer.events()[static_cast<size_t>(selected_)];
    const auto area = getLocalBounds().toFloat().reduced(24.0f);
    ev.position = juce::jlimit(0.0, 0.999, static_cast<double>((e.position.x - area.getX()) / area.getWidth()));
    ev.pitch = juce::jlimit(-8.0, 8.0, static_cast<double>((area.getBottom() - e.position.y) / 18.0));
    repaint();
}

RelationalEditor::RelationalEditor(RelationalProcessor& p)
    : AudioProcessorEditor(&p), processor(p), field(p), sequencer(p)
{
    setSize(1200, 760);
    addAndMakeVisible(field); addAndMakeVisible(sequencer);

    auto configure = [this](juce::Slider& s, juce::Label& label, const juce::String& name, const juce::String& help) {
        s.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        s.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 90, 20);
        rel::ui::setHelp(s, help);
        label.setText(name, juce::dontSendNotification);
        label.setJustificationType(juce::Justification::centred);
        addAndMakeVisible(s); addAndMakeVisible(label);
    };

    configure(ratioSlider, ratioLabel, "FM RATIO", "Sets the frequency relationship between carrier and modulator. Non-integer ratios create increasingly inharmonic sidebands.");
    configure(indexSlider, indexLabel, "FM INDEX", "Controls FM depth. Higher values introduce more sidebands and greater spectral complexity.");
    configure(levelSlider, levelLabel, "OUTPUT", "Controls the final output level of the instrument.");
    configure(driftSlider, driftLabel, "TUNING DRIFT", "Adds slow pitch variation around the selected tuning. Higher values create a more unstable character.");

    ratioAtt = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processor.apvts, "fmRatio", ratioSlider);
    indexAtt = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processor.apvts, "fmIndex", indexSlider);
    levelAtt = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processor.apvts, "level", levelSlider);
    driftAtt = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processor.apvts, "tuningDrift", driftSlider);
    startTimerHz(30);
}

void RelationalEditor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xff08090b));
    g.setColour(juce::Colour(0xffe4e7ea));
    g.setFont(juce::FontOptions(22.0f, juce::Font::bold));
    g.drawText("RELATIONAL", 24, 14, 220, 30, juce::Justification::left);
    g.setColour(juce::Colour(0xff6c737b));
    g.setFont(juce::FontOptions(10.0f));
    g.drawText("FREQUENCY / RHYTHM / SPECTRUM / MOTION", 26, 42, 320, 16, juce::Justification::left);
    g.setColour(juce::Colour(0xff2a2e33));
    g.drawLine(24.0f, 62.0f, static_cast<float>(getWidth() - 24), 62.0f, 1.0f);

    g.setColour(juce::Colour(0xffaeb7bf));
    g.setFont(juce::FontOptions(9.0f));
    g.drawText("Every interactive control provides contextual help on hover.", 24, getHeight() - 18, 360, 14, juce::Justification::left);
}

void RelationalEditor::resized()
{
    auto r = getLocalBounds().reduced(24);
    r.removeFromTop(48);
    auto top = r.removeFromTop(330);
    const int controlsW = 280;
    field.setBounds(top.removeFromLeft(top.getWidth() - controlsW).toNearestInt());

    auto controls = top.reduced(12);
    const int cellW = controls.getWidth() / 2;
    const int cellH = controls.getHeight() / 2;
    juce::Slider* sliders[] = { &ratioSlider, &indexSlider, &levelSlider, &driftSlider };
    juce::Label* labels[] = { &ratioLabel, &indexLabel, &levelLabel, &driftLabel };
    for (int i = 0; i < 4; ++i) {
        auto cell = controls.removeFromTop(cellH);
        if (i % 2 == 0) cell = cell.removeFromLeft(cellW);
        else cell = cell.removeFromRight(cellW);
        labels[i]->setBounds(cell.removeFromTop(20));
        sliders[i]->setBounds(cell);
    }
    sequencer.setBounds(r.removeFromTop(r.getHeight() - 46));
}
