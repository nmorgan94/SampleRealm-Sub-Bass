#pragma once

#include <algorithm>
#include <cmath>

//==============================================================================
/** Simple tanh soft-clip saturator, normalized to stay near unity gain as drive increases. */
class Saturator
{
public:
    void setDrive (float normalizedDrive)
    {
        // 0..1 -> 1..~20x pre-gain, exponential so the low end feels gentle.
        driveGain = std::exp (std::clamp (normalizedDrive, 0.0f, 1.0f) * 3.0f);
        normalization = std::tanh (driveGain);
    }

    float process (float input) const
    {
        return std::tanh (input * driveGain) / normalization;
    }

private:
    float driveGain = 1.0f;
    float normalization = std::tanh (1.0f);
};
