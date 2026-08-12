#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <algorithm>

//==============================================================================
AudioPluginAudioProcessor::AudioPluginAudioProcessor()
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
{
}

AudioPluginAudioProcessor::~AudioPluginAudioProcessor()
{
}

//==============================================================================
const juce::String AudioPluginAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool AudioPluginAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool AudioPluginAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool AudioPluginAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double AudioPluginAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int AudioPluginAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int AudioPluginAudioProcessor::getCurrentProgram()
{
    return 0;
}

void AudioPluginAudioProcessor::setCurrentProgram (int index)
{
    juce::ignoreUnused (index);
}

const juce::String AudioPluginAudioProcessor::getProgramName (int index)
{
    juce::ignoreUnused (index);
    return {};
}

void AudioPluginAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
    juce::ignoreUnused (index, newName);
}

//==============================================================================
void AudioPluginAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    juce::ignoreUnused (samplesPerBlock);
    voice.prepare (sampleRate);
}

void AudioPluginAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

bool AudioPluginAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}

void AudioPluginAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer,
                                              juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    buffer.clear();

    SubBassVoice::Params voiceParams;
    voiceParams.osc1Coarse = static_cast<int> (*apvts.getRawParameterValue (Parameters::osc1CoarseId.getParamID()));
    voiceParams.osc1Fine = *apvts.getRawParameterValue (Parameters::osc1FineId.getParamID());
    voiceParams.osc2Coarse = static_cast<int> (*apvts.getRawParameterValue (Parameters::osc2CoarseId.getParamID()));
    voiceParams.osc2Fine = *apvts.getRawParameterValue (Parameters::osc2FineId.getParamID());
    voiceParams.oscMix = *apvts.getRawParameterValue (Parameters::oscMixId.getParamID());
    voiceParams.attack = *apvts.getRawParameterValue (Parameters::envAttackId.getParamID());
    voiceParams.decay = *apvts.getRawParameterValue (Parameters::envDecayId.getParamID());
    voiceParams.sustain = *apvts.getRawParameterValue (Parameters::envSustainId.getParamID());
    voiceParams.release = *apvts.getRawParameterValue (Parameters::envReleaseId.getParamID());
    voiceParams.saturationDrive = *apvts.getRawParameterValue (Parameters::saturationDriveId.getParamID());
    voice.setParameters (voiceParams);

    int currentSample = 0;

    for (const auto metadata : midiMessages)
    {
        if (metadata.samplePosition > currentSample)
        {
            voice.renderNextBlock (buffer, currentSample, metadata.samplePosition - currentSample);
            currentSample = metadata.samplePosition;
        }

        handleMidiEvent (metadata.getMessage());
    }

    voice.renderNextBlock (buffer, currentSample, buffer.getNumSamples() - currentSample);

    const float gainDb = *apvts.getRawParameterValue (Parameters::masterGainId.getParamID());
    const float gainLinear = juce::Decibels::decibelsToGain (gainDb, -60.0f);

    for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
    {
        auto* channelData = buffer.getWritePointer (channel);
        juce::FloatVectorOperations::multiply (channelData, gainLinear,
                                               buffer.getNumSamples());
    }
}

void AudioPluginAudioProcessor::handleMidiEvent (const juce::MidiMessage& message)
{
    if (message.isNoteOn())
    {
        const auto note = message.getNoteNumber();
        heldNotes.erase (std::remove (heldNotes.begin(), heldNotes.end(), note), heldNotes.end());
        heldNotes.push_back (note);
        voice.noteOn (note, heldNotes.size() == 1);
    }
    else if (message.isNoteOff())
    {
        const auto note = message.getNoteNumber();
        heldNotes.erase (std::remove (heldNotes.begin(), heldNotes.end(), note), heldNotes.end());

        if (heldNotes.empty())
            voice.noteOff();
        else
            voice.noteOn (heldNotes.back(), false);
    }
}

//==============================================================================
bool AudioPluginAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* AudioPluginAudioProcessor::createEditor()
{
    return new AudioPluginAudioProcessorEditor (*this);
}

//==============================================================================
void AudioPluginAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    const auto state = apvts.copyState();
    const auto xml   = state.createXml();
    copyXmlToBinary (*xml, destData);
}

void AudioPluginAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    const auto xml = getXmlFromBinary (data, sizeInBytes);

    if (xml != nullptr && xml->hasTagName (apvts.state.getType()))
        apvts.replaceState (juce::ValueTree::fromXml (*xml));
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new AudioPluginAudioProcessor();
}
