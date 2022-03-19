/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
ExciterAudioProcessor::ExciterAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ),
treeState (*this, nullptr, "PARAMETER", createParameterLayout())
, oversamplingModel(2, 3, juce::dsp::Oversampling<float>::FilterType::filterHalfBandPolyphaseIIR)
#endif
{
    treeState.addParameterListener ("os", this);
    treeState.addParameterListener ("input", this);
    treeState.addParameterListener ("range", this);
    treeState.addParameterListener ("oddeven", this);
    treeState.addParameterListener ("mix", this);
}

ExciterAudioProcessor::~ExciterAudioProcessor()
{
    treeState.removeParameterListener ("os", this);
    treeState.removeParameterListener ("input", this);
    treeState.removeParameterListener ("range", this);
    treeState.removeParameterListener ("oddeven", this);
    treeState.removeParameterListener ("mix", this);
}

juce::AudioProcessorValueTreeState::ParameterLayout ExciterAudioProcessor::createParameterLayout()
{
  std::vector <std::unique_ptr<juce::RangedAudioParameter>> params;
    
  juce::StringArray models = {"Odd", "Even", "Tape", "Transformer"};

  // Make sure to update the number of reservations after adding params
  auto pOS = std::make_unique<juce::AudioParameterBool>("os", "OS", false);
  auto pInput = std::make_unique<juce::AudioParameterFloat>("input", "Input", 0.0, 20.0, 0.0);
  auto pRange = std::make_unique<juce::AudioParameterFloat>("range", "Range", juce::NormalisableRange<float>(1000.0, 20000.0, 1.0, 0.3), 15000.0);
  auto pOddEven = std::make_unique<juce::AudioParameterFloat>("oddeven", "Odd-Even", 0.0, 1.0, 0.5);
  auto pMix = std::make_unique<juce::AudioParameterFloat>("mix", "Mix", 0.0, 1.0, 0.0);
  
  params.push_back(std::move(pOS));
  params.push_back(std::move(pInput));
  params.push_back(std::move(pRange));
  params.push_back(std::move(pOddEven));
  params.push_back(std::move(pMix));

  return { params.begin(), params.end() };
}

void ExciterAudioProcessor::parameterChanged(const juce::String &parameterID, float newValue)
{
    if (parameterID == "os")
    {
        osToggle = newValue;
    }
    
    if (parameterID == "input")
    {
        rawGain = viator_utils::utils::dbToGain(newValue);
    }
    
    if (parameterID == "range")
    {
        cutoff = newValue;
        topBandFilter.setCutoffFrequency(cutoff);
        bottomBandFilter.setCutoffFrequency(cutoff);
    }
    
    if (parameterID == "oddeven")
    {
        oddEvenMix = newValue;
    }
    
    if (parameterID == "mix")
    {
        mix = newValue;
    }
}

//==============================================================================
const juce::String ExciterAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool ExciterAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool ExciterAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool ExciterAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double ExciterAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int ExciterAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int ExciterAudioProcessor::getCurrentProgram()
{
    return 0;
}

void ExciterAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String ExciterAudioProcessor::getProgramName (int index)
{
    return {};
}

void ExciterAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void ExciterAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = getTotalNumInputChannels();
    
    topBandFilter.prepare(spec);
    topBandFilter.setType(juce::dsp::LinkwitzRileyFilterType::highpass);
    topBandFilter.setCutoffFrequency(treeState.getRawParameterValue("range")->load());
    
    bottomBandFilter.prepare(spec);
    bottomBandFilter.setType(juce::dsp::LinkwitzRileyFilterType::lowpass);
    bottomBandFilter.setCutoffFrequency(treeState.getRawParameterValue("range")->load());
    
    
    oversamplingModel.initProcessing(samplesPerBlock);
    
    rawGain = viator_utils::utils::dbToGain(treeState.getRawParameterValue("input")->load());
    cutoff = treeState.getRawParameterValue("range")->load();
    mix = treeState.getRawParameterValue("mix")->load();
    oddEvenMix = treeState.getRawParameterValue("oddeven")->load();
    //osToggle = *treeState.getRawParameterValue("os");
}

void ExciterAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool ExciterAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
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

void ExciterAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;

    juce::dsp::AudioBlock<float> block (buffer);
    juce::dsp::AudioBlock<float> upSampledBlock (buffer);
    
    if (osToggle)
    {
        upSampledBlock = oversamplingModel.processSamplesUp(block);
        
        /** TO DO*/
        // Adjust samplerate
        
        stimulationBlock(upSampledBlock);
        oversamplingModel.processSamplesDown(block);
    }
    
    else
    {
        /** TO DO*/
        // Adjust samplerate
        
        stimulationBlock(block);
    }
}

void ExciterAudioProcessor::stimulationBlock(juce::dsp::AudioBlock<float> &currentBlock)
{
    for (int ch = 0; ch < currentBlock.getNumChannels(); ++ch)
    {
        for (int sample = 0; sample < currentBlock.getNumSamples(); ++sample)
        {
            float* data = currentBlock.getChannelPointer(ch);
            
            // Input
            const float input = data[sample];
            
            // Separate bands
            float topBandSignal = topBandFilter.processSample(ch, input);
            float bottomBandSignal = bottomBandFilter.processSample(ch, input);
            
            // Distortion
            float odd = std::atan(topBandSignal * rawGain);
            float even = topBandSignal * rawGain + std::pow(topBandSignal * rawGain, 4);
            
            // Mix the odd even distortion
            const auto outputSignal = bottomBandSignal + ((1.0 - oddEvenMix) * odd + even * oddEvenMix);
            
            // Output
            data[sample] = (1.0 - mix) * (topBandSignal + bottomBandSignal) + outputSignal * mix;
        }
    }
}

//==============================================================================
bool ExciterAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* ExciterAudioProcessor::createEditor()
{
    //return new ExciterAudioProcessorEditor (*this);
    return new juce::GenericAudioProcessorEditor (*this);
}

//==============================================================================
void ExciterAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void ExciterAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new ExciterAudioProcessor();
}
