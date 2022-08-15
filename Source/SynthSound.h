/*
  ==============================================================================
    SynthSound.h
    Created: 10 Dec 2020 1:56:00pm
    Author:  Joshua Hodge
  ==============================================================================
*/

// resources
// hot reload with visual studio
// https://youtu.be/L7hL-vkJQUU

#pragma once

#include <JuceHeader.h>

class SynthSound : public juce::SynthesiserSound
{
public:
    bool appliesToNote (int midiNoteNumber) override { return true; }
    bool appliesToChannel (int midiChannel) override { return true; }
    
};
