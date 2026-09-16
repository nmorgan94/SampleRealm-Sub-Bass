#include "EnvelopeVisualizer.h"
#include "CustomLookAndFeel.h"
#include "../dsp/Curve.h"
#include "../dsp/DriveEnvelopeModulation.h"
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
    if (juce::exactlyEqual (attackTime, attackSeconds)
        && juce::exactlyEqual (decayTime, decaySeconds)
        && juce::exactlyEqual (sustainLevel, newSustainLevel)
        && juce::exactlyEqual (releaseTime, releaseSeconds))
        return;

    attackTime = attackSeconds;
    decayTime = decaySeconds;
    sustainLevel = newSustainLevel;
    releaseTime = releaseSeconds;
    repaint();
}

void EnvelopeVisualizer::setDrive (float driveAmount, float envAmount)
{
    if (juce::exactlyEqual (drive, driveAmount)
        && juce::exactlyEqual (driveEnvAmount, envAmount))
        return;

    drive = driveAmount;
    driveEnvAmount = envAmount;
    repaint();
}

void EnvelopeVisualizer::setCurves (float attackTension, float decayTension, float releaseTension)
{
    if (juce::exactlyEqual (attackCurve, attackTension)
        && juce::exactlyEqual (decayCurve, decayTension)
        && juce::exactlyEqual (releaseCurve, releaseTension))
        return;

    attackCurve = attackTension;
    decayCurve = decayTension;
    releaseCurve = releaseTension;
    repaint();
}

std::array<EnvelopeVisualizer::Breakpoint, 5> EnvelopeVisualizer::buildBreakpoints (juce::Rectangle<float> bounds) const
{
    constexpr float sustainFraction = 0.3f;

    const auto attackFraction = timeToWidthFraction (attackTime);
    const auto decayFraction = timeToWidthFraction (decayTime);
    const auto releaseFraction = timeToWidthFraction (releaseTime);

    const auto totalFraction = attackFraction + decayFraction + sustainFraction + releaseFraction;
    const auto width = bounds.getWidth();

    const auto x0 = bounds.getX();
    const auto x1 = x0 + width * attackFraction / totalFraction;
    const auto x2 = x1 + width * decayFraction / totalFraction;
    const auto x3 = x2 + width * sustainFraction / totalFraction;
    const auto x4 = x3 + width * releaseFraction / totalFraction;

    return { { { x0, 0.0f, 0.0f },
               { x1, 1.0f, attackCurve },
               { x2, sustainLevel, decayCurve },
               { x3, sustainLevel, 0.0f },
               { x4, 0.0f, releaseCurve } } };
}

juce::Path EnvelopeVisualizer::buildPath (juce::Rectangle<float> bounds,
                                          const std::function<float (float)>& levelForEnv) const
{
    constexpr int stepsPerSegment = 24;

    const auto points = buildBreakpoints (bounds);
    const auto bottom = bounds.getBottom();
    const auto height = bounds.getHeight();

    const auto envToY = [&] (float env) { return bottom - levelForEnv (env) * height; };

    juce::Path path;
    path.startNewSubPath (points[0].x, envToY (points[0].env));

    for (size_t i = 1; i < points.size(); ++i)
    {
        for (int step = 1; step <= stepsPerSegment; ++step)
        {
            const auto segmentFraction = static_cast<float> (step) / static_cast<float> (stepsPerSegment);
            const auto shapedFraction = shapeProgress (segmentFraction, points[i].curve);

            path.lineTo (points[i - 1].x + (points[i].x - points[i - 1].x) * segmentFraction,
                         envToY (points[i - 1].env + (points[i].env - points[i - 1].env) * shapedFraction));
        }
    }

    return path;
}

void EnvelopeVisualizer::paint (juce::Graphics& g)
{
    const auto bounds = getLocalBounds().toFloat();
    const auto outline = buildPath (bounds, [] (float env) { return env; });

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

    constexpr int numDashLengths = 2;
    constexpr float dashLengths[numDashLengths] = { 4.0f, 4.0f };

    const auto driveCurve = buildPath (bounds, [this] (float env)
    {
        return drive * driveEnvelopeModulation (env, driveEnvAmount);
    });

    juce::Path dashed;
    juce::PathStrokeType (1.0f).createDashedStroke (dashed, driveCurve, dashLengths, numDashLengths);

    g.setColour (CustomLookAndFeel::text.withAlpha (0.35f));
    g.fillPath (dashed);
}
