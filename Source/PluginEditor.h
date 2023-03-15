/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "UI/AdsrComponent.h"
#include "UI/OscComponent.h"
#include "UI/FilterComponent.h"

//==============================================================================
/**
*/
class TapSynthAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    TapSynthAudioProcessorEditor (TapSynthAudioProcessor&);
    ~TapSynthAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    
    // TODO: programatically select first device
//    static void myMidiInputCallback(const juce::MidiMessage& message, juce::MidiInput* source)
//    {
//        std::cout << "Received MIDI message: " << message.getDescription() << std::endl;
//    }


private:
    
    TapSynthAudioProcessor& audioProcessor;
    OscComponent osc;
    AdsrComponent adsr;
    FilterComponent filter;
    AdsrComponent modAdsr;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TapSynthAudioProcessorEditor)
};
