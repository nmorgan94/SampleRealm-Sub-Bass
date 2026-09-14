#pragma once

#include <juce_audio_basics/juce_audio_basics.h>

//==============================================================================
inline constexpr float semitonesPerOctave = 12.0f;

inline float noteNumberToFrequencyHz (float midiNoteNumber)
{
    return 440.0f * std::pow (2.0f, (midiNoteNumber - 69.0f) / semitonesPerOctave);
}

//==============================================================================
enum class GlideMode
{
    fixedTime,  // every slide takes the same time, whatever the interval
    fixedRate   // pitch moves at a constant speed, so distance sets the time
};

//==============================================================================
class GlideProcessor
{
public:
    void prepare (double newSampleRate)
    {
        sampleRate = newSampleRate;
    }

    void startNote (float midiNoteNumber, bool legato, float glideSeconds, GlideMode mode)
    {
        if (! legato)
        {
            noteSmoother.setCurrentAndTargetValue (midiNoteNumber);
            return;
        }

        const auto currentNoteNumber = noteSmoother.getCurrentValue();
        const auto intervalSemitones = std::abs (midiNoteNumber - currentNoteNumber);

        const auto rampSeconds = mode == GlideMode::fixedRate
                               ? glideSeconds * intervalSemitones / semitonesPerOctave
                               : glideSeconds;

        noteSmoother.reset (sampleRate, rampSeconds);
        noteSmoother.setCurrentAndTargetValue (currentNoteNumber);
        noteSmoother.setTargetValue (midiNoteNumber);
    }

    float getNextNoteNumber() noexcept { return noteSmoother.getNextValue(); }

private:
    juce::SmoothedValue<float> noteSmoother;
    double sampleRate = 44100.0;
};
