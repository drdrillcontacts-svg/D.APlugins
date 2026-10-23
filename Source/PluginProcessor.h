#pragma once
#include "Core/ScopeFifo.h"
#include "Core/InputCapture.h"
#include "Core/Suite.h"
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
        return kind == da::Kind::Verb || kind == da::Kind::Delay || kind == da::Kind::FX ? 30 : 0;
    }
    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int) override {}
    const juce::String getProgramName(int) override { return {}; }
    void changeProgramName(int, const juce::String &) override {}
    void getStateInformation(juce::MemoryBlock &) override;
    void setStateInformation(const void *, int) override;
    const juce::String productName;
    const da::Kind kind;
    const std::vector<da::Parameter> metadata;
    juce::AudioProcessorValueTreeState parameters;
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
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DAPluginAudioProcessor)
};
