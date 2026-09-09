#pragma once

#include <juce_audio_processors/juce_audio_processors.h>

//==============================================================================
/**
    A rotary slider with its name shown below it. While the slider is being
    dragged, the label swaps to show the current value instead; on release it
    reverts to the name. Double-clicking the slider lets you type an exact
    value into the label.
*/
class LabeledSlider : public juce::Component,
                       private juce::Timer,
                       private juce::Label::Listener
{
public:
    explicit LabeledSlider (juce::Slider::SliderStyle style = juce::Slider::RotaryHorizontalVerticalDrag);

    void attachToParameter (juce::AudioProcessorValueTreeState& apvts, const juce::String& paramId);
    void setLabelText (const juce::String& newText);

    void resized() override;
    void mouseDoubleClick (const juce::MouseEvent&) override;
    void parentHierarchyChanged() override;

private:
    void showCurrentValue();
    void showName();
    void timerCallback() override;
    void labelTextChanged (juce::Label*) override {}
    void editorHidden (juce::Label*, juce::TextEditor&) override;

    juce::Slider slider;
    juce::Label label;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> attachment;

    juce::String nameText;
    bool isDragging = false;

    static constexpr int labelHeight = 16;
    static constexpr int revertDelayMs = 600;
    static constexpr int incDecButtonsHeight = 70;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LabeledSlider)
};
