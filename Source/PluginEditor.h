/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class LinkwitzRileyAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    LinkwitzRileyAudioProcessorEditor (LinkwitzRileyAudioProcessor&, juce::AudioProcessorValueTreeState&);
    ~LinkwitzRileyAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    LinkwitzRileyAudioProcessor& audioProcessor;
    juce::AudioProcessorValueTreeState& treeState;

    juce::Slider cutoffDial;
    std::unique_ptr <juce::AudioProcessorValueTreeState::SliderAttachment> cutoffValue;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LinkwitzRileyAudioProcessorEditor)
};
