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
	float currentDelayValueDebug_;
	float delayReadPositionDebug_;
	int delayWritePosition_;
	int delayBufferLength_;
	float mainLFOFreq_;
	float inverseSampleRate_; // so we don't have to recalculate it every sample
	//==============================================================================
	AudioParameterInt* interpolationType_;
	AudioParameterFloat* mainLFOBaseFreq_;
	AudioParameterFloat* mainLFOWidth_;
	AudioParameterFloat* secondLFOFreq_;
	AudioParameterFloat* secondLFOWidth_;
	AudioParameterFloat* predelay_;
	AudioParameterFloat* mix_;

private:
	//==============================================================================
	void NearestNeighborInterpolation(float drp, float * delayData, float &interpolatedSample);
	void LinearInterpolation(float drp, float * delayData, float &interpolatedSample);
	void CubicInterpolation(float drp, float * delayData, float &interpolatedSample);
	void SecondOrderPolynomialInterpolation(float drp, float * delayData, float &interpolatedSample);
	//==============================================================================
	
	//==============================================================================
	AudioSampleBuffer delayBuffer_;
	//==============================================================================
	float mainLFOPhase_;
	float secondLFOPhase_;
	//==============================================================================
	const int ONE_SECOND_AT_48K = 48000;
	const String PID_INTERPOLATION = "interptype";
	const String PID_SECONDLFOWIDTH = "secondlfowidth";
	const String PID_MAINLFOWIDTH = "mainlfowidth";
	const String PID_SECONDLFOFREQ = "secondlfofreq";
	const String PID_MAINLFOCENTERFREQ = "mainlfocenterfreq";
	const String PID_MIX = "mix";
	const String PID_PREDELAY = "predelay";

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FarkleAudioProcessor)
};
