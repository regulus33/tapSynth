/*
  ==============================================================================

    FilterData.h
    Created: 11 Mar 2023 2:17:23pm
    Author:  zack

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

class FilterData
{
public:
    // whenever you have a dsp process you need a prepare to play where you pass your sample rate and buffersize to your algorithm
    void prepareToPlay  (double sampleRate, int samplesPerBlock, int numChannels);
    // what's happening here is pretty dope, its just like each dsp class process function are all nested like Matroska dolls
    // they each pass down to the next child, the audio buffer (nothing more than a instant in time of sound) and modify it with their own magic. Like effects processing chains but setup slightly differently. Smaller bits of music.
    void process(juce::AudioBuffer<float>& buffer);
    void updateParameters(const int filterType, const float frequency, const float resonance, const float modulator = 1.0f);
    void reset();
    
private:
    juce::dsp::StateVariableTPTFilter<float> filter;
    bool isPrepared  { false };
};
