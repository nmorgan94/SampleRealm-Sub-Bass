#include "SubBassVoice.h"

namespace
{
    // Wheel messages arrive in coarse steps, so the bend needs smoothing to avoid zippering.
    constexpr double bendSmoothingSeconds = 0.01;
}

//==============================================================================
void SubBassVoice::prepare (double sampleRate)
{
    osc1.prepare (sampleRate);
    osc2.prepare (sampleRate);
    adsr.setSampleRate (sampleRate);
    glide.prepare (sampleRate);
    bendNormalised.reset (sampleRate, bendSmoothingSeconds);
}

void SubBassVoice::setParameters (const Params& newParams)
{
    params = newParams;

    adsr.setParameters (params.envelope);
}

void SubBassVoice::noteOn (int midiNoteNumber, bool retrigger)
{
    glide.startNote (static_cast<float> (midiNoteNumber), ! retrigger, params.glideTime, params.glideMode);

    if (retrigger)
        adsr.noteOn();
}

void SubBassVoice::noteOff()
{
    adsr.noteOff();
}

void SubBassVoice::setPitchBend (float normalisedBend)
{
    bendNormalised.setTargetValue (normalisedBend);
}

void SubBassVoice::updateOscillatorFrequencies (float noteNumber)
{
    const auto baseNote = noteNumber + static_cast<float> (params.octave) * semitonesPerOctave
                                     + bendNormalised.getNextValue() * params.pitchBendRange;

    osc1.setFrequency (noteNumberToFrequencyHz (baseNote + params.osc1Fine / 100.0f));
    osc2.setFrequency (noteNumberToFrequencyHz (baseNote + params.osc2Fine / 100.0f));
}

void SubBassVoice::renderNextBlock (juce::AudioBuffer<float>& buffer, int startSample, int numSamples)
{
    if (! adsr.isActive())
    {
        // Time still passes while silent, so a bend moved between notes has settled by the next one.
        bendNormalised.skip (numSamples);
        return;
    }

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
