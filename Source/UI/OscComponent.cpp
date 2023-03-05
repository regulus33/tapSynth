/*
  ==============================================================================

    OscComponent.cpp
    Created: 14 Feb 2021 6:51:39pm
    Author:  Joshua Hodge

  ==============================================================================
*/

#include <JuceHeader.h>
#include "OscComponent.h"

//==============================================================================

// We pass down these different IDS to we can control various data oscillators with various osc view components
OscComponent::OscComponent(juce::AudioProcessorValueTreeState& apvts,  juce::String waveSelectorId, juce::String fmFreqId, juce::String fmDepthId)
{
    juce::StringArray choices { "Sine", "Saw", "Square" };
    // populate the selector with choice values
    oscWaveSelector.addItemList(choices, 1);
    addAndMakeVisible(oscWaveSelector);
    
    // pass the audio processor value tree state reference and the parameter id
    oscWaveSelectorAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(apvts, waveSelectorId, oscWaveSelector);
    
    fmFreqSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    fmFreqSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 50, 25);
    addAndMakeVisible(fmFreqSlider);
    
    fmFreqAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, fmFreqId, fmFreqSlider);
}

OscComponent::~OscComponent()
{
}

void OscComponent::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colours::black);
}

void OscComponent::resized()
{
//    auto bounds = getLocalBounds();
    oscWaveSelector.setBounds (0, 0, 90, 20);
}

