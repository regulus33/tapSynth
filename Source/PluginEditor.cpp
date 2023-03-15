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
: AudioProcessorEditor (&p), audioProcessor (p)
, osc (audioProcessor.apvts, "OSC1WAVETYPE", "OSC1FMFREQ", "OSC1FMDEPTH")
, adsr ("Amp Envelope", audioProcessor.apvts, "ATTACK", "DECAY", "SUSTAIN", "RELEASE")
, filter (audioProcessor.apvts, "FILTERTYPE", "FILTERFREQ", "FILTERRES")
, modAdsr ("Mod Envelope", audioProcessor.apvts, "MODATTACK", "MODDECAY", "MODSUSTAIN", "MODRELEASE")


{
    setSize (620, 500);
    addAndMakeVisible(osc);
    addAndMakeVisible(adsr);
    addAndMakeVisible(filter);
    addAndMakeVisible(modAdsr);
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
    const auto paddingX = 5;
    const auto paddingY = 35;
    const auto paddingY2 = 235;
    const auto width = 300;
    const auto height = 200;
    
    // Sets the bounds of adsr and fm sections so they are in top left and right quadrants
    osc.setBounds(paddingX, paddingY, width, height);
    adsr.setBounds(osc.getRight(), paddingY, width, height);
    filter.setBounds(paddingX, paddingY2, width, height);
    modAdsr.setBounds(filter.getRight(), adsr.getBottom(), width, height);
}

