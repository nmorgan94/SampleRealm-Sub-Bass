#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

//==============================================================================
/** Background visualization of the ADSR shape, drawn behind the envelope sliders. */
class EnvelopeVisualizer : public juce::Component
{
public:
    EnvelopeVisualizer();

    void setADSR (float attackSeconds, float decaySeconds, float sustainLevel, float releaseSeconds);

    void paint (juce::Graphics&) override;

private:
    [[nodiscard]] juce::Path buildEnvelopePath (juce::Rectangle<float> bounds) const;

    float attackTime = 0.005f, decayTime = 0.1f, sustainLevel = 0.8f, releaseTime = 0.2f;
};
