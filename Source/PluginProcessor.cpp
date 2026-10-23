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
DAPluginAudioProcessor::DAPluginAudioProcessor()
    : AudioProcessor(BusesProperties()
                         .withInput("Input", juce::AudioChannelSet::stereo(), true)
                         .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      kind(da::kindForName(DA_PLUGIN_NAME)), metadata(da::parameters(kind)),
      parameters(*this, nullptr, "PARAMETERS", makeLayout(kind)), engine(kind) {
    for (size_t i = 0; i < metadata.size(); ++i)
        raw[i] = parameters.getRawParameterValue(metadata[i].id);
}
void DAPluginAudioProcessor::prepareToPlay(double sr, int) {
    engine.prepare(sr);
    captureRate = sr;
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
    engine.process(b.getWritePointer(0), b.getNumChannels() > 1 ? b.getWritePointer(1) : nullptr,
                   count, values.data());
    float out = 0;
    for (int c = 0; c < b.getNumChannels(); ++c)
        out = std::max(out, b.getMagnitude(c, 0, b.getNumSamples()));
    inputPeak.store(in, std::memory_order_relaxed);
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
