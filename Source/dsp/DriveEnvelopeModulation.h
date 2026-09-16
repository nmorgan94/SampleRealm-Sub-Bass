#pragma once

#include <algorithm>
#include <cmath>

//==============================================================================
/** Maps the amp envelope onto a drive multiplier for the Drive Env control.
    Shared with EnvelopeVisualizer so the drawn curve cannot drift from the audio.*/
inline float driveEnvelopeModulation (float env, float amount)
{
    const auto departure = std::max (0.0f, 1.0f - env);

    return amount >= 0.0f ? 1.0f - amount * departure
                          : 1.0f + amount * (1.0f - std::sqrt (departure));
}
