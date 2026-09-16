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

    addAndMakeVisible (envelopeVisualizer);

    addControl (Parameters::oscOctaveId.getParamID(), "Octave", juce::Slider::IncDecButtons);
    addControl (Parameters::osc1FineId.getParamID(), "Osc 1 Fine");
    addControl (Parameters::oscMixId.getParamID(), "Osc Mix", juce::Slider::LinearHorizontal);
    addControl (Parameters::osc2FineId.getParamID(), "Osc 2 Fine");

    addCurveKnob (addControl (Parameters::envAttackId.getParamID(), "Attack"),
                  Parameters::envAttackCurveId.getParamID());
    addCurveKnob (addControl (Parameters::envDecayId.getParamID(), "Decay"),
                  Parameters::envDecayCurveId.getParamID());
    addControl (Parameters::envSustainId.getParamID(), "Sustain");
    addCurveKnob (addControl (Parameters::envReleaseId.getParamID(), "Release"),
                  Parameters::envReleaseCurveId.getParamID());

    addControl (Parameters::saturationDriveId.getParamID(), "Drive");
    addControl (Parameters::saturationDriveEnvId.getParamID(), "Drive Env");
    constexpr int glideModePillWidth = 52;
    constexpr int glideModePillHeight = 15;

    addControl (Parameters::glideTimeId.getParamID(), "Glide")
        .setAccessory (glideModeButton, glideModePillWidth, glideModePillHeight);
    glideModeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment> (
        processorRef.getAPVTS(), Parameters::glideModeId.getParamID(), glideModeButton);

    addAndMakeVisible (pitchWheel);
    pitchWheel.attachToParameters (processorRef.getAPVTS(),
                                   Parameters::pitchBendId.getParamID(),
                                   Parameters::pitchBendRangeId.getParamID());

    masterGainSlider.setPopupDisplayEnabled (true, true, this);
    addAndMakeVisible (masterGainSlider);
    masterGainAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
        processorRef.getAPVTS(), Parameters::masterGainId.getParamID(), masterGainSlider);
    addAndMakeVisible (masterMeter);

    addAndMakeVisible (presetComboBox);
    presetComboBox.setPresetManager (processorRef.getPresetManager());

    savePresetButton.onClick = [this] { presetComboBox.promptToSavePreset(); };
    addAndMakeVisible (savePresetButton);

    deletePresetButton.onClick = [this] { presetComboBox.promptToDeletePreset(); };
    addAndMakeVisible (deletePresetButton);

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

    auto& apvts = processorRef.getAPVTS();
    envelopeVisualizer.setADSR (*apvts.getRawParameterValue (Parameters::envAttackId.getParamID()),
                                *apvts.getRawParameterValue (Parameters::envDecayId.getParamID()),
                                *apvts.getRawParameterValue (Parameters::envSustainId.getParamID()),
                                *apvts.getRawParameterValue (Parameters::envReleaseId.getParamID()));
    envelopeVisualizer.setDrive (*apvts.getRawParameterValue (Parameters::saturationDriveId.getParamID()),
                                 *apvts.getRawParameterValue (Parameters::saturationDriveEnvId.getParamID()));
    envelopeVisualizer.setCurves (*apvts.getRawParameterValue (Parameters::envAttackCurveId.getParamID()),
                                  *apvts.getRawParameterValue (Parameters::envDecayCurveId.getParamID()),
                                  *apvts.getRawParameterValue (Parameters::envReleaseCurveId.getParamID()));
}

//==============================================================================
LabeledSlider& AudioPluginAudioProcessorEditor::addControl (
    const juce::String& paramId, const juce::String& labelText, juce::Slider::SliderStyle style)
{
    auto control = std::make_unique<LabeledSlider> (style);

    control->setLabelText (labelText);
    control->attachToParameter (processorRef.getAPVTS(), paramId);
    addAndMakeVisible (*control);

    controls.push_back (std::move (control));
    return *controls.back();
}

void AudioPluginAudioProcessorEditor::addCurveKnob (LabeledSlider& owner, const juce::String& paramId)
{
    constexpr int curveKnobSize = 32;

    auto knob = std::make_unique<juce::Slider> (juce::Slider::RotaryHorizontalVerticalDrag, juce::Slider::NoTextBox);
    knob->setPopupDisplayEnabled (true, true, this);
    knob->setDoubleClickReturnValue (true, 0.0);

    curveKnobAttachments.push_back (std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
        processorRef.getAPVTS(), paramId, *knob));

    owner.setAccessory (*knob, curveKnobSize, curveKnobSize);
    curveKnobs.push_back (std::move (knob));
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
    g.setFont (CustomLookAndFeel::boldFont (22.0f));
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
        g.setFont (CustomLookAndFeel::boldFont (12.0f));
        g.drawText (rowTitles[static_cast<size_t> (row)],
                    panelBounds.removeFromTop (static_cast<float> (rowHeaderHeight)).reduced (14.0f, 0.0f),
                    juce::Justification::centredLeft);
    }

    auto masterLabelBounds = juce::Rectangle<int> (masterGainSlider.getX() - 74, 0, 66, titleBarHeight);
    g.setColour (CustomLookAndFeel::textDim);
    g.setFont (CustomLookAndFeel::boldFont (10.0f));
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

    constexpr int presetComboWidth = 130;
    constexpr int presetButtonWidth = 44;
    constexpr int presetControlHeight = 24;
    constexpr int presetGap = 6;
    constexpr int titleTextClearance = 260;

    const int presetClusterWidth = presetComboWidth + presetGap + presetButtonWidth * 2 + presetGap;
    const int masterSectionLeft = knobX - 74 - 20;
    const int presetClusterX = titleTextClearance
                              + juce::jmax (0, (masterSectionLeft - titleTextClearance - presetClusterWidth) / 2);
    const int presetY = (titleBarHeight - presetControlHeight) / 2;

    presetComboBox.setBounds (presetClusterX, presetY, presetComboWidth, presetControlHeight);
    savePresetButton.setBounds (presetComboBox.getRight() + presetGap, presetY, presetButtonWidth, presetControlHeight);
    deletePresetButton.setBounds (savePresetButton.getRight() + presetGap, presetY, presetButtonWidth, presetControlHeight);

    auto bounds = getLocalBounds();
    bounds.removeFromTop (titleBarHeight);
    bounds.reduce (16, 16);

    const int rowHeight = bounds.getHeight() / numRows;

    for (int row = 0; row < numRows; ++row)
    {
        auto panel = bounds.withTrimmedTop (row * rowHeight).withHeight (rowHeight).reduced (0, 4);
        rowBounds[static_cast<size_t> (row)] = panel;

        auto content = panel.withTrimmedTop (rowHeaderHeight).reduced (10, 4);

        if (row == envelopeRowIndex)
            envelopeVisualizer.setBounds (content);

        const int controlsInRow = juce::jmin (numColumns, static_cast<int> (controls.size()) - row * numColumns);

        const int leadingCells = row == outputRowIndex ? 1 : 0;
        const int cellsInRow = leadingCells + controlsInRow;
        if (cellsInRow <= 0)
            continue;

        const int cellWidth = content.getWidth() / numColumns;
        const int columnOffset = (content.getWidth() - cellsInRow * cellWidth) / 2;

        const auto cellAt = [&] (int col)
        {
            return content.withTrimmedLeft (columnOffset + col * cellWidth).withWidth (cellWidth).reduced (6);
        };

        if (row == outputRowIndex)
            pitchWheel.setBounds (cellAt (0));

        for (int col = 0; col < controlsInRow; ++col)
        {
            const auto index = static_cast<size_t> (row * numColumns + col);
            controls[index]->setBounds (cellAt (leadingCells + col));
        }
    }
}
