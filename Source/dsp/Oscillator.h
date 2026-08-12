#pragma once

#include <juce_core/juce_core.h>
#include <cmath>

//==============================================================================
class SineOscillator
{
public:
    void prepare (double newSampleRate)
    {
        sampleRate = newSampleRate;
        updatePhaseIncrement();
    }

    void setFrequency (float newFrequencyHz)
    {
        frequency = newFrequencyHz;
        updatePhaseIncrement();
    }

    float renderSample()
    {
        const auto sample = static_cast<float> (std::sin (phase));

        phase += phaseIncrement;
        if (phase >= juce::MathConstants<double>::twoPi)
            phase -= juce::MathConstants<double>::twoPi;

        return sample;
    }

private:
    void updatePhaseIncrement()
    {
        phaseIncrement = juce::MathConstants<double>::twoPi * frequency / sampleRate;
    }

    double sampleRate = 44100.0;
    double phase = 0.0;
    double phaseIncrement = 0.0;
    float frequency = 440.0f;
};
