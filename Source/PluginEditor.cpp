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
	currentDelaySlider_.setSliderStyle(Slider::LinearHorizontal);	
	currentDelaySlider_.setRange(0.0, 100.0, 0.1);
	currentDelaySlider_.setTextBoxStyle(Slider::TextBoxLeft, true, 120, currentDelaySlider_.getTextBoxHeight());
	currentDelaySlider_.setPopupDisplayEnabled(true, false, this);
	currentDelaySlider_.setTextValueSuffix("Current Delay"); //TODO attach a label instead
	currentDelaySlider_.setValue(0.0);
	addAndMakeVisible(&currentDelaySlider_); // TODO why does this use the address-of operator?						  
	
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

	delayWritePositionSlider_.setSliderStyle(Slider::LinearHorizontal);
	delayWritePositionSlider_.setRange(0.0, processor.delayBufferLength_, 1.0);
	delayWritePositionSlider_.setTextBoxStyle(Slider::TextBoxLeft, false, 120, delayWritePositionSlider_.getTextBoxHeight());
	delayWritePositionSlider_.setTextValueSuffix("dwp"); //TODO attach a label instead
	delayWritePositionSlider_.setValue(0.0);
	addAndMakeVisible(&delayWritePositionSlider_);

	delayReadPositionSlider_.setSliderStyle(Slider::LinearHorizontal);
	delayReadPositionSlider_.setRange(0.0, processor.delayBufferLength_, 1.0);
	delayReadPositionSlider_.setTextBoxStyle(Slider::TextBoxLeft, false, 120, delayReadPositionSlider_.getTextBoxHeight());
	delayReadPositionSlider_.setTextValueSuffix("drp"); //TODO attach a label instead
	delayReadPositionSlider_.setValue(0.0);
	addAndMakeVisible(&delayReadPositionSlider_);

	// start the timer for polling the debug values
	startTimerHz(10);
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
	currentDelaySlider_.setBounds(40, 30, 300, 40);
	mainLFOFrequencySlider_.setBounds(40, currentDelaySlider_.getBottom(), 300, 40);
	mainLFOWidthSlider_.setBounds(40, mainLFOFrequencySlider_.getBottom(), 300, 40);
	delayWritePositionSlider_.setBounds(40, mainLFOWidthSlider_.getBottom(), 300, 40);
	delayReadPositionSlider_.setBounds(40, delayWritePositionSlider_.getBottom(), 300, 40);
}

void FarkleAudioProcessorEditor::sliderValueChanged(Slider * slider)
{
	// if the slider pointer is pointing at the memory address where mainLFOFrequencySlider_ is stored, 
	if (slider == &mainLFOFrequencySlider_)
		processor.setMainLFOFrequency(mainLFOFrequencySlider_.getValue());

	if (slider == &mainLFOWidthSlider_)
		processor.setMainLFOWidth((int)(mainLFOWidthSlider_.getValue()));
}

void FarkleAudioProcessorEditor::timerCallback() {
	currentDelaySlider_.setValue(processor.currentDelayValueDebug_);
	delayWritePositionSlider_.setValue(processor.delayWritePosition_);
	delayReadPositionSlider_.setValue(processor.delayReadPositionDebug_);
}
