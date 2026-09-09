#include "LabeledSlider.h"

//==============================================================================
LabeledSlider::LabeledSlider (juce::Slider::SliderStyle style)
    : slider (style, juce::Slider::NoTextBox)
{
    addAndMakeVisible (slider);

    slider.onDragStart = [this]
    {
        stopTimer();
        isDragging = true;
        showCurrentValue();
    };
    slider.onValueChange = [this]
    {
        if (isDragging)
            showCurrentValue();
    };
    slider.onDragEnd = [this]
    {
        isDragging = false;
        startTimer (revertDelayMs);
    };

    slider.addMouseListener (this, false);

    label.setJustificationType (juce::Justification::centred);
    label.addListener (this);
    addAndMakeVisible (label);
}

void LabeledSlider::attachToParameter (juce::AudioProcessorValueTreeState& apvts, const juce::String& paramId)
{
    attachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (apvts, paramId, slider);

    slider.setDoubleClickReturnValue (false, 0.0);
}

void LabeledSlider::mouseDoubleClick (const juce::MouseEvent&)
{
    stopTimer();
    isDragging = false;
    showCurrentValue();
    label.showEditor();
}

void LabeledSlider::editorHidden (juce::Label*, juce::TextEditor& editor)
{
    slider.setValue (slider.getValueFromText (editor.getText()), juce::sendNotificationSync);
    showName();
}

void LabeledSlider::setLabelText (const juce::String& newText)
{
    nameText = newText;
    showName();
}

void LabeledSlider::timerCallback()
{
    stopTimer();
    showName();
}

void LabeledSlider::showCurrentValue()
{
    label.setText (slider.getTextFromValue (slider.getValue()), juce::dontSendNotification);
}

void LabeledSlider::showName()
{
    label.setText (nameText, juce::dontSendNotification);
}

void LabeledSlider::resized()
{
    auto bounds = getLocalBounds();
    label.setBounds (bounds.removeFromBottom (labelHeight));
    slider.setBounds (bounds);
}
