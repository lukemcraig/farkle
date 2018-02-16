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
		private Slider::Listener,
		private Button::Listener,
		private Timer
{
public:
    FarkleAudioProcessorEditor (FarkleAudioProcessor&);
    ~FarkleAudioProcessorEditor();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;
	void timerCallback() override;

private:
	//==============================================================================
	void sliderValueChanged(Slider* slider) override;
	void buttonClicked(Button* button) override;
	//==============================================================================
    FarkleAudioProcessor& processor;
	Slider currentDelaySlider_;
	Slider delayWritePositionSlider_;
	Slider delayReadPositionSlider_;
	Slider mainLFOFrequencySlider_;
	Slider mainLFOWidthSlider_;

	TextButton  linearInterpolationButton_;
	TextButton  nearestNeighborButton_;
	//==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FarkleAudioProcessorEditor)
};
