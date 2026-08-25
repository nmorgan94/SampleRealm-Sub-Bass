#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

//==============================================================================
/** Compact vertical peak meter with a clip LED, sized for the title bar. */
class LevelMeter : public juce::Component
{
public:
    void setLevel (float newLinearLevel);
    void setClipping (bool isClipping);

    void paint (juce::Graphics&) override;

private:
    float normalisedLevel = 0.0f;
    bool clipped = false;
};
