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
    filter.reset();
    juce::dsp::ProcessSpec spec;
    spec.maximumBlockSize = samplesPerBlock;
    spec.sampleRate = sampleRate;
    spec.numChannels = numChannels;
    
    filter.prepare(spec);
    
    isPrepared = true;
}
// what's happening here is pretty dope, its just like each dsp class process function are all nested like Matroska dolls
// they each pass down to the next child, the audio buffer (nothing more than a instant in time of sound) and modify it with their own magic. Like effects processing chains but setup slightly differently. Smaller bits of music.
void FilterData::process(juce::AudioBuffer<float>& buffer)
{
    jassert(isPrepared);
    
    juce::dsp::AudioBlock<float> block { buffer };
    filter.process(juce::dsp::ProcessContextReplacing<float> {block});
}

void FilterData::updateParameters(const int filterType, const float frequency, const float resonance, const float modulator)
{
    switch (filterType) {
        case 0:
            filter.setType(juce::dsp::StateVariableTPTFilterType::lowpass);
            break;
        case 1:
            filter.setType(juce::dsp::StateVariableTPTFilterType::bandpass);
            break;
        case 2:
            filter.setType(juce::dsp::StateVariableTPTFilterType::highpass);
            break;
        default:
            break;
    }
    
    // what if we have a modulator that makes the product of this op less than 20hz (the lowest cutoff freq for the filter)
    // we use std::fmax or std::fmin
    // TODO this limitation should probably be inside a function on the filter class that is solely responsible for changing the freq.
    float modFreq = frequency * modulator;
    // stabilize modfreq between 20 and 20k hz
    modFreq = std::fmax(modFreq, 20.0f);
    modFreq = std::fmin(modFreq, 20000.0f);
    
    filter.setCutoffFrequency(modFreq);
    filter.setResonance(resonance);
}

void FilterData::reset()
{
    filter.reset();
}
