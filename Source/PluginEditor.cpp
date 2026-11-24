#include "PluginEditor.h"

#include <algorithm>
#include <cmath>
#include <limits>
#include <string>

namespace
{
const auto bg = juce::Colour(0xff0a0b0d);
const auto panel = juce::Colour(0xff121419);
const auto panel2 = juce::Colour(0xff171a20);
const auto line = juce::Colour(0xff2d333b);
const auto text = juce::Colour(0xffdce2e8);
const auto muted = juce::Colour(0xff76808a);
const auto accent = juce::Colour(0xffffbd4a);
const auto accent2 = juce::Colour(0xff8fe6d0);

void moduleFrame(juce::Graphics& g,
                 juce::Rectangle<float> r,
                 const juce::String& title)
{
    g.setColour(panel);
    g.fillRect(r);

    g.setColour(line);
    g.drawRect(r, 1.0f);

    g.setColour(text);
    g.setFont(juce::FontOptions(10.0f, juce::Font::bold));
    g.drawText(title,
               r.getX() + 12.0f,
               r.getY() + 8.0f,
               260.0f,
               16.0f,
               juce::Justification::left);
}

float normalizedCents(double ratio)
{
    if (!std::isfinite(ratio) || ratio <= 0.0)
        return 0.0f;

    const double cents = 1200.0 * std::log2(ratio);

    return juce::jlimit(
        -1.0f,
        1.0f,
        static_cast<float>(cents / 1200.0));
}

juce::String midiNoteName(int midiNote)
{
    if (midiNote < 0 || midiNote > 127)
        return "--";

    static constexpr const char* names[] =
    {
        "C", "C#", "D", "D#", "E", "F",
        "F#", "G", "G#", "A", "A#", "B"
    };

    const int octave = (midiNote / 12) - 1;

    return juce::String(names[midiNote % 12])
        + juce::String(octave);
}
}

void controlFrame(juce::Graphics& g, juce::Rectangle<float> r)
{
    g.setColour(juce::Colour(0xff101216));
    g.fillRoundedRectangle(r, 4.0f);
    g.setColour(line);
    g.drawRoundedRectangle(r, 4.0f, 1.0f);
}

class RelationalLookAndFeel final : public juce::LookAndFeel_V4
{
public:
    RelationalLookAndFeel()
    {
        setColour(juce::Slider::rotarySliderFillColourId, accent);
        setColour(juce::Slider::rotarySliderOutlineColourId, line);
        setColour(juce::Slider::thumbColourId, accent2);
        setColour(juce::Slider::textBoxTextColourId, text);
        setColour(juce::Slider::textBoxOutlineColourId, line);
        setColour(juce::Slider::textBoxBackgroundColourId, bg);
        setColour(juce::PopupMenu::backgroundColourId, panel);
        setColour(juce::PopupMenu::textColourId, text);
        setColour(juce::PopupMenu::highlightedBackgroundColourId, accent.withAlpha(0.22f));
        setColour(juce::PopupMenu::highlightedTextColourId, text);
    }

    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                          float sliderPosProportional, float rotaryStartAngle,
                          float rotaryEndAngle, juce::Slider& slider) override
    {
        const float size = static_cast<float>(juce::jmin(width, height));
        const float cx = x + width * 0.5f;
        const float cy = y + height * 0.5f;
        const float radius = juce::jmax(9.0f, size * 0.34f);

        const auto bounds = juce::Rectangle<float>(
            cx - radius, cy - radius, radius * 2.0f, radius * 2.0f);

        g.setColour(juce::Colour(0xff090a0c));
        g.fillEllipse(bounds);

        g.setColour(line);
        g.drawEllipse(bounds, 1.0f);

        const float angle =
            rotaryStartAngle
            + sliderPosProportional * (rotaryEndAngle - rotaryStartAngle);

        juce::Path arc;
        arc.addCentredArc(cx, cy, radius - 2.5f, radius - 2.5f, 0.0f,
                          rotaryStartAngle, angle, true);

        g.setColour(accent.withAlpha(0.28f));
        g.strokePath(arc, juce::PathStrokeType(3.0f, juce::PathStrokeType::curved,
                                               juce::PathStrokeType::rounded));

        juce::Path valueArc;
        valueArc.addCentredArc(cx, cy, radius - 2.5f, radius - 2.5f, 0.0f,
                               rotaryStartAngle, angle, true);

        g.setColour(accent);
        g.strokePath(valueArc, juce::PathStrokeType(2.0f, juce::PathStrokeType::curved,
                                                     juce::PathStrokeType::rounded));

        const float pointerRadius = radius - 8.0f;
        const juce::Point<float> pointerEnd(
            cx + std::cos(angle - juce::MathConstants<float>::halfPi) * pointerRadius,
            cy + std::sin(angle - juce::MathConstants<float>::halfPi) * pointerRadius);

        g.setColour(text);
        g.drawLine(cx, cy, pointerEnd.x, pointerEnd.y, 1.5f);

        g.setColour(slider.isMouseOverOrDragging() ? accent2 : text);
        g.fillEllipse(cx - 2.5f, cy - 2.5f, 5.0f, 5.0f);
    }
};

RelationalLookAndFeel relationalLookAndFeel;

// ============================================================
// Frequency Field
// ============================================================

void FrequencyFieldComponent::paint(juce::Graphics& g)
{
    const auto full = getLocalBounds().toFloat();

    g.fillAll(panel2);
    moduleFrame(g, full, "FREQUENCY FIELD");

    const auto r = full.reduced(14.0f);
    const float graphTop = r.getY() + 26.0f;
    const float graphBottom = r.getBottom() - 20.0f;
    const float graphHeight =
        std::max(1.0f, graphBottom - graphTop);

    g.setColour(line);

    for (int i = 0; i <= 10; ++i)
    {
        const float x =
            r.getX()
            + r.getWidth() * static_cast<float>(i) / 10.0f;

        g.drawVerticalLine(
            static_cast<int>(x),
            graphTop,
            graphBottom);
    }

    for (int i = 0; i <= 6; ++i)
    {
        const float y =
            graphTop
            + graphHeight * static_cast<float>(i) / 6.0f;

        g.drawHorizontalLine(
            static_cast<int>(y),
            r.getX(),
            r.getRight());
    }

    g.setColour(muted);
    g.setFont(juce::FontOptions(9.0f));

    g.drawText(
        "INTERVAL POSITION",
        r.getX(),
        r.getY() + 26.0f,
        180.0f,
        15.0f,
        juce::Justification::left);

    g.drawText(
        "CENT",
        r.getRight() - 54.0f,
        graphTop - 2.0f,
        54.0f,
        14.0f,
        juce::Justification::right);

    const auto& points = processor.tuning.points();

    g.drawText(
        juce::String(processor.tuning.degreesPerPeriod())
            + " degrees",
        r.getX(),
        r.getBottom() - 16.0f,
        120.0f,
        14.0f,
        juce::Justification::left);

    if (points.empty())
        return;

    const int count = static_cast<int>(points.size());

    for (int i = 1; i < count; ++i)
    {
        const auto& a = points[static_cast<std::size_t>(i - 1)];
        const auto& b = points[static_cast<std::size_t>(i)];

        const float x0 =
            r.getX()
            + r.getWidth()
                * (static_cast<float>(i) - 0.5f)
                / static_cast<float>(count);

        const float x1 =
            r.getX()
            + r.getWidth()
                * (static_cast<float>(i) + 0.5f)
                / static_cast<float>(count);

        const float y0 =
            r.getCentreY()
            - normalizedCents(a.ratio)
                * graphHeight
                * 0.36f;

        const float y1 =
            r.getCentreY()
            - normalizedCents(b.ratio)
                * graphHeight
                * 0.36f;

        g.setColour(line.brighter(0.18f));
        g.drawLine(x0, y0, x1, y1, 1.0f);
    }

    for (int i = 0; i < count; ++i)
    {
        const auto& point =
            points[static_cast<std::size_t>(i)];

        const float x =
            r.getX()
            + r.getWidth()
                * (static_cast<float>(i) + 0.5f)
                / static_cast<float>(count);

        const float y =
            r.getCentreY()
            - normalizedCents(point.ratio)
                * graphHeight
                * 0.36f;

        const float radius =
            i == selected_ ? 7.5f : 5.0f;

        g.setColour(i == selected_ ? accent : accent2);
        g.fillEllipse(
            x - radius,
            y - radius,
            radius * 2.0f,
            radius * 2.0f);
    }
}

void FrequencyFieldComponent::mouseDown(const juce::MouseEvent& e)
{
    const auto r =
        getLocalBounds().toFloat().reduced(14.0f);

    const auto& points = processor.tuning.points();

    selected_ = -1;

    if (points.empty())
    {
        repaint();
        return;
    }

    const int count = static_cast<int>(points.size());
    float bestDistance = std::numeric_limits<float>::max();

    for (int i = 0; i < count; ++i)
    {
        const auto& point =
            points[static_cast<std::size_t>(i)];

        const float x =
            r.getX()
            + r.getWidth()
                * (static_cast<float>(i) + 0.5f)
                / static_cast<float>(count);

        const float y =
            r.getCentreY()
            - normalizedCents(point.ratio)
                * r.getHeight()
                * 0.36f;

        const float distance =
            e.position.getDistanceFrom({ x, y });

        if (distance < bestDistance && distance < 22.0f)
        {
            bestDistance = distance;
            selected_ = i;
        }
    }

    if (selected_ >= 0)
    {
        startY_ = e.position.y;
        const auto ratio =
            points[static_cast<std::size_t>(selected_)].ratio;

        startCents_ =
            ratio > 0.0
                ? 1200.0 * std::log2(ratio)
                : 0.0;
        
        processor.undoManager.beginNewTransaction();
    }

    repaint();
}

void FrequencyFieldComponent::mouseDrag(const juce::MouseEvent& e)
{
    if (selected_ < 0)
        return;

    auto& points = processor.mutableTuningPoints();

    if (selected_ >= static_cast<int>(points.size()))
        return;

    const double delta =
        static_cast<double>(startY_ - e.position.y) * 4.0;

    const double ratio =
        std::pow(2.0, (startCents_ + delta) / 1200.0);

    if (std::isfinite(ratio) && ratio > 0.0)
    {
        points[static_cast<std::size_t>(selected_)].ratio = ratio;
        processor.commitTuningEdit();
    }

    repaint();
}

void FrequencyFieldComponent::mouseUp(const juce::MouseEvent&)
{
    selected_ = -1;
    repaint();
}

// ============================================================
// Spectrum
// ============================================================

void SpectrumComponent::paint(juce::Graphics& g)
{
    const auto full = getLocalBounds().toFloat();

    g.fillAll(panel2);
    moduleFrame(g, full, "SPECTRAL ENGINE");

    const auto r = full.reduced(14.0f);

    g.setColour(muted);
    g.setFont(juce::FontOptions(9.0f));

    g.drawText(
        "INHARMONIC MODE MAP",
        r.getX(),
        r.getY() + 26.0f,
        190.0f,
        15.0f,
        juce::Justification::left);

    const auto getParameter =
        [this](const char* id, float fallback)
        {
            if (auto* value = processor.apvts.getRawParameterValue(id))
                return value->load();
            return fallback;
        };

    const float complexity =
        getParameter("fmIndex", 2.0f);

    const float inharmonicity =
        getParameter("inharmonicity", 0.25f);

    const float feedback =
        getParameter("fmFeedback", 0.08f);

    static constexpr double modes[] =
    {
        1.00, 2.01, 3.02, 4.08, 5.15, 6.31
    };

    const float availableHeight =
        std::max(20.0f, r.getHeight() - 64.0f);

    const float slotWidth =
        r.getWidth() / 7.0f;

    for (int i = 0; i < 6; ++i)
    {
        const double harmonic =
            static_cast<double>(i + 1);

        const double ratio =
            modes[i]
            + (modes[i] - harmonic)
                * static_cast<double>(inharmonicity);

        const double heightNormalized =
            juce::jlimit(
                0.05,
                1.0,
                0.12
                    + static_cast<double>(complexity)
                        * (0.03 + 0.012 * i));

        const float barHeight =
            availableHeight
            * static_cast<float>(heightNormalized);

        const float x =
            r.getX()
            + slotWidth * static_cast<float>(i)
            + 8.0f;

        g.setColour(i % 2 == 0 ? accent2 : accent);

        g.fillRect(
            x,
            r.getBottom() - barHeight - 10.0f,
            slotWidth * 0.55f,
            barHeight);

        g.setColour(muted);
        g.setFont(juce::FontOptions(8.0f));

        g.drawText(
            juce::String(ratio, 2),
            x,
            r.getBottom() - 8.0f,
            slotWidth,
            12.0f,
            juce::Justification::left);
    }

    g.setColour(muted);
    g.drawText(
        "RATIO / INDEX / FEEDBACK",
        r.getRight() - 180.0f,
        r.getY() + 26.0f,
        180.0f,
        15.0f,
        juce::Justification::right);

    g.setColour(accent.withAlpha(0.55f));
    g.fillRect(
        r.getX(),
        r.getY() + 46.0f,
        r.getWidth()
            * juce::jlimit(0.0f, 1.0f, feedback),
        2.0f);
}

// ============================================================
// Sequencer
// ============================================================

namespace
{
int cycleForRow(int row, const rel::RelationalSequencer::CycleLengths& c)
{
    switch (row)
    {
        case 0: return c.rhythm;
        case 1: return c.pitch;
        case 2: return c.fm;
        case 3: return c.tuning;
        default: return c.motion;
    }
}
}

void RelationalSequencerComponent::paint(juce::Graphics& g)
{
    const auto full = getLocalBounds().toFloat();

    g.fillAll(panel2);
    moduleFrame(g, full, "RELATIONAL SEQUENCER");

    auto outer = full.reduced(10.0f);

    const float headerH = 26.0f;
    const float cycleH = 16.0f;
    const float footerH = 26.0f;
    const float gap = 4.0f;

    auto header = outer.removeFromTop(headerH);
    header.removeFromTop(1.0f);

    const auto modeName =
        viewMode_ == 0
            ? "FIELD / EVENT SPACE"
            : viewMode_ == 1
                ? "GRID / EVENT MATRIX"
                : "RADIAL / PHASE CYCLE";

    g.setColour(text);
    g.setFont(juce::FontOptions(9.0f, juce::Font::bold));
    g.drawText(modeName,
               header.getX(), header.getY(),
               230.0f, header.getHeight(),
               juce::Justification::left);

    const auto cycles = processor.sequencer.cycleLengths();
    const auto phases = processor.sequencer.cyclePositions();
    const int period =
        static_cast<int>(std::max<std::uint64_t>(1, processor.sequencer.combinedCycleLength()));
    const double globalPhase =
        processor.sequencer.combinedCycleProgress();

    g.setColour(muted);
    g.setFont(juce::FontOptions(8.0f));
    g.drawText(
        "PERIOD " + juce::String(period),
        header.getCentreX() - 90.0f,
        header.getY(),
        180.0f,
        header.getHeight(),
        juce::Justification::centred);

    g.drawText(
        "PHASE " + juce::String(globalPhase * 100.0, 1) + "%",
        header.getRight() - 120.0f,
        header.getY(),
        120.0f,
        header.getHeight(),
        juce::Justification::right);

    // --------------------------------------------------------
    // Five cycle strips.
    // --------------------------------------------------------

    for (int row = 0; row < 5; ++row)
    {
        auto strip = outer.removeFromTop(cycleH);

        const auto cycleLength = cycleForRow(row, cycles);
        const auto phase =
            row == 0 ? phases.rhythm
            : row == 1 ? phases.pitch
            : row == 2 ? phases.fm
            : row == 3 ? phases.tuning
                        : phases.motion;

        const char* label =
            row == 0 ? "RHYTHM"
            : row == 1 ? "PITCH"
            : row == 2 ? "FM"
            : row == 3 ? "TUNING"
                       : "MOTION";

        const float labelW = 58.0f;
        const float valueW = 56.0f;
        const float trackX = strip.getX() + labelW;
        const float trackW =
            std::max(20.0f,
                     strip.getWidth() - labelW - valueW);

        g.setColour(muted);
        g.setFont(juce::FontOptions(8.0f, juce::Font::bold));
        g.drawText(label,
                   strip.getX(), strip.getY(),
                   labelW - 4.0f, strip.getHeight(),
                   juce::Justification::left);

        const auto track =
            juce::Rectangle<float>(
                trackX,
                strip.getY() + 5.0f,
                trackW,
                strip.getHeight() - 10.0f);

        g.setColour(juce::Colour(0xff090a0c));
        g.fillRoundedRectangle(track, 2.0f);

        g.setColour(line);
        g.drawRoundedRectangle(track, 2.0f, 1.0f);

        const float phaseAmount =
            cycleLength > 1
                ? static_cast<float>(phase)
                    / static_cast<float>(cycleLength)
                : 0.0f;

        const float markerX =
            track.getX()
                + juce::jlimit(0.0f, 1.0f, phaseAmount)
                    * track.getWidth();

        g.setColour(accent.withAlpha(0.20f));
        g.fillRect(track.getX(), track.getY(),
                   std::max(1.0f, markerX - track.getX()),
                   track.getHeight());

        g.setColour(row == selectedCycle_
                        ? accent
                        : accent2);
        g.fillEllipse(markerX - 4.0f,
                      track.getCentreY() - 4.0f,
                      8.0f,
                      8.0f);

        g.setColour(text);
        g.setFont(juce::FontOptions(8.0f));
        g.drawText(
            juce::String(phase + 1)
                + "/"
                + juce::String(cycleLength),
            track.getRight() + 6.0f,
            strip.getY(),
            valueW - 6.0f,
            strip.getHeight(),
            juce::Justification::right);
    }

    outer.removeFromTop(gap);

    // --------------------------------------------------------
    // Event canvas.
    // --------------------------------------------------------

    auto footer = outer.removeFromBottom(footerH);
    auto eventCanvas = outer;

    const auto& events = processor.sequencer.events();
    const int count = static_cast<int>(events.size());

    if (count > 0 && eventCanvas.getWidth() > 1.0f
        && eventCanvas.getHeight() > 1.0f)
    {
        g.setColour(line);
        g.drawRect(eventCanvas, 1.0f);

        const int activeStep = processor.sequencer.currentStep();
        const float pulseHalo = 4.0f + 3.0f * std::sin(globalPhase * juce::MathConstants<float>::twoPi * period);

        if (viewMode_ == 0)
        {
            // FIELD VIEW
            g.setColour(line.withAlpha(0.55f));
            g.drawHorizontalLine(
                static_cast<int>(eventCanvas.getCentreY()),
                eventCanvas.getX(),
                eventCanvas.getRight());

            for (int i = 0; i < count; ++i)
            {
                const auto& event =
                    events[static_cast<std::size_t>(i)];

                const float x =
                    eventCanvas.getX()
                    + eventCanvas.getWidth()
                        * static_cast<float>(
                            juce::jlimit(0.0, 0.999, event.position));

                const float y =
                    eventCanvas.getCentreY()
                    - static_cast<float>(event.pitch / 8.0)
                        * eventCanvas.getHeight() * 0.44f;

                const float radius =
                    5.0f
                    + static_cast<float>(
                        juce::jlimit(0.0, 1.0, event.velocity))
                        * 4.0f;

                if (i == activeStep)
                {
                    g.setColour(accent.withAlpha(0.35f));
                    g.fillEllipse(
                        x - radius - pulseHalo,
                        y - radius - pulseHalo,
                        (radius + pulseHalo) * 2.0f,
                        (radius + pulseHalo) * 2.0f);
                }

                g.setColour(
                    i == selected_
                        ? accent
                        : event.active
                            ? accent2
                            : muted);

                g.drawLine(
                    x,
                    eventCanvas.getBottom() - 4.0f,
                    x,
                    y,
                    1.0f);

                g.fillEllipse(
                    x - radius,
                    y - radius,
                    radius * 2.0f,
                    radius * 2.0f);

                if (i == selected_)
                {
                    g.setColour(text.withAlpha(0.7f));
                    g.drawEllipse(
                        x - radius - 4.0f,
                        y - radius - 4.0f,
                        (radius + 4.0f) * 2.0f,
                        (radius + 4.0f) * 2.0f,
                        1.0f);
                }
            }
        }
        else if (viewMode_ == 1)
        {
            // GRID VIEW
            const float cellW =
                eventCanvas.getWidth()
                / static_cast<float>(count);

            for (int i = 0; i < count; ++i)
            {
                const auto& event =
                    events[static_cast<std::size_t>(i)];

                const auto cell =
                    juce::Rectangle<float>(
                        eventCanvas.getX()
                            + cellW * static_cast<float>(i),
                        eventCanvas.getY(),
                        std::max(1.0f, cellW - 2.0f),
                        eventCanvas.getHeight());

                g.setColour(
                    i == selected_
                        ? accent.withAlpha(0.34f)
                        : event.active
                            ? accent.withAlpha(0.15f)
                            : panel);
                g.fillRect(cell);

                g.setColour(line);
                g.drawRect(cell, 1.0f);

                g.setColour(event.active ? accent2 : muted);
                g.setFont(juce::FontOptions(8.0f));
                g.drawText(
                    juce::String(i + 1),
                    cell.getX() + 2.0f,
                    cell.getY() + 3.0f,
                    cell.getWidth() - 4.0f,
                    12.0f,
                    juce::Justification::left);

                const float pitchNorm =
                    juce::jlimit(
                        -1.0f,
                        1.0f,
                        static_cast<float>(event.pitch / 8.0));

                const float y =
                    cell.getCentreY()
                    - pitchNorm * cell.getHeight() * 0.36f;

                if (i == activeStep)
                {
                    g.setColour(accent.withAlpha(0.35f));
                    g.fillEllipse(
                        cell.getCentreX() - 4.0f - pulseHalo,
                        y - 4.0f - pulseHalo,
                        (4.0f + pulseHalo) * 2.0f,
                        (4.0f + pulseHalo) * 2.0f);
                }

                // Represent event duration as width
                const float durW = std::max(4.0f, static_cast<float>(event.duration) * cell.getWidth() * 0.9f);
                const float durH = 8.0f;
                
                g.setColour(event.active ? accent2 : muted);
                g.fillRoundedRectangle(
                    cell.getCentreX() - durW * 0.5f,
                    y - durH * 0.5f,
                    durW,
                    durH,
                    4.0f);
            }
        }
        else
        {
            // RADIAL VIEW
            const auto center = eventCanvas.getCentre();
            const float radius =
                std::min(eventCanvas.getWidth(),
                         eventCanvas.getHeight()) * 0.35f;

            g.setColour(line);
            g.drawEllipse(
                center.x - radius,
                center.y - radius,
                radius * 2.0f,
                radius * 2.0f,
                1.0f);

            const double globalAngle =
                globalPhase * juce::MathConstants<double>::twoPi
                - juce::MathConstants<double>::halfPi;

            g.setColour(accent.withAlpha(0.34f));
            g.drawLine(
                center.x,
                center.y,
                center.x
                    + std::cos(globalAngle) * radius,
                center.y
                    + std::sin(globalAngle) * radius,
                2.0f);

            for (int i = 0; i < count; ++i)
            {
                const auto& event =
                    events[static_cast<std::size_t>(i)];

                const double angle =
                    std::clamp(event.position, 0.0, 0.999)
                        * juce::MathConstants<double>::twoPi
                    - juce::MathConstants<double>::halfPi;

                const float pitchNorm =
                    juce::jlimit(
                        -1.0f,
                        1.0f,
                        static_cast<float>(event.pitch / 8.0));

                const float radialDistance =
                    radius * (0.76f + 0.15f * pitchNorm);

                const float x =
                    center.x
                    + static_cast<float>(std::cos(angle))
                        * radialDistance;

                const float y =
                    center.y
                    + static_cast<float>(std::sin(angle))
                        * radialDistance;

                if (i == activeStep)
                {
                    g.setColour(accent.withAlpha(0.35f));
                    g.fillEllipse(
                        x - 5.0f - pulseHalo,
                        y - 5.0f - pulseHalo,
                        (5.0f + pulseHalo) * 2.0f,
                        (5.0f + pulseHalo) * 2.0f);
                }

                g.setColour(
                    i == selected_
                        ? accent
                        : event.active
                            ? accent2
                            : muted);

                g.fillEllipse(
                    x - 5.0f,
                    y - 5.0f,
                    10.0f,
                    10.0f);

                if (i == selected_)
                {
                    g.setColour(text.withAlpha(0.65f));
                    g.drawLine(
                        center.x,
                        center.y,
                        x,
                        y,
                        1.0f);
                }
            }
        }
    }

    // --------------------------------------------------------
    // Context footer.
    // --------------------------------------------------------

    g.setColour(line);
    g.drawHorizontalLine(
        static_cast<int>(footer.getY()),
        footer.getX(),
        footer.getRight());

    g.setColour(muted);
    g.setFont(juce::FontOptions(7.5f));

    const bool hasSelection =
        selected_ >= 0
        && selected_ < count;

    juce::String selectionText =
        hasSelection
            ? "EVENT " + juce::String(selected_ + 1)
                + (events[static_cast<std::size_t>(selected_)].active
                    ? " / ACTIVE"
                    : " / MUTED")
            : "NO EVENT SELECTED";

    if (hasSelection)
    {
        const auto& event =
            events[static_cast<std::size_t>(selected_)];

        selectionText +=
            "    PITCH " + juce::String(event.pitch, 2)
            + "    VEL " + juce::String(event.velocity, 2)
            + "    PROB " + juce::String(event.probability, 2);
    }

    g.drawText(
        selectionText,
        footer.getX(),
        footer.getY() + 8.0f,
        footer.getWidth() * 0.64f,
        14.0f,
        juce::Justification::left);

    const juce::String help =
        viewMode_ == 0
            ? "CLICK / DRAG = SELECT + MOVE     SHIFT = TOGGLE"
            : viewMode_ == 1
                ? "CLICK = TOGGLE     DRAG = PITCH"
                : "CLICK / DRAG = ROTATE     SHIFT = TOGGLE";

    g.setColour(text.withAlpha(0.72f));
    g.drawText(
        help,
        footer.getX() + footer.getWidth() * 0.42f,
        footer.getY() + 8.0f,
        footer.getWidth() * 0.58f,
        14.0f,
        juce::Justification::right);
}

void RelationalSequencerComponent::mouseDown(
    const juce::MouseEvent& e)
{
    const auto full = getLocalBounds().toFloat();
    const auto outer = full.reduced(10.0f);

    const float headerH = 26.0f;
    const float cycleH = 16.0f;
    const float footerH = 26.0f;
    const float gap = 4.0f;

    auto header = outer;
    header.removeFromTop(headerH);

    // --------------------------------------------------------
    // Cycle strips have priority.
    // --------------------------------------------------------

    for (int row = 0; row < 5; ++row)
    {
        auto strip = header.removeFromTop(cycleH);

        const float labelW = 58.0f;
        const float valueW = 56.0f;
        const float trackX = strip.getX() + labelW;
        const float trackW =
            std::max(20.0f,
                     strip.getWidth() - labelW - valueW);

        const auto track =
            juce::Rectangle<float>(
                trackX,
                strip.getY(),
                trackW,
                strip.getHeight());

        if (track.contains(e.position))
        {
            selectedCycle_ = row;
            draggingCycle_ = true;

            const float normalized =
                juce::jlimit(
                    0.0f,
                    1.0f,
                    (e.position.x - track.getX())
                        / std::max(1.0f, track.getWidth()));

            const int newLength =
                juce::jlimit(
                    1,
                    32,
                    static_cast<int>(
                        std::lround(
                            1.0f
                            + normalized * 31.0f)));

            switch (row)
            {
                case 0:
                    processor.sequencer.setRhythmCycle(newLength);
                    break;
                case 1:
                    processor.sequencer.setPitchCycle(newLength);
                    break;
                case 2:
                    processor.sequencer.setFMCycle(newLength);
                    break;
                case 3:
                    processor.sequencer.setTuningCycle(newLength);
                    break;
                default:
                    processor.sequencer.setMotionCycle(newLength);
                    break;
            }

            processor.sequencer.publishToAudio();
            repaint();
            return;
        }
    }

    header.removeFromTop(gap);

    // --------------------------------------------------------
    // Canvas interaction.
    // --------------------------------------------------------

    auto canvas = header;
    canvas.removeFromBottom(footerH);

    const int count =
        static_cast<int>(processor.sequencer.events().size());

    if (count <= 0 || canvas.getWidth() <= 1.0f
        || canvas.getHeight() <= 1.0f)
    {
        selected_ = -1;
        repaint();
        return;
    }

    const auto local = e.position;

    if (viewMode_ == 1)
    {
        const float cellW =
            canvas.getWidth()
            / static_cast<float>(count);

        const int index =
            juce::jlimit(
                0,
                count - 1,
                static_cast<int>(
                    (local.x - canvas.getX())
                        / std::max(1.0f, cellW)));

        selected_ = index;

        if (e.mods.isShiftDown())
        {
            auto& event =
                processor.sequencer.events()[
                    static_cast<std::size_t>(selected_)];

            event.active = !event.active;
        }

        dragStartX_ = local.x;
        dragStartY_ = local.y;
        
        auto& ev = processor.sequencer.events()[static_cast<std::size_t>(selected_)];
        dragStartPitch_ = ev.pitch;
        dragStartDuration_ = ev.duration;
        dragStartVelocity_ = ev.velocity;

        processor.sequencer.publishToAudio();
        repaint();
        return;
    }

    if (viewMode_ == 2)
    {
        const auto center = canvas.getCentre();

        const double clickAngle =
            std::atan2(
                local.y - center.y,
                local.x - center.x)
            + juce::MathConstants<double>::halfPi;

        const double normalized =
            std::fmod(
                clickAngle + juce::MathConstants<double>::twoPi,
                juce::MathConstants<double>::twoPi)
            / juce::MathConstants<double>::twoPi;

        double bestDistance = 1.0;
        int bestIndex = 0;

        for (int i = 0; i < count; ++i)
        {
            const auto& event =
                processor.sequencer.events()[
                    static_cast<std::size_t>(i)];

            const double distance =
                std::abs(event.position - normalized);

            const double wrapped =
                std::min(distance, 1.0 - distance);

            if (wrapped < bestDistance)
            {
                bestDistance = wrapped;
                bestIndex = i;
            }
        }

        selected_ = bestIndex;

        if (e.mods.isShiftDown())
        {
            auto& event =
                processor.sequencer.events()[
                    static_cast<std::size_t>(selected_)];

            event.active = !event.active;
            processor.sequencer.publishToAudio();
        }

        repaint();
        return;
    }

    // FIELD VIEW: the entire canvas is a selection surface.
    // The nearest event by X is selected, even when the user
    // clicks away from the actual dot.
    double normalizedX =
        juce::jlimit(
            0.0,
            0.999,
            static_cast<double>(
                (local.x - canvas.getX())
                    / std::max(1.0f, canvas.getWidth())));

    double bestXDistance = 1.0;
    int bestIndex = 0;

    for (int i = 0; i < count; ++i)
    {
        const auto& event =
            processor.sequencer.events()[
                static_cast<std::size_t>(i)];

        const double distance =
            std::abs(event.position - normalizedX);

        if (distance < bestXDistance)
        {
            bestXDistance = distance;
            bestIndex = i;
        }
    }

    selected_ = bestIndex;

    auto& event =
        processor.sequencer.events()[
            static_cast<std::size_t>(selected_)];

    dragStartX_ = local.x;
    dragStartY_ = local.y;
    dragStartPosition_ = event.position;
    dragStartPitch_ = event.pitch;
    dragStartDuration_ = event.duration;
    dragStartVelocity_ = event.velocity;

    if (e.mods.isShiftDown())
        event.active = !event.active;

    processor.sequencer.publishToAudio();
    repaint();
}

void RelationalSequencerComponent::mouseDrag(
    const juce::MouseEvent& e)
{
    if (draggingCycle_)
    {
        const auto full = getLocalBounds().toFloat();
        auto outer = full.reduced(10.0f);
        outer.removeFromTop(26.0f);

        const float cycleH = 16.0f;

        for (int row = 0; row < 5; ++row)
        {
            auto strip = outer.removeFromTop(cycleH);

            if (row != selectedCycle_)
                continue;

            const float labelW = 58.0f;
            const float valueW = 56.0f;
            const float trackX = strip.getX() + labelW;
            const float trackW =
                std::max(20.0f,
                         strip.getWidth() - labelW - valueW);

            const float normalized =
                juce::jlimit(
                    0.0f,
                    1.0f,
                    (e.position.x - trackX)
                        / std::max(1.0f, trackW));

            if (e.mods.isShiftDown() || e.mods.isRightButtonDown())
            {
                const auto cycles = processor.sequencer.cycleLengths();
                const int cycleLen = cycleForRow(selectedCycle_, cycles);
                const int newPhase = juce::jlimit(0, std::max(1, cycleLen - 1), 
                    static_cast<int>(std::lround(normalized * static_cast<float>(std::max(1, cycleLen - 1)))));
                processor.sequencer.setCyclePhase(selectedCycle_, newPhase);
            }
            else
            {
                const int newLength =
                    juce::jlimit(
                        1,
                        32,
                        static_cast<int>(
                            std::lround(
                                1.0f
                                + normalized * 31.0f)));

                switch (selectedCycle_)
                {
                    case 0: processor.sequencer.setRhythmCycle(newLength); break;
                    case 1: processor.sequencer.setPitchCycle(newLength); break;
                    case 2: processor.sequencer.setFMCycle(newLength); break;
                    case 3: processor.sequencer.setTuningCycle(newLength); break;
                    default: processor.sequencer.setMotionCycle(newLength); break;
                }
            }

            processor.sequencer.publishToAudio();
            repaint();
            return;
        }

        return;
    }

    if (selected_ < 0)
        return;

    auto& events = processor.sequencer.events();

    if (selected_ >= static_cast<int>(events.size()))
        return;

    auto& event =
        events[static_cast<std::size_t>(selected_)];

    const auto full = getLocalBounds().toFloat();
    auto outer = full.reduced(10.0f);
    outer.removeFromTop(26.0f);
    outer.removeFromTop(5 * 16.0f);
    outer.removeFromTop(4.0f);
    outer.removeFromBottom(26.0f);

    const auto canvas = outer;

    if (viewMode_ == 0)
    {
        event.velocity =
            juce::jlimit(
                0.0,
                1.0,
                dragStartVelocity_ 
                    + static_cast<double>(
                        (e.position.x - dragStartX_)
                            / std::max(1.0f, canvas.getWidth()))
                        * 2.0);

        event.pitch =
            juce::jlimit(
                -8.0,
                8.0,
                dragStartPitch_
                    + static_cast<double>(
                        (dragStartY_ - e.position.y)
                            / std::max(1.0f, canvas.getHeight()))
                        * 16.0);
    }
    else if (viewMode_ == 1)
    {
        event.pitch =
            juce::jlimit(
                -8.0,
                8.0,
                dragStartPitch_
                    + static_cast<double>(
                        (dragStartY_ - e.position.y)
                            / std::max(1.0f, canvas.getHeight()))
                        * 16.0);
                        
        event.duration =
            juce::jlimit(
                0.05,
                4.0,
                dragStartDuration_
                    + static_cast<double>(
                        (e.position.x - dragStartX_)
                            / std::max(1.0f, canvas.getWidth()))
                        * 4.0);
    }
    else
    {
        const auto center = canvas.getCentre();

        const double angle =
            std::atan2(
                e.position.y - center.y,
                e.position.x - center.x)
            + juce::MathConstants<double>::halfPi;

        event.position =
            std::fmod(
                angle + juce::MathConstants<double>::twoPi,
                juce::MathConstants<double>::twoPi)
            / juce::MathConstants<double>::twoPi;
    }

    processor.sequencer.publishToAudio();
    repaint();
}

void RelationalSequencerComponent::mouseUp(
    const juce::MouseEvent&)
{
    draggingCycle_ = false;
    repaint();
}

// ============================================================
// Relation Matrix
// ============================================================

void RelationMatrixComponent::paint(juce::Graphics& g)
{
    const auto full = getLocalBounds().toFloat();

    g.fillAll(juce::Colour(0xf00a0b0d));
    moduleFrame(g, full, "RELATIONAL COUPLING MATRIX");

    const auto r = full.reduced(18.0f);

    static constexpr const char* names[] =
    {
        "PITCH",
        "TIME",
        "FM",
        "RESONANCE",
        "TUNING"
    };

    constexpr int n = rel::RelationEngine::dimensions;

    const float labelSize = 66.0f;
    const float gridX = r.getX() + labelSize;
    const float gridY = r.getY() + 54.0f;
    const float gridWidth = r.getWidth() - labelSize;
    const float gridHeight = r.getHeight() - 70.0f;
    const float cellW = gridWidth / static_cast<float>(n);
    const float cellH = gridHeight / static_cast<float>(n);

    g.setFont(juce::FontOptions(8.0f, juce::Font::bold));

    for (int i = 0; i < n; ++i)
    {
        g.setColour(muted);

        g.drawText(
            names[i],
            gridX + i * cellW,
            gridY - 18.0f,
            cellW,
            14.0f,
            juce::Justification::centred);

        g.drawText(
            names[i],
            r.getX(),
            gridY + i * cellH + cellH * 0.5f - 7.0f,
            labelSize - 8.0f,
            14.0f,
            juce::Justification::right);
    }

    for (int destination = 0; destination < n; ++destination)
    {
        for (int source = 0; source < n; ++source)
        {
            const auto cell =
                juce::Rectangle<float>(
                    gridX + source * cellW + 2.0f,
                    gridY + destination * cellH + 2.0f,
                    cellW - 4.0f,
                    cellH - 4.0f);

            const bool diagonal = source == destination;

            double amount = diagonal
                ? 0.0
                : processor.relations_.get(source, destination);

            amount = juce::jlimit(-1.0, 1.0, amount);

            const bool selected =
                source == selectedSource_
                && destination == selectedDestination_;

            if (diagonal)
                g.setColour(panel);
            else if (selected)
                g.setColour(accent.withAlpha(0.28f));
            else
                g.setColour(panel2);

            g.fillRect(cell);

            g.setColour(line);
            g.drawRect(cell, 1.0f);

            if (!diagonal && std::abs(amount) > 0.0001)
            {
                const float centerX = cell.getCentreX();
                const float centerY = cell.getCentreY();
                const float maxHeight = cell.getHeight() * 0.34f;
                const float magnitude =
                    static_cast<float>(std::abs(amount));

                const float barHeight =
                    std::max(2.0f, maxHeight * magnitude);

                const float y =
                    amount >= 0.0
                        ? centerY - barHeight
                        : centerY;

                g.setColour(
                    amount >= 0.0 ? accent2 : accent);

                g.fillRect(
                    centerX - 5.0f,
                    y,
                    10.0f,
                    barHeight);
            }

            g.setColour(
                selected
                    ? text
                    : muted);

            g.setFont(juce::FontOptions(8.0f));

            if (diagonal)
            {
                g.drawText(
                    "—",
                    cell,
                    juce::Justification::centred);
            }
            else
            {
                g.drawText(
                    juce::String(amount * 100.0, 0) + "%",
                    cell,
                    juce::Justification::centred);
            }
        }
    }

    g.setColour(muted);
    g.setFont(juce::FontOptions(8.0f));

    g.drawText(
        "SOURCE → DESTINATION",
        r.getX(),
        r.getBottom() - 14.0f,
        220.0f,
        12.0f,
        juce::Justification::left);

    g.drawText(
        "DRAG VERTICALLY / -100% … +100%",
        r.getRight() - 260.0f,
        r.getBottom() - 14.0f,
        260.0f,
        12.0f,
        juce::Justification::right);

    if (selectedSource_ >= 0 && selectedDestination_ >= 0)
    {
        const double selectedAmount =
            juce::jlimit(
                -1.0,
                1.0,
                processor.relations_.get(
                    selectedSource_,
                    selectedDestination_));

        g.setColour(text);
        g.drawText(
            juce::String(selectedAmount * 100.0, 0) + "%",
            r.getX(),
            r.getBottom() - 30.0f,
            90.0f,
            18.0f,
            juce::Justification::left);
    }
}

void RelationMatrixComponent::mouseDown(const juce::MouseEvent& e)
{
    const auto r =
        getLocalBounds().toFloat().reduced(18.0f);

    constexpr int n = rel::RelationEngine::dimensions;

    const float labelSize = 66.0f;
    const float gridX = r.getX() + labelSize;
    const float gridY = r.getY() + 54.0f;
    const float cellW =
        (r.getWidth() - labelSize)
        / static_cast<float>(n);
    const float cellH =
        (r.getHeight() - 70.0f)
        / static_cast<float>(n);

    const int source =
        static_cast<int>(
            (e.position.x - gridX) / cellW);

    const int destination =
        static_cast<int>(
            (e.position.y - gridY) / cellH);

    if (source < 0 || source >= n
        || destination < 0 || destination >= n
        || source == destination)
    {
        selectedSource_ = -1;
        selectedDestination_ = -1;
        repaint();
        return;
    }

    selectedSource_ = source;
    selectedDestination_ = destination;
    startAmount_ =
        processor.relations_.get(
            source,
            destination);
    startY_ = e.position.y;
    
    processor.undoManager.beginNewTransaction();

    repaint();
}

void RelationMatrixComponent::mouseDrag(const juce::MouseEvent& e)
{
    if (selectedSource_ < 0
        || selectedDestination_ < 0)
    {
        return;
    }

    const double delta =
        static_cast<double>(
            startY_ - e.position.y)
        / std::max(
            1.0,
            static_cast<double>(getHeight()))
        * 3.0;

    processor.relations_.set(
        selectedSource_,
        selectedDestination_,
        juce::jlimit(
            -1.0,
            1.0,
            startAmount_ + delta));

    processor.publishRelations();
    repaint();
}

void RelationMatrixComponent::mouseUp(const juce::MouseEvent&)
{
    repaint();
}

// ============================================================
// On-Screen Keyboard
// ============================================================

juce::Rectangle<float>
OnScreenKeyboardComponent::noteBounds(int note) const noexcept
{
    const auto area =
        getLocalBounds().toFloat().reduced(8.0f);

    constexpr int firstNote = 48;
    constexpr int noteCount = 25;

    const int index =
        juce::jlimit(
            0,
            noteCount - 1,
            note - firstNote);

    const float width =
        area.getWidth()
        / static_cast<float>(noteCount);

    return {
        area.getX() + index * width,
        area.getY(),
        width - 1.0f,
        area.getHeight()
    };
}

int OnScreenKeyboardComponent::noteForPosition(float x) const noexcept
{
    const auto area =
        getLocalBounds().toFloat().reduced(8.0f);

    constexpr int firstNote = 48;
    constexpr int noteCount = 25;

    const float width =
        area.getWidth()
        / static_cast<float>(noteCount);

    if (width <= 0.0f)
        return -1;

    const int index =
        static_cast<int>(
            (x - area.getX()) / width);

    if (index < 0 || index >= noteCount)
        return -1;

    return firstNote + index;
}

void OnScreenKeyboardComponent::paint(juce::Graphics& g)
{
    const auto full = getLocalBounds().toFloat();

    g.fillAll(panel2);
    moduleFrame(g, full, "NOTE INPUT");

    g.setColour(muted);
    g.setFont(juce::FontOptions(8.0f));
    g.drawText(
        "CLICK / HOLD OR USE COMPUTER KEYS A–;",
        full.getRight() - 250.0f,
        full.getY() + 9.0f,
        238.0f,
        14.0f,
        juce::Justification::right);

    const auto area =
        full.reduced(8.0f);

    constexpr int firstNote = 48;
    constexpr int noteCount = 25;

    const float width =
        area.getWidth()
        / static_cast<float>(noteCount);

    static constexpr int blackPitchClasses[] =
    {
        1, 3, 6, 8, 10
    };

    auto isBlack =
        [](int note)
        {
            const int pc = note % 12;
            for (const int black : blackPitchClasses)
                if (pc == black)
                    return true;
            return false;
        };

    for (int i = 0; i < noteCount; ++i)
    {
        const int note = firstNote + i;

        const auto key =
            juce::Rectangle<float>(
                area.getX() + i * width,
                area.getY(),
                std::max(1.0f, width - 1.0f),
                area.getHeight());

        const bool active =
            note == activeNote_;

        g.setColour(
            active
                ? accent
                : isBlack(note)
                    ? juce::Colour(0xff090a0c)
                    : juce::Colour(0xffdadde0));

        g.fillRect(key);

        g.setColour(line);
        g.drawRect(key, 1.0f);

        if (note % 12 == 0)
        {
            g.setColour(
                active
                    ? bg
                    : muted);

            g.setFont(
                juce::FontOptions(8.0f));

            g.drawText(
                midiNoteName(note),
                key.getX() + 3.0f,
                key.getBottom() - 16.0f,
                key.getWidth() - 6.0f,
                12.0f,
                juce::Justification::left);
        }
    }
}

void OnScreenKeyboardComponent::mouseDown(
    const juce::MouseEvent& e)
{
    const int note =
        noteForPosition(e.position.x);

    if (note < 0)
        return;

    activeNote_ = note;
    processor.triggerTestNote(note, 0.82f);
    repaint();
}

void OnScreenKeyboardComponent::mouseDrag(
    const juce::MouseEvent& e)
{
    const int note =
        noteForPosition(e.position.x);

    if (note < 0 || note == activeNote_)
        return;

    if (activeNote_ >= 0)
        processor.releaseTestNote(activeNote_);

    activeNote_ = note;
    processor.triggerTestNote(note, 0.82f);
    repaint();
}

void OnScreenKeyboardComponent::mouseUp(
    const juce::MouseEvent&)
{
    if (activeNote_ >= 0)
        processor.releaseTestNote(activeNote_);

    activeNote_ = -1;
    repaint();
}

// ============================================================
// Main Editor
// ============================================================

RelationalEXEditor::RelationalEXEditor(
    RelationalEXProcessor& p)
    : AudioProcessorEditor(&p),
      processor(p),
      tooltipWindow(this, 650),
      field(p),
      spectrum(p),
      sequencer(p),
      relationMatrix(p),
      keyboard(p)
{
    setResizable(true, true);
    setResizeLimits(980, 720, 1800, 1150);
    setSize(1280, 840);

    setWantsKeyboardFocus(true);

    addAndMakeVisible(field);
    addAndMakeVisible(spectrum);
    addAndMakeVisible(sequencer);
    addAndMakeVisible(relationMatrix);
    relationMatrix.setVisible(false);
    addAndMakeVisible(keyboard);

    addAndMakeVisible(presetButton);
    addAndMakeVisible(tuningButton);
    addAndMakeVisible(rootButton);
    addAndMakeVisible(scaleButton);
    addAndMakeVisible(scalaButton);
    addAndMakeVisible(viewButton);
    addAndMakeVisible(playButton);
    addAndMakeVisible(euclidButton);
    addAndMakeVisible(relationsButton);
    addAndMakeVisible(fmModeButton);
    addAndMakeVisible(lfoTargetButton);
    addAndMakeVisible(synthModeButton);

    addAndMakeVisible(statusLabel);
    addAndMakeVisible(glideSlider);
    addAndMakeVisible(glideLabel);
    addAndMakeVisible(midiInputLabel);
    addAndMakeVisible(midiNoteLabel);
    addAndMakeVisible(midiVelocityLabel);
    addAndMakeVisible(midiFrequencyLabel);

    presetButton.setTooltip(
        "Browse factory presets designed as explorations of tuning, FM, resonance, motion and relational behavior.");

    tuningButton.setTooltip(
        "Select an equal-temperament tuning or browse the local scale database. Custom Scala scales can be loaded with LOAD .SCL.");
        
    rootButton.setTooltip("Select the Root Note for scale quantization.");
    scaleButton.setTooltip("Select a musical Scale for sequencer quantization. Events will snap to the nearest degree.");

    scalaButton.setTooltip(
        "Open a Scala .scl file and load its intervals into the Frequency Field.");

    viewButton.setTooltip(
        "Cycle through FIELD, GRID and RADIAL views. All views manipulate the same relational event data.");
        
    fmModeButton.setTooltip(
        "Toggles continuous vs snapshot FM modulation. If enabled, FM parameters apply continuously to sustained notes.");

    playButton.setTooltip(
        "Start or stop the internal relational sequencer.");

    euclidButton.setTooltip(
        "Generate an event distribution from the current Steps and Pulses relationship.");

    sequencer.setTooltip(
        "RELATIONAL SEQUENCER:\n"
        "- Click & Drag Horizontally: Set Steps (Length) of a layer.\n"
        "- Click & Drag Vertically: Set Pulses (Hits) of a layer.\n"
        "- Shift + Drag Horizontally: Rotate (Shift) the sequence.\n"
        "- FIELD View: Drag dots vertically for Pitch, horizontally for Velocity.\n"
        "Controls multiple interwoven Euclidean sequences generating pitch and rhythm.");

    relationsButton.setTooltip(
        "Open the five-domain coupling matrix. Drag a relationship vertically to change its influence from -100% to +100%.");

    field.setTooltip(
        "FREQUENCY FIELD:\n"
        "Visualizes the current tuning system or Scala file.\n"
        "Vertical lines represent available harmonic ratios and intervals for the sequencer to snap into.");

    spectrum.setTooltip(
        "SPECTRUM:\n"
        "Real-time harmonic visualization of the synthesized signal.");

    relationMatrix.setTooltip(
        "RELATION MATRIX:\n"
        "Drag relationships vertically to route modulations between domains.\n"
        "Allows Time to modulate Pitch, Pitch to modulate FM, etc.");

    synthModeButton.setTooltip(
        "Switch between standard POLYphonic voices and MONO LEGATO (glides between overlapping notes like an acidic bassline).");
        
    lfoTargetButton.setTooltip(
        "Select the macro destination for the internal LFO (Relations Matrix, FM Ratio, FM Index, or Filter Cutoff).");

    glideSlider.setTooltip(
        "Set the glide/portamento time when sliding between notes in MONO LEGATO mode.");

    midiInputLabel.setTooltip(
        "Shows the current MIDI source state. External MIDI notes are monitored when a host or standalone MIDI input routes them into the instrument.");

    midiNoteLabel.setTooltip(
        "Displays the most recent MIDI note received or generated by the test keyboard.");

    midiVelocityLabel.setTooltip(
        "Displays the velocity of the most recent MIDI Note On event.");

    midiFrequencyLabel.setTooltip(
        "Displays the current frequency assigned to the monitored note after the active tuning system and microtonal offset are applied.");

    const auto styleButton =
        [](juce::TextButton& b)
        {
            b.setColour(
                juce::TextButton::buttonColourId,
                panel2);

            b.setColour(
                juce::TextButton::buttonOnColourId,
                accent.withAlpha(0.35f));

            b.setColour(
                juce::TextButton::textColourOffId,
                text);

            b.setColour(
                juce::TextButton::textColourOnId,
                accent);
        };

    styleButton(presetButton);
    styleButton(tuningButton);
    styleButton(rootButton);
    styleButton(scaleButton);
    styleButton(scalaButton);
    styleButton(viewButton);
    styleButton(playButton);
    styleButton(euclidButton);
    styleButton(relationsButton);
    styleButton(fmModeButton);
    styleButton(synthModeButton);
    styleButton(lfoTargetButton);

    configureSlider(
        ratioSlider,
        ratioLabel,
        "FM RATIO",
        "Sets the frequency relationship between carrier and modulator. Non-integer ratios create inharmonic sidebands.",
        0.01,
        8.0,
        0.001);

    configureSlider(
        indexSlider,
        indexLabel,
        "FM INDEX",
        "Controls FM depth. Higher values create more sidebands and greater spectral complexity.",
        0.0,
        20.0,
        0.01);

    configureSlider(
        feedbackSlider,
        feedbackLabel,
        "FM FEEDBACK",
        "Feeds FM output back into phase. Higher values create dense, unstable spectra.",
        0.0,
        0.95,
        0.001);

    configureSlider(
        driftSlider,
        driftLabel,
        "TUNING DRIFT",
        "Adds pitch variation around the selected tuning. Higher values destabilize the pitch field.",
        0.0,
        30.0,
        0.01);

    configureSlider(
        inharmonicitySlider,
        inharmonicityLabel,
        "INHARMONICITY",
        "Moves resonance modes away from harmonic integer multiples, emphasizing metallic or object-like spectra.",
        0.0,
        1.0,
        0.001);

    configureSlider(
        resonanceSlider,
        resonanceLabel,
        "RESONANCE",
        "Adds the six-mode inharmonic resonator to each FM voice.",
        0.0,
        1.0,
        0.001);

    configureSlider(
        bpmSlider,
        bpmLabel,
        "BPM",
        "Sets the internal relational sequencer tempo.",
        40.0,
        240.0,
        0.1);

    configureSlider(
        lfoRateSlider,
        lfoRateLabel,
        "LFO RATE",
        "Sets the rate of the low-frequency modulation source.",
        0.01,
        20.0,
        0.001);

    configureSlider(
        lfoAmountSlider,
        lfoAmountLabel,
        "LFO AMOUNT",
        "Controls how strongly the LFO contributes to relational motion.",
        0.0,
        100.0,
        0.01);

    configureSlider(
        chaosAmountSlider,
        chaosAmountLabel,
        "CHAOS",
        "Controls deterministic nonlinear modulation. Higher values move the system farther from periodic behavior.",
        0.0,
        30.0,
        0.01);

    configureSlider(
        glideSlider,
        glideLabel,
        "GLIDE",
        "Time it takes to glide between notes in Mono Legato mode.",
        0.0,
        2.0,
        0.001);

    configureSlider(attackSlider, attackLabel, "ATTACK", "Attack time of the amplitude envelope.", 0.001, 5.0, 0.001);
    configureSlider(decaySlider, decayLabel, "DECAY", "Decay time of the amplitude envelope.", 0.001, 5.0, 0.001);
    configureSlider(sustainSlider, sustainLabel, "SUSTAIN", "Sustain level of the amplitude envelope.", 0.0, 1.0, 0.01);
    configureSlider(releaseSlider, releaseLabel, "RELEASE", "Release time of the amplitude envelope.", 0.001, 10.0, 0.001);
    configureSlider(cutoffSlider, cutoffLabel, "CUTOFF", "Low-pass filter cutoff frequency.", 20.0, 20000.0, 1.0);

    attackAtt = std::make_unique<Attachment>(processor.apvts, "envAttack", attackSlider);
    decayAtt = std::make_unique<Attachment>(processor.apvts, "envDecay", decaySlider);
    sustainAtt = std::make_unique<Attachment>(processor.apvts, "envSustain", sustainSlider);
    releaseAtt = std::make_unique<Attachment>(processor.apvts, "envRelease", releaseSlider);
    cutoffAtt = std::make_unique<Attachment>(processor.apvts, "filterCutoff", cutoffSlider);

    ratioAtt = std::make_unique<
        juce::AudioProcessorValueTreeState::SliderAttachment>(
            processor.apvts, "fmRatio", ratioSlider);

    indexAtt = std::make_unique<
        juce::AudioProcessorValueTreeState::SliderAttachment>(
            processor.apvts, "fmIndex", indexSlider);

    feedbackAtt = std::make_unique<
        juce::AudioProcessorValueTreeState::SliderAttachment>(
            processor.apvts, "fmFeedback", feedbackSlider);

    driftAtt = std::make_unique<
        juce::AudioProcessorValueTreeState::SliderAttachment>(
            processor.apvts, "tuningDrift", driftSlider);

    inharmonicityAtt = std::make_unique<
        juce::AudioProcessorValueTreeState::SliderAttachment>(
            processor.apvts, "inharmonicity", inharmonicitySlider);

    resonanceAtt = std::make_unique<
        juce::AudioProcessorValueTreeState::SliderAttachment>(
            processor.apvts, "resonance", resonanceSlider);

    bpmAtt = std::make_unique<
        juce::AudioProcessorValueTreeState::SliderAttachment>(
            processor.apvts, "bpm", bpmSlider);

    lfoRateAtt = std::make_unique<
        juce::AudioProcessorValueTreeState::SliderAttachment>(
            processor.apvts, "lfoRate", lfoRateSlider);

    lfoAmountAtt = std::make_unique<
        juce::AudioProcessorValueTreeState::SliderAttachment>(
            processor.apvts, "lfoAmount", lfoAmountSlider);

    chaosAmountAtt = std::make_unique<
        juce::AudioProcessorValueTreeState::SliderAttachment>(
            processor.apvts, "chaosAmount", chaosAmountSlider);

    glideAtt = std::make_unique<
        juce::AudioProcessorValueTreeState::SliderAttachment>(
            processor.apvts, "glideTime", glideSlider);

    fmModeButton.setClickingTogglesState(true);
    fmContinuousAtt = std::make_unique<
        juce::AudioProcessorValueTreeState::ButtonAttachment>(
            processor.apvts, "fmContinuous", fmModeButton);

    fmModeButton.onClick = [this]
    {
        fmModeButton.setButtonText(
            fmModeButton.getToggleState() ? "FM: LIVE" : "FM: NOTE");
    };
    
    fmModeButton.setButtonText(
        processor.apvts.getRawParameterValue("fmContinuous")->load() > 0.5f
            ? "FM: LIVE" : "FM: NOTE");

    presetButton.onClick = [this]
    {
        showPresetMenu();
    };

    tuningButton.onClick = [this]
    {
        showTuningMenu();
    };

    rootButton.onClick = [this] { showRootMenu(); };
    scaleButton.onClick = [this] { showScaleMenu(); };

    scalaButton.onClick = [this]
    {
        chooseScalaFile();
    };

    viewButton.onClick = [this]
    {
        const int next =
            (sequencer.viewMode() + 1) % 3;

        sequencer.setViewMode(next);

        viewButton.setButtonText(
            next == 0
                ? "FIELD"
                : next == 1
                    ? "GRID"
                    : "RADIAL");
    };

    synthModeButton.onClick = [this] { showSynthModeMenu(); };
    lfoTargetButton.onClick = [this] { showLfoTargetMenu(); };

    playButton.onClick = [this]
    {
        const bool running =
            !processor.isSequenceRunning();

        processor.setSequenceRunning(running);
        playButton.setButtonText(
            running ? "STOP" : "PLAY");
    };

    relationsButton.onClick = [this]
    {
        showRelationEditor();
    };

    euclidButton.onClick = [this]
    {
        processor.undoManager.beginNewTransaction();

        if (juce::ModifierKeys::getCurrentModifiers().isShiftDown())
        {
            processor.sequencer.generateGenerativePreset();
        }
        else
        {
            const auto* stepsValue = processor.apvts.getRawParameterValue("seqSteps");
            const auto* pulsesValue = processor.apvts.getRawParameterValue("seqPulses");

            if (stepsValue != nullptr && pulsesValue != nullptr)
            {
                const int steps = juce::jlimit(1, 32, static_cast<int>(std::lround(stepsValue->load())));
                const int pulses = juce::jlimit(0, steps, static_cast<int>(std::lround(pulsesValue->load())));

                processor.sequencer.setLength(steps);
                processor.sequencer.applyEuclidean(pulses, 0);
                processor.sequencer.publishToAudio();
            }
        }
        sequencer.repaint();
    };

    statusLabel.setText(
        "READY / INTERNAL TUNING",
        juce::dontSendNotification);

    statusLabel.setColour(
        juce::Label::textColourId,
        muted);

    statusLabel.setFont(
        juce::FontOptions(9.0f));

    for (auto* label : {
             &midiInputLabel,
             &midiNoteLabel,
             &midiVelocityLabel,
             &midiFrequencyLabel})
    {
        label->setColour(
            juce::Label::textColourId,
            muted);

        label->setFont(
            juce::FontOptions(8.0f));
    }

    midiInputLabel.setText(
        "MIDI / TEST",
        juce::dontSendNotification);

    midiNoteLabel.setText(
        "NOTE --",
        juce::dontSendNotification);

    midiVelocityLabel.setText(
        "VEL --",
        juce::dontSendNotification);

    midiFrequencyLabel.setText(
        "FREQ --",
        juce::dontSendNotification);

    addAndMakeVisible(creditLabel);
    creditLabel.setText("coded by @laurorobles", juce::dontSendNotification);
    creditLabel.setFont(juce::FontOptions(9.0f));
    creditLabel.setColour(juce::Label::textColourId, muted.withAlpha(0.6f));
    creditLabel.setJustificationType(juce::Justification::centredRight);

    startTimerHz(60);
}

void RelationalEXEditor::timerCallback()
{
    pollComputerKeyboard();
    updateMidiMonitor();
    updateScaleButtons();
    updateModeButtons();

    field.repaint();
    spectrum.repaint();
    sequencer.repaint();
    relationMatrix.repaint();
    keyboard.repaint();
}

void RelationalEXEditor::pollComputerKeyboard()
{
    for (int i = 0; i < computerKeyCount; ++i)
    {
        const bool down =
            juce::KeyPress::isKeyCurrentlyDown(
                computerKeys_[static_cast<std::size_t>(i)]);

        if (down == computerKeyStates_[static_cast<std::size_t>(i)])
            continue;

        computerKeyStates_[static_cast<std::size_t>(i)] = down;

        const int note = 60 + i;

        if (down)
            processor.triggerTestNote(note, 0.78f);
        else
            processor.releaseTestNote(note);
    }
}

void RelationalEXEditor::updateMidiMonitor()
{
    const int note = processor.monitoredMidiNote();
    const float velocity = processor.monitoredVelocity();
    const double frequency = processor.monitoredFrequency();
    const bool gate = processor.monitoredGate();

    midiInputLabel.setText(
        gate ? "MIDI / NOTE ON" : "MIDI / READY",
        juce::dontSendNotification);

    midiNoteLabel.setText(
        "NOTE " + midiNoteName(note),
        juce::dontSendNotification);

    midiVelocityLabel.setText(
        "VEL " + juce::String(velocity, 2),
        juce::dontSendNotification);

    midiFrequencyLabel.setText(
        frequency > 0.0
            ? "FREQ " + juce::String(frequency, 3) + " Hz"
            : "FREQ --",
        juce::dontSendNotification);
}

void RelationalEXEditor::configureSlider(
    juce::Slider& slider,
    juce::Label& label,
    const juce::String& name,
    const juce::String& help,
    double minimum,
    double maximum,
    double interval)
{
    slider.setLookAndFeel(&relationalLookAndFeel);

    slider.setSliderStyle(
        juce::Slider::RotaryHorizontalVerticalDrag);

    slider.setTextBoxStyle(
        juce::Slider::TextBoxBelow,
        false,
        72,
        18);

    slider.setRange(
        minimum,
        maximum,
        interval);

    slider.setColour(
        juce::Slider::rotarySliderFillColourId,
        accent);

    slider.setColour(
        juce::Slider::rotarySliderOutlineColourId,
        line);

    rel::ui::setHelp(slider, help);

    label.setText(
        name,
        juce::dontSendNotification);

    label.setColour(
        juce::Label::textColourId,
        text);

    label.setFont(
        juce::FontOptions(
            9.0f,
            juce::Font::bold));

    label.setJustificationType(
        juce::Justification::centred);

    addAndMakeVisible(slider);
    addAndMakeVisible(label);
}

void RelationalEXEditor::showPresetMenu()
{
    juce::PopupMenu menu;

    const auto& presets =
        rel::PresetLibrary::factoryPresets();

    std::string lastCategory;
    int id = 1;

    for (const auto& preset : presets)
    {
        if (preset.category != lastCategory)
        {
            if (!lastCategory.empty())
                menu.addSeparator();

            lastCategory = preset.category;
        }

        menu.addItem(
            id,
            juce::String(preset.name),
            true,
            id - 1 == processor.getCurrentProgram());

        ++id;
    }

    menu.showMenuAsync(
        juce::PopupMenu::Options()
            .withTargetComponent(&presetButton),
        [this](int result)
        {
            if (result <= 0)
                return;

            processor.applyFactoryPreset(result - 1);

            const auto name =
                processor.getProgramName(
                    processor.getCurrentProgram());

            presetButton.setButtonText(name);

            statusLabel.setText(
                "PRESET / " + name,
                juce::dontSendNotification);
        });
}

void RelationalEXEditor::showRootMenu()
{
    juce::PopupMenu menu;
    static const char* roots[] = {"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"};
    int current = static_cast<int>(processor.apvts.getParameterAsValue("rootNote").getValue());
    
    for (int i = 0; i < 12; ++i)
        menu.addItem(i + 1, roots[i], true, i == current);
        
    menu.showMenuAsync(juce::PopupMenu::Options().withTargetComponent(rootButton),
        [this](int result) {
            if (result > 0)
                processor.apvts.getParameterAsValue("rootNote").setValue(result - 1);
        });
}

void RelationalEXEditor::showScaleMenu()
{
    juce::PopupMenu menu;
    auto* param = dynamic_cast<juce::AudioParameterChoice*>(processor.apvts.getParameter("scaleType"));
    if (!param) return;
    
    int current = static_cast<int>(processor.apvts.getParameterAsValue("scaleType").getValue());
    
    for (int i = 0; i < param->choices.size(); ++i)
        menu.addItem(i + 1, param->choices[i], true, i == current);
        
    menu.showMenuAsync(juce::PopupMenu::Options().withTargetComponent(scaleButton),
        [this](int result) {
            if (result > 0)
                processor.apvts.getParameterAsValue("scaleType").setValue(result - 1);
        });
}

void RelationalEXEditor::updateScaleButtons()
{
    const int root = static_cast<int>(processor.apvts.getParameterAsValue("rootNote").getValue());
    const int scale = static_cast<int>(processor.apvts.getParameterAsValue("scaleType").getValue());
    
    static const char* roots[] = {"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"};
    rootButton.setButtonText(juce::String("ROOT: ") + roots[root]);

    auto* param = dynamic_cast<juce::AudioParameterChoice*>(processor.apvts.getParameter("scaleType"));
    if (param)
        scaleButton.setButtonText(juce::String("SCALE: ") + param->choices[scale]);
}

void RelationalEXEditor::showSynthModeMenu()
{
    juce::PopupMenu menu;
    auto* param = dynamic_cast<juce::AudioParameterChoice*>(processor.apvts.getParameter("synthMode"));
    if (!param) return;
    
    int current = static_cast<int>(processor.apvts.getParameterAsValue("synthMode").getValue());
    
    for (int i = 0; i < param->choices.size(); ++i)
        menu.addItem(i + 1, param->choices[i], true, i == current);
        
    menu.showMenuAsync(juce::PopupMenu::Options().withTargetComponent(synthModeButton),
        [this](int result) {
            if (result > 0)
                processor.apvts.getParameterAsValue("synthMode").setValue(result - 1);
        });
}

void RelationalEXEditor::showLfoTargetMenu()
{
    juce::PopupMenu menu;
    auto* param = dynamic_cast<juce::AudioParameterChoice*>(processor.apvts.getParameter("lfoTarget"));
    if (!param) return;
    
    int current = static_cast<int>(processor.apvts.getParameterAsValue("lfoTarget").getValue());
    
    for (int i = 0; i < param->choices.size(); ++i)
        menu.addItem(i + 1, param->choices[i], true, i == current);
        
    menu.showMenuAsync(juce::PopupMenu::Options().withTargetComponent(lfoTargetButton),
        [this](int result) {
            if (result > 0)
                processor.apvts.getParameterAsValue("lfoTarget").setValue(result - 1);
        });
}

void RelationalEXEditor::updateModeButtons()
{
    const int synthMode = static_cast<int>(processor.apvts.getParameterAsValue("synthMode").getValue());
    const int lfoTarget = static_cast<int>(processor.apvts.getParameterAsValue("lfoTarget").getValue());
    
    auto* paramMode = dynamic_cast<juce::AudioParameterChoice*>(processor.apvts.getParameter("synthMode"));
    if (paramMode)
        synthModeButton.setButtonText(juce::String("MODE: ") + paramMode->choices[synthMode]);
        
    auto* paramLfo = dynamic_cast<juce::AudioParameterChoice*>(processor.apvts.getParameter("lfoTarget"));
    if (paramLfo)
        lfoTargetButton.setButtonText(juce::String("LFO > ") + paramLfo->choices[lfoTarget]);
}

void RelationalEXEditor::showTuningMenu()
{
    juce::PopupMenu menu;

    for (const int divisions :
         { 12, 17, 19, 22, 24, 31, 41, 53 })
    {
        menu.addItem(
            divisions,
            juce::String(divisions) + "-TET",
            true,
            processor.tuning.degreesPerPeriod()
                == divisions);
    }

    const auto& scales =
        processor.scaleCatalog().entries();

    if (!scales.empty())
    {
        menu.addSeparator();

        constexpr int baseId = 100;
        const int count =
            juce::jmin(
                40,
                static_cast<int>(scales.size()));

        for (int i = 0; i < count; ++i)
        {
            menu.addItem(
                baseId + i,
                juce::String(
                    scales[static_cast<std::size_t>(i)].name));
        }
    }

    menu.showMenuAsync(
        juce::PopupMenu::Options()
            .withTargetComponent(&tuningButton),
        [this](int result)
        {
            if (result >= 1 && result <= 53)
            {
                processor.setTuningDivisions(result);

                tuningButton.setButtonText(
                    juce::String(result) + "-TET");

                statusLabel.setText(
                    "TUNING / "
                        + juce::String(result)
                        + "-TET",
                    juce::dontSendNotification);

                return;
            }

            if (result < 100)
                return;

            const int index = result - 100;
            const auto& entries =
                processor.scaleCatalog().entries();

            if (index < 0
                || index >= static_cast<int>(entries.size()))
            {
                return;
            }

            juce::String error;

            if (processor.loadScalaFile(
                    juce::File(juce::String::fromUTF8(entries[static_cast<std::size_t>(index)].path.c_str())),
                    error))
            {
                tuningButton.setButtonText("SCALA");

                statusLabel.setText(
                    "SCALA / "
                        + juce::String::fromUTF8(
                            entries[static_cast<std::size_t>(index)].name.c_str()),
                    juce::dontSendNotification);
            }
            else
            {
                statusLabel.setText(
                    "SCALA ERROR / " + error,
                    juce::dontSendNotification);
            }
        });
}

void RelationalEXEditor::chooseScalaFile()
{
    auto chooser =
        std::make_shared<juce::FileChooser>(
            "Load Scala scale",
            juce::File{},
            "*.scl");

    chooser->launchAsync(
        juce::FileBrowserComponent::openMode
            | juce::FileBrowserComponent::canSelectFiles,
        [this, chooser](const juce::FileChooser& fileChooser)
        {
            const auto file = fileChooser.getResult();

            if (!file.existsAsFile())
                return;

            juce::String error;

            if (processor.loadScalaFile(file, error))
            {
                tuningButton.setButtonText("SCALA");

                statusLabel.setText(
                    "SCALA / "
                        + file.getFileName(),
                    juce::dontSendNotification);
            }
            else
            {
                statusLabel.setText(
                    "SCALA ERROR / " + error,
                    juce::dontSendNotification);
            }
        });
}

void RelationalEXEditor::showRelationEditor()
{
    relationMatrix.setVisible(
        !relationMatrix.isVisible());

    relationsButton.setButtonText(
        relationMatrix.isVisible()
            ? "CLOSE RELATIONS"
            : "RELATIONS");

    if (relationMatrix.isVisible()) {
        relationMatrix.toFront(true);
        relationsButton.toFront(true);
    }
    
    resized();
}

bool RelationalEXEditor::keyPressed(
    const juce::KeyPress& key)
{
    if (key.getKeyCode() == 'z' || key.getKeyCode() == 'Z') {
        if (key.getModifiers().isCommandDown() || key.getModifiers().isCtrlDown()) {
            if (key.getModifiers().isShiftDown()) processor.undoManager.redo();
            else {
                processor.undoManager.undo();
                processor.sequencer.undo();
            }
            return true;
        }
    }

    juce::ignoreUnused(key);
    return false;
}

bool RelationalEXEditor::keyStateChanged(bool isKeyDown)
{
    juce::ignoreUnused(isKeyDown);
    return false;
}

void RelationalEXEditor::paint(
    juce::Graphics& g)
{
    g.fillAll(bg);

    const auto bounds =
        getLocalBounds().toFloat();

    g.setColour(line);
    g.drawHorizontalLine(
        62,
        18.0f,
        bounds.getRight() - 18.0f);

    g.setColour(text);
    g.setFont(
        juce::FontOptions(
            22.0f,
            juce::Font::bold));

    g.drawText(
        "RELATIONAL EX",
        18,
        12,
        240,
        28,
        juce::Justification::left);

    g.setColour(muted);
    g.setFont(
        juce::FontOptions(9.0f));

    g.drawText(
        "PITCH  /  TIME  /  SPECTRUM  /  MOTION",
        20,
        38,
        360,
        15,
        juce::Justification::left);

    g.setColour(muted.withAlpha(0.85f));
    g.setFont(juce::FontOptions(8.0f));
    g.drawText(
        "Extasis Records  ·  coded by @laurorobles",
        getWidth() - 300,
        38,
        282,
        15,
        juce::Justification::right);

    g.setColour(line);

    for (int x = 20;
         x < getWidth() - 20;
         x += 32)
    {
        g.drawVerticalLine(
            x,
            58.0f,
            62.0f);
    }
}

void RelationalEXEditor::resized()
{
    auto area =
        getLocalBounds()
            .reduced(18);

    area.removeFromTop(48);

    auto headerButtons = area.removeFromTop(30);

    presetButton.setBounds(
        headerButtons.removeFromLeft(146).reduced(2));

    tuningButton.setBounds(
        headerButtons.removeFromLeft(88).reduced(2));

    scalaButton.setBounds(
        headerButtons.removeFromLeft(102).reduced(2));

    scaleButton.setBounds(
        headerButtons.removeFromLeft(146).reduced(2));

    rootButton.setBounds(
        headerButtons.removeFromLeft(80).reduced(2));

    viewButton.setBounds(
        headerButtons.removeFromRight(74).reduced(2));

    playButton.setBounds(
        headerButtons.removeFromRight(66).reduced(2));

    euclidButton.setBounds(
        headerButtons.removeFromRight(76).reduced(2));

    relationsButton.setBounds(
        headerButtons.removeFromRight(106).reduced(2));

    fmModeButton.setBounds(
        headerButtons.removeFromRight(86).reduced(2));

    synthModeButton.setBounds(
        headerButtons.removeFromRight(106).reduced(2));
        
    lfoTargetButton.setBounds(
        headerButtons.removeFromRight(146).reduced(2));

    auto headerInfo =
        area.removeFromTop(22).reduced(1, 0);

    statusLabel.setBounds(
        headerInfo.removeFromLeft(180));

    midiInputLabel.setBounds(
        headerInfo.removeFromLeft(92));

    midiNoteLabel.setBounds(
        headerInfo.removeFromLeft(76));

    midiVelocityLabel.setBounds(
        headerInfo.removeFromLeft(66));

    midiFrequencyLabel.setBounds(
        headerInfo);

    auto upper =
        area.removeFromTop(
            std::max(310, area.getHeight() * 52 / 100));

    const int fieldWidth =
        static_cast<int>(
            upper.getWidth() * 0.57f);

    auto leftArea =
        upper.removeFromLeft(fieldWidth);

    auto envArea = leftArea.removeFromBottom(85).reduced(6, 0);
    field.setBounds(leftArea);
    
    juce::Slider* envSliders[] = { &attackSlider, &decaySlider, &sustainSlider, &releaseSlider, &cutoffSlider };
    juce::Label* envLabels[] = { &attackLabel, &decayLabel, &sustainLabel, &releaseLabel, &cutoffLabel };
    
    for (int i = 0; i < 5; ++i)
    {
        const int remaining = 5 - i;
        auto cell = envArea.removeFromLeft(std::max(1, envArea.getWidth() / remaining)).reduced(2);
        envLabels[i]->setBounds(cell.removeFromTop(18));
        envSliders[i]->setBounds(cell);
    }

    auto right = upper;

    auto spectrumArea =
        right.removeFromTop(
            std::max(110, right.getHeight() * 35 / 100));

    spectrum.setBounds(spectrumArea);

    auto controls =
        right.reduced(6);
        
    auto topControls = controls.removeFromTop(controls.getHeight() / 2);
    auto bottomControls = controls;

    const int topColumns = 4;
    const int topCellWidth =
        std::max(
            1,
            topControls.getWidth() / topColumns);

    juce::Slider* topSliders[] =
    {
        &ratioSlider,
        &indexSlider,
        &feedbackSlider,
        &driftSlider
    };

    juce::Label* topLabels[] =
    {
        &ratioLabel,
        &indexLabel,
        &feedbackLabel,
        &driftLabel
    };

    for (int i = 0; i < topColumns; ++i)
    {
        auto cell =
            topControls
                .removeFromLeft(topCellWidth)
                .reduced(4);

        topLabels[i]->setBounds(
            cell.removeFromTop(18));

        topSliders[i]->setBounds(cell);
    }

    juce::Slider* bottomSliders[] =
    {
        &inharmonicitySlider,
        &resonanceSlider,
        &bpmSlider,
        &lfoRateSlider,
        &lfoAmountSlider,
        &chaosAmountSlider,
        &glideSlider
    };

    juce::Label* bottomLabels[] =
    {
        &inharmonicityLabel,
        &resonanceLabel,
        &bpmLabel,
        &lfoRateLabel,
        &lfoAmountLabel,
        &chaosAmountLabel,
        &glideLabel
    };

    for (int i = 0; i < 7; ++i)
    {
        const int remaining = 7 - i;

        auto cell =
            bottomControls
                .removeFromLeft(
                    std::max(
                        1,
                        bottomControls.getWidth() / remaining))
                .reduced(2); // Slightly tighter padding to fit 6 items well

        bottomLabels[i]->setBounds(
            cell.removeFromTop(18));

        bottomSliders[i]->setBounds(cell);
    }

    auto keyboardArea =
        area.removeFromBottom(64);

    keyboard.setBounds(keyboardArea);

    sequencer.setBounds(area);
    
    creditLabel.setBounds(getLocalBounds().removeFromBottom(18).removeFromRight(150).withTrimmedBottom(2).withTrimmedRight(18));

    if (relationMatrix.isVisible())
    {
        auto overlay =
            getLocalBounds()
                .reduced(76);

        relationMatrix.setBounds(overlay);
        relationMatrix.toFront(true);
        // Añade esta línea para mantener el botón de cierre accesible
        relationsButton.toFront(true);
    }
}