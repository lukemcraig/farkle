/*
  ==============================================================================

  Farkle effect by Luke M Craig

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"


//==============================================================================
/**
*/
class FarkleAudioProcessorEditor  : public AudioProcessorEditor
{
public:
    FarkleAudioProcessorEditor (FarkleAudioProcessor&);
    ~FarkleAudioProcessorEditor();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;

private:
	//==============================================================================
    FarkleAudioProcessor& processor;
	Slider delayTimeSlider_;
	//==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FarkleAudioProcessorEditor)
};
