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
		private ComboBox::Listener,
		private Timer
{
public:
	typedef AudioProcessorValueTreeState::SliderAttachment SliderAttachment; // This way we don't have to retype the whole thing each time
	typedef AudioProcessorValueTreeState::ComboBoxAttachment ComboBoxAttachment;

    FarkleAudioProcessorEditor (FarkleAudioProcessor&, AudioProcessorValueTreeState&);
	void addComponents();
	void addPresetComboBox();
	void addVisualizations();
	void addParamterControls();
	void addInterpolationTypeComboBox();
    ~FarkleAudioProcessorEditor();

    //==============================================================================
    void paint (Graphics&) override;
	void resized() override;
	void timerCallback() override;
	void comboBoxChanged(ComboBox * comboBoxThatHasChanged) override;

private:
    FarkleAudioProcessor& processor;

	AudioProcessorValueTreeState& valueTreeState;

	LookAndFeel_V4 nonEditableLook;

	ComboBox presetComboBox_;

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

	ComboBox interpolationComboBox_;
	ScopedPointer<ComboBoxAttachment> interpolationAttachment;


	//==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FarkleAudioProcessorEditor)
};
