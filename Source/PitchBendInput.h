#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <optional>

//==============================================================================
/**
    Keeps a pitch bend parameter and incoming MIDI pitch wheel in step.
*/
class PitchBendInput : private juce::AsyncUpdater,
                       private juce::AudioProcessorParameter::Listener
{
public:
    explicit PitchBendInput (juce::RangedAudioParameter& bendParameter);
    ~PitchBendInput() override;

    /** Audio thread. Returns the wheel position as -1..1. */
    float handlePitchWheel (int wheelPosition);

    /** Audio thread. Returns the new bend if the parameter changed from anywhere but MIDI. */
    std::optional<float> takeParameterChange();

private:
    void handleAsyncUpdate() override;
    void parameterValueChanged (int parameterIndex, float newNormalisedValue) override;
    void parameterGestureChanged (int, bool) override {}

    juce::RangedAudioParameter& parameter;

    std::atomic<float> midiPitchBend { 0.0f };

    std::atomic<float> parameterPitchBend { 0.0f };
    std::atomic<bool> parameterPitchBendChanged { false };

    bool isEchoingMidiPitchBend = false;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PitchBendInput)
};
