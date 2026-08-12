#pragma once

#include <juce_audio_basics/juce_audio_basics.h>
#include "Oscillator.h"
#include "Saturator.h"

//==============================================================================
class SubBassVoice
{
public:
    struct Params
    {
        int osc1Coarse = 0, osc2Coarse = 0;
        float osc1Fine = 0.0f, osc2Fine = 0.0f;
        float oscMix = 0.5f;
        float attack = 0.005f, decay = 0.1f, sustain = 0.8f, release = 0.2f;
        float saturationDrive = 0.2f;
    };

    void prepare (double sampleRate);
    void setParameters (const Params& newParams);

    void noteOn (int midiNoteNumber, bool retrigger);
    void noteOff();
    bool isActive() const { return adsr.isActive(); }

    void renderNextBlock (juce::AudioBuffer<float>& buffer, int startSample, int numSamples);

private:
    void updateOscillatorFrequencies();

    SineOscillator osc1, osc2;
    juce::ADSR adsr;
    Saturator saturator;

    Params params;
    int currentMidiNote = 60;
};
