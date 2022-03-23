/*
  ==============================================================================

    UI-Constructor.cpp
    Created: 24 Oct 2021 1:40:44am
    Author:  Landon Viator

  ==============================================================================
*/

#include "../PluginEditor.h"

void ExciterAudioProcessorEditor::uiConstructor()
{
    
    // Window
    initWindow();
    
    addAndMakeVisible(amountDial);
    amountDialAttach = std::make_unique<SliderAttachment>(audioProcessor.treeState, "input", amountDial);
    addAndMakeVisible(amountDialLabel);
    amountDialLabel.setText("Amount", juce::dontSendNotification);
    amountDialLabel.attachToComponent(&amountDial, false);
    
    addAndMakeVisible(rangeDial);
    rangeDialAttach = std::make_unique<SliderAttachment>(audioProcessor.treeState, "range", rangeDial);
    rangeDial.setSkewFactorFromMidPoint(5000.0);
    rangeDial.setDialStyle(viator_gui::Dial::DialStyle::kFullDialMirrow);
    rangeDial.forceShadow();
    addAndMakeVisible(rangeDialLabel);
    rangeDialLabel.setText("Range", juce::dontSendNotification);
    rangeDialLabel.attachToComponent(&rangeDial, false);
    
    addAndMakeVisible(oddDial);
    oddDialAttach = std::make_unique<SliderAttachment>(audioProcessor.treeState, "odd", oddDial);
    addAndMakeVisible(oddDialLabel);
    oddDialLabel.setText("Odd", juce::dontSendNotification);
    oddDialLabel.attachToComponent(&oddDial, false);
    
    addAndMakeVisible(evenDial);
    evenDialAttach = std::make_unique<SliderAttachment>(audioProcessor.treeState, "even", evenDial);
    addAndMakeVisible(evenDialLabel);
    evenDialLabel.setText("Even", juce::dontSendNotification);
    evenDialLabel.attachToComponent(&evenDial, false);
    
    addAndMakeVisible(mixFader);
    mixFaderAttach = std::make_unique<SliderAttachment>(audioProcessor.treeState, "mix", mixFader);
    addAndMakeVisible(mixFaderLabel);
    mixFaderLabel.setText("Mix", juce::dontSendNotification);
    mixFaderLabel.attachToComponent(&mixFader, false);
    
    addAndMakeVisible(trimFader);
    trimFaderAttach = std::make_unique<SliderAttachment>(audioProcessor.treeState, "trim", trimFader);
    addAndMakeVisible(trimFaderLabel);
    trimFaderLabel.setText("Trim", juce::dontSendNotification);
    trimFaderLabel.attachToComponent(&trimFader, false);
    
    addAndMakeVisible(analyzer);
}
