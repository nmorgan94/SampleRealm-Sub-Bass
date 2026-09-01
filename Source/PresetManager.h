#pragma once

#include <juce_audio_processors/juce_audio_processors.h>

class PresetManager
{
public:
    explicit PresetManager (juce::AudioProcessorValueTreeState& stateToUse);

    juce::StringArray getAllPresetNames() const;
    juce::String getCurrentPresetName() const;

    bool savePreset (const juce::String& name);
    bool loadPreset (const juce::String& name);
    bool deletePreset (const juce::String& name);

    void resetToDefaults();

    static const juce::Identifier currentPresetProperty;
    static const juce::String initPresetName;

private:
    static juce::File getPresetDirectory();
    static juce::Array<juce::File> findPresetFiles();
    static juce::File getFileForPreset (const juce::String& name);

    juce::AudioProcessorValueTreeState& apvts;
};
