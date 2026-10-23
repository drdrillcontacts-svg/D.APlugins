#include "PluginProcessor.h"
#include "PluginEditor.h"
namespace {
juce::AudioProcessorValueTreeState::ParameterLayout makeLayout(da::Kind kind) {
    juce::AudioProcessorValueTreeState::ParameterLayout layout;
    for (const auto &p : da::parameters(kind)) {
        juce::NormalisableRange<float> range(p.min, p.max, p.discrete ? 1.f : 0.f);
        if (p.unit == "Hz" || p.id == "time" || p.id == "release")
            range.setSkewForCentre(std::sqrt(p.min * p.max));
        layout.add(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID{p.id, 1}, p.module + " / " + p.name, range, p.initial));
    }
    return layout;
}
} // namespace
DAPluginAudioProcessor::DAPluginAudioProcessor(const juce::String &product)
    : AudioProcessor(BusesProperties()
                         .withInput("Input", juce::AudioChannelSet::stereo(), true)
                         .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      productName(product), kind(da::kindForName(product.toStdString())),
      metadata(da::parameters(kind)), parameters(*this, nullptr, "PARAMETERS", makeLayout(kind)),
      engine(kind) {
    for (size_t i = 0; i < metadata.size(); ++i)
        raw[i] = parameters.getRawParameterValue(metadata[i].id);
}
void DAPluginAudioProcessor::prepareToPlay(double sr, int) {
    engine.prepare(sr);
    captureRate = sr;
    displaySampleRate.store(std::isfinite(sr) ? std::clamp(sr, 8000., 384000.) : 48000.,
                            std::memory_order_relaxed);
}
bool DAPluginAudioProcessor::isBusesLayoutSupported(const BusesLayout &l) const {
    return l.getMainInputChannelSet() == l.getMainOutputChannelSet() &&
           (l.getMainOutputChannelSet() == juce::AudioChannelSet::mono() ||
            l.getMainOutputChannelSet() == juce::AudioChannelSet::stereo());
}
void DAPluginAudioProcessor::processBlock(juce::AudioBuffer<float> &b, juce::MidiBuffer &) {
    juce::ScopedNoDenormals guard;
    if (b.getNumChannels() < 1)
        return;
    const auto count = size_t(b.getNumSamples());
    if (captureState.load(std::memory_order_acquire) == 1) {
        capturePosition = 0;
        captureState.store(2, std::memory_order_release);
    }
    if (captureState.load(std::memory_order_acquire) == 2) {
        for (size_t i = 0; i < count && capturePosition < captureSize; ++i, ++capturePosition) {
            captureL[capturePosition] = b.getSample(0, int(i));
            captureR[capturePosition] = b.getSample(b.getNumChannels() > 1 ? 1 : 0, int(i));
        }
        if (capturePosition == captureSize)
            captureState.store(3, std::memory_order_release);
    }
    float in = 0;
    for (int c = 0; c < b.getNumChannels(); ++c)
        in = std::max(in, b.getMagnitude(c, 0, b.getNumSamples()));
    std::array<float, 64> values{};
    for (size_t i = 0; i < metadata.size(); ++i)
        values[i] = raw[i]->load(std::memory_order_relaxed);
    // Bounded stack scratch preserves the dry waveform while the core processes in place.
    std::array<da::ScopeFrame, 256> frames{};
    for (size_t offset = 0; offset < count; offset += frames.size()) {
        size_t n = std::min(frames.size(), count - offset);
        for (size_t i = 0; i < n; ++i) {
            float a = b.getSample(0, int(offset + i));
            float z = b.getSample(b.getNumChannels() > 1 ? 1 : 0, int(offset + i));
            frames[i].input = std::isfinite(a) && std::isfinite(z)
                                  ? (std::clamp(a, -32.f, 32.f) + std::clamp(z, -32.f, 32.f)) * .5f
                                  : 0;
        }
        auto *left = b.getWritePointer(0) + offset;
        auto *right = b.getNumChannels() > 1 ? b.getWritePointer(1) + offset : nullptr;
        engine.process(left, right, n, values.data());
        for (size_t i = 0; i < n; ++i) {
            frames[i].left = left[i];
            frames[i].right = right ? right[i] : left[i];
            frames[i].output = (frames[i].left + frames[i].right) * .5f;
        }
        scopeFifo.push(frames.data(), n);
    }
    float out = 0;
    for (int c = 0; c < b.getNumChannels(); ++c)
        out = std::max(out, b.getMagnitude(c, 0, b.getNumSamples()));
    inputPeak.store(std::isfinite(in) ? in : 0.f, std::memory_order_relaxed);
    outputPeak.store(out, std::memory_order_relaxed);
    reduction.store(engine.reductionDb(), std::memory_order_relaxed);
}
bool DAPluginAudioProcessor::requestCapture() {
    int expected = 0;
    return captureState.compare_exchange_strong(expected, 1, std::memory_order_acq_rel);
}
bool DAPluginAudioProcessor::takeCapture(std::vector<float> &l, std::vector<float> &r, double &sr) {
    if (captureState.load(std::memory_order_acquire) != 3)
        return false;
    l.assign(captureL.begin(), captureL.end());
    r.assign(captureR.begin(), captureR.end());
    sr = captureRate.load();
    captureState.store(0, std::memory_order_release);
    return true;
}
std::vector<float> DAPluginAudioProcessor::currentValues() const {
    std::vector<float> v;
    for (size_t i = 0; i < metadata.size(); ++i)
        v.push_back(raw[i]->load());
    return v;
}
void DAPluginAudioProcessor::writeValues(const std::vector<float> &v) {
    for (size_t i = 0; i < std::min(v.size(), metadata.size()); ++i) {
        auto *p = parameters.getParameter(metadata[i].id);
        p->beginChangeGesture();
        p->setValueNotifyingHost(p->convertTo0to1(metadata[i].constrain(v[i])));
        p->endChangeGesture();
    }
}
juce::AudioProcessorEditor *DAPluginAudioProcessor::createEditor() {
    return new DAPluginAudioProcessorEditor(*this);
}
void DAPluginAudioProcessor::getStateInformation(juce::MemoryBlock &d) {
    auto xml = parameters.copyState().createXml();
    copyXmlToBinary(*xml, d);
}
void DAPluginAudioProcessor::setStateInformation(const void *d, int n) {
    auto xml = getXmlFromBinary(d, n);
    if (xml && xml->hasTagName(parameters.state.getType()))
        parameters.replaceState(juce::ValueTree::fromXml(*xml));
}
juce::AudioProcessor *JUCE_CALLTYPE createPluginFilter() { return new DAPluginAudioProcessor(); }
