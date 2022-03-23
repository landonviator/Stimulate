/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
ExciterAudioProcessorEditor::ExciterAudioProcessorEditor (ExciterAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
, amountDial(" %", 0.0, 100.0, 0.01, 0.0)
, rangeDial(" Hz", 1000.0, 20000.0, 1.0, 15000.0)
, oddDial(" dB", 0.0, 10.0, 0.01, 0.0)
, evenDial(" dB", 0.0, 10.0, 0.01, 0.0)
, mixFader(" %", 0.0, 1.0, 0.01, 0.0)
, trimFader(" dB", -24.0, 24.0, 0.01, 0.0)
{
    uiConstructor();
}

ExciterAudioProcessorEditor::~ExciterAudioProcessorEditor()
{
}

//==============================================================================
void ExciterAudioProcessorEditor::paint (juce::Graphics& g)
{
    uiPaint(g);
}

void ExciterAudioProcessorEditor::resized()
{
    uiResized(getWidth(), getHeight());
}
