#include "PillButton.h"
#include "CustomLookAndFeel.h"

namespace
{
    constexpr float fontHeight = 9.0f;
}

//==============================================================================
PillButton::PillButton (const juce::String& textWhenOff, const juce::String& textWhenOn)
    : juce::Button (textWhenOff + "/" + textWhenOn),
      offStateText (textWhenOff),
      onStateText (textWhenOn)
{
    setClickingTogglesState (true);
}

void PillButton::paintButton (juce::Graphics& g, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown)
{
    const auto bounds = getLocalBounds().toFloat();
    const auto cornerRadius = bounds.getHeight() * 0.5f;
    const auto isOn = getToggleState();

    const auto lift = shouldDrawButtonAsDown ? 0.25f
                    : shouldDrawButtonAsHighlighted ? 0.12f
                    : 0.0f;

    g.setColour ((isOn ? CustomLookAndFeel::accent : CustomLookAndFeel::panel).brighter (lift));
    g.fillRoundedRectangle (bounds, cornerRadius);

    g.setColour (isOn ? CustomLookAndFeel::accent : CustomLookAndFeel::panelBorder);
    g.drawRoundedRectangle (bounds.reduced (0.5f), cornerRadius, 1.0f);

    g.setColour (isOn ? CustomLookAndFeel::background : CustomLookAndFeel::textDim);
    g.setFont (juce::Font (CustomLookAndFeel::orbitronBold()).withHeight (fontHeight));
    g.drawText (isOn ? onStateText : offStateText, bounds, juce::Justification::centred);
}
