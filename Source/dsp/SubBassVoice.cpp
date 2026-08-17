#include "SubBassVoice.h"

namespace
{
    float noteToFrequency (float midiNote)
    {
        return static_cast<float> (440.0 * std::pow (2.0, (static_cast<double> (midiNote) - 69.0) / 12.0));
    }
}

//==============================================================================
void SubBassVoice::prepare (double sampleRate)
{
    osc1.prepare (sampleRate);
    osc2.prepare (sampleRate);
    adsr.setSampleRate (sampleRate);
    updateOscillatorFrequencies();
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

    saturator.setDrive (params.saturationDrive);

    updateOscillatorFrequencies();
}

void SubBassVoice::noteOn (int midiNoteNumber, bool retrigger)
{
    currentMidiNote = midiNoteNumber;
    updateOscillatorFrequencies();

    if (retrigger)
        adsr.noteOn();
}

void SubBassVoice::noteOff()
{
    adsr.noteOff();
}

void SubBassVoice::updateOscillatorFrequencies()
{
    const auto baseNote = static_cast<float> (currentMidiNote + params.octave * 12);

    osc1.setFrequency (noteToFrequency (baseNote + params.osc1Fine / 100.0f));
    osc2.setFrequency (noteToFrequency (baseNote + params.osc2Fine / 100.0f));
}

void SubBassVoice::renderNextBlock (juce::AudioBuffer<float>& buffer, int startSample, int numSamples)
{
    if (! adsr.isActive())
        return;

    for (int i = 0; i < numSamples; ++i)
    {
        const auto osc1Sample = osc1.renderSample();
        const auto osc2Sample = osc2.renderSample();
        const auto mixed = osc1Sample * (1.0f - params.oscMix) + osc2Sample * params.oscMix;
        const auto shaped = saturator.process (mixed * adsr.getNextSample());

        for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
            buffer.addSample (channel, startSample + i, shaped);
    }
}
