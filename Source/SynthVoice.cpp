/*
  ==============================================================================

    SynthVoice.cpp
    Created: 2 Jul 2022 11:49:39am
    Author:  zack

  ==============================================================================
*/

#include "SynthVoice.h"

bool SynthVoice::canPlaySound (juce::SynthesiserSound* sound) {
    return dynamic_cast<juce::SynthesiserSound*>(sound) != nullptr;
};

void SynthVoice::startNote (int midiNoteNumber, float velocity, juce::SynthesiserSound *sound, int currentPitchWheelPosition) {
    
};

void SynthVoice::stopNote (float velocity, bool allowTailOff) {
    
};

void SynthVoice::pitchWheelMoved (int newPitchWheelValue) {
    
};

void SynthVoice::controllerMoved (int controllerNumber, int newControllerValue) {
    
};

void SynthVoice::renderNextBlock (juce::AudioBuffer<float> &outputBuffer, int startSample, int numSamples) {
    
};

