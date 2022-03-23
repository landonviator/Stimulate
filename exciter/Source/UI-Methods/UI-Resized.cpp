/*
  ==============================================================================

    UI-Resized.cpp
    Created: 24 Oct 2021 1:41:10am
    Author:  Landon Viator

  ==============================================================================
*/

#include "../PluginEditor.h"

void ExciterAudioProcessorEditor::uiResized(float width, float height)
{
    auto leftMargin = height * 0.02;
    auto topMargin = height * 0.1;
    auto dialSize = width * 0.2;
    auto spaceBetween = width * 0.02;
    auto analyzerWidth = width * 0.33;
    auto faderWidth = width * 0.12;
    auto faderHeight = height * 0.8;
    
    // Plugin background UI
    amountDial.setBounds(leftMargin, topMargin, dialSize, dialSize);
    rangeDial.setBounds(amountDial.getX() + amountDial.getWidth() + spaceBetween, topMargin, dialSize, dialSize);
    oddDial.setBounds(amountDial.getX(), amountDial.getY() + amountDial.getHeight() + topMargin * 0.75, dialSize, dialSize);
    evenDial.setBounds(rangeDial.getX(), oddDial.getY(), dialSize, dialSize);
    
    analyzer.setBounds(evenDial.getX() + evenDial.getWidth(), topMargin, analyzerWidth, getHeight() - topMargin * 2.0);
    
    mixFader.setBounds(analyzer.getX() + analyzer.getWidth(), topMargin, faderWidth, faderHeight);
    trimFader.setBounds(mixFader.getX() + mixFader.getWidth(), topMargin, faderWidth, faderHeight);
    
    // Save plugin size in the tree
    saveWindowSize();
}
