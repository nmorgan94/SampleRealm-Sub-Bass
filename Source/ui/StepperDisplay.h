#pragma once

#include <juce_audio_processors/juce_audio_processors.h>

//==============================================================================
/**
    A boxed integer readout with a label.
    Drag vertically to step the value; double-click to reset it.

*/
class StepperDisplay : public juce::Component
{
public:
    StepperDisplay();

    void attachToParameter (juce::AudioProcessorValueTreeState& apvts, const juce::String& paramId);

    void setLabel (const juce::String& newLabel);
    void setShowSign (bool shouldShowSign);

    void paint (juce::Graphics&) override;
    void mouseDown (const juce::MouseEvent&) override;
    void mouseDrag (const juce::MouseEvent&) override;
    void mouseUp (const juce::MouseEvent&) override;
    void mouseDoubleClick (const juce::MouseEvent&) override;

private:
    void setValue (int newValue);

    std::unique_ptr<juce::ParameterAttachment> attachment;

    int value = 0;
    int minimum = 0;
    int maximum = 0;
    int defaultValue = 0;
    bool showSign = true;
    juce::String labelText;

    int dragStartValue = 0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (StepperDisplay)
};
