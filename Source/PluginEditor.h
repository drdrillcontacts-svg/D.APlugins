#pragma once
#include "PluginProcessor.h"
#include <future>
class DAPluginAudioProcessorEditor final : public juce::AudioProcessorEditor, private juce::Timer {
  public:
    explicit DAPluginAudioProcessorEditor(DAPluginAudioProcessor &);
    ~DAPluginAudioProcessorEditor() override;
    void paint(juce::Graphics &) override;
    void resized() override;

  private:
    struct Control {
        juce::Slider knob;
        juce::Label label;
        juce::TextButton learn{"Learn parameter"};
        std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> attachment;
    };
    DAPluginAudioProcessor &processor;
    da::LearningSession session;
    std::vector<float> lastValues;
    std::vector<std::unique_ptr<Control>> controls;
    std::vector<std::unique_ptr<juce::TextButton>> modules;
    juce::TextButton learnAll{"Learn all"}, loadReference{"Load reference"}, preview{"Preview"},
        apply{"Apply"}, reject{"Reject / cancel"}, undo{"Undo"};
    juce::ComboBox mode;
    juce::TextEditor description, results;
    juce::Label status;
    std::unique_ptr<juce::FileChooser> chooser;
    std::future<da::Features> referenceWork;
    std::future<da::Proposal> learnWork;
    da::Features reference, input;
    bool capturing = false;
    uint64_t captureRevision = 0, referenceRevision = 0;
    juce::int64 captureStarted = 0;
    std::string scope;
    da::Objective objective;
    void timerCallback() override;
    void startLearn(std::string);
    void launch(std::vector<float>, std::vector<float>, double);
    void syncManual();
    void write();
    void load();
    void setStatus(const juce::String &);
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DAPluginAudioProcessorEditor)
};
