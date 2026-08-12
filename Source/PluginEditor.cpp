#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "Parameters.h"

//==============================================================================
AudioPluginAudioProcessorEditor::AudioPluginAudioProcessorEditor (AudioPluginAudioProcessor& p)
    : AudioProcessorEditor (&p), processorRef (p)
{
    setLookAndFeel (&customLookAndFeel);

    addControl (Parameters::osc1CoarseId.getParamID(), "Osc 1 Coarse");
    addControl (Parameters::osc1FineId.getParamID(), "Osc 1 Fine");
    addControl (Parameters::osc2CoarseId.getParamID(), "Osc 2 Coarse");
    addControl (Parameters::osc2FineId.getParamID(), "Osc 2 Fine");
    addControl (Parameters::oscMixId.getParamID(), "Osc Mix");
    addControl (Parameters::envAttackId.getParamID(), "Attack");
    addControl (Parameters::envDecayId.getParamID(), "Decay");
    addControl (Parameters::envSustainId.getParamID(), "Sustain");
    addControl (Parameters::envReleaseId.getParamID(), "Release");
    addControl (Parameters::saturationDriveId.getParamID(), "Drive");
    addControl (Parameters::masterGainId.getParamID(), "Master Gain");

    setSize (560, 360);
}

AudioPluginAudioProcessorEditor::~AudioPluginAudioProcessorEditor()
{
    setLookAndFeel (nullptr);
}

//==============================================================================
AudioPluginAudioProcessorEditor::SliderWithLabel& AudioPluginAudioProcessorEditor::addControl (
    const juce::String& paramId, const juce::String& labelText)
{
    auto control = std::make_unique<SliderWithLabel>();

    control->label.setText (labelText, juce::dontSendNotification);
    control->label.setJustificationType (juce::Justification::centred);
    addAndMakeVisible (control->label);

    addAndMakeVisible (control->slider);
    control->attachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
        processorRef.getAPVTS(), paramId, control->slider);

    controls.push_back (std::move (control));
    return *controls.back();
}

//==============================================================================
void AudioPluginAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
}

void AudioPluginAudioProcessorEditor::resized()
{
    constexpr int numColumns = 4;
    const int numRows = (static_cast<int> (controls.size()) + numColumns - 1) / numColumns;

    auto bounds = getLocalBounds().reduced (10);
    const int cellWidth = bounds.getWidth() / numColumns;
    const int cellHeight = bounds.getHeight() / juce::jmax (1, numRows);

    for (size_t i = 0; i < controls.size(); ++i)
    {
        const int column = static_cast<int> (i) % numColumns;
        const int row = static_cast<int> (i) / numColumns;

        auto cell = bounds.withTrimmedLeft (column * cellWidth)
                           .withTrimmedTop (row * cellHeight)
                           .withWidth (cellWidth)
                           .withHeight (cellHeight)
                           .reduced (6);

        auto& control = *controls[i];
        control.label.setBounds (cell.removeFromTop (20));
        control.slider.setBounds (cell);
    }
}
