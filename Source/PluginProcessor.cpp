#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <limits>
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
      metadata(da::parameters(kind)), parameters(*this, nullptr, "PARAMETERS", makeLayout(kind)), factoryBank(da::factoryPresets(kind)),
      engine(kind) {
    for (size_t i = 0; i < metadata.size(); ++i) {
        raw[i] = parameters.getRawParameterValue(metadata[i].id);
        hostParameters[i]=parameters.getParameter(metadata[i].id);
    }
}
void DAPluginAudioProcessor::prepareToPlay(double sr, int) {
    cancelCapture();
    engine.prepare(sr);
    capture.prepare(sr);
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
    if(kind==da::Kind::Delay) {
        double bpm=std::numeric_limits<double>::quiet_NaN();
        if(auto *playhead=getPlayHead())
            if(auto position=playhead->getPosition())
                if(auto value=position->getBpm())bpm=*value;
        engine.setTempo(bpm);
        projectBpm.store(engine.tempo(),std::memory_order_relaxed);
        tempoStatus.store(engine.tempoIsValid()?1:engine.hasSeenTempo()?2:0,std::memory_order_relaxed);
    }
    const auto count = size_t(b.getNumSamples());
    capture.process(b.getReadPointer(0), b.getNumChannels()>1 ? b.getReadPointer(1) : nullptr, count);
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
bool DAPluginAudioProcessor::requestCapture() { return capture.request(); }
void DAPluginAudioProcessor::cancelCapture() { capture.cancel(); }
bool DAPluginAudioProcessor::takeCapture(std::vector<float> &l, std::vector<float> &r, double &sr) {
    return capture.take(l,r,sr);
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
        const float normalized = p->convertTo0to1(metadata[i].constrain(v[i]));
        if (std::abs(p->getValue() - normalized) <= 1e-7f)
            continue;
        p->beginChangeGesture();
        p->setValueNotifyingHost(normalized);
        p->endChangeGesture();
    }
}
juce::AudioProcessorEditor *DAPluginAudioProcessor::createEditor() {
    return new DAPluginAudioProcessorEditor(*this);
}
void DAPluginAudioProcessor::setCurrentProgram(int index) {
    if(index<0||index>=getNumPrograms())return;
    programWriting.store(true,std::memory_order_release);
    // Prebuilt bank/cached pointers: no file I/O, preset generation or own allocation here.
    const auto &preset=factoryBank[size_t(index)];
    for(size_t i=0;i<metadata.size();++i) {
        auto *p=hostParameters[i];const float v=p->convertTo0to1(preset.values[i]);
        if(std::abs(v-p->getValue())>1e-7f)p->setValueNotifyingHost(v);
    }
    currentProgram.store(index,std::memory_order_relaxed);
    programRevision.fetch_add(1,std::memory_order_release);
    programWriting.store(false,std::memory_order_release);
}
const juce::String DAPluginAudioProcessor::getProgramName(int index) {
    return index>=0&&index<50?juce::String(factoryBank[size_t(index)].name):juce::String{};
}
void DAPluginAudioProcessor::getStateInformation(juce::MemoryBlock &d) {
    auto xml = parameters.copyState().createXml();
    xml->setAttribute("factoryPresetIndex",getCurrentProgram());
    xml->setAttribute("presetSchema",1);
    copyXmlToBinary(*xml, d);
}
void DAPluginAudioProcessor::setStateInformation(const void *d, int n) {
    auto xml=getXmlFromBinary(d,n);
    if(!xml||!xml->hasTagName(parameters.state.getType()))return;
    auto tree=juce::ValueTree::fromXml(*xml);
    // Legacy projects must not inherit Sync/Division (or other missing values) from a prior program.
    for(const auto &m:metadata) {
        auto child=tree.getChildWithProperty("id",juce::String(m.id));
        if(!child.isValid()) {
            child=juce::ValueTree("PARAM");child.setProperty("id",juce::String(m.id),nullptr);
            tree.appendChild(child,nullptr);
            child.setProperty("value",m.initial,nullptr);
        } else child.setProperty("value",m.constrain(float(child.getProperty("value",m.initial))),nullptr);
    }
    programWriting.store(true,std::memory_order_release);
    parameters.replaceState(tree);
    currentProgram.store(std::clamp(xml->getIntAttribute("factoryPresetIndex",0),0,49));
    programRevision.fetch_add(1,std::memory_order_release);
    programWriting.store(false,std::memory_order_release);
}
juce::AudioProcessor *JUCE_CALLTYPE createPluginFilter() { return new DAPluginAudioProcessor(); }
