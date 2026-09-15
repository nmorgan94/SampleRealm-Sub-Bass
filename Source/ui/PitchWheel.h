#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include "StepperDisplay.h"

//==============================================================================
/**
    A playable pitch wheel. Drag it to bend and it springs back to centre on release,
*/
class PitchWheel : public juce::Component,
                   private juce::Timer
{
public:
    PitchWheel();
    ~PitchWheel() override;

    void attachToParameters (juce::AudioProcessorValueTreeState& apvts,
                             const juce::String& bendParamId,
                             const juce::String& rangeParamId);

    void paint (juce::Graphics&) override;
    void resized() override;
    void mouseDown (const juce::MouseEvent&) override;
    void mouseDrag (const juce::MouseEvent&) override;
    void mouseUp (const juce::MouseEvent&) override;

private:
    void timerCallback() override;

    [[nodiscard]] juce::Rectangle<int> wheelBounds() const;
    [[nodiscard]] juce::Rectangle<int> nameBounds() const;
    [[nodiscard]] float thumbTravel() const;

    StepperDisplay rangeStepper;
    std::unique_ptr<juce::ParameterAttachment> bendAttachment;

    float bend = 0.0f;
    bool isDraggingWheel = false;
    float bendAtDragStart = 0.0f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PitchWheel)
};
