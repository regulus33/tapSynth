/*
 ==============================================================================
 
 FilterData.cpp
 Created: 11 Mar 2023 2:17:23pm
 Author:  zack
 
 ==============================================================================
 */

#include "FilterData.h"
// whenever you have a dsp process you need a prepare to play where you pass your sample rate and buffersize to your algorithm
void FilterData::prepareToPlay (double sampleRate, int samplesPerBlock, int numChannels)
{
    juce::dsp::ProcessSpec spec;
    spec.maximumBlockSize = samplesPerBlock;
    spec.sampleRate = sampleRate;
    spec.numChannels = numChannels;
    
    filter.prepare(spec);
}
// what's happening here is pretty dope, its just like each dsp class process function are all nested like Matroska dolls
// they each pass down to the next child, the audio buffer (nothing more than a instant in time of sound) and modify it with their own magic. Like effects processing chains but setup slightly differently. Smaller bits of music.
void FilterData::process(juce::AudioBuffer<float>& buffer)
{
    juce::dsp::AudioBlock<float> block { buffer };
    filter.process(juce::dsp::ProcessContextReplacing<float> {block});
}
void FilterData::updateParameters(const int filterType, const float frequency, const float resonance)
{
    
}
void FilterData::reset()
{
    
}
