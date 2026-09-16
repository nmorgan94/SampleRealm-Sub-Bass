#pragma once

#include <juce_audio_basics/juce_audio_basics.h>
#include "CurvedADSR.h"
#include "DriveEnvelopeModulation.h"
#include "Glide.h"
#include "Oscillator.h"
#include "Saturator.h"

//==============================================================================
class SubBassVoice
{
public:
    struct Params
    {
        int octave = 0;
        float osc1Fine = 0.0f, osc2Fine = 0.0f;
        float oscMix = 0.5f;
        CurvedADSR::Parameters envelope;
        float saturationDrive = 0.2f;
        float saturationDriveEnv = 0.0f;
        float glideTime = 0.0f;
        GlideMode glideMode = GlideMode::fixedRate;
        float pitchBendRange = 2.0f;
    };

    void prepare (double sampleRate);
    void setParameters (const Params& newParams);

    void noteOn (int midiNoteNumber, bool retrigger);
    void noteOff();
    void setPitchBend (float normalisedBend);
    bool isActive() const { return adsr.isActive(); }

    void renderNextBlock (juce::AudioBuffer<float>& buffer, int startSample, int numSamples);

private:
    void updateOscillatorFrequencies (float noteNumber);

    SineOscillator osc1, osc2;
    CurvedADSR adsr;
    Saturator saturator;
    GlideProcessor glide;
    juce::SmoothedValue<float> bendNormalised;

    Params params;
};
