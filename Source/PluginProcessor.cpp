/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic startup code for a Juce application.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"


//==============================================================================
Fft_synth_oneAudioProcessor::Fft_synth_oneAudioProcessor()
{
	fft = NULL;
}

Fft_synth_oneAudioProcessor::~Fft_synth_oneAudioProcessor()
{
	deleteAndZero(fft);	
}

//==============================================================================
const String Fft_synth_oneAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

int Fft_synth_oneAudioProcessor::getNumParameters()
{
    return 0;
}

float Fft_synth_oneAudioProcessor::getParameter (int index)
{
    return 0.0f;
}

void Fft_synth_oneAudioProcessor::setParameter (int index, float newValue)
{
}

const String Fft_synth_oneAudioProcessor::getParameterName (int index)
{
    return String::empty;
}

const String Fft_synth_oneAudioProcessor::getParameterText (int index)
{
    return String::empty;
}

const String Fft_synth_oneAudioProcessor::getInputChannelName (int channelIndex) const
{
    return String (channelIndex + 1);
}

const String Fft_synth_oneAudioProcessor::getOutputChannelName (int channelIndex) const
{
    return String (channelIndex + 1);
}

bool Fft_synth_oneAudioProcessor::isInputChannelStereoPair (int index) const
{
    return true;
}

bool Fft_synth_oneAudioProcessor::isOutputChannelStereoPair (int index) const
{
    return true;
}

bool Fft_synth_oneAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool Fft_synth_oneAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

int Fft_synth_oneAudioProcessor::getNumPrograms()
{
    return 0;
}

int Fft_synth_oneAudioProcessor::getCurrentProgram()
{
    return 0;
}

void Fft_synth_oneAudioProcessor::setCurrentProgram (int index)
{
}

const String Fft_synth_oneAudioProcessor::getProgramName (int index)
{
    return String::empty;
}

void Fft_synth_oneAudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================

void Fft_synth_oneAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
}

void Fft_synth_oneAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

void Fft_synth_oneAudioProcessor::processBlock (AudioSampleBuffer& buffer, MidiBuffer& midiMessages)
{
	
	int bufsize = buffer.getNumSamples();
	
    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    for (int channel = 0; channel < getNumInputChannels(); ++channel)
    {
		// declare a channelData array of length bufsize=nfft
        float* channelData = buffer.getSampleData(bufsize);

        // define a fftData array of length bufsize
		// all zeros and one 1 somewhere in the first half
		// put the 1 symmetrically in the second half (real = real, imag = - imag)
		
		// if array index = k
		// then corresponding frequency is Fs/nfft*k
		
		
		// do the backward fft
		fft->processBackward(fftData, channelData, bufsize); // inverse fft
		
    }

    // In case we have more outputs than inputs, we'll clear any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    for (int i = getNumInputChannels(); i < getNumOutputChannels(); ++i)
    {
        buffer.clear (i, 0, buffer.getNumSamples());
    }
}

//==============================================================================
bool Fft_synth_oneAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* Fft_synth_oneAudioProcessor::createEditor()
{
    return new Fft_synth_oneAudioProcessorEditor (this);
}

//==============================================================================
void Fft_synth_oneAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void Fft_synth_oneAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new Fft_synth_oneAudioProcessor();
}
