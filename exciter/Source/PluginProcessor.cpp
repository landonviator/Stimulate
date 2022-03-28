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
, oversamplingModel(2, 4, juce::dsp::Oversampling<float>::FilterType::filterHalfBandPolyphaseIIR)
#endif
{
    treeState.addParameterListener ("os", this);
    treeState.addParameterListener ("phase", this);
    treeState.addParameterListener ("input", this);
    treeState.addParameterListener ("range", this);
    treeState.addParameterListener ("odd", this);
    treeState.addParameterListener ("even", this);
    treeState.addParameterListener ("mix", this);
    treeState.addParameterListener ("trim", this);
}

ExciterAudioProcessor::~ExciterAudioProcessor()
{
    treeState.removeParameterListener ("os", this);
    treeState.removeParameterListener ("phase", this);
    treeState.removeParameterListener ("input", this);
    treeState.removeParameterListener ("range", this);
    treeState.removeParameterListener ("odd", this);
    treeState.removeParameterListener ("even", this);
    treeState.removeParameterListener ("mix", this);
    treeState.removeParameterListener ("trim", this);
}

juce::AudioProcessorValueTreeState::ParameterLayout ExciterAudioProcessor::createParameterLayout()
{
  std::vector <std::unique_ptr<juce::RangedAudioParameter>> params;
    
  // Make sure to update the number of reservations after adding params
  auto pOS = std::make_unique<juce::AudioParameterBool>("os", "OS", false);
  auto pPhase = std::make_unique<juce::AudioParameterBool>("phase", "Phase", false);
  auto pInput = std::make_unique<juce::AudioParameterFloat>("input", "Amount", 0.0, 100.0, 0.0);
  auto pRange = std::make_unique<juce::AudioParameterFloat>("range", "Range", juce::NormalisableRange<float>(1000.0, 20000.0, 1.0, 0.3), 8000.0);
  auto pOdd = std::make_unique<juce::AudioParameterFloat>("odd", "Odd", 0.0, 10.0, 10.0);
  auto pEven = std::make_unique<juce::AudioParameterFloat>("even", "Even", 0.0, 10.0, 1.0);
  auto pMix = std::make_unique<juce::AudioParameterFloat>("mix", "Mix", 0.0, 100.0, 100.0);
  auto pTrim = std::make_unique<juce::AudioParameterFloat>("trim", "Trim", -24.0, 24.0, 0.0);
  
  params.push_back(std::move(pOS));
  params.push_back(std::move(pPhase));
  params.push_back(std::move(pInput));
  params.push_back(std::move(pRange));
  params.push_back(std::move(pOdd));
  params.push_back(std::move(pEven));
  params.push_back(std::move(pMix));
  params.push_back(std::move(pTrim));

  return { params.begin(), params.end() };
}

void ExciterAudioProcessor::parameterChanged(const juce::String &parameterID, float newValue)
{
    if (parameterID == "os")
    {
        updateParameters();
    }
    
    if (parameterID == "input")
    {
        updateParameters();
    }
    
    if (parameterID == "range")
    {
        updateParameters();
    }
    
    if (parameterID == "odd")
    {
        updateParameters();
    }
    
    if (parameterID == "even")
    {
        updateParameters();
    }
    
    if (parameterID == "mix")
    {
        updateParameters();
    }
    
    if (parameterID == "trim")
    {
        updateParameters();
    }
    
    if (parameterID == "phase")
    {
        updateParameters();
    }
}

void ExciterAudioProcessor::updateParameters()
{
    /** Oversampling */
    osToggle = static_cast<bool>(treeState.getRawParameterValue("os")->load());
    
    // Adjust samplerate of filters when oversampling
    if (osToggle)
    {
        spec.sampleRate = getSampleRate() * oversamplingModel.getOversamplingFactor();
        topBandFilter.prepare(spec);
        bottomBandFilter.prepare(spec);
    }
    
    else
    {
        spec.sampleRate = getSampleRate();
        topBandFilter.prepare(spec);
        bottomBandFilter.prepare(spec);
    }
    
    /** Amount */
    auto newGain = juce::jmap(static_cast<float>(treeState.getRawParameterValue("input")->load()), 0.0f, 100.0f, 0.0f, 10.0f);
    rawGain = viator_utils::utils::dbToGain(newGain);
    
    /** Range */
    cutoff = treeState.getRawParameterValue("range")->load();
    topBandFilter.setCutoffFrequency(cutoff);
    bottomBandFilter.setCutoffFrequency(cutoff);
    
    /** Odd */
    auto newOdd = juce::jmap(static_cast<float>(treeState.getRawParameterValue("odd")->load()), 0.0f, 10.0f, 0.0f, 1.0f);
    oddMix = newOdd;
                             
    /** Even */
    auto newEven = juce::jmap(static_cast<float>(treeState.getRawParameterValue("even")->load()), 0.0f, 10.0f, 0.0f, 1.0f);
    evenMix = newEven;
    
    /** Mix */
    mix = treeState.getRawParameterValue("mix")->load() * 0.01;
    
    /** Trim */
    rawTrim = viator_utils::utils::dbToGain(treeState.getRawParameterValue("trim")->load());
    
    /** Phase */
    totalPhase = static_cast<bool>(treeState.getRawParameterValue("phase")->load());
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
    
    // Oversampling
    oversamplingModel.initProcessing(samplesPerBlock);
    oversamplingModel.reset();
    
    // Member variables
    updateParameters();
    
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = getTotalNumInputChannels();
    
    // Top band filter
    topBandFilter.setType(juce::dsp::LinkwitzRileyFilterType::highpass);
    updateParameters();
    
    // Bottom band filter
    bottomBandFilter.setType(juce::dsp::LinkwitzRileyFilterType::lowpass);
    updateParameters();
    
    
}

void ExciterAudioProcessor::releaseResources()
{
    oversamplingModel.reset();
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

    // Oversample if ON
    if (osToggle)
    {
        upSampledBlock = oversamplingModel.processSamplesUp(block);
        stimulationBlock(upSampledBlock);
        oversamplingModel.processSamplesDown(block);
    }

    // Don't Oversample if OFF
    else
    {
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
            const auto outputSignal = bottomBandSignal + (odd * oddMix + even * evenMix) * rawTrim;
            
            // Output
            data[sample] = ((1.0 - mix) * (topBandSignal + bottomBandSignal) + outputSignal * mix);
            
            // Apply phase to output
            data[sample] *= getPhase(totalPhase);
        }
    }
}

int ExciterAudioProcessor::getPhase(bool currentPhaseParam)
{
    return !currentPhaseParam * 2.0 - 1.0;
}

//==============================================================================
bool ExciterAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* ExciterAudioProcessor::createEditor()
{
    return new ExciterAudioProcessorEditor (*this);
    //return new juce::GenericAudioProcessorEditor (*this);
}

//==============================================================================
void ExciterAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // Save params
    treeState.state.appendChild(variableTree, nullptr);
    juce::MemoryOutputStream stream(destData, false);
    treeState.state.writeToStream (stream);
}

void ExciterAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // Recall params
    auto tree = juce::ValueTree::readFromData (data, size_t(sizeInBytes));
    variableTree = tree.getChildWithName("Variables");
    
    if (tree.isValid())
    {
        treeState.state = tree;
        
        // Window Size
        windowWidth = variableTree.getProperty("width");
        windowHeight = variableTree.getProperty("height");
        
        updateParameters();
    }
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new ExciterAudioProcessor();
}
