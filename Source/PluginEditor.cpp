#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "Parameters.h"

namespace
{
    constexpr int titleBarHeight = 60;
    constexpr int rowHeaderHeight = 22;
}

//==============================================================================
AudioPluginAudioProcessorEditor::AudioPluginAudioProcessorEditor (AudioPluginAudioProcessor& p)
    : AudioProcessorEditor (&p), processorRef (p)
{
    setLookAndFeel (&customLookAndFeel);

    addControl (Parameters::oscOctaveId.getParamID(), "Octave");
    addControl (Parameters::osc1FineId.getParamID(), "Osc 1 Fine");
    addControl (Parameters::osc2FineId.getParamID(), "Osc 2 Fine");
    addControl (Parameters::oscMixId.getParamID(), "Osc Mix");

    addControl (Parameters::envAttackId.getParamID(), "Attack");
    addControl (Parameters::envDecayId.getParamID(), "Decay");
    addControl (Parameters::envSustainId.getParamID(), "Sustain");
    addControl (Parameters::envReleaseId.getParamID(), "Release");

    addControl (Parameters::saturationDriveId.getParamID(), "Drive");

    masterGainSlider.setPopupDisplayEnabled (true, true, this);
    addAndMakeVisible (masterGainSlider);
    masterGainAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
        processorRef.getAPVTS(), Parameters::masterGainId.getParamID(), masterGainSlider);
    addAndMakeVisible (masterMeter);

    setSize (700, 640);
    startTimerHz (30);
}

AudioPluginAudioProcessorEditor::~AudioPluginAudioProcessorEditor()
{
    setLookAndFeel (nullptr);
}

void AudioPluginAudioProcessorEditor::timerCallback()
{
    masterMeter.setLevel (processorRef.getOutputLevel());
    masterMeter.setClipping (processorRef.isOutputClipping());
}

//==============================================================================
AudioPluginAudioProcessorEditor::SliderWithLabel& AudioPluginAudioProcessorEditor::addControl (
    const juce::String& paramId, const juce::String& labelText)
{
    auto control = std::make_unique<SliderWithLabel>();

    control->label.setText (labelText, juce::dontSendNotification);
    control->label.setJustificationType (juce::Justification::centred);
    addAndMakeVisible (control->label);

    control->slider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 64, 18);
    addAndMakeVisible (control->slider);
    control->attachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
        processorRef.getAPVTS(), paramId, control->slider);

    controls.push_back (std::move (control));
    return *controls.back();
}

//==============================================================================
void AudioPluginAudioProcessorEditor::paint (juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();

    juce::ColourGradient backgroundGradient (CustomLookAndFeel::background.brighter (0.05f), bounds.getX(), bounds.getY(),
                                             CustomLookAndFeel::background.darker (0.3f), bounds.getX(), bounds.getBottom(), false);
    g.setGradientFill (backgroundGradient);
    g.fillRect (bounds);

    auto titleBounds = getLocalBounds().removeFromTop (titleBarHeight).toFloat();
    g.setColour (CustomLookAndFeel::text);
    g.setFont (juce::Font (CustomLookAndFeel::orbitronBold()).withHeight (22.0f));
    g.drawText ("SampleRealm: SUB", titleBounds.reduced (20.0f, 0.0f), juce::Justification::centredLeft);

    g.setColour (CustomLookAndFeel::accent.withAlpha (0.6f));
    g.fillRect (juce::Rectangle<float> (0.0f, static_cast<float> (titleBarHeight - 1), bounds.getWidth(), 2.0f));

    for (int row = 0; row < numRows; ++row)
    {
        auto panelBounds = rowBounds[static_cast<size_t> (row)].toFloat();

        g.setColour (CustomLookAndFeel::panel.withAlpha (0.6f));
        g.fillRoundedRectangle (panelBounds, 10.0f);
        g.setColour (CustomLookAndFeel::panelBorder);
        g.drawRoundedRectangle (panelBounds, 10.0f, 1.0f);

        g.setColour (CustomLookAndFeel::textDim);
        g.setFont (juce::Font (CustomLookAndFeel::orbitronBold()).withHeight (12.0f));
        g.drawText (rowTitles[static_cast<size_t> (row)],
                    panelBounds.removeFromTop (static_cast<float> (rowHeaderHeight)).reduced (14.0f, 0.0f),
                    juce::Justification::centredLeft);
    }

    auto masterLabelBounds = juce::Rectangle<int> (masterGainSlider.getX() - 74, 0, 66, titleBarHeight);
    g.setColour (CustomLookAndFeel::textDim);
    g.setFont (juce::Font (CustomLookAndFeel::orbitronBold()).withHeight (10.0f));
    g.drawText ("MASTER", masterLabelBounds, juce::Justification::centredRight);

    g.setFont (juce::Font (CustomLookAndFeel::orbitronRegular()).withPointHeight (9.0f));
    g.setColour (juce::Colour (0xff00d9ff).withAlpha (0.4f));
    auto versionArea = juce::Rectangle<int> (getWidth() - 60, getHeight() - 23, 50, 12);
    g.drawText ("v" + juce::String (JucePlugin_VersionString), versionArea, juce::Justification::centredRight);
}

void AudioPluginAudioProcessorEditor::resized()
{
    constexpr int meterWidth = 14;
    constexpr int meterHeight = 40;
    constexpr int knobSize = 44;
    constexpr int rightMargin = 20;
    constexpr int gap = 10;

    const int meterX = getWidth() - rightMargin - meterWidth;
    masterMeter.setBounds (meterX, (titleBarHeight - meterHeight) / 2, meterWidth, meterHeight);

    const int knobX = meterX - gap - knobSize;
    masterGainSlider.setBounds (knobX, (titleBarHeight - knobSize) / 2, knobSize, knobSize);

    auto bounds = getLocalBounds();
    bounds.removeFromTop (titleBarHeight);
    bounds.reduce (16, 16);

    const int rowHeight = bounds.getHeight() / numRows;

    for (int row = 0; row < numRows; ++row)
    {
        auto panel = bounds.withTrimmedTop (row * rowHeight).withHeight (rowHeight).reduced (0, 4);
        rowBounds[static_cast<size_t> (row)] = panel;

        auto content = panel.withTrimmedTop (rowHeaderHeight).reduced (10, 4);
        const int itemsInRow = juce::jmin (numColumns, static_cast<int> (controls.size()) - row * numColumns);
        if (itemsInRow <= 0)
            continue;

        const int cellWidth = content.getWidth() / numColumns;
        const int columnOffset = (numColumns - itemsInRow) / 2;

        for (int col = 0; col < itemsInRow; ++col)
        {
            const auto index = static_cast<size_t> (row * numColumns + col);
            auto cell = content.withTrimmedLeft ((columnOffset + col) * cellWidth).withWidth (cellWidth).reduced (6);

            auto& control = *controls[index];
            control.label.setBounds (cell.removeFromTop (16));
            control.slider.setBounds (cell);
        }
    }
}
