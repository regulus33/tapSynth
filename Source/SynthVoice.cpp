/*
 ==============================================================================
 
 SynthVoice.cpp
 Created: 10 Dec 2020 1:55:41pm
 Author:  Zachary Rowden
 
 ==============================================================================
 */

#include "SynthVoice.h"


bool SynthVoice::canPlaySound (juce::SynthesiserSound* sound)
{
    return dynamic_cast<juce::SynthesiserSound*>(sound) != nullptr;
}

// THIS IS IMPORTATN basically midiNote -> 'on' callback
// * We set the oscillator frequency of current midi note
// * We trigger the adsr envelopes
// * ETC ETC
void SynthVoice::startNote (int midiNoteNumber, float velocity, juce::SynthesiserSound *sound, int currentPitchWheelPosition)
{
    // this is
    osc.setWaveFrequency(midiNoteNumber);
    adsr.noteOn();
    modAdsr.noteOn();
}

// Same as above although reversed.
// As you can see we need to notify the envelope when the noteOff event hits so it can move from the SUSTAIN
// to the RELEASE phase
void SynthVoice::stopNote (float velocity, bool allowTailOff)
{
    adsr.noteOff();
    modAdsr.noteOff();
    
    if (! allowTailOff || ! adsr.isActive())
        clearCurrentNote();
}

void SynthVoice::controllerMoved (int controllerNumber, int newControllerValue)
{
    
}

void SynthVoice::pitchWheelMoved (int newPitchWheelValue)
{
    
}

// Similar to the prepare to play in plugin processor. Its going to be called whenever sample rates change or when plugin starts up
void SynthVoice::prepareToPlay (double sampleRate, int samplesPerBlock, int outputChannels)
{
    juce::dsp::ProcessSpec spec;
    spec.maximumBlockSize = samplesPerBlock;
    spec.sampleRate = sampleRate;
    spec.numChannels = outputChannels;
    
    
    osc.prepareToPlay(spec);
    adsr.setSampleRate(sampleRate);
    filter.prepareToPlay(sampleRate, samplesPerBlock, outputChannels);
    modAdsr.setSampleRate(sampleRate);
    gain.prepare (spec);
    
    gain.setGainLinear (0.3f);
    
    isPrepared = true;
}

void SynthVoice::updateAdsr (const float attack, const float decay, const float sustain, const float release)
{
    adsr.updateADSR (attack, decay, sustain, release);
}

void SynthVoice::updateModAdsr (const float attack, const float decay, const float sustain, const float release)
{
    modAdsr.updateADSR (attack, decay, sustain, release);
}

void SynthVoice::renderNextBlock (juce::AudioBuffer< float > &outputBuffer, int startSample, int numSamples)
{
    jassert (isPrepared);
    
    if (! isVoiceActive())
        return;
    
    // There is nothing in this buffer yet, we are going to add samples to it shortly
    synthBuffer.setSize (outputBuffer.getNumChannels(), numSamples, false, false, true);
    // TODO: why are we HARDCODING 0 here and in other call to applyEnvelope what is the value of startSample. Is it 0?
    // NOTE: according to Josh this applyEnvelope to buffer doesn't actually do anything to the buffer here. It seems that it
    // just assigns the envelope to this buffer. But I am skeptical
    // TODO: we might be able to just apply envelope to filter directly using `applyTo`
    modAdsr.applyEnvelopeToBuffer(synthBuffer, 0, numSamples);
    synthBuffer.clear();
    
    juce::dsp::AudioBlock<float> audioBlock { synthBuffer };
    osc.getNextAudioBlock(audioBlock);
    adsr.applyEnvelopeToBuffer (synthBuffer, 0, synthBuffer.getNumSamples());
    filter.process(synthBuffer);
    gain.process (juce::dsp::ProcessContextReplacing<float> (audioBlock));
    
    
    for (int channel = 0; channel < outputBuffer.getNumChannels(); ++channel)
    {
        outputBuffer.addFrom (channel, startSample, synthBuffer, channel, 0, numSamples);
        
        if (! adsr.isActive())
            clearCurrentNote();
    }
    
    
}

void SynthVoice::updateFilter(const int filterType, const float cutoff, const float resonance)
{
    float modulator = modAdsr.getNextSample();
    filter.updateParameters(filterType, cutoff, resonance, modulator);
}
