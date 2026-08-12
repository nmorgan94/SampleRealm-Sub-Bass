#pragma once

#include <juce_audio_processors/juce_audio_processors.h>

//==============================================================================

class CustomLookAndFeel : public juce::LookAndFeel_V4
{
public:
    CustomLookAndFeel()           = default;
    ~CustomLookAndFeel() override = default;


private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CustomLookAndFeel)
};
