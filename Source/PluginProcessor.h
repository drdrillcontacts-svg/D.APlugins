#pragma once
#include "Core/ScopeFifo.h"
#include "Core/InputCapture.h"
#include "Core/Suite.h"
#include "Core/FactoryPresets.h"
#include <JuceHeader.h>
#include <atomic>
#ifndef DA_PLUGIN_NAME
#define DA_PLUGIN_NAME "DVerb"
#endif
class DAPluginAudioProcessor final : public juce::AudioProcessor {
  public:
    explicit DAPluginAudioProcessor(const juce::String &product = DA_PLUGIN_NAME);
    void prepareToPlay(double, int) override;
    void releaseResources() override {}
    bool isBusesLayoutSupported(const BusesLayout &) const override;
    void processBlock(juce::AudioBuffer<float> &, juce::MidiBuffer &) override;
    juce::AudioProcessorEditor *createEditor() override;
    bool hasEditor() const override { return true; }
    const juce::String getName() const override { return productName; }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override {
        if(kind==da::Kind::Verb)return 30;
        if(kind!=da::Kind::Delay&&kind!=da::Kind::FX)return 0;
        const auto value=[&](const char *id){for(size_t i=0;i<metadata.size();++i)if(metadata[i].id==id)return metadata[i].constrain(raw[i]->load());return 0.f;};
        double ms=value("time");
        if(kind==da::Kind::Delay&&value("sync")>.5f)ms=da::syncedDelayMs(projectBpm.load(),int(value("division")));
        const double f=std::clamp(double(value("feedback")),0.,.95);
        const double repeats=f>1e-6?1+std::ceil(std::log(.001)/std::log(f)):1;
        return ms*.001*1.011*repeats+(kind==da::Kind::FX?2*value("decay")+1:.1);
    }
    int getNumPrograms() override { return 50; }
    int getCurrentProgram() override { return currentProgram.load(); }
    void setCurrentProgram(int) override;
    const juce::String getProgramName(int) override;
    void changeProgramName(int, const juce::String &) override {}
    void getStateInformation(juce::MemoryBlock &) override;
    void setStateInformation(const void *, int) override;
    const juce::String productName;
    const da::Kind kind;
    const std::vector<da::Parameter> metadata;
    juce::AudioProcessorValueTreeState parameters;
    const std::array<da::FactoryPreset,50> factoryBank;
    uint64_t presetGeneration() const noexcept { return programRevision.load(); }
    bool isLoadingProgram() const noexcept { return programWriting.load(); }
    std::atomic<double> projectBpm{120};
    std::atomic<int> tempoStatus{0}; // 0 fallback, 1 host, 2 hold last valid
    std::vector<float> currentValues() const;
    void writeValues(const std::vector<float> &);
    // Single audio producer, single message-thread consumer; ownership changes via release/acquire.
    bool requestCapture();
    void cancelCapture();
    bool takeCapture(std::vector<float> &, std::vector<float> &, double &);
    da::ScopeFifo scopeFifo;
    std::atomic<double> displaySampleRate{48000};
    std::atomic<float> inputPeak{0}, outputPeak{0}, reduction{0};

  private:
    da::Engine engine;
    std::array<std::atomic<float> *, 64> raw{};
    da::InputCapture capture;
    std::array<juce::RangedAudioParameter*,64> hostParameters{};
    std::atomic<int> currentProgram{0};
    std::atomic<uint64_t> programRevision{0};
    std::atomic<bool> programWriting{false};
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DAPluginAudioProcessor)
};
