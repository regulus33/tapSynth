/*
 ==============================================================================
 
 OscData.h
 Created: 14 Feb 2021 6:51:17pm
 Author:  Zachary Rowden
 
 ==============================================================================
 */

#pragma once

#include <JuceHeader.h>

class OscData : public juce::dsp::Oscillator<float>
{
public:
    void setWaveType(const int choice);
    void prepareToPlay(juce::dsp::ProcessSpec& spec);
    void setWaveFrequency(const int midiNoteNumber);
    void getNextAudioBlock(juce::dsp::AudioBlock<float>& block);
    void setFmParams(const float depth, const float freq);
private:
    juce::dsp::Oscillator<float> fmOsc { [](float x) {return std::sin(x);}};
    float fmMod { 0.0f };
    float fmDepth { 0.0f };
    int lastMidiNote { 0 };
};

// return std::sin (x); //Sine Wave
// return x / MathConstants<float>::pi; // Saw Wave
// return x < 0.0f ? -1.0f : 1.0f;  // Square Wave
