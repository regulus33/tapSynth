/*
  ==============================================================================

    SynthSound.h
    Created: 2 Jul 2022 12:07:04pm
    Author:  zack

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

class SynthSound : public juce::SynthesiserSound
{
public:
    bool appliesToNote ( int midiNoteNumber) override { return true; }
    bool appliesToChannel ( int midiNoteNumber) override { return true; }
};
