#include "PluginEditor.h"
#include <cmath>

namespace {
const auto bg = juce::Colour(0xff0a0b0d);
const auto panel = juce::Colour(0xff121419);
const auto panel2 = juce::Colour(0xff171a20);
const auto line = juce::Colour(0xff2d333b);
const auto text = juce::Colour(0xffdce2e8);
const auto muted = juce::Colour(0xff76808a);
const auto accent = juce::Colour(0xffffbd4a);
const auto accent2 = juce::Colour(0xff8fe6d0);

void moduleFrame(juce::Graphics& g, juce::Rectangle<float> r, const juce::String& title)
{
    g.setColour(panel);
    g.fillRect(r);
    g.setColour(line);
    g.drawRect(r, 1.0f);
    g.setColour(text);
    g.setFont(juce::FontOptions(10.0f, juce::Font::bold));
    g.drawText(title, r.getX() + 12.0f, r.getY() + 8.0f, 220.0f, 16.0f, juce::Justification::left);
}
}

void FrequencyFieldComponent::paint(juce::Graphics& g)
{
    g.fillAll(panel2);
    auto r = getLocalBounds().toFloat().reduced(14.0f);
    g.setColour(line);
    for (int i = 0; i <= 10; ++i) {
        const float x = r.getX() + r.getWidth() * i / 10.0f;
        g.drawVerticalLine(static_cast<int>(x), r.getY() + 26.0f, r.getBottom() - 20.0f);
    }
    for (int i = 0; i <= 6; ++i) {
        const float y = r.getY() + 26.0f + (r.getHeight() - 46.0f) * i / 6.0f;
        g.drawHorizontalLine(static_cast<int>(y), r.getX(), r.getRight());
    }

    const auto& pts = processor.tuning.points();
    if (pts.empty()) return;
    const int n = static_cast<int>(pts.size());
    for (int i = 0; i < n; ++i) {
        const auto& p = pts[static_cast<std::size_t>(i)];
        const float x = r.getX() + r.getWidth() * (i + 0.5f) / static_cast<float>(n);
        const float cents = static_cast<float>(1200.0 * std::log2(std::max(p.ratio, 1.0e-9)));
        const float norm = juce::jlimit(-1.0f, 1.0f, cents / 1200.0f);
        const float y = r.getCentreY() - norm * (r.getHeight() * 0.36f);
        const float radius = i == selected_ ? 7.5f : 5.0f;
        g.setColour(i == selected_ ? accent : accent2);
        g.fillEllipse(x - radius, y - radius, radius * 2.0f, radius * 2.0f);
        if (i > 0) {
            const auto& prev = pts[static_cast<std::size_t>(i - 1)];
            const float prevCents = static_cast<float>(1200.0 * std::log2(std::max(prev.ratio, 1.0e-9)));
            const float prevNorm = juce::jlimit(-1.0f, 1.0f, prevCents / 1200.0f);
            const float prevY = r.getCentreY() - prevNorm * (r.getHeight() * 0.36f);
            const float prevX = r.getX() + r.getWidth() * (i - 0.5f) / static_cast<float>(n);
            g.setColour(line.brighter(0.2f));
            g.drawLine(prevX, prevY, x, y, 1.0f);
        }
    }

    g.setColour(muted);
    g.setFont(juce::FontOptions(9.0f));
    g.drawText("RATIO FIELD / CENTS", r.getX(), r.getY() + 26.0f, 180.0f, 15.0f, juce::Justification::left);
    g.drawText(juce::String(processor.tuning.degreesPerPeriod()) + " degrees", r.getX(), r.getBottom() - 16.0f, 120.0f, 14.0f, juce::Justification::left);
}

void FrequencyFieldComponent::mouseDown(const juce::MouseEvent& e)
{
    const auto r = getLocalBounds().toFloat().reduced(14.0f);
    const auto& pts = processor.tuning.points();
    selected_ = -1;
    float best = 100000.0f;
    for (int i = 0; i < static_cast<int>(pts.size()); ++i) {
        const float x = r.getX() + r.getWidth() * (i + 0.5f) / static_cast<float>(pts.size());
        const float cents = static_cast<float>(1200.0 * std::log2(std::max(pts[static_cast<std::size_t>(i)].ratio, 1e-9)));
        const float y = r.getCentreY() - juce::jlimit(-1.0f, 1.0f, cents / 1200.0f) * (r.getHeight() * 0.36f);
        const float d = e.position.getDistanceFrom({x, y});
        if (d < best && d < 20.0f) { best = d; selected_ = i; }
    }
    if (selected_ >= 0) {
        startY_ = e.position.y;
        startCents_ = 1200.0 * std::log2(std::max(pts[static_cast<std::size_t>(selected_)].ratio, 1e-9));
    }
    repaint();
}

void FrequencyFieldComponent::mouseDrag(const juce::MouseEvent& e)
{
    if (selected_ < 0) return;
    const double delta = static_cast<double>(startY_ - e.position.y) * 4.0;
    auto& points = processor.mutableTuningPoints();
    if (selected_ < static_cast<int>(points.size())) {
        points[static_cast<std::size_t>(selected_)].ratio = std::pow(2.0, (startCents_ + delta) / 1200.0);
        processor.commitTuningEdit();
    }
    repaint();
}

void FrequencyFieldComponent::mouseUp(const juce::MouseEvent&) { selected_ = -1; repaint(); }

void SpectrumComponent::paint(juce::Graphics& g)
{
    g.fillAll(panel2);
    const auto r = getLocalBounds().toFloat().reduced(14.0f);
    g.setColour(muted);
    g.setFont(juce::FontOptions(9.0f));
    g.drawText("INHARMONIC MODE MAP", r.getX(), r.getY() + 26.0f, 180.0f, 15.0f, juce::Justification::left);

    const float complexity = processor.apvts.getRawParameterValue("fmIndex")->load();
    const float inharm = processor.apvts.getRawParameterValue("inharmonicity")->load();
    const float feedback = processor.apvts.getRawParameterValue("fmFeedback")->load();
    static constexpr double modes[] = { 1.0, 2.01, 3.02, 4.08, 5.15, 6.31 };
    for (int i = 0; i < 6; ++i) {
        const float h = static_cast<float>(juce::jlimit(0.05, 1.0, 0.12 + complexity * (0.03 + 0.012 * i)));
        const float ratio = static_cast<float>(modes[i] + (modes[i] - (i + 1.0)) * inharm);
        const float x = r.getX() + r.getWidth() * i / 6.0f + 8.0f;
        const float barW = r.getWidth() / 7.0f;
        const float barH = (r.getHeight() - 54.0f) * h;
        g.setColour(i % 2 == 0 ? accent2 : accent);
        g.fillRect(x, r.getBottom() - barH - 10.0f, barW * 0.55f, barH);
        g.setColour(muted);
        g.setFont(juce::FontOptions(8.0f));
        g.drawText(juce::String(ratio, 2), x, r.getBottom() - 8.0f, barW, 12.0f, juce::Justification::left);
    }
    g.setColour(muted);
    g.drawText("RATIO / INDEX / FEEDBACK", r.getRight() - 170.0f, r.getY() + 26.0f, 170.0f, 15.0f, juce::Justification::right);
    g.setColour(accent.withAlpha(0.55f));
    g.fillRect(r.getX(), r.getY() + 46.0f, r.getWidth() * feedback, 2.0f);
}

void RelationalSequencerComponent::paint(juce::Graphics& g)
{
    g.fillAll(panel2);
    auto r = getLocalBounds().toFloat().reduced(14.0f);
    g.setColour(muted); g.setFont(juce::FontOptions(9.0f));
    g.drawText(viewMode_ == 0 ? "FIELD / EVENT SPACE" : viewMode_ == 1 ? "GRID / EVENT MATRIX" : "RADIAL / PHASE CYCLE",
               r.getX(), r.getY() + 26.0f, 240.0f, 15.0f, juce::Justification::left);

    const auto& events = processor.sequencer.events();
    const int n = static_cast<int>(events.size());
    if (n <= 0) return;

    if (viewMode_ == 0) {
        for (int i = 0; i < n; ++i) {
            const auto& ev = events[static_cast<std::size_t>(i)];
            const float x = r.getX() + r.getWidth() * static_cast<float>(ev.position);
            const float y = r.getCentreY() - static_cast<float>(ev.pitch) * 7.0f;
            const float radius = 4.0f + static_cast<float>(ev.velocity) * 4.0f;
            g.setColour(i == selected_ ? accent : (ev.active ? accent2 : line));
            g.fillEllipse(x - radius, y - radius, radius * 2.0f, radius * 2.0f);
            g.setColour(line);
            g.drawLine(x, r.getCentreY(), x, y, 1.0f);
        }
        g.setColour(line);
        g.drawHorizontalLine(static_cast<int>(r.getCentreY()), r.getX(), r.getRight());
    } else if (viewMode_ == 1) {
        const float cellW = r.getWidth() / static_cast<float>(n);
        for (int i = 0; i < n; ++i) {
            const auto& ev = events[static_cast<std::size_t>(i)];
            const auto cell = juce::Rectangle<float>(r.getX() + i * cellW, r.getY() + 34.0f, cellW - 2.0f, r.getHeight() - 44.0f);
            g.setColour(ev.active ? accent.withAlpha(0.75f) : panel);
            g.fillRect(cell);
            g.setColour(line); g.drawRect(cell, 1.0f);
            g.setColour(text); g.setFont(juce::FontOptions(8.0f));
            g.drawText(juce::String(i + 1), cell, juce::Justification::centredTop);
        }
    } else {
        const auto center = r.getCentre();
        const float radius = std::min(r.getWidth(), r.getHeight()) * 0.34f;
        g.setColour(line); g.drawEllipse(center.x - radius, center.y - radius, radius * 2.0f, radius * 2.0f, 1.0f);
        for (int i = 0; i < n; ++i) {
            const auto& ev = events[static_cast<std::size_t>(i)];
            const float a = static_cast<float>(juce::MathConstants<double>::twoPi * ev.position - juce::MathConstants<double>::halfPi);
            const float rr = radius * (0.55f + 0.035f * static_cast<float>(juce::jlimit(-8.0, 8.0, ev.pitch) + 8.0));
            const float x = center.x + std::cos(a) * rr;
            const float y = center.y + std::sin(a) * rr;
            g.setColour(i == selected_ ? accent : (ev.active ? accent2 : line));
            g.fillEllipse(x - 5.0f, y - 5.0f, 10.0f, 10.0f);
        }
    }
}

void RelationalSequencerComponent::mouseDown(const juce::MouseEvent& e)
{
    const auto r = getLocalBounds().toFloat().reduced(14.0f);
    const int n = static_cast<int>(processor.sequencer.events().size());
    selected_ = -1;
    if (n <= 0) return;
    if (viewMode_ == 1) {
        const int index = juce::jlimit(0, n - 1, static_cast<int>((e.position.x - r.getX()) / (r.getWidth() / n)));
        processor.sequencer.events()[static_cast<std::size_t>(index)].active = !processor.sequencer.events()[static_cast<std::size_t>(index)].active;
        selected_ = index;
    } else if (viewMode_ == 0) {
        float best = 24.0f;
        for (int i = 0; i < n; ++i) {
            const auto& ev = processor.sequencer.events()[static_cast<std::size_t>(i)];
            const float x = r.getX() + r.getWidth() * static_cast<float>(ev.position);
            const float y = r.getCentreY() - static_cast<float>(ev.pitch) * 7.0f;
            const float d = e.position.getDistanceFrom({x, y});
            if (d < best) { best = d; selected_ = i; }
        }
    } else {
        const auto center = r.getCentre();
        const float a = std::atan2(e.position.y - center.y, e.position.x - center.x) + juce::MathConstants<float>::halfPi;
        double pos = std::fmod(a + juce::MathConstants<float>::twoPi, juce::MathConstants<float>::twoPi) / juce::MathConstants<float>::twoPi;
        for (int i = 0; i < n; ++i) {
            const auto& ev = processor.sequencer.events()[static_cast<std::size_t>(i)];
            const double d = std::abs(ev.position - pos);
            if (d < 0.04) { selected_ = i; break; }
        }
    }
    repaint();
}

void RelationalSequencerComponent::mouseDrag(const juce::MouseEvent& e)
{
    if (selected_ < 0 || selected_ >= static_cast<int>(processor.sequencer.events().size())) return;
    auto& ev = processor.sequencer.events()[static_cast<std::size_t>(selected_)];
    const auto r = getLocalBounds().toFloat().reduced(14.0f);
    if (viewMode_ == 0) {
        ev.position = juce::jlimit(0.0, 0.999, static_cast<double>((e.position.x - r.getX()) / r.getWidth()));
        ev.pitch = juce::jlimit(-8.0, 8.0, static_cast<double>((r.getCentreY() - e.position.y) / 7.0f));
    } else if (viewMode_ == 2) {
        const auto c = r.getCentre();
        const double a = std::atan2(e.position.y - c.y, e.position.x - c.x) + juce::MathConstants<double>::halfPi;
        ev.position = std::fmod(a + juce::MathConstants<double>::twoPi, juce::MathConstants<double>::twoPi) / juce::MathConstants<double>::twoPi;
    }
    processor.sequencer.publishToAudio();
    repaint();
}

void RelationalSequencerComponent::mouseUp(const juce::MouseEvent&) { repaint(); }

RelationalEXEditor::RelationalEXEditor(RelationalEXProcessor& p)
    : AudioProcessorEditor(&p), tooltipWindow(this, 650), processor(p), field(p), spectrum(p), sequencer(p)
{
    setResizable(true, true);
    setResizeLimits(980, 680, 1800, 1100);
    setSize(1280, 820);

    addAndMakeVisible(field); addAndMakeVisible(spectrum); addAndMakeVisible(sequencer);
    addAndMakeVisible(presetButton); addAndMakeVisible(tuningButton); addAndMakeVisible(scalaButton);
    addAndMakeVisible(viewButton); addAndMakeVisible(playButton); addAndMakeVisible(euclidButton);
    addAndMakeVisible(statusLabel);

    presetButton.setTooltip("Browse inspiring factory presets grouped by tuning, FM, motion, modal and extreme sound design.");
    tuningButton.setTooltip("Select an equal-temperament tuning preset. Custom Scala tuning can be loaded with LOAD .SCL.");
    scalaButton.setTooltip("Open a Scala .scl file and load its scale intervals into the Frequency Field.");
    viewButton.setTooltip("Cycle the sequencer visualization between Field, Grid and Radial modes.");
    playButton.setTooltip("Start or stop the internal relational sequencer clock.");
    euclidButton.setTooltip("Regenerate the current sequence using its Steps and Pulses relationship.");

    auto styleButton = [](juce::TextButton& b) {
        b.setButtonText(b.getButtonText());
        b.setColour(juce::TextButton::buttonColourId, panel2);
        b.setColour(juce::TextButton::buttonOnColourId, accent.withAlpha(0.35f));
        b.setColour(juce::TextButton::textColourOffId, text);
        b.setColour(juce::TextButton::textColourOnId, accent);
    };
    styleButton(presetButton); styleButton(tuningButton); styleButton(scalaButton);
    styleButton(viewButton); styleButton(playButton); styleButton(euclidButton);

    configureSlider(ratioSlider, ratioLabel, "FM RATIO", "Sets the frequency relationship between carrier and modulator. Non-integer ratios create inharmonic sidebands.", 0.01, 8.0, 0.001);
    configureSlider(indexSlider, indexLabel, "FM INDEX", "Controls FM depth. Higher values create more sidebands and greater spectral complexity.", 0.0, 20.0, 0.01);
    configureSlider(feedbackSlider, feedbackLabel, "FM FEEDBACK", "Feeds the FM output back into phase. Higher values add dense, unstable sidebands and aggressive timbres.", 0.0, 0.95, 0.001);
    configureSlider(driftSlider, driftLabel, "TUNING DRIFT", "Adds subtle random pitch movement around the selected tuning. Higher values destabilize the pitch field.", 0.0, 30.0, 0.01);
    configureSlider(inharmonicitySlider, inharmonicityLabel, "INHARMONICITY", "Moves the resonance modes away from harmonic integer multiples, producing metallic or object-like spectra.", 0.0, 1.0, 0.001);
    configureSlider(resonanceSlider, resonanceLabel, "RESONANCE", "Adds a six-mode inharmonic resonator to the FM voice. Higher values emphasize ringing body and physical-object color.", 0.0, 1.0, 0.001);
    configureSlider(bpmSlider, bpmLabel, "BPM", "Internal sequencer tempo in beats per minute. The current prototype advances on sixteenth-note boundaries.", 40.0, 240.0, 0.1);
    configureSlider(lfoRateSlider, lfoRateLabel, "LFO RATE", "Sets the free-running low-frequency modulation rate used by the motion layer.", 0.01, 20.0, 0.001);
    configureSlider(lfoAmountSlider, lfoAmountLabel, "LFO AMOUNT", "Controls how strongly the LFO perturbs the pitch phase path. Keep it subtle for motion rather than vibrato.", 0.0, 100.0, 0.01);
    configureSlider(chaosAmountSlider, chaosAmountLabel, "CHAOS", "Adds deterministic nonlinear drift from a logistic-map modulation source. Higher values create less periodic motion.", 0.0, 30.0, 0.01);

    ratioAtt = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processor.apvts, "fmRatio", ratioSlider);
    indexAtt = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processor.apvts, "fmIndex", indexSlider);
    feedbackAtt = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processor.apvts, "fmFeedback", feedbackSlider);
    driftAtt = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processor.apvts, "tuningDrift", driftSlider);
    inharmonicityAtt = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processor.apvts, "inharmonicity", inharmonicitySlider);
    resonanceAtt = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processor.apvts, "resonance", resonanceSlider);
    bpmAtt = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processor.apvts, "bpm", bpmSlider);
    lfoRateAtt = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processor.apvts, "lfoRate", lfoRateSlider);
    lfoAmountAtt = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processor.apvts, "lfoAmount", lfoAmountSlider);
    chaosAmountAtt = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processor.apvts, "chaosAmount", chaosAmountSlider);

    presetButton.onClick = [this] { showPresetMenu(); };
    tuningButton.onClick = [this] { showTuningMenu(); };
    scalaButton.onClick = [this] { chooseScalaFile(); };
    viewButton.onClick = [this] { sequencer.setViewMode(sequencer.viewMode() + 1); viewButton.setButtonText(sequencer.viewMode() == 0 ? "FIELD" : sequencer.viewMode() == 1 ? "GRID" : "RADIAL"); };
    playButton.onClick = [this] { processor.setSequenceRunning(!processor.isSequenceRunning()); playButton.setButtonText(processor.isSequenceRunning() ? "STOP" : "PLAY"); };
    euclidButton.onClick = [this] {
        const int steps = static_cast<int>(processor.apvts.getRawParameterValue("seqSteps")->load());
        const int pulses = static_cast<int>(processor.apvts.getRawParameterValue("seqPulses")->load());
        processor.sequencer.setLength(steps); processor.sequencer.applyEuclidean(juce::jlimit(0, steps, pulses)); processor.sequencer.publishToAudio(); sequencer.repaint();
    };

    statusLabel.setText("READY / INTERNAL TUNING", juce::dontSendNotification);
    statusLabel.setColour(juce::Label::textColourId, muted);
    statusLabel.setFont(juce::FontOptions(9.0f));
    startTimerHz(24);
}

void RelationalEXEditor::configureSlider(juce::Slider& s, juce::Label& label, const juce::String& name, const juce::String& help, double min, double max, double step)
{
    s.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    s.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 74, 18);
    s.setRange(min, max, step);
    s.setColour(juce::Slider::rotarySliderFillColourId, accent);
    s.setColour(juce::Slider::rotarySliderOutlineColourId, line);
    rel::ui::setHelp(s, help);
    label.setText(name, juce::dontSendNotification);
    label.setColour(juce::Label::textColourId, muted);
    label.setFont(juce::FontOptions(8.0f, juce::Font::bold));
    label.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(s); addAndMakeVisible(label);
}

void RelationalEXEditor::showPresetMenu()
{
    juce::PopupMenu menu;
    const auto& presets = rel::PresetLibrary::factoryPresets();
    std::string last;
    int id = 1;
    for (const auto& p : presets) {
        if (p.category != last) {
            if (!last.empty()) menu.addSeparator();
            last = p.category;
        }
        const bool checked = (id - 1 == processor.getCurrentProgram());
        menu.addItem(id, juce::String(p.name), true, checked);
        ++id;
    }
    menu.showMenuAsync(juce::PopupMenu::Options().withTargetComponent(&presetButton), [this](int result) {
        if (result > 0) {
            processor.applyFactoryPreset(result - 1);
            presetButton.setButtonText(processor.getProgramName(processor.getCurrentProgram()));
            statusLabel.setText("PRESET / " + processor.getProgramName(processor.getCurrentProgram()), juce::dontSendNotification);
        }
    });
}

void RelationalEXEditor::showTuningMenu()
{
    juce::PopupMenu menu;
    for (int div : {12, 17, 19, 22, 24, 31, 41, 53})
        menu.addItem(div, juce::String(div) + "-TET", true, processor.tuning.degreesPerPeriod() == div);

    const auto& scales = processor.scaleCatalog().entries();
    if (!scales.empty()) {
        menu.addSeparator();
        const int base = 100;
        const int count = juce::jmin(40, static_cast<int>(scales.size()));
        for (int i = 0; i < count; ++i)
            menu.addItem(base + i, juce::String(scales[static_cast<std::size_t>(i)].name));
    }

    menu.showMenuAsync(juce::PopupMenu::Options().withTargetComponent(&tuningButton), [this, scalesPtr = &processor.scaleCatalog()](int result) {
        if (result >= 1 && result <= 53) {
            processor.setTuningDivisions(result);
            tuningButton.setButtonText(juce::String(result) + "-TET");
            statusLabel.setText("TUNING / " + juce::String(result) + "-TET", juce::dontSendNotification);
            return;
        }
        if (result >= 100) {
            const int index = result - 100;
            const auto& entries = scalesPtr->entries();
            if (index >= 0 && index < static_cast<int>(entries.size())) {
                juce::String error;
                if (processor.loadScalaFile(juce::File(entries[static_cast<std::size_t>(index)].path), error)) {
                    tuningButton.setButtonText("SCALA");
                    statusLabel.setText("SCALA / " + juce::String(entries[static_cast<std::size_t>(index)].name), juce::dontSendNotification);
                } else statusLabel.setText("SCALA ERROR / " + error, juce::dontSendNotification);
            }
        }
    });
}

void RelationalEXEditor::chooseScalaFile()
{
    auto chooser = std::make_shared<juce::FileChooser>("Load Scala scale", juce::File{}, "*.scl");
    chooser->launchAsync(juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles, [this, chooser](const juce::FileChooser& fc) {
        const auto file = fc.getResult();
        if (!file.existsAsFile()) return;
        juce::String error;
        if (processor.loadScalaFile(file, error)) {
            tuningButton.setButtonText("SCALA");
            statusLabel.setText("SCALA / " + file.getFileName(), juce::dontSendNotification);
        } else {
            statusLabel.setText("SCALA ERROR / " + error, juce::dontSendNotification);
        }
    });
}

void RelationalEXEditor::paint(juce::Graphics& g)
{
    g.fillAll(bg);
    const auto b = getLocalBounds().toFloat();
    g.setColour(line); g.drawHorizontalLine(62, 18.0f, b.getRight() - 18.0f);
    g.setColour(text); g.setFont(juce::FontOptions(22.0f, juce::Font::bold));
    g.drawText("RELATIONAL EX", 18, 12, 240, 28, juce::Justification::left);
    g.setColour(muted); g.setFont(juce::FontOptions(9.0f));
    g.drawText("FREQUENCY / RHYTHM / SPECTRUM / MOTION", 20, 38, 360, 15, juce::Justification::left);

    // Thin alignment ticks, intentionally functional rather than decorative.
    g.setColour(line);
    for (int x = 20; x < getWidth() - 20; x += 32) g.drawVerticalLine(x, 58.0f, 62.0f);
}

void RelationalEXEditor::resized()
{
    auto area = getLocalBounds().reduced(18);
    area.removeFromTop(56);
    auto headerRight = area.removeFromTop(44);
    presetButton.setBounds(headerRight.removeFromLeft(166).reduced(2));
    tuningButton.setBounds(headerRight.removeFromLeft(106).reduced(2));
    scalaButton.setBounds(headerRight.removeFromLeft(112).reduced(2));
    viewButton.setBounds(headerRight.removeFromRight(92).reduced(2));
    playButton.setBounds(headerRight.removeFromRight(74).reduced(2));
    euclidButton.setBounds(headerRight.removeFromRight(82).reduced(2));
    statusLabel.setBounds(headerRight.removeFromLeft(380));

    auto top = area.removeFromTop(360);
    const int leftW = static_cast<int>(top.getWidth() * 0.58f);
    auto fieldArea = top.removeFromLeft(leftW);
    field.setBounds(fieldArea);

    auto right = top;
    auto specArea = right.removeFromTop(right.getHeight() * 0.46f);
    spectrum.setBounds(specArea);
    auto controls = right;
    controls = controls.reduced(8);

    const int columns = 4;
    const int cellW = controls.getWidth() / columns;
    const int cellH = controls.getHeight();
    juce::Slider* sliders[] = { &ratioSlider, &indexSlider, &feedbackSlider, &driftSlider };
    juce::Label* labels[] = { &ratioLabel, &indexLabel, &feedbackLabel, &driftLabel };
    for (int i = 0; i < 4; ++i) {
        auto c = controls.removeFromLeft(cellW).reduced(4);
        labels[i]->setBounds(c.removeFromTop(18));
        sliders[i]->setBounds(c);
    }

    auto mid2 = area.removeFromTop(142);
    juce::Slider* sliders2[] = { &inharmonicitySlider, &resonanceSlider, &bpmSlider, &lfoRateSlider, &lfoAmountSlider, &chaosAmountSlider };
    juce::Label* labels2[] = { &inharmonicityLabel, &resonanceLabel, &bpmLabel, &lfoRateLabel, &lfoAmountLabel, &chaosAmountLabel };
    for (int i = 0; i < 6; ++i) {
        auto c = mid2.removeFromLeft(mid2.getWidth() / (6 - i)).reduced(4);
        labels2[i]->setBounds(c.removeFromTop(18));
        sliders2[i]->setBounds(c);
    }

    sequencer.setBounds(area.removeFromTop(area.getHeight() - 4));
}
