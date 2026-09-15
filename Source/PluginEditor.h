#pragma once

#include "PluginProcessor.h"
#include "ui/CustomLookAndFeel.h"
#include "ui/EnvelopeVisualizer.h"
#include "ui/LabeledSlider.h"
#include "ui/LevelMeter.h"
#include "ui/PillButton.h"
#include "ui/PitchWheel.h"
#include "ui/PresetComboBox.h"

//==============================================================================
class AudioPluginAudioProcessorEditor final : public juce::AudioProcessorEditor,
                                              private juce::Timer
{
public:
    explicit AudioPluginAudioProcessorEditor (AudioPluginAudioProcessor&);
    ~AudioPluginAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    void timerCallback() override;

    LabeledSlider& addControl (const juce::String& paramId, const juce::String& labelText,
                               juce::Slider::SliderStyle style = juce::Slider::RotaryHorizontalVerticalDrag);

    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    AudioPluginAudioProcessor& processorRef;

    CustomLookAndFeel customLookAndFeel;

    std::vector<std::unique_ptr<LabeledSlider>> controls;

    EnvelopeVisualizer envelopeVisualizer;

    PillButton glideModeButton { "TIME", "RATE" };
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> glideModeAttachment;
    LabeledSlider* glideControl = nullptr;
    PitchWheel pitchWheel;

    PresetComboBox presetComboBox;
    juce::TextButton savePresetButton { "Save" };
    juce::TextButton deletePresetButton { "Delete" };

    // Master gain lives in the title bar, not the parameter grid, so it's wired up separately.
    juce::Slider masterGainSlider { juce::Slider::RotaryHorizontalVerticalDrag, juce::Slider::NoTextBox };
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> masterGainAttachment;
    LevelMeter masterMeter;

    static constexpr int numColumns = 4;
    static constexpr int numRows = 3;
    static constexpr int envelopeRowIndex = 1;
    static constexpr int outputRowIndex = 2;

    std::array<juce::String, numRows> rowTitles { "OSCILLATOR", "ENVELOPE", "TONE & PITCH" };
    std::array<juce::Rectangle<int>, numRows> rowBounds;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioPluginAudioProcessorEditor)
};
