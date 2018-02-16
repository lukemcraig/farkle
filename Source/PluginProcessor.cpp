/*
  ==============================================================================

  Farkle effect by Luke M Craig

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"


//==============================================================================
FarkleAudioProcessor::FarkleAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{

	delayWritePosition_ = 0;
	currentDelayValueDebug_ = 0.0;
	mainLFOFreq_ = 10.0;
	mainLFOWidth_ = 5;
	lfoPhase_ = 0.0;
	delayReadPositionDebug_ = 0.0;
}

FarkleAudioProcessor::~FarkleAudioProcessor()
{
}

//==============================================================================
const String FarkleAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool FarkleAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool FarkleAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool FarkleAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double FarkleAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int FarkleAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int FarkleAudioProcessor::getCurrentProgram()
{
    return 0;
}

void FarkleAudioProcessor::setCurrentProgram (int index)
{
}

const String FarkleAudioProcessor::getProgramName (int index)
{
    return {};
}

void FarkleAudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void FarkleAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need.
	delayBufferLength_ = ONE_SECOND_AT_48K; // TODO set this more intelligently

	delayBuffer_.setSize(2, delayBufferLength_);	// set the buffer to 2 channels and the size of the delayBufferLength_
	delayBuffer_.clear(); // initialize the memory to 0

	inverseSampleRate_ = 1 / (float)sampleRate;

}

void FarkleAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool FarkleAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void FarkleAudioProcessor::processBlock (AudioSampleBuffer& buffer, MidiBuffer& midiMessages)
{
	// TODO ScopedNoDenormals?
    ScopedNoDenormals noDenormals;
    const int totalNumInputChannels  = getTotalNumInputChannels();
    const int totalNumOutputChannels = getTotalNumOutputChannels();
	const int numSamples = buffer.getNumSamples();

	int dwp = 0; // local delay write position
	float ph = 0.0;
	float currentDelay = 0.0;
	float drp = 0.0;
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
		// get the pointer to the main audio buffer
        float* channelData = buffer.getWritePointer (channel);
		// get the pointer to the delay buffer
		float* delayData = delayBuffer_.getWritePointer(channel);
		// copy the state of this instance variable so that the channels are independently processed
		dwp = delayWritePosition_;
		ph = lfoPhase_;
		
		for (int sample = 0; sample < numSamples; ++sample) {			
			// calculate the (fractional) delay in ms based on the lfo's current amplitude
			currentDelay = mainLFOWidth_ * (0.5f + 0.5f * sinf(2.0 * PI * lfoPhase_)); //TODO make this more effecient
			
			// then the delay read position is (hypothetically) that ^ and 3 more samples behind the write position 
			drp = fmodf((float)dwp - //TODO fmodf is to account for the circular buffer?
				(float)(currentDelay * getSampleRate()) + 
				(float)delayBufferLength_ - 3.0, 
				(float)delayBufferLength_);
			// and then we need to do (linear) interpolation to calculate a sample amplitude "inbetween" integer indicies
			// get the fractional part of the drp
			float fraction = drp - floorf(drp);
			// get the sample index to the left of the fractional sample
			int previousSample = (int)floorf(drp);
			// get the sample index to the right of the fractional sample, accounting for the circular buffer
			int nextSample = (previousSample + 1) % delayBufferLength_;
			// calculate the "inbetween" sample amplitude by getting a weighted average of the 
			float interpolatedSample = fraction * (float) delayData[nextSample] + (1.0f-fraction)*(float)delayData[previousSample]; //TODO polynomial interpolation	
			
			// the input sample is written to delayData at the write pointer
			delayData[dwp] = channelData[sample];

			// the output sample is the interpolatedSample
			channelData[sample] = interpolatedSample;

			// the write pointer increments exactly one sample
			if (++dwp >= delayBufferLength_)
				dwp = 0; // loop back to 0 if necessary

			// update the main LFO's phase by the amount it should be increased per sample, at its current frequency
			ph += mainLFOFreq_ * inverseSampleRate_;
			// wrap it between 0.0 and 1.0
			if (ph >= 1.0) {
				ph -= 1.0;
			}
		}
    }
	// now that all channels are finished with this block, update the instance variable
	delayWritePosition_ = dwp;
	lfoPhase_ = ph;
	// debugging instance variables
	currentDelayValueDebug_ = currentDelay;
	delayReadPositionDebug_ = drp;
	// clear garbage output channels that didn't contain input data
	for (int i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
		buffer.clear(i, 0, buffer.getNumSamples());
}

//==============================================================================
bool FarkleAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* FarkleAudioProcessor::createEditor()
{
    return new FarkleAudioProcessorEditor (*this);
}

//==============================================================================
void FarkleAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void FarkleAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================

void FarkleAudioProcessor::setMainLFOFrequency(float newFreq)
{
	mainLFOFreq_ = newFreq;
}

void FarkleAudioProcessor::setMainLFOWidth(int newWidth)
{
	mainLFOWidth_ = newWidth;
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new FarkleAudioProcessor();
}
