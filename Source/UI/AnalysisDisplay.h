#pragma once
#include "Core/ScopeFifo.h"
#include "HardwareLook.h"
namespace da::ui {
struct ReferenceDisplay {
    da::Features features;
    std::array<float, 256> waveform{};
    std::array<float, 128> spectrum{};
    juce::String name;
};
class AnalysisDisplay final : public juce::Component {
  public:
    AnalysisDisplay(da::Kind, const HardwareLook &);
    void consume(da::ScopeFifo &, double sampleRate, float inputPeak, float outputPeak,
                 float reduction);
    void setParameters(std::vector<float> values) { parameters = std::move(values); }
    void setReference(const ReferenceDisplay &r) { reference = r; }
    const ReferenceDisplay &currentReference() const { return reference; }
    void setStatus(juce::String s) {
        state = std::move(s);
        repaint();
    }
    void paint(juce::Graphics &) override;
    void paintDetail(juce::Graphics &, juce::Rectangle<float>);
    void paintReferencePanel(juce::Graphics &, juce::Rectangle<float>) const;
    float currentOutputDb() const { return outDb; }
    float currentInputDb() const { return inDb; }
    float currentReduction() const { return gr; }

  private:
    da::Kind kind;
    const HardwareLook &look;
    std::vector<da::Parameter> metadata;
    std::vector<float> parameters;
    ReferenceDisplay reference;
    std::array<float, 2048> input{}, output{};
    std::array<da::ScopeFrame, 256> phase{};
    std::array<float, 128> spectrum{}, outSpectrum{};
    std::array<std::array<float, 128>, 12> history{};
    size_t historyPosition = 0;
    juce::dsp::FFT fft{11};
    size_t position = 0, phasePosition = 0;
    double rate = 48000;
    juce::int64 lastAudio = 0;
    float inDb = -90, outDb = -90, gr = 0;
    juce::String state{"READY / PLAY AUDIO"};
    float value(const char *, float fallback = 0) const;
    void transform(const std::array<float, 2048> &, std::array<float, 128> &);
    void graph(juce::Graphics &, juce::Rectangle<float>);
};
void meter(juce::Graphics &, const HardwareLook &, juce::Rectangle<float>, float db,
           bool scale = false);
ReferenceDisplay summarizeReference(const juce::AudioBuffer<float> &, double, const juce::String &);
} // namespace da::ui
