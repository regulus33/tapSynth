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
    fmOsc.prepare(spec);
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
    setFrequency(juce::MidiMessage::getMidiNoteInHertz (midiNoteNumber) + fmMod);
    lastMidiNote = midiNoteNumber;
}


void OscData::getNextAudioBlock(juce::dsp::AudioBlock<float>& block)
{
    // To do the FM mod we need to get the value of the modulated frequency on a sample by sample basis
    // rather than just applying the thing to the whole block.
    
    // block is a vector of audio info
    for(int ch = 0; ch < block.getNumChannels(); ++ch) {
        for (int s = 0; s < block.getNumSamples(); ++s) {
            // this is the value of the sample at a given point in time
            // fmMod is really just the value of the oscillator's oscillation at any given point in time
            // then we attenuate it by simply multiplying it by fmDepth;n
            fmMod = fmOsc.processSample(block.getSample(ch, s)) * fmDepth;
              
        }
    }
    process (juce::dsp::ProcessContextReplacing<float> (block));
}

void OscData::setFmParams(const float depth, const float freq)
{
    fmOsc.setFrequency(freq);
    fmDepth = depth;
    setFrequency(juce::MidiMessage::getMidiNoteInHertz(lastMidiNote) + fmMod);
}
