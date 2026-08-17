#pragma once

#include "PluginProcessor.h"
#include "ui/CustomLookAndFeel.h"

//==============================================================================
class AudioPluginAudioProcessorEditor final : public juce::AudioProcessorEditor
{
public:
    explicit AudioPluginAudioProcessorEditor (AudioPluginAudioProcessor&);
    ~AudioPluginAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    struct SliderWithLabel
    {
        juce::Slider slider { juce::Slider::RotaryHorizontalVerticalDrag, juce::Slider::TextBoxBelow };
        juce::Label label;
        std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> attachment;
    };

    SliderWithLabel& addControl (const juce::String& paramId, const juce::String& labelText);

    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    AudioPluginAudioProcessor& processorRef;

    CustomLookAndFeel customLookAndFeel;

    std::vector<std::unique_ptr<SliderWithLabel>> controls;

    static constexpr int numColumns = 4;
    static constexpr int numRows = 3;

    std::array<juce::String, numRows> rowTitles { "OSCILLATOR", "ENVELOPE", "OUTPUT" };
    std::array<juce::Rectangle<int>, numRows> rowBounds;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioPluginAudioProcessorEditor)
};
