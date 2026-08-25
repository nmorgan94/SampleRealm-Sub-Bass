#include "LevelMeter.h"
#include "CustomLookAndFeel.h"

//==============================================================================
void LevelMeter::setLevel (float newLinearLevel)
{
    const auto db = juce::Decibels::gainToDecibels (newLinearLevel, -60.0f);
    const auto normalised = juce::jlimit (0.0f, 1.0f, (db + 60.0f) / 60.0f);

    if (! juce::approximatelyEqual (normalised, normalisedLevel))
    {
        normalisedLevel = normalised;
        repaint();
    }
}

void LevelMeter::setClipping (bool isClipping)
{
    if (clipped != isClipping)
    {
        clipped = isClipping;
        repaint();
    }
}

void LevelMeter::paint (juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();
    auto clipBounds = bounds.removeFromTop (6.0f);
    bounds.removeFromTop (3.0f);

    g.setColour (clipped ? juce::Colours::red : CustomLookAndFeel::panelBorder);
    g.fillRoundedRectangle (clipBounds, 2.0f);

    g.setColour (CustomLookAndFeel::panel);
    g.fillRoundedRectangle (bounds, 2.0f);
    g.setColour (CustomLookAndFeel::panelBorder);
    g.drawRoundedRectangle (bounds, 2.0f, 1.0f);

    auto fillBounds = bounds.removeFromBottom (bounds.getHeight() * normalisedLevel);
    g.setColour (CustomLookAndFeel::accent);
    g.fillRoundedRectangle (fillBounds, 2.0f);
}
