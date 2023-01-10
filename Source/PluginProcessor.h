/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

//==============================================================================
/**
*/
class SimpleDelayLineAudioProcessor  : public AudioProcessor
{
public:
    //==============================================================================
    SimpleDelayLineAudioProcessor();
    ~SimpleDelayLineAudioProcessor();

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (AudioBuffer<float>&, MidiBuffer&) override;

    //==============================================================================
    AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
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
    
    // sliders
    juce::AudioProcessorValueTreeState parameters;
    // definimos prototipo
    juce::AudioProcessorValueTreeState::ParameterLayout initializeGUI();

private:
//    juce::dsp::SimpleDelayLine<float, juce::dsp::DelayLineInterpolationTypes::Linear>> myDelay{192000};
    juce::dsp::DelayLine<float, juce::dsp::DelayLineInterpolationTypes::Linear>> myDelay{192000};
    double mySampleRate{0.0};
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SimpleDelayLineAudioProcessor)
};
