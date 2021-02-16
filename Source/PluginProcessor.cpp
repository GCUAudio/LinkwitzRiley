/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
LinkwitzRileyAudioProcessor::LinkwitzRileyAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ), treeState(*this, nullptr, juce::Identifier("PARAMETERS"),
                           { std::make_unique<juce::AudioParameterFloat>("cutoff", "Cutoff", 20.0f, 20000.0f, 200.0f) })
#endif
{
    treeState.addParameterListener("cutoff", this);
}

LinkwitzRileyAudioProcessor::~LinkwitzRileyAudioProcessor()
{
}

//==============================================================================
const juce::String LinkwitzRileyAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool LinkwitzRileyAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool LinkwitzRileyAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool LinkwitzRileyAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double LinkwitzRileyAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int LinkwitzRileyAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int LinkwitzRileyAudioProcessor::getCurrentProgram()
{
    return 0;
}

void LinkwitzRileyAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String LinkwitzRileyAudioProcessor::getProgramName (int index)
{
    return {};
}

void LinkwitzRileyAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void LinkwitzRileyAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    juce::dsp::ProcessSpec spec;

    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = getTotalNumOutputChannels();

    lwrFilter.reset();
    lwrFilter.prepare(spec);
}

void LinkwitzRileyAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool LinkwitzRileyAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
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
#endif

void LinkwitzRileyAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer(channel);

        for (int i = 0; i < buffer.getNumSamples(); i++)
        {
            float in = channelData[i];
            float outLow = 0.0f;
            float outHigh = 0.0f;

            lwrFilter.processSample(channel, in, outLow, outHigh);
            channelData[i] = outLow + outHigh;
        }   
    }
}

//==============================================================================
bool LinkwitzRileyAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* LinkwitzRileyAudioProcessor::createEditor()
{
    return new LinkwitzRileyAudioProcessorEditor (*this, treeState);
}

//==============================================================================
void LinkwitzRileyAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void LinkwitzRileyAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new LinkwitzRileyAudioProcessor();
}

// Function called when parameter is changed
void LinkwitzRileyAudioProcessor::parameterChanged(const juce::String& parameterID, float newValue)
{
    if (parameterID == "cutoff")
        lwrFilter.setCutoffFrequency(newValue);

}
