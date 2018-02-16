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
	delayTimeSlider_.setTextBoxStyle(Slider::TextBoxLeft, false, 90, delayTimeSlider_.getTextBoxHeight());
	delayTimeSlider_.setPopupDisplayEnabled(true, false, this);
	delayTimeSlider_.setTextValueSuffix("Delay Time (samples)");
	delayTimeSlider_.setValue(0.0);
	addAndMakeVisible(&delayTimeSlider_); // TODO why does this use the address-of operator?
	// add the listener to the slider									  
	delayTimeSlider_.addListener(this);
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
    g.drawFittedText ("Hello World!", getLocalBounds(), Justification::centred, 1);
}

void FarkleAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
}

void FarkleAudioProcessorEditor::sliderValueChanged(Slider * slider)
{
	// if the slider pointer is pointing at the memory address where delayTimeSlider_ is stored, 
	// update the delay time in PluginProcessor 
	if (slider == &delayTimeSlider_) 
		processor.setDelayTime((int)(delayTimeSlider_.getValue()));
}
