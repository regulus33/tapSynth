/*
 ==============================================================================
 
 This file contains the basic framework code for a JUCE plugin processor.
 
 ==============================================================================
 */

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
TapSynthAudioProcessor::TapSynthAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
: AudioProcessor (BusesProperties()
#if ! JucePlugin_IsMidiEffect
#if ! JucePlugin_IsSynth
                  .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
#endif
                  .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
#endif
                  ), apvts (*this, nullptr, "Parameters", createParams())
#endif
{
    synth.addSound (new SynthSound());
    synth.addVoice (new SynthVoice());
}

TapSynthAudioProcessor::~TapSynthAudioProcessor()
{
    
}

//==============================================================================
// We should change this at some point the plugin name I mean
const juce::String TapSynthAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

// of course
bool TapSynthAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

// does it send midi out? (like in a midi effect)
bool TapSynthAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

// its not (I believe this is just for meta data purposes, should not impact functionality)
bool TapSynthAudioProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
    return true;
#else
    return false;
#endif
}

// If you have like a delay effect or even a synth with a long decay time, the host application would like
// to faid you out when the track is bypassed or muted for some reason.
// This kind of echoey spacial frozen sound you hear at the end of a reverberated sound is called the "tail"
// I am not sure how they recommend we use it but I imagine just a query to the buffer length would be enough.
double TapSynthAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

// num programs is basically numPatches or numPresets
int TapSynthAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
    // so this should be at least 1, even if you're not really implementing programs.
}

// get the default program OR retrieve from db if we want to re-open one
int TapSynthAudioProcessor::getCurrentProgram()
{
    return 0;
}

// This is here because a host app might have some preset management system. Perhaps you can automate preset iteration
// so when we start managing presets we need to make sure to use these public overrde thingies so we stay in sync with host
void TapSynthAudioProcessor::setCurrentProgram (int index)
{
    // set params on synth(presumably the presets are retrieved from some db?)
}

// returns the program display name again probably important to use so daw can access
const juce::String TapSynthAudioProcessor::getProgramName (int index)
{
    return {};
}

// ditto
void TapSynthAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

// Very important. Its called on DAW start and any time the user changes their
// buffer size or sample rate.
void TapSynthAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
//    Tells the synthesiser what the sample rate is for the audio it's being used to render.
//    This value is propagated to the voices so that they can use it to render the correct pitches.
    synth.setCurrentPlaybackSampleRate (sampleRate);
    
    // for each voice (by default there seems to be just 1)
    for (int i = 0; i < synth.getNumVoices(); i++)
    {
        // call prepare to play on each voice
        if (auto voice = dynamic_cast<SynthVoice*>(synth.getVoice(i)))
        {
            voice->prepareToPlay (sampleRate, samplesPerBlock, getTotalNumOutputChannels());
        }
    }
}

// Clear buffers etc.
void TapSynthAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

// thisn buses layout thing is some kind of way for the host to configure the audio routing
// it tells the
#ifndef JucePlugin_PreferredChannelConfigurations
bool TapSynthAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
#if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
#else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
        && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;
    
    // This checks if the input layout matches the output layout
#if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
#endif
    
    return true;
#endif
}
#endif

// Main dsp function
void TapSynthAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    // there are cases where the audio buffer SHOULD* have 0s but you end up with something closer to 0.0000555555555555555555
    // in these cases the processBlock is just going to round those near 0 numbers down to 0
    juce::ScopedNoDenormals noDenormals;
    
    // self explanatory
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
    
    // iterate through input channels (like in case of effect it would likely be 2)
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        // we are clearing the buffer here at channel i from sample index 0 to end of buffer.
        // The audio buffer SHOULD be clear but we clear it anyway. We need to think of this buffer as something
        // allocated to us by the host. In a synth its just full of zeros bc we are generating sound not effecting it.
        // NOTE: we DO depend on 2 values always to render the result of a function such as a sin() osc over time.
        // We do this by rendering the next block in the current block - see SynthVoice for more info about blocks
        // But we use the sample at current index to create the sample that will appear in the next index.
        buffer.clear (i, 0, buffer.getNumSamples());
    
    // In this synth FOR NOW* we just have 1 voice, but this would run for each note in a chord or some other case where you dup voices.
    for (int i = 0; i < synth.getNumVoices(); ++i)
    {
        // Check if the synth voice is a juce SynthVoice
        if (auto voice = dynamic_cast<SynthVoice*>(synth.getVoice(i)))
        {
            // go into the Audio Processor Value Tree state and retrieve all parameters that were set in the UI
            
            // osc
            auto& oscWaveChoice = *apvts.getRawParameterValue ("OSC1WAVETYPE");
            auto& fmDepth = *apvts.getRawParameterValue("OSC1FMDEPTH");
            auto& fmFreq = *apvts.getRawParameterValue("OSC1FMFREQ");
            
            // amp adsr
            auto& attack = *apvts.getRawParameterValue ("ATTACK");
            auto& decay = *apvts.getRawParameterValue ("DECAY");
            auto& sustain = *apvts.getRawParameterValue ("SUSTAIN");
            auto& release = *apvts.getRawParameterValue ("RELEASE");
            
            // filter
            auto& filterType = *apvts.getRawParameterValue("FILTERTYPE");
            auto& cutoff = *apvts.getRawParameterValue("FILTERFREQ");
            auto& resonance = *apvts.getRawParameterValue("FILTERRES");
            
            // mod env
            auto& modAttack = *apvts.getRawParameterValue ("MODATTACK");
            auto& modDecay = *apvts.getRawParameterValue ("MODDECAY");
            auto& modSustain = *apvts.getRawParameterValue ("MODSUSTAIN");
            auto& modRelease = *apvts.getRawParameterValue ("MODRELEASE");
            
            // # 1. we tell the voices what params of various data have changed to
            voice->getOscillator().setWaveType(oscWaveChoice);
            voice->getOscillator().setFmParams(fmDepth, fmFreq);
            voice->updateAdsr(attack.load(), decay.load(), sustain.load(), release.load());
            voice->updateFilter(filterType.load(), cutoff.load(), resonance.load());
            voice->updateModAdsr(modAttack, modDecay, modSustain, modRelease);
        }
    }
    
   
    // render next block DOES NOT mean its processing something in the future as far as I can tell.
    // the way we inform the next bit of audio in cals to sin() or in an oscillator we do with internal tracking
    // on the objects themselves. Like the juce::Oscillator which uses a phase accumulator. Its basically a pointer
    // moving through a lookup table of precomputed values (not sure if this is always the case) and it moves at a rate that is
    // proportional to the frequency of the oscillator.
    
    // # 2. we tell the synth->voices to start rendering the the block after params have already been saved up in voice->updateAdsr() and related calls above
    synth.renderNextBlock (buffer, midiMessages, 0, buffer.getNumSamples());
}

//==============================================================================
bool TapSynthAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* TapSynthAudioProcessor::createEditor()
{
    return new TapSynthAudioProcessorEditor (*this);
}

//==============================================================================
void TapSynthAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void TapSynthAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new TapSynthAudioProcessor();
}


// The only purpose of this function is to create a vector of pointers
// that declare the paramter objects we will be using to shape the sound
// typically called during plugin initialization to create the necessary AudioParameter objects
// *** YOU NEED TO DECLARE ALL PARAMETERS USED IN THE PLUGIN UP HERE ***
juce::AudioProcessorValueTreeState::ParameterLayout TapSynthAudioProcessor::createParams()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;
    
    // OSC select
    params.push_back(std::make_unique<juce::AudioParameterChoice>(juce::ParameterID({"OSC1WAVETYPE", 1}), "Osc 1 Wave Type", juce::StringArray {"Sin", "Saw", "Square"}, 0));
    
    // FM
    params.push_back (std::make_unique<juce::AudioParameterFloat>(juce::ParameterID {"OSC1FMFREQ", 1}, "Osc 1 FM Frequency", juce::NormalisableRange<float> { 0.0f, 1000.0f, 0.01f, 0.3f }, 5.0f));
    params.push_back (std::make_unique<juce::AudioParameterFloat>(juce::ParameterID {"OSC1FMDEPTH", 1}, "Osc 1 FM Depth", juce::NormalisableRange<float> { 0.0f, 1000.0f, 0.01f, 0.3f}, 500.0f));
    
    // ADSR
    // {lower bound, upper bound, increment value}
    params.push_back (std::make_unique<juce::AudioParameterFloat>(juce::ParameterID {"ATTACK", 1}, "Attack", juce::NormalisableRange<float> { 0.0f, 1.0f, 0.01f}, 0.1f));
    params.push_back (std::make_unique<juce::AudioParameterFloat>(juce::ParameterID {"DECAY", 1}, "Decay", juce::NormalisableRange<float> { 0.0f, 1.0f, 0.01f}, 0.1f));
    params.push_back (std::make_unique<juce::AudioParameterFloat>(juce::ParameterID {"SUSTAIN", 1}, "Sustain", juce::NormalisableRange<float> { 0.0f, 1.0f, 0.01f}, 1.0f));
    params.push_back (std::make_unique<juce::AudioParameterFloat>(juce::ParameterID {"RELEASE", 1}, "Release", juce::NormalisableRange<float> { 0.0f, 3.0f, 0.01f}, 0.4f));
    
    // Filter adsr
    // {lower bound, upper bound, increment value}
    params.push_back (std::make_unique<juce::AudioParameterFloat>(juce::ParameterID {"MODATTACK", 1}, "Mod Attack", juce::NormalisableRange<float> { 0.0f, 1.0f, 0.01f}, 0.1f));
    params.push_back (std::make_unique<juce::AudioParameterFloat>(juce::ParameterID {"MODDECAY", 1}, "Mod Decay", juce::NormalisableRange<float> { 0.0f, 1.0f, 0.01f}, 0.1f));
    params.push_back (std::make_unique<juce::AudioParameterFloat>(juce::ParameterID {"MODSUSTAIN", 1}, "Mod Sustain", juce::NormalisableRange<float> { 0.0f, 1.0f, 0.01f}, 1.0f));
    params.push_back (std::make_unique<juce::AudioParameterFloat>(juce::ParameterID {"MODRELEASE", 1}, "Mod Release", juce::NormalisableRange<float> { 0.0f, 3.0f, 0.01f}, 0.4f));
    
    
    // Filter
    // TODO: dry me
    params.push_back (std::make_unique<juce::AudioParameterChoice> (juce::ParameterID {"FILTERTYPE", 1}, "Filter Type", juce::StringArray { "Low-Pass", "Band-Pass", "High-Pass" }, 0));
    
    params.push_back (std::make_unique<juce::AudioParameterFloat>(juce::ParameterID {"FILTERFREQ", 1}, "Filter Frequency", juce::NormalisableRange<float> { 20.0f, 20000.0f, 0.01f, 0.6f}, 200.0f));
    
    params.push_back (std::make_unique<juce::AudioParameterFloat>(juce::ParameterID {"FILTERRES", 1}, "Filter Resonance", juce::NormalisableRange<float> { 1.0f, 10.0, 0.01f}, 1.0f));
    
    return { params.begin(), params.end() };
}
