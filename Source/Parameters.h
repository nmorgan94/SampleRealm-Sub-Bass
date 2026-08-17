#pragma once

#include <juce_audio_processors/juce_audio_processors.h>

//==============================================================================
namespace Parameters
{
    constexpr int versionHint = 1;

    inline const juce::ParameterID masterGainId { "masterGain", versionHint };

    inline const juce::ParameterID oscOctaveId { "oscOctave", versionHint };
    inline const juce::ParameterID osc1FineId { "osc1Fine", versionHint };
    inline const juce::ParameterID osc2FineId { "osc2Fine", versionHint };
    inline const juce::ParameterID oscMixId { "oscMix", versionHint };

    inline const juce::ParameterID envAttackId { "envAttack", versionHint };
    inline const juce::ParameterID envDecayId { "envDecay", versionHint };
    inline const juce::ParameterID envSustainId { "envSustain", versionHint };
    inline const juce::ParameterID envReleaseId { "envRelease", versionHint };

    inline const juce::ParameterID saturationDriveId { "saturationDrive", versionHint };

    juce::AudioProcessorValueTreeState::ParameterLayout createLayout();
}
