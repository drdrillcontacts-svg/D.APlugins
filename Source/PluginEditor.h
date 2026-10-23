#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"

class DAPluginAudioProcessorEditor final : public juce::AudioProcessorEditor
{
public:
    explicit DAPluginAudioProcessorEditor(DAPluginAudioProcessor&);
    void paint(juce::Graphics&) override;
    void resized() override;
private:
    DAPluginAudioProcessor& processor;
    juce::Slider knobs[7];
    juce::Label knobLabels[7], knobValues[7], status;
    juce::TextButton loadReference{"LOAD FILE"}, learnButton{"LEARN MY SOUND"}, matchButton{"AI MATCH"};
    juce::ComboBox modeBox;
    using Attachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    std::unique_ptr<Attachment> attachments[4];
    juce::String statusText{"REFERENCE READY  /  AI ENGINE NOT CONNECTED"};
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DAPluginAudioProcessorEditor)
};
