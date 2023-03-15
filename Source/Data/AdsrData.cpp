/*
  ==============================================================================

    AdsrData.cpp
    Created: 7 Feb 2021 2:29:21pm
    Author:  Zachary Rowden

  ==============================================================================
*/

#include "AdsrData.h"

// Function generator that produces a number between 0.1 and 1.0 depending on the phase of the ADSR cycle the current sample is in
// It seems like most modulation sources are just scalars (like I know what that means :P)
void AdsrData::updateADSR(const float attack, const float decay, const float sustain, const float release)
{
    adsrParams.attack = attack;
    adsrParams.decay = decay;
    adsrParams.sustain = sustain;
    adsrParams.release = release;
    
    setParameters (adsrParams);
}
