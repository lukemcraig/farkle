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
	/** Sets the main LFO frequency
	*/
	void setMainLFOFrequency(float newFreq);
	/** Sets the main LFO Width in samples
	*/
	void setMainLFOWidth(float newWidth);
	//==============================================================================
	float currentDelayValueDebug_;
	int delayReadPositionDebug_;
	int delayWritePosition_;
	int delayBufferLength_;
	int interpolationType;
	//==============================================================================
	float mainLFOFreq_;
	float mainLFOWidth_;

private:
	//==============================================================================
	void NearestNeighborInterpolation(float drp, float * delayData, float &interpolatedSample);
	void LinearInterpolation(float drp, float * delayData, float &interpolatedSample);
	void CubicInterpolation(float drp, float * delayData, float &interpolatedSample);
	void SecondOrderPolynomialInterpolation(float drp, float * delayData, float &interpolatedSample);
	//==============================================================================
	AudioSampleBuffer delayBuffer_;
	//==============================================================================
	float lfoPhase_;
	float inverseSampleRate_; // so we don't have to recalculate it every sample
	//==============================================================================
	const int ONE_SECOND_AT_48K = 48000;
	const double PI = 3.14159265358979323846;
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FarkleAudioProcessor)
};
