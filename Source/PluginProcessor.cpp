/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
SimpleDelayLineAudioProcessor::SimpleDelayLineAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", AudioChannelSet::stereo(), true)
                     #endif
                       ), parameters(*this, nullptr, "PARAMETERS", initializeGUI()) //se añaden sliders al constructer
#endif
{
}

SimpleDelayLineAudioProcessor::~SimpleDelayLineAudioProcessor()
{
}

juce::AudioProcessorValueTreeState::ParameterLayout SimpleDelayLineAudioProcessor::initializeGUI()
{
    // se añaden 2 sliders con sus respectivos ids
    std::vector <std::unique_ptr<juce::RangedAudioParameter>> params;
    
    // Slider 1: tiempo de delay
    params.push_back(std::make_unique<juce::AudioParameterFloat>("TIME_ID",
                                                                 "TIME_NAME",
                                                                 0.01f,
                                                                 0.9f,
                                                                 0.25f));
    // Slider 2: feedback
    params.push_back(std::make_unique<juce::AudioParameterFloat>("FEEDBACK_ID",
                                                                 "FEEDBACK_NAME",
                                                                 0.01f,
                                                                 0.9f,
                                                                 0.25f));
    
    return {
        params.begin(),
        params.end()
    };
}


//==============================================================================
const String SimpleDelayLineAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool SimpleDelayLineAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool SimpleDelayLineAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool SimpleDelayLineAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double SimpleDelayLineAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int SimpleDelayLineAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int SimpleDelayLineAudioProcessor::getCurrentProgram()
{
    return 0;
}

void SimpleDelayLineAudioProcessor::setCurrentProgram (int index)
{
}

const String SimpleDelayLineAudioProcessor::getProgramName (int index)
{
    return {};
}

void SimpleDelayLineAudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void SimpleDelayLineAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    
    // Para todo dsp es necesario declarar un spec. Este indica datos para procesamiento de audio
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate; // obtenido desde DAW
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = getTotalNumInputChannels();
    
    myDelay.reset();
    myDelay.prepare(spec);
    myDelay.setDelay(24000);
    
    mySampleRate = sampleRate; // seteamos el samplerate
    
}

void SimpleDelayLineAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool SimpleDelayLineAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void SimpleDelayLineAudioProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
    ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.
    
    int delayTimeInSamples = *parameters.getRawParameterValue("TIME_ID") * mySampleRate;
    myDelay.setDelay(delayTimeInSamples);
    
    
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
//        auto* channelData = buffer.getWritePointer (channel);

        auto* inSamples = buffer.getReadPointer(channel);
        auto* outSamples = buffer.getWritePointer(channel);
        // ..do something to the data...
        for (int i = 0 ; i < buffer.getNumSamples(); i++ ){
            float delayedSample = myDelay.popSample(channel);
            float inDelay = inSamples[i] + (*parameters.getRawParameterValue("FEEDBACK_ID") * delayedSample);
            
            myDelay.pushSample(channel, inDelay)
            outSamples[i] = inSamples[i] + delayedSample;
        }
    }
}

//==============================================================================
bool SimpleDelayLineAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* SimpleDelayLineAudioProcessor::createEditor()
{
    return new SimpleDelayLineAudioProcessorEditor (*this);
}

//==============================================================================
void SimpleDelayLineAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void SimpleDelayLineAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SimpleDelayLineAudioProcessor();
}
