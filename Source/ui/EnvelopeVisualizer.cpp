#include "EnvelopeVisualizer.h"
#include "CustomLookAndFeel.h"
#include <cmath>

namespace
{
    float timeToWidthFraction (float seconds)
    {
        constexpr float minTime = 0.001f, maxTime = 5.0f, steepness = 50.0f;
        const auto clamped = juce::jlimit (minTime, maxTime, seconds);
        return std::log1p (steepness * (clamped - minTime)) / std::log1p (steepness * (maxTime - minTime));
    }
}

//==============================================================================
EnvelopeVisualizer::EnvelopeVisualizer()
{
    setInterceptsMouseClicks (false, false);
}

void EnvelopeVisualizer::setADSR (float attackSeconds, float decaySeconds, float newSustainLevel, float releaseSeconds)
{
    if (juce::approximatelyEqual (attackTime, attackSeconds)
        && juce::approximatelyEqual (decayTime, decaySeconds)
        && juce::approximatelyEqual (sustainLevel, newSustainLevel)
        && juce::approximatelyEqual (releaseTime, releaseSeconds))
        return;

    attackTime = attackSeconds;
    decayTime = decaySeconds;
    sustainLevel = newSustainLevel;
    releaseTime = releaseSeconds;
    repaint();
}

juce::Path EnvelopeVisualizer::buildEnvelopePath (juce::Rectangle<float> bounds) const
{
    constexpr float sustainFraction = 0.3f;

    const auto attackFraction = timeToWidthFraction (attackTime);
    const auto decayFraction = timeToWidthFraction (decayTime);
    const auto releaseFraction = timeToWidthFraction (releaseTime);

    const auto totalFraction = attackFraction + decayFraction + sustainFraction + releaseFraction;
    const auto width = bounds.getWidth();

    const auto attackWidth = width * attackFraction / totalFraction;
    const auto decayWidth = width * decayFraction / totalFraction;
    const auto sustainWidth = width * sustainFraction / totalFraction;
    const auto releaseWidth = width * releaseFraction / totalFraction;

    const auto top = bounds.getY();
    const auto bottom = bounds.getBottom();
    const auto sustainY = bottom - sustainLevel * bounds.getHeight();

    const auto x0 = bounds.getX();
    const auto x1 = x0 + attackWidth;
    const auto x2 = x1 + decayWidth;
    const auto x3 = x2 + sustainWidth;
    const auto x4 = x3 + releaseWidth;

    juce::Path path;
    path.startNewSubPath (x0, bottom);
    path.lineTo (x1, top);
    path.lineTo (x2, sustainY);
    path.lineTo (x3, sustainY);
    path.lineTo (x4, bottom);

    return path;
}

void EnvelopeVisualizer::paint (juce::Graphics& g)
{
    const auto bounds = getLocalBounds().toFloat();
    const auto outline = buildEnvelopePath (bounds);

    auto fill = outline;
    fill.lineTo (bounds.getRight(), bounds.getBottom());
    fill.lineTo (bounds.getX(), bounds.getBottom());
    fill.closeSubPath();

    juce::ColourGradient fillGradient (CustomLookAndFeel::accent.withAlpha (0.18f), bounds.getX(), bounds.getY(),
                                       CustomLookAndFeel::accent.withAlpha (0.0f), bounds.getX(), bounds.getBottom(), false);
    g.setGradientFill (fillGradient);
    g.fillPath (fill);

    g.setColour (CustomLookAndFeel::accent.withAlpha (0.4f));
    g.strokePath (outline, juce::PathStrokeType (1.5f));
}
