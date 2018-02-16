/*
  ==============================================================================

  Farkle effect by Luke M Craig

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"


//==============================================================================
FarkleAudioProcessorEditor::FarkleAudioProcessorEditor (FarkleAudioProcessor& p)
    : AudioProcessorEditor (&p), processor (p)
{
    // set the gui's window size
    setSize (400, 300);
	// make the gui's window resizeable
	setResizable(true, true);

	// make a horizontal slider widget for the delay time
	delayTimeSlider_.setSliderStyle(Slider::LinearHorizontal);	
	delayTimeSlider_.setRange(0.0, processor.delayBufferLength_, 1.0);
	delayTimeSlider_.setTextBoxStyle(Slider::TextBoxLeft, false, 120, delayTimeSlider_.getTextBoxHeight());
	delayTimeSlider_.setPopupDisplayEnabled(true, false, this);
	delayTimeSlider_.setTextValueSuffix(" Delay Time (samples)"); //TODO attach a label instead
	delayTimeSlider_.setValue(0.0);
	addAndMakeVisible(&delayTimeSlider_); // TODO why does this use the address-of operator?
	// add the listener to the slider									  
	delayTimeSlider_.addListener(this);

	// make a horizontal slider widget for the main FLO frequency
	mainLFOFrequencySlider_.setSliderStyle(Slider::LinearHorizontal);
	mainLFOFrequencySlider_.setRange(0.0, 100, 1.0);
	mainLFOFrequencySlider_.setTextBoxStyle(Slider::TextBoxLeft, false, 120, mainLFOFrequencySlider_.getTextBoxHeight());
	mainLFOFrequencySlider_.setPopupDisplayEnabled(true, false, this);
	mainLFOFrequencySlider_.setTextValueSuffix(" Main LFO frequency (Hz)"); //TODO attach a label instead
	mainLFOFrequencySlider_.setValue(0.0);
	addAndMakeVisible(&mainLFOFrequencySlider_); // TODO why does this use the address-of operator?
	// add the listener to the slider									  
	mainLFOFrequencySlider_.addListener(this);

	// make a horizontal slider widget for the main FLO width (aka depth)
	mainLFOWidthSlider_.setSliderStyle(Slider::LinearHorizontal);
	mainLFOWidthSlider_.setRange(0.0, 100, 1.0);
	mainLFOWidthSlider_.setTextBoxStyle(Slider::TextBoxLeft, false, 120, mainLFOWidthSlider_.getTextBoxHeight());
	mainLFOWidthSlider_.setPopupDisplayEnabled(true, false, this);
	mainLFOWidthSlider_.setTextValueSuffix(" Main LFO Width (Samples)"); //TODO attach a label instead
	mainLFOWidthSlider_.setValue(0.0);
	addAndMakeVisible(&mainLFOWidthSlider_); // TODO why does this use the address-of operator?
	 // add the listener to the slider									  
	mainLFOWidthSlider_.addListener(this);
}

FarkleAudioProcessorEditor::~FarkleAudioProcessorEditor()
{
}

//==============================================================================
void FarkleAudioProcessorEditor::paint (Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));

    g.setColour (Colours::white);
    g.setFont (15.0f);
    g.drawFittedText ("Farkle", getLocalBounds(), Justification::topLeft, 1);
}

void FarkleAudioProcessorEditor::resized()
{
    // lay out the positions of any widgets
	delayTimeSlider_.setBounds(40, 30, 300, 40);
	mainLFOFrequencySlider_.setBounds(40, delayTimeSlider_.getBottom(), 300, 40);
	mainLFOWidthSlider_.setBounds(40, mainLFOFrequencySlider_.getBottom(), 300, 40);
}

void FarkleAudioProcessorEditor::sliderValueChanged(Slider * slider)
{
	// if the slider pointer is pointing at the memory address where delayTimeSlider_ is stored, 
	if (slider == &delayTimeSlider_) 
		// update the delay time in PluginProcessor 
		processor.setDelayTime((int)(delayTimeSlider_.getValue()));

	if (slider == &mainLFOFrequencySlider_)
		processor.setMainLFOFrequency(mainLFOFrequencySlider_.getValue());

	if (slider == &mainLFOWidthSlider_)
		processor.setMainLFOWidth((int)(mainLFOWidthSlider_.getValue()));
}
