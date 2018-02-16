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
class FarkleAudioProcessorEditor  : public AudioProcessorEditor,
		private Slider::Listener
{
public:
    FarkleAudioProcessorEditor (FarkleAudioProcessor&);
    ~FarkleAudioProcessorEditor();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;

private:
	//==============================================================================
	void sliderValueChanged(Slider* slider) override;
	//==============================================================================
    FarkleAudioProcessor& processor;
	Slider delayTimeSlider_;
	Slider mainLFOFrequencySlider_;
	Slider mainLFOWidthSlider_;
	//==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FarkleAudioProcessorEditor)
};
