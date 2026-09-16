#pragma once

#include <juce_core/juce_core.h>
#include <algorithm>
#include <cmath>

//==============================================================================
/** Simple tanh soft-clip saturator, normalized to stay near unity gain as drive increases. */
class Saturator
{
public:
    void setDrive (float normalizedDrive)
    {
        const auto clamped = std::clamp (normalizedDrive, 0.0f, 1.0f);

        // Called per sample so drive can follow the envelope, but it only actually moves while
        // Drive Env is non-zero and the envelope is ramping
        if (juce::exactlyEqual (clamped, drive))
            return;

        drive = clamped;

        // 0..1 -> 1..~20x pre-gain, exponential so the low end feels gentle.
        driveGain = std::exp (drive * 3.0f);
        normalization = std::tanh (driveGain);
    }

    float process (float input) const
    {
        if (juce::exactlyEqual (drive, 0.0f))
            return input;

        const auto saturated = std::tanh (input * driveGain) / normalization;
        return input + drive * (saturated - input);
    }

private:
    float drive = 0.0f;
    float driveGain = 1.0f;
    float normalization = std::tanh (1.0f);
};
