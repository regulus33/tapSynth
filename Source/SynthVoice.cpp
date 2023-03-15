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

void SynthVoice::startNote (int midiNoteNumber, float velocity, juce::SynthesiserSound *sound, int currentPitchWheelPosition)
{
    osc.setWaveFrequency(midiNoteNumber);
    adsr.noteOn();
}

void SynthVoice::stopNote (float velocity, bool allowTailOff)
{
    adsr.noteOff();
    
    if (! allowTailOff || ! adsr.isActive())
        clearCurrentNote();
}

void SynthVoice::controllerMoved (int controllerNumber, int newControllerValue)
{
    
}

void SynthVoice::pitchWheelMoved (int newPitchWheelValue)
{
    
}

void SynthVoice::prepareToPlay (double sampleRate, int samplesPerBlock, int outputChannels)
{
    juce::dsp::ProcessSpec spec;
    spec.maximumBlockSize = samplesPerBlock;
    spec.sampleRate = sampleRate;
    spec.numChannels = outputChannels;
    
    osc.prepareToPlay(spec);
    adsr.setSampleRate (sampleRate);
    filter.prepareToPlay(sampleRate, samplesPerBlock, outputChannels);
    modAdsr.setSampleRate(sampleRate);
    gain.prepare (spec);
    
    gain.setGainLinear (0.3f);
    
    isPrepared = true;
}

void SynthVoice::updateAdsr (const float attack, const float decay, const float sustain, const float release)
{
    adsr.updateADSR(attack, decay, sustain, release);
}

void SynthVoice::renderNextBlock (juce::AudioBuffer< float > &outputBuffer, int startSample, int numSamples)
{
    jassert (isPrepared);
    
    if (! isVoiceActive())
        return;
    
    // We actually clear the buffer
    synthBuffer.setSize (outputBuffer.getNumChannels(), numSamples, false, false, true);
    modAdsr.applyEnvelopeToBuffer(synthBuffer, 0, numSamples);
    synthBuffer.clear();
    
    juce::dsp::AudioBlock<float> audioBlock { synthBuffer };
    osc.getNextAudioBlock(audioBlock);
    // TODO why synthBuffer.getNumSamples() here and no numSamples????
    adsr.applyEnvelopeToBuffer(synthBuffer, 0, synthBuffer.getNumSamples());
    filter.process(synthBuffer);
    gain.process (juce::dsp::ProcessContextReplacing<float> (audioBlock));
    
    // ****** ON ENVELOPES *******
    // The ADSR envelope is typically applied to an amplitude parameter, such as the gain or amplitude of an oscillator, to shape the overall volume contour of a sound. However, it's also possible to apply the ADSR envelope to other parameters, such as filter cutoff frequency or resonance, to shape the filter's frequency response over time.
    
    // In JUCE, the ADSR class provides a general-purpose envelope generator that can be used to shape any parameter that requires a time-varying value. The ADSR class provides several functions that can be used to apply the envelope to different parameters or objects:
    
    // applyEnvelopeToBuffer() (used in the line just below) applies the envelope to an audio buffer, typically representing the amplitude of a sound.
    
    // getNextSample() generates the next sample of the envelope waveform, which can be used to directly control a parameter such as a filter cutoff frequency.

    // applyTo() applies the envelope to any parameter that can be interpolated between two values, such as a filter cutoff frequency.

    adsr.applyEnvelopeToBuffer (synthBuffer, 0, synthBuffer.getNumSamples());
    
    for (int channel = 0; channel < outputBuffer.getNumChannels(); ++channel)
    {
        outputBuffer.addFrom (channel, startSample, synthBuffer, channel, 0, numSamples);
        
        if (! adsr.isActive())
            clearCurrentNote();
    }
    
    // This is how we get the mod value, TODO
//    modAdsr.getNextSample()
//    filter.updateParameters(<#const int filterType#>, <#const float frequency#>, <#const float resonance#>, <#const float modulator#>)
    
}

void SynthVoice::updateFilter(const int filterType, const float cutoff, const float resonance)
{
    float modulator = modAdsr.getNextSample();
    filter.updateParameters(filterType, cutoff, resonance, modulator);
}

void SynthVoice::updateModAdsr(const float attack, const float decay, const float sustain, const float release)
{
    modAdsr.updateADSR(attack,decay,sustain,release);
    
}
