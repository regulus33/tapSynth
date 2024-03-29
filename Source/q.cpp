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
const juce::String TapSynthAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool TapSynthAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool TapSynthAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

bool TapSynthAudioProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
    return true;
#else
    return false;
#endif
}

double TapSynthAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int TapSynthAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
    // so this should be at least 1, even if you're not really implementing programs.
}

int TapSynthAudioProcessor::getCurrentProgram()
{
    return 0;
}

void TapSynthAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String TapSynthAudioProcessor::getProgramName (int index)
{
    return {};
}

void TapSynthAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void TapSynthAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    synth.setCurrentPlaybackSampleRate (sampleRate);
    
    for (int i = 0; i < synth.getNumVoices(); i++)
    {
        if (auto voice = dynamic_cast<SynthVoice*>(synth.getVoice(i)))
        {
            voice->prepareToPlay (sampleRate, samplesPerBlock, getTotalNumOutputChannels());
        }
    }
}

void TapSynthAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

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

void TapSynthAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
    
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());
    
    // Loops through each SYNTH VOICE, so this happens as many times as you have polyphony minus keys pressed.
    for (int i = 0; i < synth.getNumVoices(); ++i)
    {
        if (auto voice = dynamic_cast<SynthVoice*>(synth.getVoice(i)))
        {
            
            // OSC and osc mod
            auto& oscWaveChoice = *apvts.getRawParameterValue ("OSC1WAVETYPE");
            auto& fmDepth = *apvts.getRawParameterValue("OSC1FMDEPTH");
            auto& fmFreq = *apvts.getRawParameterValue("OSC1FMFREQ");
            
            // Amp envelope
            auto& attack = *apvts.getRawParameterValue ("ATTACK");
            auto& decay = *apvts.getRawParameterValue ("DECAY");
            auto& sustain = *apvts.getRawParameterValue ("SUSTAIN");
            auto& release = *apvts.getRawParameterValue ("RELEASE");
            
            
            // Filter
            auto& filterType = *apvts.getRawParameterValue("FILTERTYPE");
            auto& cutoff = *apvts.getRawParameterValue("FILTERFREQ");
            auto& resonance = *apvts.getRawParameterValue("FILTERRES");
            
            // Modulation envelope
            auto& modAttack = *apvts.getRawParameterValue ("MODATTACK");
            auto& modDecay = *apvts.getRawParameterValue ("MODDECAY");
            auto& modSustain = *apvts.getRawParameterValue ("MODSUSTAIN");
            auto& modRelease = *apvts.getRawParameterValue ("MODRELEASE");
            
            
            voice->getOscillator().setWaveType(oscWaveChoice);
            voice->getOscillator().setFmParams(fmDepth, fmFreq);
            // load() < std::atomic
            voice->updateAdsr (attack.load(), decay.load(), sustain.load(), release.load());
            voice->updateFilter(filterType.load(), cutoff.load(), resonance.load());
            // why not atomic? TODO
            voice->updateModAdsr(modAttack, modDecay, modSustain, modRelease);
            
        }
    }
    
    // This is renderinng all the voices that you see being processed in the for loop above
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
    
    // Filter ADSR
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
