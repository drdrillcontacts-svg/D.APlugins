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
    struct ReferencePanel final : public juce::Component {
        std::function<void(juce::Graphics &)> onPaint;
        void paint(juce::Graphics &g) override {
            if (onPaint)
                onPaint(g);
        }
    };
    DAPluginAudioProcessor &processor;
    da::ui::Layout layout;
    da::ui::HardwareLook hardware;
    juce::Component stage;
    da::ui::Chassis chassis;
    da::ui::AnalysisDisplay analysis;
    juce::TooltipWindow tooltips{this, 700};
    da::LearningSession session;
    std::vector<float> lastValues;
    std::vector<std::unique_ptr<da::ui::ParameterControl>> controls;
    std::vector<std::unique_ptr<juce::TextButton>> modules;
    juce::TextButton captureInput{"CAPTURE INPUT"}, openReference{"REFERENCE..."},
        learnAll{"LEARN ALL"}, showResults{"DETAILS / LOG"}, loadReference{"LOAD REFERENCE"},
        useReferenceMode{"USE REFERENCE IN LEARN"}, closeReference{"CLOSE"}, preview{"PREVIEW"},
        apply{"APPLY"}, reject{"REJECT"}, undo{"UNDO"}, closeResults{"BACK TO CONTROLS"};
    juce::ComboBox presetMenu;
    juce::TextButton previousPreset{"<"}, nextPreset{">"};
    uint64_t observedPresetGeneration = 0;
    juce::TextEditor results;
    juce::Label status;
    juce::Component drawer;
    ReferencePanel referenceDrawer;
    std::unique_ptr<juce::FileChooser> chooser;
    std::future<da::ui::ReferenceDisplay> referenceWork;
    std::future<da::Proposal> learnWork;
    da::Features reference;
    bool capturing = false, referenceGuidanceEnabled = false;
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
