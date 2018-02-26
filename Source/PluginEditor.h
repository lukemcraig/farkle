/*
  ==============================================================================

  Farkle effect by Luke M Craig

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"
#include "PresetFile.h"

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
	Label nameLabel_;
	ComboBox presetComboBox_;
	OwnedArray<PresetFile> presetArray_;

	Label visualizationsLabel_;
	Slider currentDelaySlider_;
	Slider delayWritePositionSlider_;
	Slider delayReadPositionSlider_;
	Slider mainLFOFrequencySlider_;

	Label currentDelayLabel_;
	Label delayWritePositionLabel_;
	Label delayReadPositionLabel_;
	Label mainLFOFrequencyLabel_;

	Label paramsLabel_;

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

	Label mainLFOBaseFrequencyLabel_;
	Label mainLFOWidthLabel_;
	Label secondLFOFrequencyLabel_;
	Label secondLFOWidthLabel_;
	Label predelayLabel_;
	Label mixLabel_;

	ComboBox interpolationComboBox_;
	ScopedPointer<ComboBoxAttachment> interpolationAttachment;
	Label interpolationLabel_;
	//==============================================================================
	const int LEFT_PADDING = 80;
	//==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FarkleAudioProcessorEditor)
};
