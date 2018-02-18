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
	typedef AudioProcessorValueTreeState::SliderAttachment SliderAttachment;

    FarkleAudioProcessorEditor (FarkleAudioProcessor&, AudioProcessorValueTreeState&);
	void addComponents();
	void addVisualizations();
	void addParamterControls();
	void addInterpolationTypeButtons();
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

	AudioProcessorValueTreeState& valueTreeState;

	LookAndFeel_V4 nonEditableLook;

	Slider currentDelaySlider_;
	Slider delayWritePositionSlider_;
	Slider delayReadPositionSlider_;
	Slider mainLFOFrequencySlider_;

	Slider mainLFOBaseFrequencySlider_;
	ScopedPointer<SliderAttachment> mainLfoFreqAttachment;
	Slider mainLFOWidthSlider_;
	ScopedPointer<SliderAttachment> mainLfoWidthAttachment;

	Slider secondLFOFrequencySlider_;
	ScopedPointer<SliderAttachment> secondLfoFreqAttachment;
	Slider secondLFOWidthSlider_;
	ScopedPointer<SliderAttachment> secondLfoWidthAttachment;

	Slider predelaySlider_;
	ScopedPointer<SliderAttachment> predelayAttachment;
	Slider mixSlider_;
	ScopedPointer<SliderAttachment> mixAttachment;

	TextButton  linearInterpolationButton_;
	TextButton  nearestNeighborButton_;
	TextButton  secondOrderInterpolationButton_;
	TextButton  cubicInterpolationButton;

	//==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FarkleAudioProcessorEditor)
};
