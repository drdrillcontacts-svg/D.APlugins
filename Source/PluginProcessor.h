#pragma once
#include <JuceHeader.h>
#ifndef DA_PLUGIN_NAME
#define DA_PLUGIN_NAME "DVerb"
#endif
class DAPluginAudioProcessor final : public juce::AudioProcessor {
public:
 DAPluginAudioProcessor(); ~DAPluginAudioProcessor() override = default;
 void prepareToPlay(double,int) override; void releaseResources() override;
 bool isBusesLayoutSupported(const BusesLayout&) const override;
 void processBlock(juce::AudioBuffer<float>&,juce::MidiBuffer&) override;
 juce::AudioProcessorEditor* createEditor() override; bool hasEditor() const override{return true;}
 const juce::String getName() const override{return DA_PLUGIN_NAME;}
 bool acceptsMidi() const override{return false;} bool producesMidi() const override{return false;}
 bool isMidiEffect() const override{return false;} double getTailLengthSeconds() const override{return 0.0;}
 int getNumPrograms() override{return 1;} int getCurrentProgram() override{return 0;}
 void setCurrentProgram(int) override{} const juce::String getProgramName(int) override{return {};}
 void changeProgramName(int,const juce::String&) override{}
 void getStateInformation(juce::MemoryBlock&) override; void setStateInformation(const void*,int) override;
 juce::AudioProcessorValueTreeState parameters;
private: JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DAPluginAudioProcessor)
};
