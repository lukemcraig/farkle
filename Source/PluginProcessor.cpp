/*
  ==============================================================================

  Farkle effect by Luke M Craig

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <assert.h> 

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
                       ),
#endif
	parameters(*this, nullptr)
{
	delayWritePosition_ = 0;
	delayReadPositionDebug_ = 0.0;
	currentDelayValueDebug_ = 0.0;
	mainLFOFreq_ = 0.0;
	mainLFOPhase_ = 0.0;
	secondLFOPhase_ = 0.0;

	parameters.createAndAddParameter(PID_MAINLFOCENTERFREQ, // parameter ID
		"Main LFO Center Frequency", // paramter Name
		String("Hz"), // parameter label (suffix)
		NormalisableRange<float>(0.0f, 5.0f), //range
		1.92f, // default value
		nullptr,
		nullptr);

	parameters.createAndAddParameter(PID_MAINLFOWIDTH, // parameter ID
		"Main LFO Width", // paramter Name
		String(), // parameter label (suffix)
		NormalisableRange<float>(0.0f, 1.0f), //range
		0.02f, // default value
		nullptr,
		nullptr);

	parameters.createAndAddParameter(PID_SECONDLFOFREQ, // parameter ID
		"2nd LFO Frequency", // paramter Name
		String(), // parameter label (suffix)
		NormalisableRange<float>(0.0f, 5.0f), //range
		0.958f, // default value
		nullptr,
		nullptr);

	parameters.createAndAddParameter(PID_SECONDLFOWIDTH, // parameter ID
		"2nd LFO Width", // paramter Name
		String(), // parameter label (suffix)
		NormalisableRange<float>(0.0f, 1.0f), //range
		0.685f, // default value
		nullptr,
		nullptr);

	parameters.createAndAddParameter(PID_MIX, // parameter ID
		"Mix", // paramter Name
		String("%"), // parameter label (suffix)
		NormalisableRange<float>(0.0f, 1.0f), //range
		0.75f, // default value
		nullptr,
		nullptr);

	parameters.createAndAddParameter(PID_PREDELAY, // parameter ID
		"Predelay", // paramter Name
		String(), // parameter label (suffix)
		NormalisableRange<float>(0.0f, 3.0f), //range
		0.247f, // default value
		nullptr,
		nullptr);

	parameters.createAndAddParameter(PID_INTERPOLATION, // parameter ID
		"Interpolation Type", // paramter Name
		String(), // parameter label (suffix)
		NormalisableRange<float>(0.0f, 3.0f), //range
		1, // default value
		nullptr,
		nullptr,
		false, // isMetaParameter 
		true, // isAutomatableParameter 
		true); // isDiscrete

	parameters.state = ValueTree(Identifier("FarkleParameters"));
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
	delayBufferLength_ = ONE_SECOND_AT_48K * 5; // TODO set this more intelligently
	// set the buffer to the number of input channels and the size of the delayBufferLength_
	delayBuffer_.setSize(getTotalNumInputChannels(), delayBufferLength_);	
	delayBuffer_.clear(); // initialize the memory to 0
	mainLFOPhase_ = 0.0;
	secondLFOPhase_ = 0.0;
	inverseSampleRate_ = 1.0 / (float)sampleRate;
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
	float local_mainLFOPhase = 0.0;
	float local_secondLFOPhase = 0.0;
	float local_secondLFOWidth = 0.0;
	float local_mainLFOFrequency = 0.0;
	float local_mainLFOBaseFrequency = 0.0;
	float local_secondLFOFrequency = 0.0;
	float local_mainLFOWidth = 0.0;
	float currentDelay = 0.0;
	float local_predelay = 0.0;
	float local_mix = 0.0;
	float one_minus_mix = 0.0;
	float drp = 0.0;
	int local_interpolationType = 1;
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
		// get the pointer to the main audio buffer
        float* channelData = buffer.getWritePointer (channel);
		// get the pointer to the delay buffer
		float* delayData = delayBuffer_.getWritePointer(channel);
		// copy the state of this instance variables so that the channels are independently processed
		local_mainLFOBaseFrequency = *parameters.getRawParameterValue(PID_MAINLFOCENTERFREQ);
		local_secondLFOFrequency = *parameters.getRawParameterValue(PID_SECONDLFOFREQ);
		local_mainLFOWidth = *parameters.getRawParameterValue(PID_MAINLFOWIDTH);
		local_secondLFOWidth = *parameters.getRawParameterValue(PID_SECONDLFOWIDTH);
		local_predelay = *parameters.getRawParameterValue(PID_PREDELAY);
		local_mix = *parameters.getRawParameterValue(PID_MIX);
		one_minus_mix = 1.0f - local_mix;
		local_interpolationType = *parameters.getRawParameterValue(PID_INTERPOLATION);
		dwp = delayWritePosition_;
		local_mainLFOPhase = mainLFOPhase_;
		local_secondLFOPhase = secondLFOPhase_;
		local_mainLFOFrequency = mainLFOFreq_;

		for (int sample = 0; sample < numSamples; ++sample) {	

			float interpolatedSample = 0.0;
			// calculate the (fractional) delay in seconds based on the lfo's current amplitude
			currentDelay = local_mainLFOWidth * (0.5f + 0.5f * sinf(2.0 * float_Pi * local_mainLFOPhase)); //TODO make this more effecient
			currentDelay += local_predelay;
			// then the delay read position is (hypothetically) the currentDelay and 3 more samples behind the write position 
			drp = (float)dwp - (float)(currentDelay) * (float)getSampleRate() + (float)delayBufferLength_ - 3.0;
			// and then wrap it around the circular buffer (fmodf instead of % because it's a float)
			drp = fmodf(drp,  (float)delayBufferLength_);

			// and then we need to do interpolation to calculate a sample amplitude "inbetween" integer indicies
			if(local_interpolationType == 0)
				NearestNeighborInterpolation(drp, delayData, interpolatedSample);
			if (local_interpolationType == 1)
				LinearInterpolation(drp, delayData, interpolatedSample);
			if (local_interpolationType == 2)
				SecondOrderPolynomialInterpolation(drp, delayData, interpolatedSample);
			if (local_interpolationType == 3)
				CubicInterpolation(drp, delayData, interpolatedSample);

			// the input sample is written to delayData at the write pointer
			delayData[dwp] = channelData[sample];			

			// the output sample is the interpolatedSample
			float out  = (interpolatedSample*local_mix) + (channelData[sample] * one_minus_mix);
			channelData[sample] = out;
			//channelData[sample] = interpolatedSample;

			// the write pointer increments exactly one sample
			if (++dwp >= delayBufferLength_)
				dwp = 0; // loop back to 0 if necessary

			// update the second LFO's phase by the amount it should be increased per sample, at its current frequency
			local_secondLFOPhase += local_secondLFOFrequency * inverseSampleRate_;
			// wrap it between 0.0 and 1.0
			if (local_secondLFOPhase >= 1.0) {
				local_secondLFOPhase -= 1.0;
			}
			assert(local_secondLFOPhase >= 0.0);
			assert(local_secondLFOPhase<1.0);

			// update the main LFO's frequency
			local_mainLFOFrequency = local_mainLFOBaseFrequency + local_secondLFOWidth * (sinf(2.0* float_Pi * local_secondLFOPhase) ); //TODO make this more effecient
			assert(local_mainLFOFrequency>=0.0);

			// update the main LFO's phase by the amount it should be increased per sample, at its current frequency
			local_mainLFOPhase += local_mainLFOFrequency * inverseSampleRate_;
			// wrap it between 0.0 and 1.0
			if (local_mainLFOPhase >= 1.0) { //TODO do i need a f here?
				local_mainLFOPhase -= 1.0;
			}
			assert(local_mainLFOPhase >=0.0);
			assert(local_mainLFOPhase < 1.0);	
		}
    }
	// now that all channels are finished with this block, update the instance variable
	delayWritePosition_ = dwp;
	mainLFOPhase_ = local_mainLFOPhase;
	mainLFOFreq_ = local_mainLFOFrequency;
	secondLFOPhase_ = local_secondLFOPhase;

	// debugging instance variables
	currentDelayValueDebug_ = currentDelay;
	delayReadPositionDebug_ = drp;

	// clear garbage output channels that didn't contain input data
	for (int i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
		buffer.clear(i, 0, buffer.getNumSamples());
}

void FarkleAudioProcessor::NearestNeighborInterpolation(float drp, float * delayData, float &interpolatedSample)
{
	float fraction = drp - floorf(drp);
	// get the sample index to the left of the fractional sample
	int previousSample = (int)floorf(drp);
	// get the sample index to the right of the fractional sample, accounting for the circular buffer
	int nextSample = (previousSample + 1) % delayBufferLength_;
	// get the nearest neighbors amplitude
	if(fraction<0.5)
		interpolatedSample = delayData[previousSample];
	else
		interpolatedSample = delayData[nextSample];
}

void FarkleAudioProcessor::LinearInterpolation(float drp, float * delayData, float &interpolatedSample)
{
	// x(t) = (n + 1 - t)x[n] + (t - n)x[n + 1],  n <= t < n + 1 

	// get the fractional part of the delay read position
	float t = drp - floorf(drp);
	// get the sample index to the left of the fractional sample
	int previousSampleIndex = (int)floorf(drp);
	// get the sample index to the right of the fractional sample, accounting for the circular buffer
	int nextSampleIndex = (previousSampleIndex + 1) % delayBufferLength_;
	// calculate the "inbetween" sample amplitude by getting a weighted average of the 
	interpolatedSample = (1.0f - t)*delayData[previousSampleIndex] + t * delayData[nextSampleIndex];
}

void FarkleAudioProcessor::SecondOrderPolynomialInterpolation(float drp, float * delayData, float &interpolatedSample)
{
	// x(t) = ( (t-n-1)(t-n)x[n-1] - 2(t-n-1)(t-n+1)x[n] + (t-n)(t-n+1)x[n+1] ) / 2

	// get the fractional part of the delay read position
	float t = drp - floorf(drp);
	// get the sample index to the left of the fractional sample x[n]
	int previousSampleIndex = (int)floorf(drp); // x[n]
	// get the sample index 2 to the left of the fractional sample x[n-1], accounting for the circular buffer
	int previousPreviousSampleIndex = (previousSampleIndex-1+ delayBufferLength_) % delayBufferLength_;
	// get the sample index to the right of the fractional sample, accounting for the circular buffer
	int nextSampleIndex = (previousSampleIndex + 1) % delayBufferLength_;
	
	float term1 = (t-1.0f)*t*delayData[previousPreviousSampleIndex];
	float term2 = -2.0f*(t-1.0f)*(t+1.0f)*(delayData[previousSampleIndex]);
	float term3 = t*(t+1.0f)*delayData[nextSampleIndex]; 

	interpolatedSample = (term1 + term2 + term3) / 2.0f; //TODO what is the difference with the f or not?
}

void FarkleAudioProcessor::CubicInterpolation(float drp, float * delayData, float &interpolatedSample)
{
	// x(t) = c3(t-n)^3 + c2(t-n)^2 + c1(t-n) + c0  for n<= t < n+1

	// c3 = -x[n-1]+x[n]-x[n+1]+x[n+2]
	// c2 = x[n-1] - x[n] - a0
	// c1 = x[n+1]-x[n-1]
	// c0 = x[n]

	// get the fractional part of the delay read position
	float t = drp - floorf(drp);
	// get the sample index to the left of the fractional sample x[n]
	int n = (int)floorf(drp); // x[n]
	// get the sample index 2 to the left of the fractional sample x[n-1], accounting for the circular buffer
	int n_minus_1 = (n - 1 + delayBufferLength_) % delayBufferLength_;
	// get the sample index to the right of the fractional sample, accounting for the circular buffer
	int n_plus_1 = (n + 1) % delayBufferLength_;
	// get the sample index 2 to the right of the fractional sample, accounting for the circular buffer
	int n_plus_2 = (n + 2) % delayBufferLength_;

	float a0 = -0.5*(delayData[n_plus_1]- delayData[n_minus_1]);

	// coeffecients
	float c3 = -delayData[n_minus_1] + delayData[n] - delayData[n_plus_1] + delayData[n_plus_2];
	float c2 = delayData[n_minus_1] - delayData[n] - a0; 
	float c1 = delayData[n_plus_1]-delayData[n_minus_1];
	float c0 = delayData[n];

	interpolatedSample = c3*(t*t*t) + c2*(t*t) + c1*(t) + c0;
}

//==============================================================================
bool FarkleAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* FarkleAudioProcessor::createEditor()
{
    return new FarkleAudioProcessorEditor (*this,parameters);
}

//==============================================================================
void FarkleAudioProcessor::getStateInformation (MemoryBlock& destData)
{
	ScopedPointer<XmlElement> xml(parameters.state.createXml()); 
	copyXmlToBinary(*xml, destData);

}

void FarkleAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
	ScopedPointer<XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

	if (xmlState != nullptr)
		if (xmlState->hasTagName(parameters.state.getType()))
			parameters.state = ValueTree::fromXml(*xmlState);
}

void FarkleAudioProcessor::loadPreset(File file)
{
		XmlDocument  xmlFile( file);
		ScopedPointer<XmlElement> xmlState = xmlFile.getDocumentElement();
		if (xmlState != nullptr)
			if (xmlState->hasTagName(parameters.state.getType()))
				parameters.state = ValueTree::fromXml(*xmlState);	
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new FarkleAudioProcessor();
}
