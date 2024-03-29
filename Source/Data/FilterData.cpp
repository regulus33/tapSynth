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
    
    float modFreq = frequency * modulator;
    // TODO IF modfreq is always between 0.1 and 1.0 then its not possible for the product to exceed 20000.0f
    // You probably only need to do std::fmax(modFreq, 20.0f)
    modFreq = std::fmin(std::fmax(modFreq, 20.0f), 20000.0f);
    
    filter.setCutoffFrequency(modFreq);
    filter.setResonance(resonance);
}

void FilterData::reset()
{
    filter.reset();
}
