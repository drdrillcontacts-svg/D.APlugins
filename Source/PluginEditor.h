#pragma once
#include "PluginProcessor.h"
#include "UI/AnalysisDisplay.h"
#include "UI/HardwareLook.h"
#include <future>
class DAPluginAudioProcessorEditor final : public juce::AudioProcessorEditor, private juce::Timer {
  public:
    explicit DAPluginAudioProcessorEditor(DAPluginAudioProcessor &);
    ~DAPluginAudioProcessorEditor() override;
    void paint(juce::Graphics &) override;
    void resized() override;

  private:
    DAPluginAudioProcessor &processor;
    da::ui::HardwareLook hardware;
    da::ui::Layout layout;
    juce::Component stage;
    da::ui::Chassis chassis;
    da::ui::AnalysisDisplay analysis;
    juce::TooltipWindow tooltips{this, 700};
    da::LearningSession session;
    std::vector<float> lastValues;
    std::vector<std::unique_ptr<da::ui::ParameterControl>> controls;
    std::vector<std::unique_ptr<juce::TextButton>> modules;
    juce::TextButton learnAll{"LEARN ALL"}, loadReference{"LOAD FILE"}, preview{"PREVIEW"},
        apply{"APPLY"}, reject{"REJECT"}, undo{"UNDO"}, showResults{"DETAILS / LOG"}, advanced{"ADVANCED"},
        closeResults{"BACK TO CONTROLS"};
    juce::ComboBox mode;
    juce::TextEditor description, results;
    juce::Label status;
    juce::Component drawer;
    std::unique_ptr<juce::FileChooser> chooser;
    std::future<da::ui::ReferenceDisplay> referenceWork;
    std::future<da::Proposal> learnWork;
    da::Features reference;
    bool capturing = false, advancedView = false;
    void updateControlPage();
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
    void paintDetails(juce::Graphics &);
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DAPluginAudioProcessorEditor)
};
