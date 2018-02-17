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

	LookAndFeel_V4 nonEditableLook;

	Slider currentDelaySlider_;
	Slider delayWritePositionSlider_;
	Slider delayReadPositionSlider_;

	Slider mainLFOBaseFrequencySlider_;
	Slider mainLFOFrequencySlider_;
	Slider mainLFOWidthSlider_;

	Slider secondLFOFrequencySlider_;
	Slider secondLFOWidthSlider_;

	TextButton  linearInterpolationButton_;
	TextButton  nearestNeighborButton_;
	TextButton  secondOrderInterpolationButton_;
	TextButton  cubicInterpolationButton;
	//==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FarkleAudioProcessorEditor)
};
