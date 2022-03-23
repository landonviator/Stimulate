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
    
    addAndMakeVisible(rangeDial);
    rangeDial.setSkewFactorFromMidPoint(5000.0);
    rangeDial.setDialStyle(viator_gui::Dial::DialStyle::kFullDialMirrow);
    rangeDial.forceShadow();
    
    addAndMakeVisible(oddDial);
    addAndMakeVisible(evenDial);
    addAndMakeVisible(mixFader);
    addAndMakeVisible(trimFader);
    
    addAndMakeVisible(analyzer);
}
