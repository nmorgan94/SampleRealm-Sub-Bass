#include "SubBassVoice.h"

//==============================================================================
void SubBassVoice::prepare (double sampleRate)
{
    osc1.prepare (sampleRate);
    osc2.prepare (sampleRate);
    adsr.setSampleRate (sampleRate);
    glide.prepare (sampleRate);
}

void SubBassVoice::setParameters (const Params& newParams)
{
    params = newParams;

    juce::ADSR::Parameters adsrParams;
    adsrParams.attack = params.attack;
    adsrParams.decay = params.decay;
    adsrParams.sustain = params.sustain;
    adsrParams.release = params.release;
    adsr.setParameters (adsrParams);
    glide.setGlideTime (params.glideTime);
}

void SubBassVoice::noteOn (int midiNoteNumber, bool retrigger)
{
    glide.startNote (static_cast<float> (midiNoteNumber), ! retrigger);

    if (retrigger)
        adsr.noteOn();
}

void SubBassVoice::noteOff()
{
    adsr.noteOff();
}

void SubBassVoice::updateOscillatorFrequencies (float noteNumber)
{
    const auto baseNote = noteNumber + static_cast<float> (params.octave * 12);

    osc1.setFrequency (noteNumberToFrequencyHz (baseNote + params.osc1Fine / 100.0f));
    osc2.setFrequency (noteNumberToFrequencyHz (baseNote + params.osc2Fine / 100.0f));
}

void SubBassVoice::renderNextBlock (juce::AudioBuffer<float>& buffer, int startSample, int numSamples)
{
    if (! adsr.isActive())
        return;

    for (int i = 0; i < numSamples; ++i)
    {
        updateOscillatorFrequencies (glide.getNextNoteNumber());

        const auto osc1Sample = osc1.renderSample();
        const auto osc2Sample = osc2.renderSample();
        const auto mixed = osc1Sample * (1.0f - params.oscMix) + osc2Sample * params.oscMix;
        const auto env = adsr.getNextSample();

        saturator.setDrive (params.saturationDrive
                            * driveEnvelopeModulation (env, params.saturationDriveEnv));
        
                            const auto shaped = saturator.process (mixed) * env;

        for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
            buffer.addSample (channel, startSample + i, shaped);
    }
}
