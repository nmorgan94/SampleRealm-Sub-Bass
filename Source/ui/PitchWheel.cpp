#include "PitchWheel.h"
#include "CustomLookAndFeel.h"

namespace
{
    constexpr int springTimerHz = 60;
    constexpr float springDecayPerTick = 0.45f;
    constexpr float springSettleThreshold = 0.001f;

    constexpr float trackWidth = 20.0f;
    constexpr float thumbHeight = 12.0f;

    constexpr int nameHeight = 16;
    constexpr float nameFontHeight = 13.0f;

    constexpr int stepperWidth = 96;
    constexpr int stepperHeight = 20;
    constexpr int stepperGap = 4;
}

//==============================================================================
PitchWheel::PitchWheel()
{
    setMouseCursor (juce::MouseCursor::UpDownResizeCursor);

    rangeStepper.setLabel ("RANGE");
    rangeStepper.setShowSign (false);
    addAndMakeVisible (rangeStepper);
}

PitchWheel::~PitchWheel()
{
    // Closing the editor mid-drag or mid-spring would otherwise leave the bend held off-centre
    // and the host's gesture open.
    if (! (isDraggingWheel || isTimerRunning()))
        return;

    stopTimer();
    bendAttachment->setValueAsPartOfGesture (0.0f);
    bendAttachment->endGesture();
}

void PitchWheel::attachToParameters (juce::AudioProcessorValueTreeState& apvts,
                                     const juce::String& bendParamId,
                                     const juce::String& rangeParamId)
{
    bendAttachment = std::make_unique<juce::ParameterAttachment> (
        *apvts.getParameter (bendParamId),
        [this] (float newBend)
        {
            if (isDraggingWheel || isTimerRunning())
                return;

            bend = newBend;
            repaint();
        });
    bendAttachment->sendInitialUpdate();

    rangeStepper.attachToParameter (apvts, rangeParamId);
}

//==============================================================================
juce::Rectangle<int> PitchWheel::wheelBounds() const
{
    return getLocalBounds().withTrimmedBottom (nameHeight + stepperHeight + stepperGap);
}

juce::Rectangle<int> PitchWheel::nameBounds() const
{
    return getLocalBounds().removeFromBottom (nameHeight);
}

float PitchWheel::thumbTravel() const
{
    return (wheelBounds().toFloat().getHeight() - thumbHeight) * 0.5f;
}

void PitchWheel::resized()
{
    rangeStepper.setBounds (getLocalBounds().withTrimmedBottom (nameHeight)
                                            .removeFromBottom (stepperHeight)
                                            .withSizeKeepingCentre (stepperWidth, stepperHeight));
}

//==============================================================================
void PitchWheel::paint (juce::Graphics& g)
{
    const auto wheel = wheelBounds().toFloat();
    const auto track = juce::Rectangle<float> (trackWidth, wheel.getHeight()).withCentre (wheel.getCentre());
    const auto cornerRadius = trackWidth * 0.5f;

    juce::ColourGradient trackGradient (CustomLookAndFeel::panel.brighter (0.2f), track.getCentreX(), track.getY(),
                                        CustomLookAndFeel::panel.darker (0.5f), track.getCentreX(), track.getBottom(),
                                        false);
    g.setGradientFill (trackGradient);
    g.fillRoundedRectangle (track, cornerRadius);

    g.setColour (CustomLookAndFeel::panelBorder);
    g.drawRoundedRectangle (track.reduced (0.5f), cornerRadius, 1.0f);
    g.drawLine (track.getX(), track.getCentreY(), track.getRight(), track.getCentreY(), 1.0f);

    const auto thumb = juce::Rectangle<float> (trackWidth, thumbHeight)
                           .withCentre ({ track.getCentreX(), track.getCentreY() - bend * thumbTravel() });

    g.setColour (CustomLookAndFeel::accent);
    g.fillRoundedRectangle (thumb, thumbHeight * 0.4f);

    g.setColour (CustomLookAndFeel::textDim);
    g.setFont (CustomLookAndFeel::boldFont (nameFontHeight));
    g.drawText ("Pitch", nameBounds(), juce::Justification::centred);
}

//==============================================================================
void PitchWheel::mouseDown (const juce::MouseEvent& event)
{
    if (! wheelBounds().contains (event.getPosition()))
        return;

    // Grabbing the wheel while it springs back continues the gesture that is still open.
    const auto wasSpringingBack = isTimerRunning();

    isDraggingWheel = true;
    bendAtDragStart = bend;
    stopTimer();

    if (! wasSpringingBack)
        bendAttachment->beginGesture();
}

void PitchWheel::mouseDrag (const juce::MouseEvent& event)
{
    if (! isDraggingWheel)
        return;

    const auto draggedUpBy = event.mouseDownPosition.y - event.position.y;

    bend = juce::jlimit (-1.0f, 1.0f, bendAtDragStart + draggedUpBy / thumbTravel());
    bendAttachment->setValueAsPartOfGesture (bend);
    repaint (wheelBounds());
}

void PitchWheel::mouseUp (const juce::MouseEvent&)
{
    if (! isDraggingWheel)
        return;

    isDraggingWheel = false;
    startTimerHz (springTimerHz);
}

void PitchWheel::timerCallback()
{
    bend *= springDecayPerTick;

    const auto hasSettled = std::abs (bend) < springSettleThreshold;
    if (hasSettled)
        bend = 0.0f;

    bendAttachment->setValueAsPartOfGesture (bend);
    repaint (wheelBounds());

    if (hasSettled)
    {
        stopTimer();
        bendAttachment->endGesture();
    }
}
