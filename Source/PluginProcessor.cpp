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
  : keyboard(NULL), gain(0.8)
{
  memset(freqs, 0, sizeof(freqs));
  memset(phase, 0, sizeof(phase));
}

Fft_synth_oneAudioProcessor::~Fft_synth_oneAudioProcessor()
{
}

//==============================================================================
const String Fft_synth_oneAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

int Fft_synth_oneAudioProcessor::getNumParameters()
{
    return 1;
}

float Fft_synth_oneAudioProcessor::getParameter (int index)
{
  if(index == 0)
    return gain;
  return 0.0f;
}

void Fft_synth_oneAudioProcessor::setParameter (int index, float newValue)
{
  if(index == 0)
    gain = newValue;
}

const String Fft_synth_oneAudioProcessor::getParameterName (int index)
{
  if(index == 0)
    return "Gain";
  return String::empty;
}

const String Fft_synth_oneAudioProcessor::getParameterText (int index)
{
  if(index == 0)
    return "Gain";
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
	
	// Plan initialisation
	nfft = samplesPerBlock; // As a starter. we then should aim at a finer resolution. this is(hopefully) the same as bufsize
	nfft = 8192;
	Fs   = sampleRate;
	
	if (fft == NULL){
		fft = new FastFourierTransformer(nfft);  //size of fft frame
	}	

	fftData = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * nfft); //allocate memory for the frame
}

void Fft_synth_oneAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
	fftw_free(fftData);
}

void Fft_synth_oneAudioProcessor::processBlock (AudioSampleBuffer& buffer, MidiBuffer& midiMessages)
{
	
	juce::MidiBuffer::Iterator iterator (midiMessages);  //for iterating through incoming midiMessages
	juce::MidiMessage msg;								 //both the message and the int are outputs for the iterator
	int sampleNum;
	while (iterator.getNextEvent (msg, sampleNum)){
	  if (msg.isNoteOn()){
	    freqs[msg.getNoteNumber()] = msg.getFloatVelocity();
	  }else if(msg.isNoteOff()){
	    freqs[msg.getNoteNumber()] = 0.0;
	  }
		
	if(keyboard != NULL)
	  keyboard->processNextMidiEvent(msg);
	}

		
                // define a fftData array of length bufsize (done in prepare to play)
		// all zeros and one 1 somewhere in the first half
		// put the 1 symmetrically in the second half (real = real, imag = - imag)		

	int bufsize = buffer.getNumSamples();
	float amplitude; // linear amplitude of the fundamental frequency 

	memset(fftData, 0, 2*nfft*sizeof(fftData[0][0]));

	for(int note=0; note<128; ++note){
	  if(freqs[note] > 0.0){
	    amplitude = freqs[note] * gain; //(0.5 linear = -6 dBFS).
	    float freq = MidiMessage::getMidiNoteInHertz(note);
	    int i = floor(freq*nfft/Fs);
	    fftData[i][0] = amplitude*cos(phase[note]) * 6; // real=magnitude*cos(phase)
	    fftData[i][1] = amplitude*sin(phase[note]) * 6; // imag=magnitude*sin(phase)
	    fftData[nfft-i][0]=  fftData[i][0] ; // Fill up the second half symmetrically, real = real
	    fftData[nfft-i][1]= -fftData[i][1] ; // Fill up the second half symmetrically, imag=-imag


	    // Compute the phase shift of the fundamental during this buffer:
	    // shift = 2.pi.freq.elapsedTime = 2.pi.freq.bufsize/Fs
	    // phase is equal to the floating point remainder (fmod) from the above sum
	    phase[note] += 2*M_PI*(bufsize - 1)*(i/nfft);
	    phase[note] = fmod ( phase[note], 2*M_PI ) ; // we delete any 2*PI rotations, in order to keep the phase within limits.
	  }
	}

    for (int channel = 0; channel < getNumInputChannels(); ++channel){
      float* channelData = buffer.getSampleData (channel);
      // do the backward fft
      fft->processBackward(fftData, nfft, channelData, bufsize); // inverse fft		
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

void Fft_synth_oneAudioProcessor::setKeyboardState(MidiKeyboardState* state){
  if(keyboard != NULL)
    keyboard->removeListener(this);
  keyboard = state;
  keyboard->addListener(this);
}

void Fft_synth_oneAudioProcessor::handleNoteOn(MidiKeyboardState *source, int midiChannel, int midiNoteNumber, float velocity){
  freqs[midiNoteNumber] = velocity;
}

void Fft_synth_oneAudioProcessor::handleNoteOff(MidiKeyboardState *source, int midiChannel, int midiNoteNumber){
  freqs[midiNoteNumber] = 0.0;
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new Fft_synth_oneAudioProcessor();
}
