/*
  ==============================================================================

    Farkle effect by Luke M Craig

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"


//==============================================================================
/**
*/
class FarkleAudioProcessor  : public AudioProcessor
{
public:
    //==============================================================================
    FarkleAudioProcessor();
    ~FarkleAudioProcessor();

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (AudioSampleBuffer&, MidiBuffer&) override;

    //==============================================================================
    AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect () const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const String getProgramName (int index) override;
    void changeProgramName (int index, const String& newName) override;

    //==============================================================================
    void getStateInformation (MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
	//==============================================================================

	/** Sets the delay time (in samples) and updates the delayReadPosition_ accordingly.
		Should be called from the GUI.
	*/
	void setDelayTime(int newDelayTime);
	//==============================================================================
	double delayTime_;
	int delayReadPosition_;
	int delayWritePosition_;
	int delayBufferLength_;
private:
	//==============================================================================
	void setDelayReadPosition();
	//==============================================================================
	AudioSampleBuffer delayBuffer_;
	//==============================================================================
	const int FOUR_SECONDS_AT_48K = 192000;
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FarkleAudioProcessor)
};
