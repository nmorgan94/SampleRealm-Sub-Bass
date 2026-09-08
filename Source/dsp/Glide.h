#pragma once

#include <juce_audio_basics/juce_audio_basics.h>

//==============================================================================
inline float noteNumberToFrequencyHz (float midiNoteNumber)
{
    return 440.0f * std::pow (2.0f, (midiNoteNumber - 69.0f) / 12.0f);
}

//==============================================================================
class GlideProcessor
{
public:
    void prepare (double newSampleRate)
    {
        sampleRate = newSampleRate;
        noteSmoother.reset (sampleRate, static_cast<double> (glideTimeSeconds));
    }

    void setGlideTime (float newGlideTimeSeconds)
    {
        if (juce::approximatelyEqual (newGlideTimeSeconds, glideTimeSeconds))
            return;

        glideTimeSeconds = newGlideTimeSeconds;
        noteSmoother.reset (sampleRate, static_cast<double> (glideTimeSeconds));
    }

    void startNote (float midiNoteNumber, bool legato)
    {
        if (legato)
            noteSmoother.setTargetValue (midiNoteNumber);
        else
            noteSmoother.setCurrentAndTargetValue (midiNoteNumber);
    }

    float getNextNoteNumber() noexcept { return noteSmoother.getNextValue(); }

private:
    juce::SmoothedValue<float> noteSmoother;
    double sampleRate = 44100.0;
    float glideTimeSeconds = 0.0f;
};
