/*
  ==============================================================================

    OscData.cpp
    Created: 14 Feb 2021 6:51:17pm
    Author:  Zachary Rowden

  ==============================================================================
*/

#include "OscData.h"

void OscData::prepareToPlay(juce::dsp::ProcessSpec& spec)
{
    prepare(spec);
}

void OscData::setWaveType(const int choice)
{
    // return std::sin (x); //Sine Wave
    // return x / MathConstants<float>::pi; // Saw Wave
    // return x < 0.0f ? -1.0f : 1.0f;  // Square Wave
    switch (choice) {
        case 0:
            // Sine
            initialise([](float x) { return std::sin (x); });
            break;
        case 1:
            // Saw
            initialise([](float x) { return x / juce::MathConstants<float>::pi; });
            break;
        case 2:
            // Square
            initialise([](float x) {return x < 0.0f ? -1.0 : 1.0f;});
            break;
            
        default:
            jassertfalse;
            break;
    }
}

void OscData::setWaveFrequency(const int midiNoteNumber)
{
    setFrequency(juce::MidiMessage::getMidiNoteInHertz (midiNoteNumber));
}


void OscData::getNextAudioBlock(juce::dsp::AudioBlock<float>& block)
{
    process (juce::dsp::ProcessContextReplacing<float> (block));
}
