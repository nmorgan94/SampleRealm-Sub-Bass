#include "StepperDisplay.h"
#include "CustomLookAndFeel.h"

namespace
{
    constexpr int pixelsPerStep = 3;
    constexpr float cornerRadius = 4.0f;
    constexpr float labelFontHeight = 10.0f;
    constexpr float valueFontHeight = 12.0f;
}

//==============================================================================
StepperDisplay::StepperDisplay()
{
    setMouseCursor (juce::MouseCursor::UpDownResizeCursor);
}

void StepperDisplay::attachToParameter (juce::AudioProcessorValueTreeState& apvts, const juce::String& paramId)
{
    auto& parameter = *apvts.getParameter (paramId);

    const auto& range = parameter.getNormalisableRange();
    minimum = juce::roundToInt (range.start);
    maximum = juce::roundToInt (range.end);
    defaultValue = juce::roundToInt (range.convertFrom0to1 (parameter.getDefaultValue()));

    attachment = std::make_unique<juce::ParameterAttachment> (
        parameter,
        [this] (float newValue) { setValue (juce::roundToInt (newValue)); });
    attachment->sendInitialUpdate();
}

//==============================================================================
void StepperDisplay::setValue (int newValue)
{
    value = juce::jlimit (minimum, maximum, newValue);
    repaint();
}

void StepperDisplay::setLabel (const juce::String& newLabel)
{
    labelText = newLabel;
    repaint();
}

void StepperDisplay::setShowSign (bool shouldShowSign)
{
    showSign = shouldShowSign;
    repaint();
}

//==============================================================================
void StepperDisplay::paint (juce::Graphics& g)
{
    auto bounds = getLocalBounds();

    g.setColour (CustomLookAndFeel::panel);
    g.fillRoundedRectangle (bounds.toFloat(), cornerRadius);

    g.setColour (CustomLookAndFeel::panelBorder);
    g.drawRoundedRectangle (bounds.toFloat().reduced (0.5f), cornerRadius, 1.0f);

    const auto labelBounds = bounds.removeFromLeft (bounds.getWidth() / 2);

    g.setColour (CustomLookAndFeel::textDim);
    g.setFont (CustomLookAndFeel::boldFont (labelFontHeight));
    g.drawText (labelText, labelBounds, juce::Justification::centred);

    g.setColour (CustomLookAndFeel::text);
    g.setFont (CustomLookAndFeel::boldFont (valueFontHeight));
    g.drawText ((showSign && value >= 0 ? "+" : "") + juce::String (value), bounds, juce::Justification::centred);
}

//==============================================================================
void StepperDisplay::mouseDown (const juce::MouseEvent&)
{
    dragStartValue = value;

    if (attachment != nullptr)
        attachment->beginGesture();
}

void StepperDisplay::mouseDrag (const juce::MouseEvent& event)
{
    const auto newValue = juce::jlimit (minimum, maximum, dragStartValue - event.getDistanceFromDragStartY() / pixelsPerStep);

    if (newValue == value)
        return;

    setValue (newValue);

    if (attachment != nullptr)
        attachment->setValueAsPartOfGesture (static_cast<float> (value));
}

void StepperDisplay::mouseUp (const juce::MouseEvent&)
{
    if (attachment != nullptr)
        attachment->endGesture();
}

void StepperDisplay::mouseDoubleClick (const juce::MouseEvent&)
{
    // JUCE delivers this after mouseUp, so the drag gesture is already closed.
    setValue (defaultValue);

    if (attachment != nullptr)
        attachment->setValueAsCompleteGesture (static_cast<float> (value));
}
