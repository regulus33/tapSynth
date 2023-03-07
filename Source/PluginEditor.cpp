/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
// NOTE: this is an initializer list, its just a way of setting values for instance variables
// See README for more info

// NOTE: osc (audioProcessor.apvts) is a shorthand for initializing the OscComponent()
TapSynthAudioProcessorEditor::TapSynthAudioProcessorEditor (TapSynthAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p), osc (audioProcessor.apvts, "OSC1WAVETYPE", "OSC1FMFREQ", "OSC1FMDEPTH"), adsr (audioProcessor.apvts)
{
    // TODO: programatically select first device
//    auto deviceList = juce::MidiInput::getAvailableDevices();
//    auto deviceName& = deviceList[0]; // select the first device in the list
//    auto midiInput = juce::MidiInput::openDevice(deviceList.indexOf(deviceName), myMidiInputCallback);


    setSize (400, 300);
    addAndMakeVisible (osc);
    addAndMakeVisible (adsr);
}

TapSynthAudioProcessorEditor::~TapSynthAudioProcessorEditor()
{
}

//==============================================================================
void TapSynthAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colours::black);
//    g.setColour(juce::Colours::white);
//    g.drawRect(getLocalBounds());
  
}

void TapSynthAudioProcessorEditor::resized()
{
    osc.setBounds(10, 10, 180, 200);
    adsr.setBounds (getWidth() / 2, 0, getWidth() / 2, getHeight());
}

