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
    setSize (400, 400);
	// make the gui's window resizeable
	setResizable(true, true);

	// make a horizontal slider widget for the delay time
	currentDelaySlider_.setSliderStyle(Slider::LinearHorizontal);	
	currentDelaySlider_.setRange(0.0, 0.5, 0.01);
	currentDelaySlider_.setTextBoxStyle(Slider::TextBoxLeft, true, 120, currentDelaySlider_.getTextBoxHeight());
	currentDelaySlider_.setPopupDisplayEnabled(true, false, this);
	currentDelaySlider_.setTextValueSuffix("Current Delay"); //TODO attach a label instead
	currentDelaySlider_.setValue(0.0);
	addAndMakeVisible(&currentDelaySlider_); // TODO why does this use the address-of operator?						  
	
	// make a horizontal slider widget for the main FLO frequency
	mainLFOFrequencySlider_.setSliderStyle(Slider::LinearHorizontal);
	mainLFOFrequencySlider_.setRange(0.5, 5.0,0.001);
	mainLFOFrequencySlider_.setTextBoxStyle(Slider::TextBoxLeft, false, 120, mainLFOFrequencySlider_.getTextBoxHeight());
	mainLFOFrequencySlider_.setPopupDisplayEnabled(true, false, this);
	mainLFOFrequencySlider_.setTextValueSuffix(" Main LFO frequency (Hz)"); //TODO attach a label instead
	mainLFOFrequencySlider_.setValue(0.0);
	addAndMakeVisible(&mainLFOFrequencySlider_); // TODO why does this use the address-of operator?
	// add the listener to the slider									  
	mainLFOFrequencySlider_.addListener(this);

	// make a horizontal slider widget for the main FLO width (aka depth)
	mainLFOWidthSlider_.setSliderStyle(Slider::LinearHorizontal);
	mainLFOWidthSlider_.setRange(0.001, 0.05, 0.001);
	mainLFOWidthSlider_.setTextBoxStyle(Slider::TextBoxLeft, false, 120, mainLFOWidthSlider_.getTextBoxHeight());
	mainLFOWidthSlider_.setPopupDisplayEnabled(true, false, this);
	mainLFOWidthSlider_.setTextValueSuffix(" Main LFO Width (seconds)"); //TODO attach a label instead
	mainLFOWidthSlider_.setValue(0.01);
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

	nearestNeighborButton_.setButtonText("Nearest Neighbor");
	nearestNeighborButton_.setTriggeredOnMouseDown(true);
	nearestNeighborButton_.addListener(this);
	addAndMakeVisible(nearestNeighborButton_); //TODO why does this not use the address-of operator?

	linearInterpolationButton_.setButtonText("Linear");
	linearInterpolationButton_.setTriggeredOnMouseDown(true);
	linearInterpolationButton_.addListener(this);
	addAndMakeVisible(linearInterpolationButton_);

	secondOrderInterpolationButton_.setButtonText("Second Order");
	secondOrderInterpolationButton_.setTriggeredOnMouseDown(true);
	secondOrderInterpolationButton_.addListener(this);
	addAndMakeVisible(secondOrderInterpolationButton_);

	cubicInterpolationButton.setButtonText("Cubic");
	cubicInterpolationButton.setTriggeredOnMouseDown(true);
	cubicInterpolationButton.addListener(this);
	addAndMakeVisible(cubicInterpolationButton);

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
	nearestNeighborButton_.setBounds(40, delayReadPositionSlider_.getBottom(), 100, 40);
	linearInterpolationButton_.setBounds(nearestNeighborButton_.getRight(), delayReadPositionSlider_.getBottom(), 100, 40);
	secondOrderInterpolationButton_.setBounds(40, linearInterpolationButton_.getBottom(), 100, 40);
	cubicInterpolationButton.setBounds(secondOrderInterpolationButton_.getRight(), linearInterpolationButton_.getBottom(), 100, 40);
}

void FarkleAudioProcessorEditor::sliderValueChanged(Slider * slider)
{
	// if the slider pointer is pointing at the memory address where mainLFOFrequencySlider_ is stored, 
	if (slider == &mainLFOFrequencySlider_)
		processor.setMainLFOFrequency(mainLFOFrequencySlider_.getValue());

	if (slider == &mainLFOWidthSlider_)
		processor.setMainLFOWidth(mainLFOWidthSlider_.getValue());
}

void FarkleAudioProcessorEditor::buttonClicked(Button* button)
{
	if (button == &nearestNeighborButton_) {
		processor.interpolationType = 0;
	}
	if (button == &linearInterpolationButton_) {
		processor.interpolationType = 1;
	}
	if (button == &secondOrderInterpolationButton_) {
		processor.interpolationType = 2;
	}
	if (button == &cubicInterpolationButton) {
		processor.interpolationType = 3;
	}
}

void FarkleAudioProcessorEditor::timerCallback() {
	currentDelaySlider_.setValue(processor.currentDelayValueDebug_);
	delayWritePositionSlider_.setValue(processor.delayWritePosition_);
	delayReadPositionSlider_.setValue(processor.delayReadPositionDebug_);
}
