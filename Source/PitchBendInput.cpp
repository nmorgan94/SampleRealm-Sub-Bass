#include "PitchBendInput.h"

//==============================================================================
PitchBendInput::PitchBendInput (juce::RangedAudioParameter& bendParameter)
    : parameter (bendParameter)
{
    parameter.addListener (this);
}

PitchBendInput::~PitchBendInput()
{
    parameter.removeListener (this);
}

//==============================================================================
float PitchBendInput::handlePitchWheel (int wheelPosition)
{
    const auto normalisedBend = juce::MPEValue::from14BitInt (wheelPosition).asSignedFloat();

    midiPitchBend.store (normalisedBend, std::memory_order_relaxed);
    triggerAsyncUpdate();

    return normalisedBend;
}

std::optional<float> PitchBendInput::takeParameterChange()
{
    if (! parameterPitchBendChanged.exchange (false, std::memory_order_relaxed))
        return std::nullopt;

    return parameterPitchBend.load (std::memory_order_relaxed);
}

//==============================================================================
void PitchBendInput::handleAsyncUpdate()
{
    const auto normalisedBend = midiPitchBend.load (std::memory_order_relaxed);

    // The listener fires synchronously inside this call; the flag stops it sending a bend back to
    // the voice that MIDI has already applied, and may since have superseded.
    const juce::ScopedValueSetter<bool> echoing (isEchoingMidiPitchBend, true);
    parameter.setValueNotifyingHost (parameter.convertTo0to1 (normalisedBend));
}

void PitchBendInput::parameterValueChanged (int, float newNormalisedValue)
{
    // Checking the thread first means the flag is only ever read by the thread that writes it;
    // host automation can arrive here on any thread.
    if (juce::MessageManager::existsAndIsCurrentThread() && isEchoingMidiPitchBend)
        return;

    parameterPitchBend.store (parameter.convertFrom0to1 (newNormalisedValue), std::memory_order_relaxed);
    parameterPitchBendChanged.store (true, std::memory_order_relaxed);
}
