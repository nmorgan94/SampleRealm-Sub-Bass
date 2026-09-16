#pragma once

#include <algorithm>
#include <cmath>

//==============================================================================
/** Bends a 0..1 progress value. Positive tension moves quickly at first and eases into
    the end, negative starts slowly and accelerates, and zero leaves it linear.
*/
inline float shapeProgress (float progress, float tension)
{
    constexpr float minimumTension = 0.001f;
    constexpr float maximumSteepness = 20.0f;

    if (progress <= 0.0f || progress >= 1.0f || std::abs (tension) < minimumTension)
        return std::clamp (progress, 0.0f, 1.0f);

    const auto steepness = 1.0f + std::abs (tension) * (maximumSteepness - 1.0f);

    return tension > 0.0f ? std::log (1.0f + progress * (steepness - 1.0f)) / std::log (steepness)
                          : (std::pow (steepness, progress) - 1.0f) / (steepness - 1.0f);
}
