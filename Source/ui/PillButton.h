#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

//==============================================================================
/**
    A small pill-shaped toggle for switching between two named modes.
*/
class PillButton : public juce::Button
{
public:
    PillButton (const juce::String& textWhenOff, const juce::String& textWhenOn);

    void paintButton (juce::Graphics&, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;

private:
    juce::String offStateText, onStateText;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PillButton)
};
