#include "PluginEditor.h"
#include <chrono>
#include <set>
namespace {
const juce::Colour background(0xff090b0e), panel(0xff15181c), red(0xffe9163e), text(0xfff2f0f1),
    muted(0xffaeb2ba);
template <class T> bool ready(std::future<T> &f) {
    return f.valid() && f.wait_for(std::chrono::seconds(0)) == std::future_status::ready;
}
} // namespace
DAPluginAudioProcessorEditor::DAPluginAudioProcessorEditor(DAPluginAudioProcessor &p)
    : AudioProcessorEditor(&p), processor(p), session(p.currentValues()),
      lastValues(session.values) {
    setResizable(true, true);
    setResizeLimits(940, p.metadata.size() > 12 ? 940 : 800, 1500, 1100);
    for (size_t i = 0; i < p.metadata.size(); ++i) {
        const auto &meta = p.metadata[i];
        auto control = std::make_unique<Control>();
        control->knob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        control->knob.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 100, 20);
        control->knob.setColour(juce::Slider::rotarySliderFillColourId, red);
        control->knob.setColour(juce::Slider::textBoxTextColourId, text);
        control->knob.setColour(juce::Slider::textBoxOutlineColourId, panel);
        control->knob.setTextValueSuffix(" " + juce::String(meta.unit));
        control->knob.setTooltip(meta.module + " / " + meta.name +
                                 "; heuristic suggestions require approval");
        control->label.setText(meta.module + " / " + meta.name, juce::dontSendNotification);
        control->label.setJustificationType(juce::Justification::centred);
        control->label.setColour(juce::Label::textColourId, text);
        control->label.setFont(juce::Font(juce::FontOptions(12.f)));
        control->learn.onClick = [this, id = meta.id] { startLearn(id); };
        control->attachment =
            std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
                p.parameters, meta.id, control->knob);
        addAndMakeVisible(control->knob);
        addAndMakeVisible(control->label);
        addAndMakeVisible(control->learn);
        controls.push_back(std::move(control));
    }
    std::set<std::string> seen;
    for (const auto &meta : p.metadata)
        if (seen.insert(meta.module).second) {
            auto button = std::make_unique<juce::TextButton>("Learn " + juce::String(meta.module));
            button->onClick = [this, module = meta.module] { startLearn(module); };
            addAndMakeVisible(*button);
            modules.push_back(std::move(button));
        }
    for (auto *button : {&learnAll, &loadReference, &preview, &apply, &reject, &undo}) {
        button->setColour(juce::TextButton::buttonColourId, panel);
        button->setColour(juce::TextButton::textColourOffId, text);
        addAndMakeVisible(*button);
    }
    learnAll.setColour(juce::TextButton::buttonColourId, red);
    apply.setColour(juce::TextButton::buttonColourId, red);
    mode.addItem("Input-aware estimate", 1);
    mode.addItem("Reference tonal/image guide", 2);
    mode.addItem("Description-guided estimate", 3);
    mode.setSelectedId(1);
    addAndMakeVisible(mode);
    description.setTextToShowWhenEmpty("Goals: dark, warm, wide, smooth, cinematic", muted);
    addAndMakeVisible(description);
    results.setMultiLine(true);
    results.setReadOnly(true);
    results.setColour(juce::TextEditor::backgroundColourId, panel);
    results.setColour(juce::TextEditor::textColourId, text);
    results.setText("Development checkpoint — not a finished suite.\nSuggestions use deterministic "
                    "audio features, not a neural model. Reference guidance is not iterative "
                    "output matching.\nReverb/shimmer quality, matching accuracy and all host "
                    "workflows require further validation. See docs/STATUS.md.");
    addAndMakeVisible(results);
    status.setColour(juce::Label::textColourId, text);
    addAndMakeVisible(status);
    setStatus("Ready — play audio, choose a scope, inspect suggestions before applying");
    learnAll.onClick = [this] { startLearn("all"); };
    loadReference.onClick = [this] { load(); };
    preview.onClick = [this] {
        syncManual();
        if (session.preview())
            write();
        else
            setStatus("No current suggestion to preview");
    };
    apply.onClick = [this] {
        syncManual();
        if (session.apply())
            write();
        else
            setStatus("No current suggestion to apply");
    };
    reject.onClick = [this] {
        syncManual();
        session.reject();
        ++session.revision;
        capturing = false;
        write();
    };
    undo.onClick = [this] {
        syncManual();
        if (session.undo())
            write();
        else
            setStatus("No learning operation to undo");
    };
    setSize(1120, p.metadata.size() > 12 ? 970 : 850);
    startTimerHz(15);
}
DAPluginAudioProcessorEditor::~DAPluginAudioProcessorEditor() {
    stopTimer();
    syncManual();
    if (session.status == "Previewing") {
        session.reject();
        processor.writeValues(session.values);
    }
}
void DAPluginAudioProcessorEditor::setStatus(const juce::String &s) {
    status.setText(s, juce::dontSendNotification);
}
void DAPluginAudioProcessorEditor::syncManual() {
    auto values = processor.currentValues();
    bool changed = false;
    for (size_t i = 0; i < values.size(); ++i)
        if (std::abs(values[i] - lastValues[i]) > 1e-5f)
            changed = true;
    if (changed) {
        session.reject();
        session.values = values;
        ++session.revision;
        lastValues = values;
        setStatus("Manual/host change — older suggestions invalidated");
    }
}
void DAPluginAudioProcessorEditor::write() {
    processor.writeValues(session.values);
    lastValues = processor.currentValues();
    setStatus(session.status);
}
void DAPluginAudioProcessorEditor::startLearn(std::string requestedScope) {
    syncManual();
    if (capturing || learnWork.valid() || referenceWork.valid()) {
        setStatus("Analysis busy — cancel or wait");
        return;
    }
    session.reject();
    ++session.revision;
    write();
    try {
        objective = {};
        if (mode.getSelectedId() == 2) {
            objective.mode = da::Mode::Reference;
            if (!reference.valid())
                throw std::runtime_error("Reference unavailable — load a non-silent audio file");
        }
        if (mode.getSelectedId() == 3)
            objective = da::describe(description.getText().toStdString());
        if (!processor.requestCapture())
            throw std::runtime_error("Capture busy — play audio to finish the pending capture");
        scope = std::move(requestedScope);
        captureRevision = session.revision;
        capturing = true;
        captureStarted = juce::Time::currentTimeMillis();
        setStatus("Analyzing — capturing 65,536 input samples; keep audio playing");
    } catch (const std::exception &e) {
        setStatus(juce::String("Failed: ") + e.what());
    }
}
void DAPluginAudioProcessorEditor::launch(std::vector<float> l, std::vector<float> r, double sr) {
    auto kind = processor.kind;
    auto values = session.values;
    auto target = reference;
    auto goal = objective;
    auto selected = scope;
    auto revision = captureRevision;
    learnWork = std::async(std::launch::async, [l = std::move(l), r = std::move(r), sr, kind,
                                                values, target, goal, selected, revision] {
        auto features = da::analyze(l.data(), r.data(), l.size(), sr);
        return da::learn(kind, features, &target, goal, values, selected, revision);
    });
}
void DAPluginAudioProcessorEditor::load() {
    if (capturing || learnWork.valid() || referenceWork.valid()) {
        setStatus("Analysis busy");
        return;
    }
    chooser = std::make_unique<juce::FileChooser>("Choose reference (first 30 seconds analyzed)",
                                                  juce::File{}, "*.wav;*.aiff;*.aif;*.flac");
    auto safe = juce::Component::SafePointer<DAPluginAudioProcessorEditor>(this);
    chooser->launchAsync(
        juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
        [safe](const juce::FileChooser &c) {
            if (!safe)
                return;
            auto file = c.getResult();
            if (!file.existsAsFile()) {
                safe->setStatus("Cancelled — no reference selected");
                return;
            }
            safe->syncManual();
            safe->session.reject();
            ++safe->session.revision;
            safe->write();
            safe->reference = {};
            safe->referenceRevision = safe->session.revision;
            safe->setStatus("Analyzing reference — decoding up to 30 seconds on worker");
            safe->referenceWork = std::async(std::launch::async, [file] {
                juce::AudioFormatManager formats;
                formats.registerBasicFormats();
                std::unique_ptr<juce::AudioFormatReader> reader(formats.createReaderFor(file));
                if (!reader || reader->numChannels == 0 || reader->sampleRate < 8000 ||
                    reader->sampleRate > 384000 || reader->lengthInSamples < 1024)
                    throw std::runtime_error("Unsupported, invalid or too-short reference file");
                int length = int(std::min<juce::int64>(reader->lengthInSamples,
                                                       juce::int64(reader->sampleRate * 30)));
                juce::AudioBuffer<float> buffer(2, length);
                buffer.clear();
                if (!reader->read(&buffer, 0, length, 0, true, true))
                    throw std::runtime_error("Reference decode failed");
                if (reader->numChannels == 1)
                    buffer.copyFrom(1, 0, buffer, 0, 0, length);
                auto f = da::analyze(buffer.getReadPointer(0), buffer.getReadPointer(1),
                                     size_t(length), reader->sampleRate);
                if (!f.valid())
                    throw std::runtime_error("Reference is silent or invalid");
                return f;
            });
        });
}
void DAPluginAudioProcessorEditor::timerCallback() {
    syncManual();
    std::vector<float> l, r;
    double sr = 0;
    if (processor.takeCapture(l, r, sr)) {
        if (capturing && captureRevision == session.revision) {
            capturing = false;
            launch(std::move(l), std::move(r), sr);
        } else
            capturing = false;
    }
    if (capturing && juce::Time::currentTimeMillis() - captureStarted > 15000) {
        capturing = false;
        ++session.revision;
        setStatus("Failed — capture timed out; transport must be running");
    }
    if (ready(referenceWork))
        try {
            auto analyzedReference = referenceWork.get();
            if (referenceRevision != session.revision) {
                setStatus("Cancelled — stale reference analysis discarded");
                return;
            }
            reference = analyzedReference;
            setStatus("Reference available — RMS " + juce::String(da::db(float(reference.rms)), 1) +
                      " dBFS; guidance is not an exact match");
        } catch (const std::exception &e) {
            setStatus(juce::String("Reference unavailable: ") + e.what());
        }
    if (ready(learnWork))
        try {
            auto proposal = learnWork.get();
            juce::String message(proposal.limitation);
            for (auto &s : proposal.values) {
                const auto &p = processor.metadata[s.index];
                message += "\n" + juce::String(p.module + " / " + p.name) + ": " +
                           juce::String(session.values[s.index], 2) + " -> " +
                           juce::String(s.value, 2) + " " + juce::String(p.unit) + " (confidence " +
                           juce::String(s.confidence, 2) + ")\n" + juce::String(s.reason);
            }
            if (session.offer(std::move(proposal))) {
                results.setText(message);
                setStatus(session.status);
            } else
                setStatus("Cancelled — stale analysis discarded");
        } catch (const std::exception &e) {
            setStatus(juce::String("Failed: ") + e.what());
        }
    repaint();
}
void DAPluginAudioProcessorEditor::paint(juce::Graphics &g) {
    g.fillAll(background);
    g.setColour(red);
    g.fillRect(0, 0, getWidth(), 3);
    g.setColour(text);
    g.setFont(juce::Font(juce::FontOptions(24.f, juce::Font::bold)));
    g.drawText("D.A PLUGINS  /  " + processor.getName(), 20, 12, 600, 32,
               juce::Justification::left);
    g.setFont(juce::Font(juce::FontOptions(12.f)));
    g.setColour(muted);
    g.drawText("LEARN THE SOUND. RECREATE THE FEEL.    |    Analysis-based development build", 20,
               45, 850, 24, juce::Justification::left);
    g.setColour(panel);
    g.fillRoundedRectangle(16.f, 78.f, float(getWidth() - 32), 56.f, 6.f);
    float in = processor.inputPeak.load(), out = processor.outputPeak.load(),
          gr = processor.reduction.load();
    g.setColour(text);
    g.drawText("IN " + juce::String(da::db(in), 1) + " dBFS    OUT " +
                   juce::String(da::db(out), 1) + " dBFS    GR " + juce::String(gr, 1) + " dB",
               30, 86, 530, 22, juce::Justification::left);
    g.setColour(red);
    g.fillRect(30.f, 116.f,
               float(std::clamp((da::db(out) + 60) / 60, 0.f, 1.f)) * float(getWidth() - 60), 5.f);
}
void DAPluginAudioProcessorEditor::resized() {
    int width = getWidth(), columns = 4, cell = (width - 32) / columns,
        rows = int((controls.size() + 3) / 4);
    int top = 144, cellHeight = std::min(140, (getHeight() - 440) / std::max(rows, 1));
    for (size_t i = 0; i < controls.size(); ++i) {
        int x = 16 + int(i % 4) * cell, y = top + int(i / 4) * cellHeight;
        auto &c = *controls[i];
        c.label.setBounds(x, y, cell, 26);
        c.knob.setBounds(x + 8, y + 26, cell - 16, cellHeight - 54);
        c.learn.setBounds(x + 16, y + cellHeight - 26, cell - 32, 24);
    }
    int y = top + rows * cellHeight + 8;
    int mw = (width - 32) / std::max(1, int(modules.size()));
    for (size_t i = 0; i < modules.size(); ++i)
        modules[i]->setBounds(16 + int(i) * mw, y, mw - 5, 26);
    y += 34;
    mode.setBounds(16, y, 270, 28);
    loadReference.setBounds(296, y, 160, 28);
    description.setBounds(466, y, width - 482, 28);
    y += 36;
    int bw = (width - 32) / 6, i = 0;
    for (auto *b : {&learnAll, &preview, &apply, &reject, &undo})
        b->setBounds(16 + bw * i++, y, bw - 6, 28);
    y += 35;
    status.setBounds(16, y, width - 32, 28);
    y += 32;
    results.setBounds(16, y, width - 32, std::max(50, getHeight() - y - 16));
}
