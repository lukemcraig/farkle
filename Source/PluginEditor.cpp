/*
  ==============================================================================

  Farkle effect by Luke M Craig

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"


//==============================================================================
FarkleAudioProcessorEditor::FarkleAudioProcessorEditor (FarkleAudioProcessor& p, AudioProcessorValueTreeState& vts)
    : AudioProcessorEditor (&p), processor (p), valueTreeState(vts)
{
    // set the gui's window size
    setSize (400, 600);
	// make the gui's window resizeable
	setResizable(true, true);

	nonEditableLook.setColour(Slider::thumbColourId, Colours::black);
	
	addComponents();	

	// start the timer for polling the debug values
	startTimerHz(30);
}

FarkleAudioProcessorEditor::~FarkleAudioProcessorEditor()
{

}

void FarkleAudioProcessorEditor::addComponents()
{
	addPresetComboBox();

	addVisualizations();

	addParamterControls();

	addInterpolationTypeComboBox();
}

void FarkleAudioProcessorEditor::addPresetComboBox() {
	File cwd = File::getSpecialLocation(File::currentApplicationFile).getParentDirectory();
	DirectoryIterator iter(cwd,true,"*.xml");
	while (iter.next())
	{
		File theFileItFound(iter.getFile());
		String filename(theFileItFound.getFileNameWithoutExtension());
		presetArray_.add(new PresetFile(filename, theFileItFound));		
	}
	for(int i=0;i<presetArray_.size();i++)
	{
		PresetFile * preset = presetArray_[i];
		String name = preset->name_;
		presetComboBox_.addItem(name, i + 1);
	}
	addAndMakeVisible(presetComboBox_);
	presetComboBox_.addListener(this);
}

void FarkleAudioProcessorEditor::addVisualizations()
{

	// make a horizontal slider widget for the current delay time
	currentDelaySlider_.setSliderStyle(Slider::LinearHorizontal);
	currentDelaySlider_.setRange(0.0, processor.delayBufferLength_ * processor.inverseSampleRate_, 0.0001);
	currentDelaySlider_.setTextBoxStyle(Slider::TextBoxLeft, true, 120, currentDelaySlider_.getTextBoxHeight());
	currentDelaySlider_.setPopupDisplayEnabled(true, false, this);
	currentDelaySlider_.setTextValueSuffix("Current Delay"); //TODO attach a label instead
	currentDelaySlider_.setValue(0.0);

	currentDelaySlider_.setLookAndFeel(&nonEditableLook); // TODO why does this need the address-of operator?		
	addAndMakeVisible(&currentDelaySlider_);

	// make a horizontal slider widget for the calculated main LFO frequency
	mainLFOFrequencySlider_.setSliderStyle(Slider::LinearHorizontal);
	mainLFOFrequencySlider_.setRange(0.01, 6.0, 0.001);
	mainLFOFrequencySlider_.setTextBoxStyle(Slider::TextBoxLeft, false, 120, mainLFOFrequencySlider_.getTextBoxHeight());
	mainLFOFrequencySlider_.setPopupDisplayEnabled(true, false, this);
	mainLFOFrequencySlider_.setTextValueSuffix(" Main LFO frequency (Hz)"); //TODO attach a label instead
	mainLFOFrequencySlider_.setValue(processor.mainLFOFreq_);
	mainLFOFrequencySlider_.setLookAndFeel(&nonEditableLook);
	addAndMakeVisible(&mainLFOFrequencySlider_);

	// delay write position
	delayWritePositionSlider_.setSliderStyle(Slider::LinearHorizontal);
	delayWritePositionSlider_.setRange(0.0, processor.delayBufferLength_, 1.0);
	delayWritePositionSlider_.setTextBoxStyle(Slider::TextBoxLeft, false, 120, delayWritePositionSlider_.getTextBoxHeight());
	delayWritePositionSlider_.setTextValueSuffix("dwp"); //TODO attach a label instead
	delayWritePositionSlider_.setValue(0.0);
	delayWritePositionSlider_.setLookAndFeel(&nonEditableLook);
	addAndMakeVisible(&delayWritePositionSlider_);

	// delay read position
	delayReadPositionSlider_.setSliderStyle(Slider::LinearHorizontal);
	delayReadPositionSlider_.setRange(0.0, processor.delayBufferLength_, 1.0);
	delayReadPositionSlider_.setTextBoxStyle(Slider::TextBoxLeft, false, 120, delayReadPositionSlider_.getTextBoxHeight());
	delayReadPositionSlider_.setTextValueSuffix("drp"); //TODO attach a label instead
	delayReadPositionSlider_.setValue(0.0);
	delayReadPositionSlider_.setLookAndFeel(&nonEditableLook);
	addAndMakeVisible(&delayReadPositionSlider_);
}

void FarkleAudioProcessorEditor::addParamterControls()
{
	// make a horizontal slider widget for the main LFO base frequency
	mainLFOBaseFrequencySlider_.setSliderStyle(Slider::LinearHorizontal);
	mainLFOBaseFrequencySlider_.setTextBoxStyle(Slider::TextBoxLeft, false, 120, mainLFOBaseFrequencySlider_.getTextBoxHeight());
	mainLFOBaseFrequencySlider_.setPopupDisplayEnabled(true, false, this);
	mainLFOBaseFrequencySlider_.setTextValueSuffix(" Main LFO Base frequency (Hz)"); //TODO attach a label instead

	addAndMakeVisible(&mainLFOBaseFrequencySlider_);								  
	mainLfoFreqAttachment = new SliderAttachment(valueTreeState, processor.PID_MAINLFOCENTERFREQ, mainLFOBaseFrequencySlider_);

	// make a horizontal slider widget for the main LFO width (aka depth)
	mainLFOWidthSlider_.setSliderStyle(Slider::LinearHorizontal);
	mainLFOWidthSlider_.setTextBoxStyle(Slider::TextBoxLeft, false, 120, mainLFOWidthSlider_.getTextBoxHeight());
	mainLFOWidthSlider_.setPopupDisplayEnabled(true, false, this);
	mainLFOWidthSlider_.setTextValueSuffix(" Main LFO Width "); //TODO attach a label instead

	addAndMakeVisible(&mainLFOWidthSlider_);								  
	mainLfoWidthAttachment = new SliderAttachment(valueTreeState, processor.PID_MAINLFOWIDTH, mainLFOWidthSlider_);

	// make a horizontal slider widget for the second LFO frequency
	secondLFOFrequencySlider_.setSliderStyle(Slider::LinearHorizontal);	
	secondLFOFrequencySlider_.setTextBoxStyle(Slider::TextBoxLeft, false, 120, secondLFOFrequencySlider_.getTextBoxHeight());
	secondLFOFrequencySlider_.setPopupDisplayEnabled(true, false, this);
	secondLFOFrequencySlider_.setTextValueSuffix(" Second LFO frequency (Hz)"); //TODO attach a label instead

	addAndMakeVisible(&secondLFOFrequencySlider_);
	secondLfoFreqAttachment = new SliderAttachment(valueTreeState, processor.PID_SECONDLFOFREQ, secondLFOFrequencySlider_);

	// make a horizontal slider widget for the second LFO width (aka depth)
	secondLFOWidthSlider_.setSliderStyle(Slider::LinearHorizontal);
	secondLFOWidthSlider_.setTextBoxStyle(Slider::TextBoxLeft, false, 120, secondLFOWidthSlider_.getTextBoxHeight());
	secondLFOWidthSlider_.setPopupDisplayEnabled(true, false, this);
	secondLFOWidthSlider_.setTextValueSuffix(" Second LFO Width "); //TODO attach a label instead

	addAndMakeVisible(&secondLFOWidthSlider_);
	secondLfoWidthAttachment = new SliderAttachment(valueTreeState, processor.PID_SECONDLFOWIDTH, secondLFOWidthSlider_);

	// make a horizontal slider widget for the predelay
	predelaySlider_.setSliderStyle(Slider::LinearHorizontal);
	predelaySlider_.setTextBoxStyle(Slider::TextBoxLeft, false, 120, predelaySlider_.getTextBoxHeight());
	predelaySlider_.setPopupDisplayEnabled(true, false, this);
	predelaySlider_.setTextValueSuffix(" Predelay (sec)"); //TODO attach a label instead

	addAndMakeVisible(&predelaySlider_);
	predelayAttachment = new SliderAttachment(valueTreeState, processor.PID_PREDELAY, predelaySlider_);

	// make a horizontal slider widget for the mix %
	mixSlider_.setSliderStyle(Slider::LinearHorizontal);
	mixSlider_.setRange(0.0, 1.0, 0.01);
	mixSlider_.setTextBoxStyle(Slider::TextBoxLeft, false, 120, mixSlider_.getTextBoxHeight());
	mixSlider_.setPopupDisplayEnabled(true, false, this);
	mixSlider_.setTextValueSuffix(" Mix %"); //TODO attach a label instead
	addAndMakeVisible(&mixSlider_);
	mixAttachment = new SliderAttachment(valueTreeState, processor.PID_MIX, mixSlider_);
}

void FarkleAudioProcessorEditor::addInterpolationTypeComboBox()
{
	interpolationComboBox_.addItem("Nearest Neighbor", 1);
	interpolationComboBox_.addItem("Linear", 2);
	interpolationComboBox_.addItem("Second Order", 3);
	interpolationComboBox_.addItem("Cubic", 4);
	addAndMakeVisible(interpolationComboBox_);
	interpolationAttachment = new ComboBoxAttachment(valueTreeState, processor.PID_INTERPOLATION, interpolationComboBox_);
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

inline void FarkleAudioProcessorEditor::resized()
{
	// lay out the positions of the widgets
	presetComboBox_.setBounds(40, 30, 150, 30);
	// visualizations
	currentDelaySlider_.setBounds(40, presetComboBox_.getBottom(), 300, 40);
	delayWritePositionSlider_.setBounds(40, currentDelaySlider_.getBottom(), 300, 40);
	delayReadPositionSlider_.setBounds(40, delayWritePositionSlider_.getBottom(), 300, 40);
	mainLFOFrequencySlider_.setBounds(40, delayReadPositionSlider_.getBottom(), 300, 40);
	// user parameters
	mainLFOBaseFrequencySlider_.setBounds(40, mainLFOFrequencySlider_.getBottom() + 20, 300, 40);
	mainLFOWidthSlider_.setBounds(40, mainLFOBaseFrequencySlider_.getBottom(), 300, 40);

	secondLFOFrequencySlider_.setBounds(40, mainLFOWidthSlider_.getBottom(), 300, 40);
	secondLFOWidthSlider_.setBounds(40, secondLFOFrequencySlider_.getBottom(), 300, 40);
	predelaySlider_.setBounds(40, secondLFOWidthSlider_.getBottom(), 300, 40);
	mixSlider_.setBounds(40, predelaySlider_.getBottom(), 300, 40);
	// interpolation combo box
	interpolationComboBox_.setBounds(40, mixSlider_.getBottom(), 150, 30);
}

void FarkleAudioProcessorEditor::timerCallback() {
	currentDelaySlider_.setValue(processor.currentDelayValueDebug_);
	mainLFOFrequencySlider_.setValue(processor.mainLFOFreq_);
	delayWritePositionSlider_.setValue(processor.delayWritePosition_);
	delayReadPositionSlider_.setValue(processor.delayReadPositionDebug_);
}

void FarkleAudioProcessorEditor::comboBoxChanged(ComboBox * comboBoxThatHasChanged)
{
	if (comboBoxThatHasChanged == &presetComboBox_) 
	{
		processor.loadPreset(presetArray_[presetComboBox_.getSelectedItemIndex()]->xmlFile_);
	}
}
