/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class ExciterAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    ExciterAudioProcessorEditor (ExciterAudioProcessor&);
    ~ExciterAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    ExciterAudioProcessor& audioProcessor;
    
    void uiConstructor();
    void initWindow();
    void uiPaint(juce::Graphics &g);
    void uiResized(float width, float height);
    void saveWindowSize();
    
    
    juce::Image background;
    bool constructorFinished {false};
    
    viator_gui::Dial amountDial;
    viator_gui::Dial rangeDial;
    viator_gui::Dial oddDial;
    viator_gui::Dial evenDial;
    viator_gui::Fader mixFader;
    viator_gui::Fader trimFader;
    viator_gui::Border analyzer;
    
    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    using ButtonAttachment = juce::AudioProcessorValueTreeState::ButtonAttachment;
    
    std::unique_ptr<SliderAttachment> amountDialAttach;
    std::unique_ptr<SliderAttachment> rangeDialAttach;
    std::unique_ptr<SliderAttachment> oddDialAttach;
    std::unique_ptr<SliderAttachment> evenDialAttach;
    std::unique_ptr<SliderAttachment> mixFaderAttach;
    std::unique_ptr<SliderAttachment> trimFaderAttach;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ExciterAudioProcessorEditor)
};
